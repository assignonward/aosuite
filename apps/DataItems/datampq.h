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
#ifndef DATAMPQ_H
#define DATAMPQ_H

#include "dataitem.h"
#include <gmpxx.h>

class DataMpq : public DataItem
{
public:
    explicit  DataMpq( const mpq_class &d, typeCode_t tc = AO_UNDEFINED_DATAITEM, QObject *p = nullptr )
                : DataItem( tc, p ), v( d ) {}
              DataMpq( const DataItemBA &di, QObject *p = nullptr );
   mpq_class &value() { return v; }
  DataItemBA  toDataItem( bool cf = false ) const;

protected:
  mpq_class v;
};

#endif // DATAMPQ_H
