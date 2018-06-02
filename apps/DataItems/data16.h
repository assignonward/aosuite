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
#ifndef DATA16_H
#define DATA16_H

#include "dataitem.h"

class Data16 : public DataItem
{
    Q_OBJECT
public:
    explicit  Data16( qint16 d = 0, typeCode_t tc = AO_UNDEFINED_DATAITEM, QObject *p = NULL )
                : DataItem( tc, p ), v( d ) {}
              Data16( const Data16 &d, QObject *p = NULL )
                : DataItem( d.typeCode, p ? p : d.parent() ), v( d.v ) {}
              Data16( const DataItemBA &di, QObject *p = NULL );
      qint16  value() { return v; }
  DataItemBA  toDataItem( bool cf = false ) const;
virtual void  operator = ( const DataItemBA &di );
        void  operator = ( const Data16 &d ) { v = d.v; typeCode = d.typeCode; csVal = d.csVal; }
        void  operator = ( const qint16 &d ) { v = d; }
      Data16  operator + ( const Data16 &d ) { Data16 c(*this); c.v = v + d.v; return c; }
      Data16  operator + ( const qint16 &d ) { Data16 c(*this); c.v = v + d;   return c; }
      Data16  operator - ( const Data16 &d ) { Data16 c(*this); c.v = v - d.v; return c; }
      Data16  operator - ( const qint16 &d ) { Data16 c(*this); c.v = v - d;   return c; }
      Data16  operator +=( const Data16 &d ) { v += d.v; return *this; }
      Data16  operator +=( const qint16 &d ) { v += d;   return *this; }
      Data16  operator -=( const Data16 &d ) { v -= d.v; return *this; }
      Data16  operator -=( const qint16 &d ) { v -= d;   return *this; }
        bool  operator ==( const Data16 &d ) { return (v == d.v); }
        bool  operator ==( const qint16 &d ) { return (v == d  ); }
        bool  operator !=( const Data16 &d ) { return (v != d.v); }
        bool  operator !=( const qint16 &d ) { return (v != d  ); }
        bool  operator <=( const Data16 &d ) { return (v <= d.v); }
        bool  operator <=( const qint16 &d ) { return (v <= d  ); }
        bool  operator >=( const Data16 &d ) { return (v >= d.v); }
        bool  operator >=( const qint16 &d ) { return (v >= d  ); }
        bool  operator < ( const Data16 &d ) const { return (v <  d.v); }
        bool  operator < ( const qint16 &d ) const { return (v <  d  ); }
        bool  operator > ( const Data16 &d ) const { return (v >  d.v); }
        bool  operator > ( const qint16 &d ) const { return (v >  d  ); }

protected:
      qint16  v; // generic value
};

#endif // DATA16_H
