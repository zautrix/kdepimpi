/*
    Copyright (C) 1999 Torben Weis <weis@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include "kurl.h"

#ifndef KDE_QT_ONLY
#include <kdebug.h>
#include <kglobal.h>
//US#include <kidna.h>
#endif

#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#ifdef _WIN32_

#else
#include <unistd.h>
#endif
#include <qurl.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qregexp.h>
//US#include <qstylesheet.h>
#include <qmap.h>
#include <qtextcodec.h>

static const QString fileProt = "file";

static QTextCodec * codecForHint( int encoding_hint /* not 0 ! */ )
{
    return QTextCodec::codecForMib( encoding_hint );
}

static QString encode( const QString& segment, bool encode_slash, int encoding_hint )
{
  const char *encode_string;
  if (encode_slash)
    encode_string = "<>#@\"&%?={}|^~[]\'`\\:+/";
  else
    encode_string = "<>#@\"&%?={}|^~[]\'`\\:+";

  QCString local;
  if (encoding_hint==0)
    local = segment.local8Bit();
  else
  {
      QTextCodec * textCodec = codecForHint( encoding_hint );
      if (!textCodec)
          local = segment.local8Bit();
      else
          local = textCodec->fromUnicode( segment );
  }

  int old_length = local.length();

  if ( !old_length )
    return segment.isNull() ? QString::null : QString(""); // differenciate null and empty

  // a worst case approximation
  QChar *new_segment = new QChar[ old_length * 3 + 1 ];
  int new_length = 0;

  for ( int i = 0; i < old_length; i++ )
  {
    // 'unsave' and 'reserved' characters
    // according to RFC 1738,
    // 2.2. URL Character Encoding Issues (pp. 3-4)
    // WABA: Added non-ascii
    unsigned char character = local[i];
    if ( (character <= 32) || (character >= 127) ||
         strchr(encode_string, character) )
    {
      new_segment[ new_length++ ] = '%';

      unsigned int c = character / 16;
      c += (c > 9) ? ('A' - 10) : '0';
      new_segment[ new_length++ ] = c;

      c = character % 16;
      c += (c > 9) ? ('A' - 10) : '0';
      new_segment[ new_length++ ] = c;

    }
    else
        new_segment[ new_length++ ] = QChar(local[i] );
  }

  QString result = QString(new_segment, new_length);
  delete [] new_segment;
  return result;
}

static QString encodeHost( const QString& segment, bool encode_slash, int encoding_hint )
{
  // Hostnames are encoded differently
  // we use the IDNA transformation instead

  // Note: when merging qt-addon, use QResolver::domainToAscii here
#ifndef KDE_QT_ONLY
  Q_UNUSED( encode_slash );
  Q_UNUSED( encoding_hint );
  return KIDNA::toAscii(segment);
#else
  return encode(segment, encode_slash, encoding_hint);
#endif
}

static int hex2int( unsigned int _char )
{
  if ( _char >= 'A' && _char <='F')
    return _char - 'A' + 10;
  if ( _char >= 'a' && _char <='f')
    return _char - 'a' + 10;
  if ( _char >= '0' && _char <='9')
    return _char - '0';
  return -1;
}

// WABA: The result of lazy_encode isn't usable for a URL which
// needs to satisfies RFC requirements. However, the following
// operation will make it usable again:
//      encode(decode(...))
//
// As a result one can see that url.prettyURL() does not result in
// a RFC compliant URL but that the following sequence does:
//      KURL(url.prettyURL()).url()


static QString lazy_encode( const QString& segment )
{
  int old_length = segment.length();

  if ( !old_length )
    return QString::null;

  // a worst case approximation
  QChar *new_segment = new QChar[ old_length * 3 + 1 ];
  int new_length = 0;

  for ( int i = 0; i < old_length; i++ )
  {
    unsigned int character = segment[i].unicode(); // Don't use latin1()
                                                   // It returns 0 for non-latin1 values
    // Small set of really ambiguous chars
    if ((character < 32) ||  // Low ASCII
        ((character == '%') && // The escape character itself
           (i+2 < old_length) && // But only if part of a valid escape sequence!
          (hex2int(segment[i+1].unicode())!= -1) &&
          (hex2int(segment[i+2].unicode())!= -1)) ||
        (character == '?') || // Start of query delimiter
        (character == '@') || // Username delimiter
        (character == '#') || // Start of reference delimiter
        ((character == 32) && (i+1 == old_length))) // A trailing space
    {
      new_segment[ new_length++ ] = '%';

      unsigned int c = character / 16;
      c += (c > 9) ? ('A' - 10) : '0';
      new_segment[ new_length++ ] = c;

      c = character % 16;
      c += (c > 9) ? ('A' - 10) : '0';
      new_segment[ new_length++ ] = c;
    }
    else
    new_segment[ new_length++ ] = segment[i];
  }

  QString result = QString(new_segment, new_length);
  delete [] new_segment;
  return result;
}

static void decode( const QString& segment, QString &decoded, QString &encoded, int encoding_hint=0, bool updateDecoded = true )
{
  decoded = QString::null;
  encoded = segment;

  int old_length = segment.length();
  if ( !old_length )
    return;

  QTextCodec *textCodec = 0;
  if (encoding_hint)
      textCodec = codecForHint( encoding_hint );

  if (!textCodec)
      textCodec = QTextCodec::codecForLocale();

  QCString csegment = textCodec->fromUnicode(segment);
  // Check if everything went ok
  if (textCodec->toUnicode(csegment) != segment)
  {
      // Uh oh
      textCodec = codecForHint( 106 ); // Fall back to utf-8
      csegment = textCodec->fromUnicode(segment);
  } 
  old_length = csegment.length();

  int new_length = 0;
  int new_length2 = 0;

  // make a copy of the old one
  char *new_segment = new char[ old_length + 1 ];
  QChar *new_usegment = new QChar[ old_length * 3 + 1 ];

  int i = 0;
  while( i < old_length )
  {
    bool bReencode = false;
    unsigned char character = csegment[ i++ ];
    if ((character <= ' ') || (character > 127))
       bReencode = true;

    new_usegment [ new_length2++ ] = character;
    if (character == '%' )
    {
      int a = i+1 < old_length ? hex2int( csegment[i] ) : -1;
      int b = i+1 < old_length ? hex2int( csegment[i+1] ) : -1;
      if ((a == -1) || (b == -1)) // Only replace if sequence is valid
      {
         // Contains stray %, make sure to re-encode!
         bReencode = true;
      }
      else
      {
         // Valid %xx sequence
         character = a * 16 + b; // Replace with value of %dd
         if (!character && updateDecoded)
            break; // Stop at %00

         new_usegment [ new_length2++ ] = (unsigned char) csegment[i++];
         new_usegment [ new_length2++ ] = (unsigned char) csegment[i++];
      }
    }
    if (bReencode)
    {
      new_length2--;
      new_usegment [ new_length2++ ] = '%';

      unsigned int c = character / 16;
      c += (c > 9) ? ('A' - 10) : '0';
      new_usegment[ new_length2++ ] = c;

      c = character % 16;
      c += (c > 9) ? ('A' - 10) : '0';
      new_usegment[ new_length2++ ] = c;
    }

    new_segment [ new_length++ ] = character;
  }
  new_segment [ new_length ] = 0;

  encoded = QString( new_usegment, new_length2);

  // Encoding specified
  if (updateDecoded)
  {
     QByteArray array;
     array.setRawData(new_segment, new_length);
     decoded = textCodec->toUnicode( array, new_length );
     array.resetRawData(new_segment, new_length);
     QCString validate = textCodec->fromUnicode(decoded);

     if (strcmp(validate.data(), new_segment) != 0)
     {
        decoded = QString::fromLocal8Bit(new_segment, new_length);
     }
  }

  delete [] new_segment;
  delete [] new_usegment;
}

static QString decode(const QString &segment, int encoding_hint = 0)
{
  QString result;
  QString tmp;
  decode(segment, result, tmp, encoding_hint);
  return result;
}

static QString cleanpath(const QString &path, bool cleanDirSeparator=true)
{
  if (path.isEmpty()) return QString::null;
  int len = path.length();
  bool slash = (len && path[len-1] == '/') ||
               (len > 1 && path[len-2] == '/' && path[len-1] == '.');

  // The following code cleans up directory path much like
  // QDir::cleanDirPath() except it can be made to ignore multiple
  // directory separators by setting the flag to false.  That fixes
  // bug# 15044, mail.altavista.com and other similar brain-dead server
  // implementations that do not follow what has been specified in
  // RFC 2396!! (dA)
  QString result;
  int cdUp, orig_pos, pos;

  cdUp = 0;
  pos = orig_pos = len;
  while ( pos && (pos = path.findRev('/',--pos)) != -1 )
  {
    len = orig_pos - pos - 1;
    if ( len == 2 && path[pos+1] == '.' && path[pos+2] == '.' )
      cdUp++;
    else
    {
      // Ignore any occurances of '.'
      // This includes entries that simply do not make sense like /..../
      if ( (len || !cleanDirSeparator) &&
           (len != 1 || path[pos+1] != '.' ) )
      {
          if ( !cdUp )
              result.prepend(path.mid(pos, len+1));
          else
              cdUp--;
      }
    }
    orig_pos = pos;
  }

  if ( result.isEmpty() )
    result = "/";
  else if ( slash && result.at(result.length()-1) != '/' )
       result.append('/');

  return result;
}

bool KURL::isRelativeURL(const QString &_url)
{
  int len = _url.length();
  if (!len) return true; // Very short relative URL.
  const QChar *str = _url.unicode();

  // Absolute URL must start with alpha-character
  if (!isalpha(str[0].latin1()))
     return true; // Relative URL

  for(int i = 1; i < len; i++)
  {
     char c = str[i].latin1(); // Note: non-latin1 chars return 0!
     if (c == ':')
        return false; // Absolute URL

     // Protocol part may only contain alpha, digit, + or -
     if (!isalpha(c) && !isdigit(c) && (c != '+') && (c != '-'))
        return true; // Relative URL
  }
  // URL did not contain ':'
  return true; // Relative URL
}

KURL::List::List(const KURL &url)
{
    append( url );
}

KURL::List::List(const QStringList &list)
{
  for (QStringList::ConstIterator it = list.begin();
       it != list.end();
       it++)
    {
      append( KURL(*it) );
    }
}

QStringList KURL::List::toStringList() const
{
  QStringList lst;
   for( KURL::List::ConstIterator it = begin();
        it != end();
        it++)
   {
      lst.append( (*it).url() );
   }
   return lst;
}


KURL::KURL()
{
  reset();
}

KURL::~KURL()
{
}


KURL::KURL( const QString &url, int encoding_hint )
{
  reset();
  parse( url, encoding_hint );
}

KURL::KURL( const char * url, int encoding_hint )
{
  reset();
  parse( QString::fromLatin1(url), encoding_hint );
}

KURL::KURL( const QCString& url, int encoding_hint )
{
  reset();
  parse( QString::fromLatin1(url), encoding_hint );
}

KURL::KURL( const KURL& _u )
{
  *this = _u;
}

QDataStream & operator<< (QDataStream & s, const KURL & a)
{
  QString QueryForWire=a.m_strQuery_encoded;
  if (!a.m_strQuery_encoded.isNull())
    QueryForWire.prepend("?");

    s << a.m_strProtocol << a.m_strUser << a.m_strPass << a.m_strHost
      << a.m_strPath << a.m_strPath_encoded << QueryForWire << a.m_strRef_encoded
      << Q_INT8(a.m_bIsMalformed ? 1 : 0) << a.m_iPort;
    return s;
}

QDataStream & operator>> (QDataStream & s, KURL & a)
{
    Q_INT8 malf;
    QString QueryFromWire;
    s >> a.m_strProtocol >> a.m_strUser >> a.m_strPass >> a.m_strHost
      >> a.m_strPath >> a.m_strPath_encoded >> QueryFromWire >> a.m_strRef_encoded
      >> malf >> a.m_iPort;
    a.m_bIsMalformed = (malf != 0);

    if ( QueryFromWire.isEmpty() )
      a.m_strQuery_encoded = QString::null;
    else
      a.m_strQuery_encoded = QueryFromWire.mid(1);

    return s;
}

#ifndef QT_NO_NETWORKPROTOCOL
KURL::KURL( const QUrl &u )
{
  *this = u;
}
#endif

KURL::KURL( const KURL& _u, const QString& _rel_url, int encoding_hint )
{
  // WORKAROUND THE RFC 1606 LOOPHOLE THAT ALLOWS
  // http:/index.html AS A VALID SYNTAX FOR RELATIVE
  // URLS. ( RFC 2396 section 5.2 item # 3 )
  QString rUrl = _rel_url;
  int len = _u.m_strProtocol.length();
  if ( !_u.m_strHost.isEmpty() && !rUrl.isEmpty() &&
       rUrl.find( _u.m_strProtocol, 0, false ) == 0 &&
       rUrl[len] == ':' && (rUrl[len+1] != '/' ||
       (rUrl[len+1] == '/' && rUrl[len+2] != '/')) )
  {
    rUrl.remove( 0, rUrl.find( ':' ) + 1 );
  }

  if ( rUrl.isEmpty() )
  {
    *this = _u;
  }
  else if ( rUrl[0] == '#' )
  {
    *this = _u;
    QString ref = decode(rUrl.mid(1), encoding_hint);
    if ( ref.isNull() )
        ref = ""; // we know there was an (empty) html ref, we saw the '#'
    setHTMLRef( ref );
  }
  else if ( isRelativeURL( rUrl) )
  {
    *this = _u;
    m_strQuery_encoded = QString::null;
    m_strRef_encoded = QString::null;
    if ( rUrl[0] == '/')
    {
        if ((rUrl.length() > 1) && (rUrl[1] == '/'))
        {
           m_strHost = QString::null;
        }
        m_strPath = QString::null;
        m_strPath_encoded = QString::null;
    }
    else if ( rUrl[0] != '?' )
    {
       int pos = m_strPath.findRev( '/' );
       if (pos >= 0)
          m_strPath.truncate(pos);
       m_strPath += '/';
       if (!m_strPath_encoded.isEmpty())
       {
          pos = m_strPath_encoded.findRev( '/' );
          if (pos >= 0)
             m_strPath_encoded.truncate(pos);
          m_strPath_encoded += '/';
       }
    }
    else
    {
       if ( m_strPath.isEmpty() )
          m_strPath = '/';
    }
    KURL tmp( url() + rUrl, encoding_hint);
    *this = tmp;
    cleanPath(false);
  }
  else
  {
    KURL tmp( rUrl, encoding_hint);
    *this = tmp;
    // Preserve userinfo if applicable.
    if (!_u.m_strUser.isEmpty() && m_strUser.isEmpty() && (_u.m_strHost == m_strHost) && (_u.m_strProtocol == m_strProtocol))
    {
       m_strUser = _u.m_strUser;
       m_strPass = _u.m_strPass;
    }
  }
}

void KURL::reset()
{
  m_strProtocol = QString::null;
  m_strUser = QString::null;
  m_strPass = QString::null;
  m_strHost = QString::null;
  m_strPath = QString::null;
  m_strPath_encoded = QString::null;
  m_strQuery_encoded = QString::null;
  m_strRef_encoded = QString::null;
  m_bIsMalformed = true;
  m_iPort = 0;
}

bool KURL::isEmpty() const
{
  return (m_strPath.isEmpty() && m_strProtocol.isEmpty());
}

void KURL::parse( const QString& _url, int encoding_hint )
{
  //kdDebug(126) << "parse " << _url << endl;
  // Return immediately whenever the given url
  // is empty or null.
  if ( _url.isEmpty() )
  {
    m_strProtocol = _url;
    return;
  }

  QString port;
  bool badHostName = false;
  int start = 0;
  uint len = _url.length();
  const QChar* buf = _url.unicode();
  const QChar* orig = buf;

  QChar delim;
  QString tmp;

  uint pos = 0;

  // Node 1: Accept alpha or slash
  QChar x = buf[pos++];
  if ( x == '/' )
    goto Node9;
  if ( !isalpha( x.unicode ()  ) )
    goto NodeErr;

  // Node 2: Accept any amount of (alpha|digit|'+'|'-')
  // '.' is not currently accepted, because current KURL may be confused.
  // Proceed with :// :/ or :
  while( pos < len && (isalpha((int)buf[pos].unicode ()) || isdigit((int)buf[pos].unicode ()) ||
          buf[pos] == '+' || buf[pos] == '-')) pos++;

  if ( pos+2 < len && buf[pos] == ':' && buf[pos+1] == '/' && buf[pos+2] == '/' )
    {
      m_strProtocol = QString( orig, pos ).lower();
      pos += 3;
    }
  else if (pos+1 < len && buf[pos] == ':' ) // Need to always compare length()-1 otherwise KURL passes "http:" as legal!!
    {
      m_strProtocol = QString( orig, pos ).lower();
      //kdDebug(126)<<"setting protocol to "<<m_strProtocol<<endl;
      pos++;
      start = pos;
      goto Node9;
    }
  else
    goto NodeErr;

  //Node 3: We need at least one character here
  if ( pos == len )
      goto NodeErr;
  start = pos;

  // Node 4: Accept any amount of characters.
  if (buf[pos] == '[')     // An IPv6 host follows.
      goto Node8;
  // Terminate on / or @ or ? or # or " or ; or <
  x = buf[pos];
  while( (x != ':') && (x != '@') && (x != '/') && (x != '?') && (x != '#') )
  {
     if ((x == '\"') || (x == ';') || (x == '<'))
        badHostName = true;
     if (++pos == len)
        break;
     x = buf[pos];
  }
  if ( pos == len )
    {
      if (badHostName)
         goto NodeErr;

      setHost(decode(QString( buf + start, pos - start ), encoding_hint));
      goto NodeOk;
    }
  if ( x == '@' )
    {
      m_strUser = decode(QString( buf + start, pos - start ), encoding_hint);
      pos++;
      goto Node7;
    }
  else if ( (x == '/') || (x == '?') || (x == '#'))
    {
      if (badHostName)
         goto NodeErr;

      setHost(decode(QString( buf + start, pos - start ), encoding_hint));
      start = pos;
      goto Node9;
    }
  else if ( x != ':' )
    goto NodeErr;
  m_strUser = decode(QString( buf + start, pos - start ), encoding_hint);
  pos++;

  // Node 5: We need at least one character
  if ( pos == len )
    goto NodeErr;
  start = pos++;

  // Node 6: Read everything until @, /, ? or #
  while( (pos < len) &&
		(buf[pos] != '@') &&
		(buf[pos] != '/') &&
		(buf[pos] != '?') &&
		(buf[pos] != '#')) pos++;
  // If we now have a '@' the ':' seperates user and password.
  // Otherwise it seperates host and port.
  if ( (pos == len) || (buf[pos] != '@') )
    {
      // Ok the : was used to separate host and port
      if (badHostName)
         goto NodeErr;
      setHost(m_strUser);
      m_strUser = QString::null;
      QString tmp( buf + start, pos - start );
      char *endptr;
      m_iPort = (unsigned short int)strtol(tmp.ascii(), &endptr, 10);
      if ((pos == len) && (strlen(endptr) == 0))
        goto NodeOk;
      // there is more after the digits
      pos -= strlen(endptr);
      start = pos++;
      goto Node9;
    }
  m_strPass = decode(QString( buf + start, pos - start), encoding_hint);
  pos++;

  // Node 7: We need at least one character
 Node7:
  if ( pos == len )
    goto NodeErr;

 Node8:
  if (buf[pos] == '[')
  {
    // IPv6 address
    start = ++pos; // Skip '['

    if (pos == len)
    {
       badHostName = true;
       goto NodeErr;
    }
    // Node 8b: Read everything until ] or terminate
    badHostName = false;
    x = buf[pos];
    while( (x != ']') )
    {
       if ((x == '\"') || (x == ';') || (x == '<'))
          badHostName = true;
       if (++pos == len)
       {
          badHostName = true;
          break;
       }
       x = buf[pos];
    }
    if (badHostName)
       goto NodeErr;
    setHost(decode(QString( buf + start, pos - start ), encoding_hint));
    if (pos < len) pos++; // Skip ']'
    if (pos == len)
       goto NodeOk;
  }
  else
  {
    // Non IPv6 address
    start = pos;

    // Node 8b: Read everything until / : or terminate
    badHostName = false;
    x = buf[pos];
    while( (x != ':') && (x != '@') && (x != '/') && (x != '?') && (x != '#') )
    {
       if ((x == '\"') || (x == ';') || (x == '<'))
          badHostName = true;
       if (++pos == len)
          break;
       x = buf[pos];
    }
    if (badHostName)
       goto NodeErr;
    if ( pos == len )
    {
       setHost(decode(QString( buf + start, pos - start ), encoding_hint));
       goto NodeOk;
    }
    setHost(decode(QString( buf + start, pos - start ), encoding_hint));
  }
  x = buf[pos];
  if ( x == '/' )
    {
      start = pos++;
      goto Node9;
    }
  else if ( x != ':' )
    goto NodeErr;
  pos++;

  // Node 8a: Accept at least one digit
  if ( pos == len )
    goto NodeErr;
  start = pos;
  if ( !isdigit( buf[pos++].unicode () ) )
    goto NodeErr;

  // Node 8b: Accept any amount of digits
  while( pos < len && isdigit( buf[pos].unicode () ) ) pos++;
  port = QString( buf + start, pos - start );
  m_iPort = port.toUShort();
  if ( pos == len )
    goto NodeOk;
  start = pos++;

 Node9: // parse path until query or reference reached

  while( pos < len && buf[pos] != '#' && buf[pos]!='?' ) pos++;

  tmp = QString( buf + start, pos - start );
  //kdDebug(126)<<" setting encoded path&query to:"<<tmp<<endl;
  setEncodedPath( tmp, encoding_hint );

  if ( pos == len )
      goto NodeOk;

 //Node10: // parse query or reference depending on what comes first
  delim = (buf[pos++]=='#'?'?':'#');

  start = pos;

  while(pos < len && buf[pos]!=delim ) pos++;

  tmp = QString(buf + start, pos - start);
  if (delim=='#')
      setQuery(tmp, encoding_hint);
  else
      m_strRef_encoded = tmp;

  if (pos == len)
      goto NodeOk;

 //Node11: // feed the rest into the remaining variable
  tmp = QString( buf + pos + 1, len - pos - 1);
  if (delim == '#')
      m_strRef_encoded = tmp;
  else
      setQuery(tmp, encoding_hint);

 NodeOk:
  //kdDebug(126)<<"parsing finished. m_strProtocol="<<m_strProtocol<<" m_strHost="<<m_strHost<<" m_strPath="<<m_strPath<<endl;
  m_bIsMalformed = false; // Valid URL

  //kdDebug()<<"Prot="<<m_strProtocol<<"\nUser="<<m_strUser<<"\nPass="<<m_strPass<<"\nHost="<<m_strHost<<"\nPath="<<m_strPath<<"\nQuery="<<m_strQuery_encoded<<"\nRef="<<m_strRef_encoded<<"\nPort="<<m_iPort<<endl;
  if (m_strProtocol.isEmpty())
  {
    m_strProtocol = fileProt;
  }
  return;

 NodeErr:
//  kdDebug(126) << "KURL couldn't parse URL \"" << _url << "\"" << endl;
  reset();
  m_strProtocol = _url;
}

KURL& KURL::operator=( const QString& _url )
{
  reset();
  parse( _url );

  return *this;
}

KURL& KURL::operator=( const char * _url )
{
  reset();
  parse( QString::fromLatin1(_url) );

  return *this;
}

#ifndef QT_NO_NETWORKPROTOCOL
KURL& KURL::operator=( const QUrl & u )
{
  m_strProtocol = u.protocol();
  m_strUser = u.user();
  m_strPass = u.password();
  m_strHost = u.host();
#ifdef QT_4_COMPAT
  m_strPath = u.path( );
#else
  m_strPath = u.path( FALSE );
#endif
  m_strPath_encoded = QString::null;
  m_strQuery_encoded = u.query();
  m_strRef_encoded = u.ref();
  m_bIsMalformed = !u.isValid();
  m_iPort = u.port();

  return *this;
}
#endif

KURL& KURL::operator=( const KURL& _u )
{
  m_strProtocol = _u.m_strProtocol;
  m_strUser = _u.m_strUser;
  m_strPass = _u.m_strPass;
  m_strHost = _u.m_strHost;
  m_strPath = _u.m_strPath;
  m_strPath_encoded = _u.m_strPath_encoded;
  m_strQuery_encoded = _u.m_strQuery_encoded;
  m_strRef_encoded = _u.m_strRef_encoded;
  m_bIsMalformed = _u.m_bIsMalformed;
  m_iPort = _u.m_iPort;

  return *this;
}

bool KURL::operator==( const KURL& _u ) const
{
  if ( isMalformed() || _u.isMalformed() )
    return false;

  if ( m_strProtocol == _u.m_strProtocol &&
       m_strUser == _u.m_strUser &&
       m_strPass == _u.m_strPass &&
       m_strHost == _u.m_strHost &&
       m_strPath == _u.m_strPath &&
       // The encoded path may be null, but the URLs are still equal (David)
       ( m_strPath_encoded.isNull() || _u.m_strPath_encoded.isNull() ||
         m_strPath_encoded == _u.m_strPath_encoded ) &&
       m_strQuery_encoded == _u.m_strQuery_encoded &&
       m_strRef_encoded == _u.m_strRef_encoded &&
       m_iPort == _u.m_iPort )
  {
    return true;
  }

  return false;
}

bool KURL::operator==( const QString& _u ) const
{
  KURL u( _u );
  return ( *this == u );
}

bool KURL::cmp( const KURL &u, bool ignore_trailing ) const
{
  return equals( u, ignore_trailing );
}

bool KURL::equals( const KURL &_u, bool ignore_trailing ) const
{
  if ( isMalformed() || _u.isMalformed() )
    return false;

  if ( ignore_trailing )
  {
    QString path1 = path(1);
    QString path2 = _u.path(1);
    if ( path1 != path2 )
      return false;

    if ( m_strProtocol == _u.m_strProtocol &&
         m_strUser == _u.m_strUser &&
         m_strPass == _u.m_strPass &&
         m_strHost == _u.m_strHost &&
         m_strQuery_encoded == _u.m_strQuery_encoded &&
         m_strRef_encoded == _u.m_strRef_encoded &&
         m_iPort == _u.m_iPort )
      return true;

    return false;
  }

  return ( *this == _u );
}

bool KURL::isParentOf( const KURL& _u ) const
{
  if ( isMalformed() || _u.isMalformed() )
    return false;

  if ( m_strProtocol == _u.m_strProtocol &&
       m_strUser == _u.m_strUser &&
       m_strPass == _u.m_strPass &&
       m_strHost == _u.m_strHost &&
       m_strQuery_encoded == _u.m_strQuery_encoded &&
       m_strRef_encoded == _u.m_strRef_encoded &&
       m_iPort == _u.m_iPort )
  {
    if ( path().isEmpty() || _u.path().isEmpty() )
        return false; // can't work with implicit paths

    QString p1( cleanpath( path() ) );
    if ( p1.at(p1.length()-1) != '/' )
        p1 += '/';
    QString p2( cleanpath( _u.path() ) );
    if ( p2.at(p2.length()-1) != '/' )
        p2 += '/';

    //kdDebug(126) << "p1=" << p1 << endl;
    //kdDebug(126) << "p2=" << p2 << endl;
    //kdDebug(126) << "p1.length()=" << p1.length() << endl;
    //kdDebug(126) << "p2.left(!$)=" << p2.left( p1.length() ) << endl;
    return p2.startsWith( p1 );
  }
  return false;
}

void KURL::setFileName( const QString& _txt )
{
  m_strRef_encoded = QString::null;
  int i = 0;
  while( _txt[i] == '/' ) ++i;
  QString tmp;
  if ( i )
    tmp = _txt.mid( i );
  else
    tmp = _txt;

  QString path = m_strPath_encoded.isEmpty() ? m_strPath : m_strPath_encoded;
  if ( path.isEmpty() )
    path = "/";
  else
  {
    int lastSlash = path.findRev( '/' );
    if ( lastSlash == -1)
    {
      // The first character is not a '/' ???
      // This looks strange ...
      path = "/";
    }
    else if ( path.right(1) != "/" )
      path.truncate( lastSlash+1 ); // keep the "/"
  }
  if (m_strPath_encoded.isEmpty())
  {
     path += tmp;
     setPath( path );
  }
  else
  {
     path += encode_string(tmp);
     setEncodedPath( path );
  }
  cleanPath();
}

void KURL::cleanPath( bool cleanDirSeparator ) // taken from the old KURL
{
  m_strPath = cleanpath(m_strPath, cleanDirSeparator);
  // WABA: Is this safe when "/../" is encoded with %?
  m_strPath_encoded = cleanpath(m_strPath_encoded, cleanDirSeparator);
}

static QString trailingSlash( int _trailing, const QString &path )
{
  QString result = path;

  if ( _trailing == 0 )
    return result;
  else if ( _trailing == 1 )
  {
    int len = result.length();
    if ( len == 0 )
      result = QString::null;
    else if ( result[ len - 1 ] != '/' )
      result += "/";
    return result;
  }
  else if ( _trailing == -1 )
  {
    if ( result == "/" )
      return result;
    int len = result.length();
    if ( len != 0 && result[ len - 1 ] == '/' )
      result.truncate( len - 1 );
    return result;
  }
  else {
    assert( 0 );
    return QString::null;
  }
}

void KURL::adjustPath( int _trailing )
{
  if (!m_strPath_encoded.isEmpty())
  {
     m_strPath_encoded = trailingSlash( _trailing, m_strPath_encoded );
  }
  m_strPath = trailingSlash( _trailing, m_strPath );
}


QString KURL::encodedPathAndQuery( int _trailing, bool _no_empty_path, int encoding_hint ) const
{
  QString tmp;
  if (!m_strPath_encoded.isEmpty() && encoding_hint == 0)
  {
     tmp = trailingSlash( _trailing, m_strPath_encoded );
  }
  else
  {
     tmp = path( _trailing );
     if ( _no_empty_path && tmp.isEmpty() )
        tmp = "/";
     tmp = encode( tmp, false, encoding_hint );
  }

  // TODO apply encoding_hint to the query
  if (!m_strQuery_encoded.isNull())
      tmp += '?' + m_strQuery_encoded;
  return tmp;
}

void KURL::setEncodedPath( const QString& _txt, int encoding_hint )
{
  m_strPath_encoded = _txt;

  decode( m_strPath_encoded, m_strPath, m_strPath_encoded, encoding_hint );
  // Throw away encoding for local files, makes file-operations faster.
  if (m_strProtocol == fileProt)
     m_strPath_encoded = QString::null;
}


void KURL::setEncodedPathAndQuery( const QString& _txt, int encoding_hint )
{
  int pos = _txt.find( '?' );
  if ( pos == -1 )
  {
    setEncodedPath(_txt, encoding_hint);
    m_strQuery_encoded = QString::null;
  }
  else
  {
    setEncodedPath(_txt.left( pos ), encoding_hint);
    setQuery(_txt.right(_txt.length() - pos - 1), encoding_hint);
  }
}

QString KURL::path( int _trailing ) const
{
  return trailingSlash( _trailing, path() );
}

bool KURL::isLocalFile() const
{
  if ( (m_strProtocol != fileProt ) || hasSubURL() )
     return false;
     
  if (m_strHost.isEmpty() || (m_strHost == "localhost"))
     return true;
     
  char hostname[ 256 ];
  hostname[ 0 ] = '\0';
#ifdef _WIN32_
  // pending LR fixme
  //hostname = "localhost";
#else
  if (!gethostname( hostname, 255 ))
     hostname[sizeof(hostname)-1] = '\0';
#endif
  for(char *p = hostname; *p; p++)
     *p = tolower(*p);
     
  return (m_strHost == hostname);
}

void KURL::setFileEncoding(const QString &encoding)
{
  if (!isLocalFile())
     return;

  QString q = query();

  if (!q.isEmpty() && (q[0] == '?'))
     q = q.mid(1);

  QStringList args = QStringList::split('&', q);
  for(QStringList::Iterator it = args.begin();
      it != args.end();)
  {
      QString s = decode_string(*it);
      if (s.startsWith("charset="))
//US changed erase into remove ???
         it = args.remove(it);
      else
         ++it;
  }
  if (!encoding.isEmpty())
     args.append("charset="+encode_string(encoding));

  if (args.isEmpty())
     setQuery(QString::null);
  else
     setQuery(args.join("&"));
}

QString KURL::fileEncoding() const
{
  if (!isLocalFile())
     return QString::null;

  QString q = query();

  if (q.isEmpty())
     return QString::null;

  if (q[0] == '?')
     q = q.mid(1);

  QStringList args = QStringList::split('&', q);
  for(QStringList::ConstIterator it = args.begin();
      it != args.end();
      ++it)
  {
      QString s = decode_string(*it);
      if (s.startsWith("charset="))
         return s.mid(8);
  }
  return QString::null;
}

bool KURL::hasSubURL() const
{
  if ( m_strProtocol.isEmpty() || m_bIsMalformed )
    return false;
  if (m_strRef_encoded.isEmpty())
     return false;
  if (m_strRef_encoded.startsWith("gzip:"))
     return true;
  if (m_strRef_encoded.startsWith("bzip:"))
     return true;
  if (m_strRef_encoded.startsWith("bzip2:"))
     return true;
  if (m_strRef_encoded.startsWith("tar:"))
     return true;
  if ( m_strProtocol == "error" ) // anything that starts with error: has suburls
     return true;
  return false;
}

QString KURL::url( int _trailing, int encoding_hint ) const
{
  if( m_bIsMalformed )
  {
    // Return the whole url even when the url is
    // malformed.  Under such conditions the url
    // is stored in m_strProtocol.
    return m_strProtocol;
  }

  QString u = m_strProtocol;
  if (!u.isEmpty())
     u += ":";

  if ( hasHost() )
  {
    u += "//";
    if ( hasUser() )
    {
      u += encode(m_strUser, true, encoding_hint);
      if ( hasPass() )
      {
        u += ":";
        u += encode(m_strPass, true, encoding_hint);
      }
      u += "@";
    }
    bool IPv6 = (m_strHost.find(':') != -1);
    if (IPv6)
       u += '[' + m_strHost + ']';
    else
       u += encodeHost(m_strHost, true, encoding_hint);
    if ( m_iPort != 0 ) {
      QString buffer;
      buffer.sprintf( ":%u", m_iPort );
      u += buffer;
    }
  }

  u += encodedPathAndQuery( _trailing, false, encoding_hint );

  if ( hasRef() )
  {
    u += "#";
    u += m_strRef_encoded;
  }

  return u;
}

QString KURL::prettyURL( int _trailing ) const
{
  if( m_bIsMalformed )
  {
    // Return the whole url even when the url is
    // malformed.  Under such conditions the url
    // is stored in m_strProtocol.
    return m_strProtocol;
  }

  QString u = m_strProtocol;
  if (!u.isEmpty())
     u += ":";

  if ( hasHost() )
  {
    u += "//";
    if ( hasUser() )
    {
      u += lazy_encode(m_strUser);
      // Don't show password!
      u += "@";
    }
    bool IPv6 = (m_strHost.find(':') != -1);
    if (IPv6)
    {
       u += '[' + m_strHost + ']';
    }
    else
    {
       u += lazy_encode(m_strHost);
    }
    if ( m_iPort != 0 ) {
      QString buffer;
      buffer.sprintf( ":%u", m_iPort );
      u += buffer;
    }
  }

  u += trailingSlash( _trailing, lazy_encode( m_strPath ) );
  if (!m_strQuery_encoded.isNull())
      u += '?' + m_strQuery_encoded;

  if ( hasRef() )
  {
    u += "#";
    u += m_strRef_encoded;
  }

  return u;
}

QString KURL::prettyURL( int _trailing, AdjustementFlags _flags) const
{
	QString u = prettyURL(_trailing);
	if (_flags & StripFileProtocol && u.startsWith("file:"))
		u.remove(0, 5);
	return u;
}

QString KURL::htmlURL() const
{
//US QStyleSheet::escape was not in my Qt distribution. Why ???
//US  return QStyleSheet::escape(prettyURL());
  return prettyURL();
}

KURL::List KURL::split( const KURL& _url )
{
  QString ref;
  KURL::List lst;
  KURL url = _url;

  while(true)
  {
     KURL u = url;
     u.m_strRef_encoded = QString::null;
     lst.append(u);
     if (url.hasSubURL())
     {
        url = KURL(url.m_strRef_encoded);
     }
     else
     {
        ref = url.m_strRef_encoded;
        break;
     }
  }

  // Set HTML ref in all URLs.
  KURL::List::Iterator it;
  for( it = lst.begin() ; it != lst.end(); ++it )
  {
     (*it).m_strRef_encoded = ref;
  }

  return lst;
}

KURL::List KURL::split( const QString& _url )
{
  return split(KURL(_url));
}

KURL KURL::join( const KURL::List & lst )
{
  if (lst.isEmpty()) return KURL();
  KURL tmp;

  KURL::List::ConstIterator first = lst.fromLast();
  for( KURL::List::ConstIterator it = first; it != lst.end(); --it )
  {
     KURL u(*it);
     if (it != first)
     {
         if (u.m_strRef_encoded.isEmpty()) u.m_strRef_encoded = tmp.url();
        else u.m_strRef_encoded += "#" + tmp.url(); // Support more than one suburl thingy
     }
     tmp = u;
  }

  return tmp;
}

QString KURL::fileName( bool _strip_trailing_slash ) const
{
  QString fname;
  if (hasSubURL()) { // If we have a suburl, then return the filename from there
    KURL::List list = KURL::split(*this);
    KURL::List::Iterator it = list.fromLast();
    return (*it).fileName(_strip_trailing_slash);
  }
  const QString &path = m_strPath;

  int len = path.length();
  if ( len == 0 )
    return fname;

  if ( _strip_trailing_slash )
  {
    while ( len >= 1 && path[ len - 1 ] == '/' )
      len--;
  }
  else if ( path[ len - 1 ] == '/' )
    return fname;

  // Does the path only consist of '/' characters ?
  if ( len == 1 && path[ 0 ] == '/' )
    return fname;

  // Skip last n slashes
  int n = 1;
  if (!m_strPath_encoded.isEmpty())
  {
     // This is hairy, we need the last unencoded slash.
     // Count in the encoded string how many encoded slashes follow the last
     // unencoded one.
     int i = m_strPath_encoded.findRev( '/', len - 1 );
     QString fileName_encoded = m_strPath_encoded.mid(i+1);
#ifdef QT_4_COMPAT
     n += fileName_encoded.count("%2f", Qt::CaseInsensitive);
#else
     n += fileName_encoded.contains("%2f", false);
#endif
  }
  int i = len;
  do {
    i = path.findRev( '/', i - 1 );
  }
  while (--n && (i > 0));

  // If ( i == -1 ) => the first character is not a '/'
  // So it's some URL like file:blah.tgz, return the whole path
  if ( i == -1 ) {
    if ( len == (int)path.length() )
      fname = path;
    else
      // Might get here if _strip_trailing_slash is true
      fname = path.left( len );
  }
  else
  {
     fname = path.mid( i + 1, len - i - 1 ); // TO CHECK
  }
  return fname;
}

void KURL::addPath( const QString& _txt )
{
  if (hasSubURL())
  {
     KURL::List lst = split( *this );
     KURL &u = lst.last();
     u.addPath(_txt);
     *this = join( lst );
     return;
  }
  
  m_strPath_encoded = QString::null;

  if ( _txt.isEmpty() )
    return;

  int i = 0;
  int len = m_strPath.length();
  // NB: avoid three '/' when building a new path from nothing
  if ( len == 0 ) {
    while( _txt[i] == '/' ) ++i;
  }
  // Add the trailing '/' if it is missing
  else if ( _txt[0] != '/' && ( len == 0 || m_strPath[ len - 1 ] != '/' ) )
    m_strPath += "/";

  // No double '/' characters
  i = 0;
  if ( len != 0 && m_strPath[ len - 1 ] == '/' )
  {
    while( _txt[i] == '/' )
      ++i;
  }

  m_strPath += _txt.mid( i );
}

QString KURL::directory( bool _strip_trailing_slash_from_result,
                         bool _ignore_trailing_slash_in_path ) const
{
  QString result = m_strPath_encoded.isEmpty() ? m_strPath : m_strPath_encoded;
  if ( _ignore_trailing_slash_in_path )
    result = trailingSlash( -1, result );

  if ( result.isEmpty() || result == "/" )
    return result;

  int i = result.findRev( "/" );
  // If ( i == -1 ) => the first character is not a '/'
  // So it's some URL like file:blah.tgz, with no path
  if ( i == -1 )
    return QString::null;

  if ( i == 0 )
  {
    result = "/";
    return result;
  }

  if ( _strip_trailing_slash_from_result )
    result = result.left( i );
  else
    result = result.left( i + 1 );

  if (!m_strPath_encoded.isEmpty())
    result = decode(result);

  return result;
}


bool KURL::cd( const QString& _dir )
{
  if ( _dir.isEmpty() || m_bIsMalformed )
    return false;

  if (hasSubURL())
  {
     KURL::List lst = split( *this );
     KURL &u = lst.last();
     u.cd(_dir);
     *this = join( lst );
     return true;
  }

  // absolute path ?
  if ( _dir[0] == '/' )
  {
    m_strPath_encoded = QString::null;
    m_strPath = _dir;
    setHTMLRef( QString::null );
    m_strQuery_encoded = QString::null;
    return true;
  }

  // Users home directory on the local disk ?
  if ( ( _dir[0] == '~' ) && ( m_strProtocol == fileProt ))
  {
    m_strPath_encoded = QString::null;
    m_strPath = QDir::homeDirPath();
    m_strPath += "/";
    m_strPath += _dir.right(m_strPath.length() - 1);
    setHTMLRef( QString::null );
    m_strQuery_encoded = QString::null;
    return true;
  }

  // relative path
  // we always work on the past of the first url.
  // Sub URLs are not touched.

  // append '/' if necessary
  QString p = path(1);
  p += _dir;
  p = cleanpath( p );
  setPath( p );

  setHTMLRef( QString::null );
  m_strQuery_encoded = QString::null;

  return true;
}

KURL KURL::upURL( ) const
{
  if (!query().isEmpty())
  {
     KURL u(*this);
     u.setQuery(QString::null);
     return u;
  };

  if (!hasSubURL())
  {
     KURL u(*this);
     u.cd("../");
     return u;
  }

  // We have a subURL.
  KURL::List lst = split( *this );
  if (lst.isEmpty())
      return KURL(); // Huh?
  while (true)
  {
     KURL &u = lst.last();
     QString old = u.path();
     u.cd("../");
     if (u.path() != old)
         break; // Finshed.
     if (lst.count() == 1)
         break; // Finished.
     lst.remove(lst.fromLast());
  }
  return join( lst );
}

QString KURL::htmlRef() const
{
  if ( !hasSubURL() )
  {
    return decode( ref() );
  }

  List lst = split( *this );
  return decode( (*lst.begin()).ref() );
}

QString KURL::encodedHtmlRef() const
{
  if ( !hasSubURL() )
  {
    return ref();
  }

  List lst = split( *this );
  return (*lst.begin()).ref();
}

void KURL::setHTMLRef( const QString& _ref )
{
  if ( !hasSubURL() )
  {
    m_strRef_encoded = encode( _ref, true, 0 /*?*/);
    return;
  }

  List lst = split( *this );

  (*lst.begin()).setRef( encode( _ref, true, 0 /*?*/) );

  *this = join( lst );
}

bool KURL::hasHTMLRef() const
{
  if ( !hasSubURL() )
  {
    return hasRef();
  }

  List lst = split( *this );
  return (*lst.begin()).hasRef();
}

void
KURL::setProtocol( const QString& _txt )
{
   m_strProtocol = _txt;
   m_bIsMalformed = false;
}

void
KURL::setUser( const QString& _txt )
{
   m_strUser = _txt;
}

void
KURL::setPass( const QString& _txt )
{
   m_strPass = _txt;
}

void
KURL::setHost( const QString& _txt )
{
#ifndef KDE_QT_ONLY
   m_strHost = KIDNA::toUnicode(_txt);
   if (m_strHost.isEmpty())
      m_strHost = _txt.lower(); // Probably an invalid hostname, but...
#else
   m_strHost = _txt.lower();
#endif
}

void
KURL::setPort( unsigned short int _p )
{
   m_iPort = _p;
}

void KURL::setPath( const QString & path )
{
  if (isEmpty())
    m_bIsMalformed = false;
  if (m_strProtocol.isEmpty())
  {
    m_strProtocol = fileProt;
  }
  m_strPath = path;
  m_strPath_encoded = QString::null;
}

void KURL::setDirectory( const QString &dir)
{
//US this has to be fixed. endsWith is not available in my distribution
//US  if ( dir.endsWith("/"))
//US     setPath(dir);
//US  else
     setPath(dir+"/");
}

void KURL::setQuery( const QString &_txt, int encoding_hint)
{
   if (!_txt.length())
   {
      m_strQuery_encoded = _txt;
      return;
   }
   if (_txt[0] =='?')
      m_strQuery_encoded = _txt.mid(1);
   else
      m_strQuery_encoded = _txt;

   int l = m_strQuery_encoded.length();
   int i = 0;
   QString result;
   while (i < l)
   {
      int s = i;
      // Re-encode. Break encoded string up according to the reserved
      // characters '&:;=/?' and re-encode part by part.
      while(i < l)
      {
         char c = m_strQuery_encoded[i].latin1();
         if ((c == '&') || (c == ':') || (c == ';') ||
             (c == '=') || (c == '/') || (c == '?'))
            break;
         i++;
      }
      if (i > s)
      {
         QString tmp = m_strQuery_encoded.mid(s, i-s);
         QString newTmp;
         decode( tmp, newTmp, tmp, encoding_hint, false );
         result += tmp;
      }
      if (i < l)
      {
         result += m_strQuery_encoded[i];
         i++;
      }
   }
   m_strQuery_encoded = result;
}

QString KURL::query() const
{
    if (m_strQuery_encoded.isNull())
        return QString::null;
    return '?'+m_strQuery_encoded;
}

QString KURL::decode_string(const QString &str, int encoding_hint)
{
   return decode(str, encoding_hint);
}

QString KURL::encode_string(const QString &str, int encoding_hint)
{
   return encode(str, false, encoding_hint);
}

QString KURL::encode_string_no_slash(const QString &str, int encoding_hint)
{
   return encode(str, true, encoding_hint);
}

bool urlcmp( const QString& _url1, const QString& _url2 )
{
  // Both empty ?
  if ( _url1.isEmpty() && _url2.isEmpty() )
    return true;
  // Only one empty ?
  if ( _url1.isEmpty() || _url2.isEmpty() )
    return false;

  KURL::List list1 = KURL::split( _url1 );
  KURL::List list2 = KURL::split( _url2 );

  // Malformed ?
  if ( list1.isEmpty() || list2.isEmpty() )
    return false;

  return ( list1 == list2 );
}

bool urlcmp( const QString& _url1, const QString& _url2, bool _ignore_trailing, bool _ignore_ref )
{
  // Both empty ?
  if ( _url1.isEmpty() && _url2.isEmpty() )
    return true;
  // Only one empty ?
  if ( _url1.isEmpty() || _url2.isEmpty() )
    return false;

  KURL::List list1 = KURL::split( _url1 );
  KURL::List list2 = KURL::split( _url2 );

  // Malformed ?
  if ( list1.isEmpty() || list2.isEmpty() )
    return false;

  unsigned int size = list1.count();
  if ( list2.count() != size )
    return false;

  if ( _ignore_ref )
  {
    (*list1.begin()).setRef(QString::null);
    (*list2.begin()).setRef(QString::null);
  }

  KURL::List::Iterator it1 = list1.begin();
  KURL::List::Iterator it2 = list2.begin();
  for( ; it1 != list1.end() ; ++it1, ++it2 )
    if ( !(*it1).equals( *it2, _ignore_trailing ) )
      return false;

  return true;
}
/*US we do not need this functions

QMap< QString, QString > KURL::queryItems( int options ) const {
  return queryItems(options, 0);
}

QMap< QString, QString > KURL::queryItems( int options, int encoding_hint ) const {
  if ( m_strQuery_encoded.isEmpty() )
    return QMap<QString,QString>();

  QMap< QString, QString > result;
  QStringList items = QStringList::split( '&', m_strQuery_encoded );
  for ( QStringList::const_iterator it = items.begin() ; it != items.end() ; ++it ) {
    int equal_pos = (*it).find( '=' );
    if ( equal_pos > 0 ) { // = is not the first char...
      QString name = (*it).left( equal_pos );
      if ( options & CaseInsensitiveKeys )
	name = name.lower();
      QString value = (*it).mid( equal_pos + 1 );
      if ( value.isEmpty() )
	result.insert( name, QString::fromLatin1("") );
      else {
	// ### why is decoding name not neccessary?
	value.replace( '+', ' ' ); // + in queries means space
	result.insert( name, decode_string( value, encoding_hint ) );
      }
    } else if ( equal_pos < 0 ) { // no =
      QString name = (*it);
      if ( options & CaseInsensitiveKeys )
	name = name.lower();
      result.insert( name, QString::null );
    }
  }

  return result;
}

QString KURL::queryItem( const QString& _item ) const
{
  return queryItem( _item, 0 );
}

QString KURL::queryItem( const QString& _item, int encoding_hint ) const
{
  QString item = _item + '=';
  if ( m_strQuery_encoded.length() <= 1 )
    return QString::null;

  QStringList items = QStringList::split( '&', m_strQuery_encoded );
  unsigned int _len = item.length();
  for ( QStringList::ConstIterator it = items.begin(); it != items.end(); ++it )
  {
    if ( (*it).startsWith( item ) )
    {
      if ( (*it).length() > _len )
      {
        QString str = (*it).mid( _len );
        str.replace( '+', ' ' ); // + in queries means space.
        return decode_string( str, encoding_hint );
      }
      else // empty value
        return QString::fromLatin1("");
    }
  }

  return QString::null;
}
US we do not need this functions*/

void KURL::removeQueryItem( const QString& _item )
{
  QString item = _item + '=';
  if ( m_strQuery_encoded.length() <= 1 )
    return;

  QStringList items = QStringList::split( '&', m_strQuery_encoded );
  for ( QStringList::Iterator it = items.begin(); it != items.end(); )
  {
    if ( (*it).startsWith( item ) || (*it == _item) )
    {
      QStringList::Iterator deleteIt = it;
      ++it;
      items.remove(deleteIt);
    }
    else
    {
       ++it;
    }
  }
  m_strQuery_encoded = items.join( "&" );
}

void KURL::addQueryItem( const QString& _item, const QString& _value, int encoding_hint )
{
  QString item = _item + '=';
  QString value = encode( _value, true, encoding_hint );

  if (!m_strQuery_encoded.isEmpty())
     m_strQuery_encoded += '&';
  m_strQuery_encoded += item + value;
}

// static
KURL KURL::fromPathOrURL( const QString& text )
{
    if ( text.isEmpty() )
        return KURL();
    
    KURL url;
    if ( text[0] == '/' )
        url.setPath( text );
    else
        url = text;

    return url;
}
