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
// Shares represents a number of shares in a blockchain's share balance
//
#ifndef SHARES_H
#define SHARES_H

#include <QObject>
#include "bytecodes.h"

class Shares : public QObject
{
    Q_OBJECT
public:
    explicit Shares( __int128 val = 0, QObject *p = nullptr);
             Shares( const Shares &f, QObject *p = nullptr ) : QObject( p ? p : f.parent() ), n( f.n ) {}
             Shares( const QByteArray &ba, QObject *p = nullptr );
        void operator = ( const QByteArray &ba );
        void operator = ( const   Shares &f ) { n = f.n; }
        void operator = ( const __int128 &f ) { n = f;   }
      Shares operator + ( const   Shares &f ) { Shares c(*this); c.n = n + f.n; return c; }
      Shares operator + ( const __int128 &f ) { Shares c(*this); c.n = n + f;   return c; }
      Shares operator - ( const   Shares &f ) { Shares c(*this); c.n = n - f.n; return c; }
      Shares operator - ( const __int128 &f ) { Shares c(*this); c.n = n - f;   return c; }
      Shares operator +=( const   Shares &f ) { n = n + f.n; return *this; }
      Shares operator +=( const __int128 &f ) { n = n + f;   return *this; }
      Shares operator -=( const   Shares &f ) { n = n - f.n; return *this; }
      Shares operator -=( const __int128 &f ) { n = n - f;   return *this; }
        bool operator ==( const   Shares &f ) { return (n == f.n); }
        bool operator ==( const __int128 &f ) { return (n == f  ); }
        bool operator !=( const   Shares &f ) { return (n != f.n); }
        bool operator !=( const __int128 &f ) { return (n != f  ); }
        bool operator <=( const   Shares &f ) { return (n <= f.n); }
        bool operator <=( const __int128 &f ) { return (n <= f  ); }
        bool operator >=( const   Shares &f ) { return (n >= f.n); }
        bool operator >=( const __int128 &f ) { return (n >= f  ); }
        bool operator < ( const   Shares &f ) { return (n <  f.n); }
        bool operator < ( const __int128 &f ) { return (n <  f  ); }
        bool operator > ( const   Shares &f ) { return (n >  f.n); }
        bool operator > ( const __int128 &f ) { return (n >  f  ); }
  QByteArray toByteArray( unsigned char code = AO_TRANSACTION_AMT );

signals:

public slots:

private:
  __int128 n;
};

#endif // SHARES_H
