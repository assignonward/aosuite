/*
 * MIT License
 *
 * Copyright (c) 2018 Assign Onward
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
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
