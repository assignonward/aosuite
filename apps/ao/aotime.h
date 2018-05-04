// Assign Onward
//
// The universal clock - Unix time in seconds since epoch, multiplied by 2^64
//   - future plans include adding 32 bits of trivial quasi-random dither to break most ties

#ifndef AOTIME_H
#define AOTIME_H

#include <QObject>

// These AO constants need to be multiplied
// by 2^64 after being loaded due to compiler
// challenges handling 128 bit constants
#define AO_SECOND  1
#define AO_MINUTE (60 * AO_SECOND)
#define AO_HOUR   (60 * AO_MINUTE)
#define AO_DAY    (24 * AO_HOUR  )
#define AO_WEEK   ( 7 * AO_DAY   )

class AOTime : public QObject
{
    Q_OBJECT
public:
       explicit AOTime( __int128 tm = 0, QObject *p = nullptr);
                AOTime( const AOTime &tm, QObject *p = nullptr )
                  : QObject( p ? p : tm.parent() ), t( tm.get() ) {}
           void operator = ( const   AOTime &a ) { t = a.t; }
           void operator = ( const __int128 &a ) { t = a;   }
         AOTime operator + ( const   AOTime &a ) { AOTime c(*this); c.t = t + a.t; return c; }
         AOTime operator + ( const __int128 &a ) { AOTime c(*this); c.t = t + a;   return c; }
         AOTime operator - ( const   AOTime &a ) { AOTime c(*this); c.t = t - a.t; return c; }
         AOTime operator - ( const __int128 &a ) { AOTime c(*this); c.t = t - a;   return c; }
         AOTime operator +=( const   AOTime &a ) { t = t + a.t; return *this; }
         AOTime operator +=( const __int128 &a ) { t = t + a;   return *this; }
         AOTime operator -=( const   AOTime &a ) { t = t - a.t; return *this; }
         AOTime operator -=( const __int128 &a ) { t = t - a;   return *this; }
           bool operator ==( const   AOTime &a ) { return (t == a.t); }
           bool operator ==( const __int128 &a ) { return (t == a  ); }
           bool operator !=( const   AOTime &a ) { return (t != a.t); }
           bool operator !=( const __int128 &a ) { return (t != a  ); }
           bool operator <=( const   AOTime &a ) { return (t <= a.t); }
           bool operator <=( const __int128 &a ) { return (t <= a  ); }
           bool operator >=( const   AOTime &a ) { return (t >= a.t); }
           bool operator >=( const __int128 &a ) { return (t >= a  ); }
           bool operator < ( const   AOTime &a ) { return (t <  a.t); }
           bool operator < ( const __int128 &a ) { return (t <  a  ); }
           bool operator > ( const   AOTime &a ) { return (t >  a.t); }
           bool operator > ( const __int128 &a ) { return (t >  a  ); }
static __int128 now();
static __int128 shiftUp64( __int128 v );
       __int128 get() const { return t; }
           void set( __int128 nt ) { t = nt; }
           bool future();
           bool past();

private:
  __int128 t;
};

#endif // AOTIME_H
