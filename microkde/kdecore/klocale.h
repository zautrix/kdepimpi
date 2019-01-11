#ifndef MINIKDE_KLOCALE_H
#define MINIKDE_KLOCALE_H

#include <qstring.h>
#include <qstringlist.h>
#include <qdatetime.h>
#include <qdict.h>

#ifndef I18N_NOOP
#define I18N_NOOP(x) (x)
#endif

class KCalendarSystem;
void setLocaleDict( QDict<QString> * dict );
QString i18n(const char *text);
QString i18n(const char *hint, const char *text);
QString i18n(const char *text1, const char *textn, int num);

// Qt3's uic generates i18n( "msg", "comment" ) calls which conflict
// with our i18n method. we use uic -tr tr2i18n to redirect
// to the right i18n() function
inline QString tr2i18n(const char* message, const char* =0) {
  return i18n( message);
}

class KLocale
{
  public:
    KLocale();

    QString formatNumber(double num, int precision = -1) const;
    QString formatNumber(const QString &numStr) const;
    double readNumber(const QString &numStr, bool * ok = 0) const;
    
    QString decimalSymbol() const;
    QString thousandsSeparator() const;
    QString positiveSign() const;
    QString negativeSign() const;


    QString translate( const char *index ) const;
    QString translate( const char *index, const char *fallback) const;

    enum IntDateFormat { Undefined=-1, Default=0, Format1=1, ISODate=2, Userdefined=3 };
    
    QString formatDate(const QDate &pDate, bool shortFormat = false, IntDateFormat intIntDateFormat = Undefined) const;
    QString formatTime(const QTime &pTime, bool includeSecs = false, IntDateFormat intIntDateFormat = Undefined) const;
    QString formatDateTime(const QDateTime &pDateTime, IntDateFormat intIntDateFormat = Undefined) const;
    QString formatDateTime(const QDateTime &pDateTime,
                           bool shortFormat,
                           bool includeSecs = false, IntDateFormat intIntDateFormat = Undefined) const;

    QDate readDate(const QString &str, bool* ok = 0) const;
    QDate readDate( const QString &intstr, const QString &fmt, bool* ok = 0) const;
    QTime readTime(const QString &str, bool* ok = 0) const;
    QDate readDate(const QString &intstr, IntDateFormat intIntDateFormat, bool* ok) const;

    QDateTime readDateTime(const QString &intstr, IntDateFormat intIntDateFormat, bool* ok) const;

    bool use12Clock() const;
    bool weekStartsMonday() const;
    int weekStartDay() const;
    
    QString weekDayName(int,bool=false) const;
    QString monthName(int,bool=false) const;

    QString country() const;
    int weekNum ( const QDate & );
    QString dateFormat(IntDateFormat intIntDateFormat = Undefined) const;
    QString dateFormatShort(IntDateFormat intIntDateFormat = Undefined) const;
    QString timeFormat(IntDateFormat intIntDateFormat = Undefined) const;

    void insertCatalogue ( const QString & );

    KCalendarSystem *calendar();
    void setHore24Format ( bool );
    void setWeekStartMonday( bool );
    void setIntDateFormat( IntDateFormat );
    void setIntTimeFormat( IntDateFormat );
    IntDateFormat getIntDateFormat( );
    IntDateFormat getIntTimeFormat( );
    void setLanguage( int );
    int language();
    void setDateFormat( QString );
    void setDateFormatShort( QString );

  QString m_decimalSymbol;
  QString m_thousandsSeparator;
  QString m_currencySymbol;
  QString m_monetaryDecimalSymbol;
  QString m_monetaryThousandsSeparator;
  QString m_positiveSign;
  QString m_negativeSign;

    int timezoneOffset( QString );
    QStringList timeZoneList() const;
    void setDaylightSaving( bool, int , int );
    int localTimeOffset(const QDateTime &);
    void setTimezone( const QString &timeZone , bool oddTZ);
  private:
    QTime readTime(const QString &str, bool seconds, bool *ok) const;
    QDate readDate(const QString &str, bool shortFormat, bool *ok) const;
    KCalendarSystem *mCalendarSystem;
    bool mWeekStartsMonday;
    bool mHourF24Format;
    IntDateFormat mIntDateFormat;
    IntDateFormat mIntTimeFormat;
    int mLanguage;
    QString mDateFormat;
    QString mDateFormatShort;
    QStringList mTimeZoneList;
    bool daylightEnabled;
    int mDaylightTZoffset;
    int mNondaylightTZoffset;
    bool mSouthDaylight;
    int daylightStart, daylightEnd, mTimeZoneOffset;
};

#endif
