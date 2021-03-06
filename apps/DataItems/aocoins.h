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
// A quantity of coins, as a rational number

#ifndef AOCOINS_H
#define AOCOINS_H

#include "datampq.h"

class AOCoins : public DataMpq
{
    Q_OBJECT
public:
       explicit  AOCoins( const mpq_class &c, typeCode_t typ = AO_N_COINS, QObject *p = nullptr )
                   : DataMpq( c, typ, p ) {}
                 AOCoins( const mpq_class &c, QObject *p = nullptr )
                   : DataMpq( c, AO_N_COINS, p ) {}
                 AOCoins( const DataItemBA &di, QObject *p = nullptr )
                   : DataMpq( di, p ) {}
                 AOCoins( const AOCoins &c, QObject *p = nullptr )
                   : DataMpq( c.get(), c.typeCode, p ? p : c.parent() ) {}
           void  operator = ( const DataItemBA &di ) { DataMpq::operator = ( di ); }

      mpq_class  get() const { return v; }
           void  set( const mpq_class &c ) { v = c; }
};

#endif // AOCOINS_H
