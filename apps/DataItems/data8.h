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
#ifndef DATA8_H
#define DATA8_H

#include "dataitem.h"

class Data8 : public DataItem
{
    Q_OBJECT
public:
    explicit  Data8( qint8 d = 0, typeCode_t tc = AO_UNDEFINED_DATAITEM, QObject *p = NULL )
                : DataItem( tc, p ), v( d ) {}
              Data8( const Data8 &d, QObject *p = NULL )
                : DataItem( d.typeCode, p ? p : d.parent() ), v( d.v ) {}
              Data8( const DataItemBA &di, QObject *p = NULL );
  DataItemBA  toDataItem( bool cf = false ) const;
virtual void  operator = ( const DataItemBA &di );
        void  operator = ( const Data8 &d ) { v = d.v; DataItem::operator =( d ); }
        void  operator = ( const qint8 &d ) { v = d; }
       Data8  operator + ( const Data8 &d ) { Data8 c(*this); c.v = v + d.v; return c; }
       Data8  operator + ( const qint8 &d ) { Data8 c(*this); c.v = v + d;   return c; }
       Data8  operator - ( const Data8 &d ) { Data8 c(*this); c.v = v - d.v; return c; }
       Data8  operator - ( const qint8 &d ) { Data8 c(*this); c.v = v - d;   return c; }
       Data8  operator +=( const Data8 &d ) { v += d.v; return *this; }
       Data8  operator +=( const qint8 &d ) { v += d;   return *this; }
       Data8  operator -=( const Data8 &d ) { v -= d.v; return *this; }
       Data8  operator -=( const qint8 &d ) { v -= d;   return *this; }
        bool  operator ==( const Data8 &d ) { return (v == d.v); }
        bool  operator ==( const qint8 &d ) { return (v == d  ); }
        bool  operator !=( const Data8 &d ) { return (v != d.v); }
        bool  operator !=( const qint8 &d ) { return (v != d  ); }
        bool  operator <=( const Data8 &d ) { return (v <= d.v); }
        bool  operator <=( const qint8 &d ) { return (v <= d  ); }
        bool  operator >=( const Data8 &d ) { return (v >= d.v); }
        bool  operator >=( const qint8 &d ) { return (v >= d  ); }
        bool  operator < ( const Data8 &d ) { return (v <  d.v); }
        bool  operator < ( const qint8 &d ) { return (v <  d  ); }
        bool  operator > ( const Data8 &d ) { return (v >  d.v); }
        bool  operator > ( const qint8 &d ) { return (v >  d  ); }

protected:
      qint8  v; // generic value
};

#endif // DATA8_H
