/*
	libvcard - vCard parsing library for vCard version 3.0

	Copyright (C) 1998 Rik Hemsley rik@kde.org

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to
  deal in the Software without restriction, including without limitation the
  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
  sell copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
  AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <qregexp.h>
#include <qdatetime.h>

#include <VCardDefines.h>
#include <VCardVCardEntity.h>

using namespace VCARD;

VCardEntity::VCardEntity()
	:	Entity()
{
  cardList_.setAutoDelete( TRUE );
}

VCardEntity::VCardEntity(const VCardEntity & x)
	:	Entity(x)
{
  cardList_.setAutoDelete( TRUE );
}

VCardEntity::VCardEntity(const QCString & s)
	:	Entity(s)
{
  cardList_.setAutoDelete( TRUE );
}

	VCardEntity &
VCardEntity::operator = (VCardEntity & x)
{
	if (*this == x) return *this;

	Entity::operator = (x);
	return *this;
}

	VCardEntity &
VCardEntity::operator = (const QCString & s)
{
	Entity::operator = (s);
	return *this;
}

	bool
VCardEntity::operator == (VCardEntity & x)
{
	x.parse();
	return false;
}

VCardEntity::~VCardEntity()
{
}

	void
VCardEntity::_parse()
{
#if 0
    QTime tim;
    tim.start();
    int num  = 0;
    // old code 
	vDebug("parse");
	QCString s(strRep_);

	int i = s.find(QRegExp("BEGIN:VCARD", false));

	while (i != -1) {

		i = s.find(QRegExp("BEGIN:VCARD", false), 11);

		QCString cardStr(s.left(i));

		VCard * v = new VCard(cardStr);

		cardList_.append(v);

		v->parse();

		s.remove(0, i);
	}

#else
    // this code is up to 17 (!)  times faster
    int start = 0;
    QTime tim;
    tim.start();
    int i = 11;
    int len = strRep_.length();
    int num  = 0;
	while (i < len ) {
        while( i < len ) {
            int add = 1;
            if ( strRep_.at(i) == 'B' ) {
                if ( i+add < len && strRep_.at(i+add++) == 'E')
                if ( i+add < len && strRep_.at(i+add++) == 'G')
                if ( i+add < len && strRep_.at(i+add++) == 'I')
                if ( i+add < len && strRep_.at(i+add++) == 'N')
                if ( i+add < len && strRep_.at(i+add++) == ':')
                if ( i+add < len && strRep_.at(i+add++) == 'V')
                if ( i+add < len && strRep_.at(i+add++) == 'C')
                if ( i+add < len && strRep_.at(i+add++) == 'A')
                if ( i+add < len && strRep_.at(i+add++) == 'R')
                if ( i+add < len && strRep_.at(i+add++) == 'D')
                    if ( i+add < len && (strRep_.at(i+add) == '\r' || strRep_.at(i+add) == '\n' ))
                    break;
            }
            ++i;
        }
        if ( i <= len ) {
            ++num;
            char* dat = strRep_.data()+start;
            VCard * v = new VCard( QCString (  dat,i-start ) );
            start = i;
            cardList_.append(v);
            v->parse();
        }
        i+= 11;
	}
#endif
    //qDebug("***time %d found %d", tim.elapsed(), num);
}

	void
VCardEntity::_assemble()
{
	VCardListIterator it(cardList_);

	for (; it.current(); ++it)
		strRep_ += it.current()->asString() + "\r\n"; // One CRLF for luck.
}

	VCardList &
VCardEntity::cardList()
{
	parse();
	return cardList_;
}

	void
VCardEntity::setCardList(const VCardList & l)
{
	parse();
//US	cardList_ = l;
    VCardListIterator it(l);

    for (; it.current(); ++it) {
      VCard* v = new VCard(*it.current());
      cardList_.append(v);
    }

}

