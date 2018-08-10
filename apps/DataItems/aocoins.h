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
// A quantity of coins, fixed point 64.64

#ifndef AOCOINS_H
#define AOCOINS_H

#include "data128.h"

class AOCoins : public Data128
{
    Q_OBJECT
public:
       explicit  AOCoins( __int128 c = 0, typeCode_t typ = AO_N_COINS, QObject *p = nullptr )
                   : Data128( c, typ, p ) {}
                 AOCoins( __int128 c, QObject *p = nullptr )
                   : Data128( c, AO_N_COINS, p ) {}
                 AOCoins( const DataItemBA &di, QObject *p = nullptr )
                   : Data128( di, p ) {}
                 AOCoins( const AOCoins &c, QObject *p = nullptr )
                   : Data128( c.get(), c.typeCode, p ? p : c.parent() ) {}
           void  operator = ( const DataItemBA &di ) { Data128::operator = ( di ); }

static __int128  shiftUp64( __int128 m ) { return m << 64; }
       __int128  get() const { return v; }
           void  set( __int128 m ) { v = m; }
};

#endif // AOTIME_H
