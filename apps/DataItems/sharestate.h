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
#ifndef SHARESTATE_H
#define SHARESTATE_H

#include "data8.h"

#define AOSS_ASSIGNED_AWAY 0
#define AOSS_UNKNOWN       1
#define AOSS_REC_LOCKED    2  // Locked while attempting to record an assignment
#define AOSS_UND_LOCKED    4  // Locked for underwriting contract
#define AOSS_AVAILABLE     7

class ShareState : public Data8
{
    Q_OBJECT
public:
    explicit  ShareState( qint8 val = 0, QObject *p = NULL )
                : Data8( val, AO_SHARE_STATE, p ) { v = AOSS_UNKNOWN; }
              ShareState( const QByteArray &di, QObject *p = NULL )
                : Data8( di, p ) {}
              ShareState( const ShareState &f, QObject *p = NULL )
                : Data8( f.v, AO_SHARE_STATE, p ? p : f.parent() ) { v = AOSS_UNKNOWN; }
        void  operator = ( const QByteArray &di ) { Data8::operator = ( di  ); }
        void  operator = ( const qint8     &val ) { Data8::operator = ( val ); }
       qint8  getState() const { return v; }
        void  setState( const qint8 &s ) { v = s; }
};

#endif // SHARESTATE_H
