#ifndef MINIKDE_KDEBUG_H
#define MINIKDE_KDEBUG_H

#include <stdio.h>

#include <qstring.h>
#ifdef QT_4_COMPAT
#include <qt4compat.h>
#endif
class kdbgstream;
typedef kdbgstream & (*KDBGFUNC)(kdbgstream &); // manipulator function

class kdbgstream {
 public:
    kdbgstream(unsigned int _area, unsigned int _level, bool _print = true) :
      area(_area), level(_level),  print(  _print ) { print = false; }
    /*   kdbgstream(const char * initialString, unsigned int _area, unsigned int _level, bool _print = false) :
         output(QString::fromLatin1(initialString)), area(_area), level(_level),  print(_print) { print = false; }*/
    ~kdbgstream()
    {
        // if (!output.isEmpty()) {
        // fprintf(stderr,"ASSERT: debug output not ended with \\n\n");
        //*this << "\n";
        //}
    }
   kdbgstream &operator<<(bool)  {

	return *this;
    }
    kdbgstream &operator<<(short)  {

	return *this;
    }
    kdbgstream &operator<<(unsigned short) {
     
        return *this;
    }
    kdbgstream &operator<<(char)  {

	return *this;
    }
    kdbgstream &operator<<(unsigned char) {
       
        return *this;
    }

    kdbgstream &operator<<(int)  {

	return *this;
    }
    kdbgstream &operator<<(unsigned int) {
      
        return *this;
    }
    kdbgstream &operator<<(long) {
        return *this;
    }
    kdbgstream &operator<<(unsigned long) {
        return *this;
    }
    kdbgstream &operator<<(const QString&) {
	return *this;
    }
    kdbgstream &operator<<(const char*) {
	return *this;
    }
    kdbgstream &operator<<(const QCString&) {
      return *this;
    }
    kdbgstream& operator<<(KDBGFUNC f) {
	return (*f)(*this);
    }
    kdbgstream& operator<<(double) {
	if (!print) return *this;
      return *this;
    }
    void flush() {
	return;
    }
 private:
    QString output;
    unsigned int area, level;
    bool print;
};

inline kdbgstream &endl( kdbgstream &s) { s << "\n"; return s; }

inline kdbgstream kdDebug(int area = 0) { return kdbgstream(area, 0); }
inline kdbgstream kdWarning(int area = 0) { return kdbgstream(area, 0); }
inline kdbgstream kdError(int area = 0) { return kdbgstream(area, 0); }

#endif
