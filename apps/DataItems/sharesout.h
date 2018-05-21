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
// SharesOut represents a number of shares outstanding at a block's time of recording in a chain
//
#ifndef SHARESOUT_H
#define SHARESOUT_H

#include "data128.h"

class SharesOut : public Data128
{
public:
    explicit  SharesOut( __int128 val = 0, QObject *p = NULL )
                : Data128( AO_SHARES_OUT, val, p ) {}
              SharesOut( const SharesOut &f, QObject *p = NULL )
                : Data128( AO_SHARES_OUT, f.v, p ? p : f.parent() ) {}
        void  operator = ( const QByteArray &di ) { Data128::operator = ( di ); }
};

#endif // SHARESOUT_H
