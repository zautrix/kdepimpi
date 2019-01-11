#include <qregexp.h>
#include <qapplication.h>

#include "kdebug.h"
#include "kcalendarsystemgregorian.h"

#include "klocale.h"

#include <qstringlist.h>

//#define COLLECT_TRANSLATION


QDict<QString> *mLocaleDict = 0;
void setLocaleDict( QDict<QString> * dict )
{
    mLocaleDict = dict;

}

#ifdef COLLECT_TRANSLATION 

QStringList missingTrans;
QStringList existingTrans1;
QStringList existingTrans2;

void addMissing(const char *text) 
{

    QString mis ( text );
    if ( !missingTrans.contains( mis  ) )
        missingTrans.append(mis);

}
void addExist(const char *text,QString trans ) 
{
    //return;
    QString mis ( text );
    if ( !existingTrans1.contains( mis  ) ) {
        existingTrans1.append(mis);
        existingTrans2.append(trans);

    }

}

#include <qfile.h>
#include <qtextstream.h>
#include <qtextcodec.h>
#endif
void dumpMissing()
{
#ifdef COLLECT_TRANSLATION 
    QString fileName = "/tmp/usernewtrans.txt";
    QFile file( fileName );
    if (!file.open( IO_WriteOnly ) ) {
        return ;
    } 
    QTextStream ts( &file );
    ts.setCodec( QTextCodec::codecForName("utf8") );

    int i;
    for ( i = 0; i< missingTrans.count(); ++i ) {

        QString text = missingTrans[i].replace( QRegExp("\n"),"\\n" );
        ts << "{ \""<<text<< "\",\""<< text <<"\" },\n";

    }
    file.close();
    {
    QString fileName = "/tmp/usertrans.txt";
    QFile file( fileName );
    if (!file.open( IO_WriteOnly ) ) {
        return ;
    } 
    QTextStream ts( &file );
    ts.setCodec( QTextCodec::codecForName("utf8") );

    int i;
    for ( i = 0; i< existingTrans1.count(); ++i ) {

        QString text = existingTrans1[i].replace( QRegExp("\n"),"\\n" );
        QString text2 = existingTrans2[i].replace( QRegExp("\n"),"\\n" );
        ts << "{ \""<<text<< "\",\""<< text2 <<"\" },\n";

    }
    file.close();
    }
#endif
}
QString i18n(const char *text)
{
    if ( ! mLocaleDict ) {
#ifdef COLLECT_TRANSLATION 
        addMissing( text );
#endif
        return QString( text );
    }
    else {
        QString* ret  =  mLocaleDict->find(QString(text)) ;
        if ( ret == 0 ) {
#ifdef COLLECT_TRANSLATION 
            addMissing( text );
#endif
            return QString( text );
        }
        else {
            if ( (*ret).isEmpty() ) {
#ifdef COLLECT_TRANSLATION 
                addMissing( text );
#endif
                return QString( text );
            }
            else {
#ifdef COLLECT_TRANSLATION 
                addExist( text, *ret );
#endif
                return (*ret);
            }
        }
    }

}

QString i18n(const char *,const char *text)
{
  return i18n( text );
}

QString i18n(const char *text1, const char *textn, int num)
{
  if ( num == 1 ) return i18n( text1 );
  else {
    QString text = i18n( textn );
    int pos = text.find( "%n" );
    if ( pos >= 0 ) text.replace( pos, 2, QString::number( num ) );
    return text;
  }
}

inline void put_it_in( QChar *buffer, uint& index, const QString &s )
{
  for ( uint l = 0; l < s.length(); l++ )
    buffer[index++] = s.at( l );
}

inline void put_it_in( QChar *buffer, uint& index, int number )
{
  buffer[index++] = number / 10 + '0';
  buffer[index++] = number % 10 + '0';
}

static int readInt(const QString &str, uint &pos)
{
  if (!str.at(pos).isDigit()) return -1;
  int result = 0;
  for (; str.length() > pos && str.at(pos).isDigit(); pos++)
    {
      result *= 10;
      result += str.at(pos).digitValue();
    }

  return result;
}

KLocale::KLocale() : mCalendarSystem( 0 )
{

    m_decimalSymbol = ".";
    m_positiveSign = "";
    m_negativeSign = "-";
    m_thousandsSeparator = ",";




    mWeekStartsMonday = true;
    mHourF24Format = true;
    mIntDateFormat = Default;
    mIntTimeFormat = Default;
    mLanguage = 0;  
    mDateFormat = "%a %Y %b %d";
    mDateFormatShort = "%Y-%m-%d"; 
    mTimeZoneList <<  ("-11:00 US/Samoa")
            <<  ("-10:00 US/Hawaii")
            <<  ("-09:00 US/Alaska")
            <<  ("-08:00 US/Pacific")
            <<  ("-07:00 US/Mountain")
            <<  ("-06:00 US/Central")
            <<  ("-05:00 US/Eastern")
            <<  ("-04:00 Brazil/West")
            <<  ("-03:00 Brazil/East")
            <<  ("-02:00 Brazil/DeNoronha")
            <<  ("-01:00 Atlantic/Azores")
            <<  (" 00:00 Europe/London(UTC)")
            <<  ("+01:00 Europe/Oslo(CET)")
            <<  ("+02:00 Europe/Helsinki")
            <<  ("+03:00 Europe/Moscow")
            <<  ("+04:00 Indian/Mauritius")
            <<  ("+05:00 Indian/Maldives")
            <<  ("+06:00 Indian/Chagos")
            <<  ("+07:00 Asia/Bangkok")
            <<  ("+08:00 Asia/Hongkong")
            <<  ("+09:00 Asia/Tokyo")
            <<  ("+10:00 Asia/Vladivostok")
            <<  ("+11:00 Asia/Magadan")
            <<  ("+12:00 Asia/Kamchatka")
            //  <<  (" xx:xx User defined offset")
            << i18n (" Local Time");
    mSouthDaylight = false;
    mTimeZoneOffset = 0;
    daylightEnabled = false;
}

void KLocale::setDateFormat( QString s )
{
    mDateFormat = s;
}

void KLocale::setDateFormatShort( QString s )
{
    mDateFormatShort = s;
}

void KLocale::setHore24Format ( bool b )
{
    mHourF24Format = b;
}
void KLocale::setWeekStartMonday( bool b )
{
    mWeekStartsMonday = b;
}

KLocale::IntDateFormat KLocale::getIntDateFormat( )
{
    return mIntDateFormat;

}
void KLocale::setIntDateFormat( KLocale::IntDateFormat i )
{
    mIntDateFormat = i;
}
KLocale::IntDateFormat KLocale::getIntTimeFormat( )
{
    return mIntTimeFormat;

}
void KLocale::setIntTimeFormat( KLocale::IntDateFormat i )
{
    mIntTimeFormat = i;
}

void KLocale::setLanguage( int i )
{
    mLanguage = i;
}
int  KLocale::language( )
{
    return mLanguage;
}
QString KLocale::translate( const char *index ) const
{
  return i18n( index );
}

QString KLocale::translate( const char *, const char *fallback) const
{
  return i18n( fallback );
}

int KLocale::weekNum ( const QDate & date )
{
    QDate seda = date;
    int weekNum = 0;
    int dayofweek = seda.dayOfWeek(); // 1... 7 Mo .. So. Do = 4
    int daystoprevthursday = (dayofweek + 3) % 7 ;
    int dayofyear = seda.dayOfYear();
    int prevThursday = dayofyear - ( daystoprevthursday  );
    int subweeknum = 0;
    if ( prevThursday < 1 ) {
        seda = seda.addDays( - daystoprevthursday );
        dayofyear = seda.dayOfYear();
        prevThursday = dayofyear;
        subweeknum = prevThursday / 7;
        if ( prevThursday % 7 != 0 )
            ++subweeknum; 
    } else {
        if ( dayofyear >= 360 ) { //maybe week 1
            seda = seda.addDays( 7 - daystoprevthursday ); 
            dayofyear = seda.dayOfYear();
            if ( dayofyear < 360 && ( dayofweek < 4 || dayofweek == 7 && !mWeekStartsMonday ) )
                return 1;
        }
    }
    if ( ! weekNum ) {
        weekNum = prevThursday / 7;
        if ( prevThursday % 7 != 0 )
            ++weekNum; 
        if (  dayofweek < 4  )
            ++weekNum;
        else if ( dayofweek == 7 && !mWeekStartsMonday )
            ++weekNum;
        if ( weekNum > subweeknum )
            weekNum -= subweeknum;
    }
    return weekNum;
}
QString KLocale::formatTime(const QTime &pTime, bool includeSecs, IntDateFormat intIntDateFormat) const
{
    const QString rst = timeFormat(intIntDateFormat);

    // only "pm/am" here can grow, the rest shrinks, but
    // I'm rather safe than sorry
    QChar *buffer = new QChar[rst.length() * 3 / 2 + 30];

    uint index = 0;
    bool escape = false;
    int number = 0;

    for ( uint format_index = 0; format_index < rst.length(); format_index++ )
        {
            if ( !escape )
                {
                    if ( rst.at( format_index ).unicode() == '%' )
                        escape = true;
                    else
                        buffer[index++] = rst.at( format_index );
                }
            else
                {
                    switch ( rst.at( format_index ).unicode() )
                        {
                        case '%':
                            buffer[index++] = '%';
                            break;
                        case 'H':
                            put_it_in( buffer, index, pTime.hour() );
                            break;
                        case 'I':
                            put_it_in( buffer, index, ( pTime.hour() + 11) % 12 + 1 );
                            break;
                        case 'M':
                            put_it_in( buffer, index, pTime.minute() );
                            break;
                        case 'S':
                            if (includeSecs)
                                put_it_in( buffer, index, pTime.second() );
                            else
                                {
                                    // we remove the seperator sign before the seconds and
                                    // assume that works everywhere
                                    --index;
                                    break;
                                }
                            break;
                        case 'k':
                            number = pTime.hour();
                        case 'l':
                            // to share the code
                            if ( rst.at( format_index ).unicode() == 'l' )
                                number = (pTime.hour() + 11) % 12 + 1;
                            if ( number / 10 )
                                buffer[index++] = number / 10 + '0';
                            buffer[index++] = number % 10 + '0';
                            break;
                        case 'p':
                            {
                                QString s;
                                if ( pTime.hour() >= 12 )
                                    put_it_in( buffer, index, i18n("pm") );
                                else
                                    put_it_in( buffer, index, i18n("am") );
                                break;
                            }
                        default:
                            buffer[index++] = rst.at( format_index );
                            break;
                        }
                    escape = false;
                }
        }
    QString ret( buffer, index );
    delete [] buffer;
    return ret;
}    

QString KLocale::formatDate(const QDate &pDate, bool shortFormat, IntDateFormat intIntDateFormat) const
{
  const QString rst = shortFormat?dateFormatShort(intIntDateFormat):dateFormat(intIntDateFormat);

  // I'm rather safe than sorry
  QChar *buffer = new QChar[rst.length() * 3 / 2 + 50];

  unsigned int index = 0;
  bool escape = false;
  int number = 0;

  for ( uint format_index = 0; format_index < rst.length(); ++format_index )
    {
      if ( !escape )
	{
	  if ( rst.at( format_index ).unicode() == '%' )
	    escape = true;
	  else
	    buffer[index++] = rst.at( format_index );
	}
      else
	{
	  switch ( rst.at( format_index ).unicode() )
	    {
	    case '%':
	      buffer[index++] = '%';
	      break;
	    case 'Y':
	      put_it_in( buffer, index, pDate.year() / 100 );
	    case 'y':
	      put_it_in( buffer, index, pDate.year() % 100 );
	      break;
	    case 'n':
	      number = pDate.month();
	    case 'e':
	      // to share the code
	      if ( rst.at( format_index ).unicode() == 'e' )
		number = pDate.day();
	      if ( number / 10 )
		buffer[index++] = number / 10 + '0';
	      buffer[index++] = number % 10 + '0';
	      break;
	    case 'm':
	      put_it_in( buffer, index, pDate.month() );
	      break;
	    case 'b':
	      put_it_in( buffer, index, monthName(pDate.month(), true) );
	      break;
	    case 'B':
	      put_it_in( buffer, index, monthName(pDate.month(), false) );
	      break;
	    case 'd':
	      put_it_in( buffer, index, pDate.day() );
	      break;
	    case 'a':
	      put_it_in( buffer, index, weekDayName(pDate.dayOfWeek(), true) );
	      break;
	    case 'A':
	      put_it_in( buffer, index, weekDayName(pDate.dayOfWeek(), false) );
	      break;
	    default:
	      buffer[index++] = rst.at( format_index );
	      break;
	    }
	  escape = false;
	}
    }
  QString ret( buffer, index );
  delete [] buffer;
  return ret;
}

QString KLocale::formatDateTime(const QDateTime &pDateTime,
				bool shortFormat,
				bool includeSeconds,
				IntDateFormat intIntDateFormat) const
{
  QString format("%1 %2");

  if ( intIntDateFormat == Default )
    format = "%1 %2";
  else if ( intIntDateFormat == Format1 )
    format = "%1 %2";
  else  if ( intIntDateFormat == ISODate )
    format = "%1T%2";

  QString res = format.arg(formatDate( pDateTime.date(), shortFormat, intIntDateFormat ))
               .arg(formatTime( pDateTime.time(), includeSeconds , intIntDateFormat ));

  //qDebug("KLocale::formatDateTime transformed %s, into %s", pDateTime.toString().latin1(), res.latin1() );

  return res;
}

QString KLocale::formatDateTime(const QDateTime &pDateTime, IntDateFormat intIntDateFormat) const
{
  return formatDateTime(pDateTime, true, true, intIntDateFormat);
}

QDate KLocale::readDate(const QString &intstr, bool* ok) const
{
  QDate date;
  date = readDate(intstr, true, ok);
  if (date.isValid()) return date;
  return readDate(intstr, false, ok);
}

QDate KLocale::readDate(const QString &intstr, bool shortFormat, bool* ok) const
{
  QString fmt = (shortFormat ? dateFormatShort() : dateFormat()).simplifyWhiteSpace();
  return readDate( intstr, fmt, ok );
}

QDate KLocale::readDate(const QString &intstr, const QString &fmt, bool* ok) const
{
  //kdDebug(173) << "KLocale::readDate intstr=" << intstr << " fmt=" << fmt << endl;
  QString str = intstr.simplifyWhiteSpace().lower();
  int day = -1, month = -1;
  // allow the year to be omitted if not in the format
  int year = QDate::currentDate().year();
  uint strpos = 0;
  uint fmtpos = 0;

  while (fmt.length() > fmtpos || str.length() > strpos)
    {
      if ( !(fmt.length() > fmtpos && str.length() > strpos) )
        goto error;

      QChar c = fmt.at(fmtpos++);

      if (c != '%') {
        if (c.isSpace())
          strpos++;
        else if (c != str.at(strpos++))
          goto error;
        continue;
      }

      // remove space at the begining
      if (str.length() > strpos && str.at(strpos).isSpace())
        strpos++;

      c = fmt.at(fmtpos++);
      switch (c.latin1())
      {
	case 'a':
	case 'A':
	  // this will just be ignored
	  { // Cristian Tache: porting to Win: Block added because of "j" redefinition
  	  for (int j = 1; j < 8; j++) {
  	    QString s = weekDayName(j, c == 'a').lower();
  	    int len = s.length();
  	    if (str.mid(strpos, len) == s)
  	      strpos += len;
  	  }
  	  break;
  	}
	case 'b':
	case 'B':
	  { // Cristian Tache: porting to Win: Block added because of "j" redefinition
  	  for (int j = 1; j < 13; j++) {
  	    QString s = monthName(j, c == 'b').lower();
  	    int len = s.length();
  	    if (str.mid(strpos, len) == s) {
  	      month = j;
  	      strpos += len;
  	    }
  	  }
  	  break;
  	}
	case 'd':
	case 'e':
	  day = readInt(str, strpos);
	  if (day < 1 || day > 31)
	    goto error;

	  break;

	case 'n':
	case 'm':
	  month = readInt(str, strpos);
	  if (month < 1 || month > 12)
	    goto error;

	  break;

	case 'Y':
	case 'y':
	  year = readInt(str, strpos);
	  if (year < 0)
	    goto error;
	  // Qt treats a year in the range 0-100 as 1900-1999.
	  // It is nicer for the user if we treat 0-68 as 2000-2068
	  if (year < 69)
	    year += 2000;
	  else if (c == 'y')
	    year += 1900;

	  break;
	}
    }
  //kdDebug(173) << "KLocale::readDate day=" << day << " month=" << month << " year=" << year << endl;
  if ( year != -1 && month != -1 && day != -1 )
  {
    if (ok) *ok = true;
    return QDate(year, month, day);
  }
 error:
  if (ok) *ok = false;
  return QDate(); // invalid date
}

QTime KLocale::readTime(const QString &intstr, bool *ok) const
{
  QTime _time;
  _time = readTime(intstr, false, ok);
  if (_time.isValid()) return _time;
  return readTime(intstr, true, ok);
}

QTime KLocale::readTime(const QString &intstr, bool seconds, bool *ok) const
{
  QString str = intstr.simplifyWhiteSpace().lower();
  QString Format = timeFormat().simplifyWhiteSpace();
  if (!seconds)
    Format.replace(QRegExp(QString::fromLatin1(".%S")), QString::null);

  int hour = -1, minute = -1, second = seconds ? -1 : 0; // don't require seconds

  bool g_12h = false;
  bool pm = false;
  uint strpos = 0;
  uint Formatpos = 0;

  while (Format.length() > Formatpos || str.length() > strpos)
    {
      if ( !(Format.length() > Formatpos && str.length() > strpos) ) goto error;

      QChar c = Format.at(Formatpos++);

      if (c != '%')
	{
	  if (c.isSpace())
	    strpos++;
	  else if (c != str.at(strpos++))
	    goto error;
	  continue;
	}

      // remove space at the begining
      if (str.length() > strpos && str.at(strpos).isSpace())
	strpos++;

      c = Format.at(Formatpos++);
      switch (c.latin1())
	{
	case 'p':
	  {
	    QString s;
	    s = i18n("pm").lower();
	    int len = s.length();
	    if (str.mid(strpos, len) == s)
	      {
		pm = true;
		strpos += len;
	      }
	    else
	      {
		s = i18n("am").lower();
		len = s.length();
		if (str.mid(strpos, len) == s) {
		  pm = false;
		  strpos += len;
		}
		else
		  goto error;
	      }
	  }
	  break;

	case 'k':
	case 'H':
	  g_12h = false;
	  hour = readInt(str, strpos);
	  if (hour < 0 || hour > 23)
	    goto error;

	  break;

	case 'l':
	case 'I':
	  g_12h = true;
	  hour = readInt(str, strpos);
	  if (hour < 1 || hour > 12)
	    goto error;

	  break;

	case 'M':
	  minute = readInt(str, strpos);
	  if (minute < 0 || minute > 59)
	    goto error;

	  break;

	case 'S':
	  second = readInt(str, strpos);
	  if (second < 0 || second > 59)
	    goto error;

	  break;
	}
    }
  if (g_12h)
    {
      hour %= 12;
      if (pm) hour += 12;
    }

  if (ok) *ok = true;
  return QTime(hour, minute, second);

 error:
  if (ok) *ok = false;
  return QTime(-1, -1, -1); // return invalid date if it didn't work
  // This will be removed in the near future, since it gives a warning on stderr.
  // The presence of the bool* (since KDE-3.0) removes the need for an invalid QTime.
}

QDateTime KLocale::readDateTime(const QString &intstr, 
				IntDateFormat intIntDateFormat, 
				bool* ok) const
{
  bool ok1, ok2;

  // AT the moment we can not read any other format then ISODate
  if ( intIntDateFormat != ISODate )
    {
      qDebug("KLocale::readDateTime, only ISODate is supported.");
      return QDateTime();
    }  

  int pos = intstr.find("T");
  QString date = intstr.left(pos);
  QString time = intstr.mid(pos+1);

  QString dformat = dateFormat(intIntDateFormat);
  QString tformat = timeFormat(intIntDateFormat);

  QDate m_date = readDate(date, dformat, &ok1);
  // QTime m_time = readTime(time, tformat, &ok2);
  QTime m_time = readTime(time, true, &ok2);

  QDateTime m_dt;

  if (ok)
    {
      if ((ok1 == false) || (ok2 == false))
	*ok = false;
      else
	*ok = true;
    }

  //only set values if both operations returned true.
  if ((ok1 == true) && (ok2 == true))
    {
      m_dt.setDate(m_date);
      m_dt.setTime(m_time);
    }

  //qDebug("KLocale::readDateTime() transformed %s into %s (%s), %s (%s) : err1=%i, err2=%i", intstr.latin1(), date.latin1(), dformat.latin1(), time.latin1(), tformat.latin1(), ok1, ok2);
  return m_dt;
}

QDate KLocale::readDate(const QString &intstr, 
				IntDateFormat intIntDateFormat, 
				bool* ok) const
{
  bool ok1;

  QString dformat = dateFormat(intIntDateFormat);

  QDate m_date = readDate(intstr, dformat, &ok1);

  if (ok)
    *ok = ok1;

  //qDebug("KLocale::readDate() transformed %s into %s (%s), %s (%s) : err1=%i, err2=%i", intstr.latin1(), date.latin1(), dformat.latin1(), time.latin1(), tformat.latin1(), ok1, ok2);
  return m_date;
}


bool KLocale::use12Clock() const
{
    return !mHourF24Format ;;
}

bool KLocale::weekStartsMonday() const
{
    return mWeekStartsMonday;
}

int KLocale::weekStartDay() const
{
    if ( mWeekStartsMonday )
        return 1;
    return 7;
}

QString KLocale::weekDayName(int i,bool shortName) const
{
  if ( shortName )
    switch ( i )
      {
      case 1:  return i18n("Monday", "Mon");
      case 2:  return i18n("Tuesday", "Tue");
      case 3:  return i18n("Wednesday", "Wed");
      case 4:  return i18n("Thursday", "Thu");
      case 5:  return i18n("Friday", "Fri");
      case 6:  return i18n("Saturday", "Sat");
      case 7:  return i18n("Sunday", "Sun");
      }
  else
    switch ( i )
      {
      case 1:  return i18n("Monday");
      case 2:  return i18n("Tuesday");
      case 3:  return i18n("Wednesday");
      case 4:  return i18n("Thursday");
      case 5:  return i18n("Friday");
      case 6:  return i18n("Saturday");
      case 7:  return i18n("Sunday");
      }

  return QString::null;
}

QString KLocale::monthName(int i,bool shortName) const
{
  if ( shortName )
    switch ( i )
      {
      case 1:   return i18n("January", "Jan");
      case 2:   return i18n("February", "Feb");
      case 3:   return i18n("March", "Mar");
      case 4:   return i18n("April", "Apr");
      case 5:   return i18n("May short", "May");
      case 6:   return i18n("June", "Jun");
      case 7:   return i18n("July", "Jul");
      case 8:   return i18n("August", "Aug");
      case 9:   return i18n("September", "Sep");
      case 10:  return i18n("October", "Oct");
      case 11:  return i18n("November", "Nov");
      case 12:  return i18n("December", "Dec");
      }
  else
    switch (i)
      {
      case 1:   return i18n("January");
      case 2:   return i18n("February");
      case 3:   return i18n("March");
      case 4:   return i18n("April");
      case 5:   return i18n("May long", "May");
      case 6:   return i18n("June");
      case 7:   return i18n("July");
      case 8:   return i18n("August");
      case 9:   return i18n("September");
      case 10:  return i18n("October");
      case 11:  return i18n("November");
      case 12:  return i18n("December");
      }

  return QString::null;
}

QString KLocale::country() const
{
  return QString::null;
}

QString KLocale::dateFormat(IntDateFormat intIntDateFormat) const
{
  const IntDateFormat dformat = (intIntDateFormat == Undefined)?mIntDateFormat:intIntDateFormat;
    
  if ( dformat == ISODate )
    return "%Y-%m-%d";

    if ( QApplication::desktop()->width() < 480 ) {
        if ( dformat == Default )
            return "%a %d %b %Y";
        else  if ( dformat == Format1 )
            return "%a %b %d %Y";
    } else {
        if ( dformat == Default )
            return "%A %d %B %Y";
        else  if ( dformat == Format1 )
            return "%A %B %d %Y";

    }
    return  mDateFormat ;
}

QString KLocale::dateFormatShort(IntDateFormat intIntDateFormat) const
{
  const IntDateFormat dformat = (intIntDateFormat == Undefined)?mIntDateFormat:intIntDateFormat;
 
  if ( dformat == Default )
        return "%d.%m.%Y"; 
    else  if ( dformat == Format1 )
        return "%m.%d.%Y"; 
    else  if ( dformat == ISODate )  // = Qt::ISODate
        return "%Y-%m-%d"; 
  return mDateFormatShort ;

}


QString KLocale::timeFormat(IntDateFormat intIntTimeFormat) const
{
    const IntDateFormat tformat = (intIntTimeFormat == Undefined)?mIntTimeFormat:intIntTimeFormat;
 
    if ( tformat == Default )
        if ( mHourF24Format)
            return "%H:%M:%S";
        else
            return "%I:%M:%S%p";

    else  if ( tformat == Format1 )
        if ( mHourF24Format)
            return "%H:%M:%S";
        else
            return "%I:%M:%S%p";

    else  if ( tformat == ISODate )  // = Qt::ISODate
        if ( mHourF24Format)
            return "%H:%M:%S";
        else
            return "%I:%M:%S%p";
    // to satisfy the compiler
    return "%H:%M:%S";
}

void KLocale::insertCatalogue ( const QString & )
{
}

KCalendarSystem *KLocale::calendar()
{
  if ( !mCalendarSystem ) {
    mCalendarSystem = new KCalendarSystemGregorian;
  }
  
  return mCalendarSystem;
}

int KLocale::timezoneOffset( QString timeZone )
{
    int ret = 1001;
    int index = mTimeZoneList.findIndex( timeZone );
    if ( index < 24 ) 
       ret =  ( index-11 ) * 60 ;
    return ret;
}

QStringList KLocale::timeZoneList() const
{
    return mTimeZoneList;
}
void KLocale::setTimezone( const QString &timeZone, bool oddTZ )
{
    mTimeZoneOffset = timezoneOffset( timeZone );
    if ( oddTZ )
        mTimeZoneOffset += 30;
}

void KLocale::setDaylightSaving( bool b, int start , int end )
{
    daylightEnabled = b;
    daylightStart = start;
    daylightEnd = end;
    mSouthDaylight = (end < start);
    // qDebug("klocale daylight %d %d %d ", b,  start ,  end );
}

int KLocale::localTimeOffset( const QDateTime &dt )
{
    bool addDaylight = false;
    if ( daylightEnabled ) {
        int d_end, d_start;
        int dayofyear = dt.date().dayOfYear();
        int year = dt.date().year();
        int add = 0;
        if (  QDate::leapYear(year) )
            add = 1;
        QDate date (  year,1,1 );
        if ( daylightEnd > 59 )
            d_end = daylightEnd +add;
        else
            d_end = daylightEnd;
        if ( daylightStart > 59 )
            d_start = daylightStart +add;
        else
            d_start = daylightStart;
        QDate s_date = date.addDays( d_start -1 );
        QDate e_date = date.addDays( d_end -1 );
        int dof = s_date.dayOfWeek();
        if ( dof < 7 )
            s_date = s_date.addDays( -dof );
        dof = e_date.dayOfWeek();
        if ( dof < 7 )
            e_date = e_date.addDays( -dof );
        QTime startTime ( 3,0,0 );
        QDateTime startDt( s_date,  startTime );
        QDateTime endDt( e_date,  startTime );
        //qDebug("dayligt saving start %s end %s ",startDt.toString().latin1(),endDt.toString().latin1( ));
        if ( mSouthDaylight ) {
            if ( ! ( endDt < dt && dt < startDt) )
                addDaylight = true;  
           } else {
            if ( startDt < dt && dt < endDt )
                addDaylight = true;  
           
            
        }
    }
    int addMin = 0;
    if ( addDaylight )
        addMin = 60;
    return mTimeZoneOffset + addMin;
}
// ******************************************************************
// added LR
QString KLocale::formatNumber(double num, int precision) const
{
  bool neg = num < 0;
  if (precision == -1) precision = 2;
  QString res = QString::number(neg?-num:num, 'f', precision);
  int pos = res.find('.');
  if (pos == -1) pos = res.length();
  else res.replace(pos, 1, decimalSymbol());

  while (0 < (pos -= 3))
    res.insert(pos, thousandsSeparator()); // thousand sep

  // How can we know where we should put the sign?
  res.prepend(neg?negativeSign():positiveSign());

  return res;
}
QString KLocale::formatNumber(const QString &numStr) const
{
  return formatNumber(numStr.toDouble());
}
double KLocale::readNumber(const QString &_str, bool * ok) const
{
  QString str = _str.stripWhiteSpace();
  bool neg = str.find(negativeSign()) == 0;
  if (neg)
    str.remove( 0, negativeSign().length() );

  /* will hold the scientific notation portion of the number.
     Example, with 2.34E+23, exponentialPart == "E+23"
  */
  QString exponentialPart;
  int EPos;

  EPos = str.find('E', 0, false);

  if (EPos != -1)
  {
    exponentialPart = str.mid(EPos);
    str = str.left(EPos);
  }

  int pos = str.find(decimalSymbol());
  QString major;
  QString minor;
  if ( pos == -1 )
    major = str;
  else
    {
      major = str.left(pos);
      minor = str.mid(pos + decimalSymbol().length());
    }

  // Remove thousand separators
  int thlen = thousandsSeparator().length();
  int lastpos = 0;
  while ( ( pos = major.find( thousandsSeparator() ) ) > 0 )
  {
    // e.g. 12,,345,,678,,922 Acceptable positions (from the end) are 5, 10, 15... i.e. (3+thlen)*N
    int fromEnd = major.length() - pos;
    if ( fromEnd % (3+thlen) != 0 // Needs to be a multiple, otherwise it's an error
        || pos - lastpos > 3 // More than 3 digits between two separators -> error
        || pos == 0          // Can't start with a separator
        || (lastpos>0 && pos-lastpos!=3))   // Must have exactly 3 digits between two separators
    {
      if (ok) *ok = false;
      return 0.0;
    }

    lastpos = pos;
    major.remove( pos, thlen );
  }
  if (lastpos>0 && major.length()-lastpos!=3)   // Must have exactly 3 digits after the last separator
  {
    if (ok) *ok = false;
    return 0.0;
  }

  QString tot;
  if (neg) tot = '-';

  tot += major + '.' + minor + exponentialPart;

  return tot.toDouble(ok);
}
QString KLocale::decimalSymbol() const
{

  return m_decimalSymbol;
}

QString KLocale::thousandsSeparator() const
{

  return m_thousandsSeparator;
}
QString KLocale::positiveSign() const
{
  return m_positiveSign;
}

QString KLocale::negativeSign() const
{
  return m_negativeSign;
}
