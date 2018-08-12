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
// DataMpq is the base class for objects which are basically a rational number
//
#ifndef DATAMPZ_H
#define DATAMPZ_H

#include "dataitem.h"
#include <gmpxx.h>

class DataMpz : public DataItem
{
public:
    explicit  DataMpz( const mpz_class &d, typeCode_t tc = AO_UNDEFINED_DATAITEM, QObject *p = nullptr )
                : DataItem( tc, p ), v( d ) {}
              DataMpz( const DataItemBA &di, QObject *p = nullptr );
   mpz_class &value() { return v; }
  DataItemBA  toDataItem( bool cf = false ) const;
  DataMpz  operator += ( const mpz_class &d ) { v += d;   return *this; }
  DataMpz  operator += ( const   DataMpz &d ) { v += d.v; return *this; }
  DataMpz  operator += ( const       int &d ) { v += d;   return *this; }
  DataMpz  operator -= ( const mpz_class &d ) { v -= d;   return *this; }
  DataMpz  operator -= ( const   DataMpz &d ) { v -= d.v; return *this; }
  DataMpz  operator -= ( const       int &d ) { v -= d;   return *this; }
     bool  operator <  ( const mpz_class &d ) { return (v <  d  ); }
     bool  operator <  ( const   DataMpz &d ) { return (v <  d.v); }
     bool  operator <  ( const       int &d ) { return (v <  d  ); }
     bool  operator >  ( const mpz_class &d ) { return (v >  d  ); }
     bool  operator >  ( const   DataMpz &d ) { return (v >  d.v); }
     bool  operator >  ( const       int &d ) { return (v >  d  ); }
     bool  operator <= ( const mpz_class &d ) { return (v <= d  ); }
     bool  operator <= ( const   DataMpz &d ) { return (v <= d.v); }
     bool  operator <= ( const       int &d ) { return (v <= d  ); }
     bool  operator >= ( const mpz_class &d ) { return (v >= d  ); }
     bool  operator >= ( const   DataMpz &d ) { return (v >= d.v); }
     bool  operator >= ( const       int &d ) { return (v >= d  ); }
     bool  operator == ( const mpz_class &d ) { return (v == d  ); }
     bool  operator == ( const   DataMpz &d ) { return (v == d.v); }
     bool  operator == ( const       int &d ) { return (v == d  ); }

protected:
  mpz_class v;
};

#endif // DATAMPZ_H
