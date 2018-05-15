/* MIT License
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
// Data128 is the base class for objects which are basically a 128 bit integer, with a few kinks.
//
#ifndef DATA128_H
#define DATA128_H

#include "dataitem.h"

class Data128 : public DataItem
{
    Q_OBJECT
public:
    explicit  Data128( typeCode_t tc = AO_UNDEFINED_DATAITEM, __int128 d = 0, QObject *p = nullptr )
                : DataItem( tc, p ), v( d ) {}
              Data128( const Data128 &d, QObject *p = nullptr )
                : DataItem( d.typeCode, p ? p : d.parent() ), v( d.v ) {}
              Data128( const QByteArray &di, QObject *p = nullptr );
  QByteArray  toDataItem() const;
        void  operator = ( const QByteArray &di );
        void  operator = ( const  Data128 &d ) { v = d.v; typeCode = d.typeCode; csVal = d.csVal; }
        void  operator = ( const __int128 &d ) { v = d; }
     Data128  operator + ( const  Data128 &d ) { Data128 c(*this); c.v = v + d.v; return c; }
     Data128  operator + ( const __int128 &d ) { Data128 c(*this); c.v = v + d;   return c; }
     Data128  operator - ( const  Data128 &d ) { Data128 c(*this); c.v = v - d.v; return c; }
     Data128  operator - ( const __int128 &d ) { Data128 c(*this); c.v = v - d;   return c; }
     Data128  operator +=( const  Data128 &d ) { v += d.v; return *this; }
     Data128  operator +=( const __int128 &d ) { v += d;   return *this; }
     Data128  operator -=( const  Data128 &d ) { v -= d.v; return *this; }
     Data128  operator -=( const __int128 &d ) { v -= d;   return *this; }
        bool  operator ==( const  Data128 &d ) { return (v == d.v); }
        bool  operator ==( const __int128 &d ) { return (v == d  ); }
        bool  operator !=( const  Data128 &d ) { return (v != d.v); }
        bool  operator !=( const __int128 &d ) { return (v != d  ); }
        bool  operator <=( const  Data128 &d ) { return (v <= d.v); }
        bool  operator <=( const __int128 &d ) { return (v <= d  ); }
        bool  operator >=( const  Data128 &d ) { return (v >= d.v); }
        bool  operator >=( const __int128 &d ) { return (v >= d  ); }
        bool  operator < ( const  Data128 &d ) { return (v <  d.v); }
        bool  operator < ( const __int128 &d ) { return (v <  d  ); }
        bool  operator > ( const  Data128 &d ) { return (v >  d.v); }
        bool  operator > ( const __int128 &d ) { return (v >  d  ); }

protected:
    __int128  v; // generic value
};

#endif // DATA128_H
