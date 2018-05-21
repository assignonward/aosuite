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
#ifndef NOTE_H
#define NOTE_H

#include "datavarlenlong.h"

class Note : public DataVarLenLong
{
    Q_OBJECT
public:
    explicit  Note( QByteArray ba = QByteArray(), QObject *p = NULL ) : DataVarLenLong( AO_NOTE, ba, p ) {}
              Note( const Note &n, QObject *p = NULL ) : DataVarLenLong( AO_NOTE, n.ba, p ? p : n.parent() ) {}
      qint32  size() { return ba.size(); }
        void  operator = ( const QByteArray &di ) { DataVarLenLong::operator = ( di ); }
};

#endif // NOTE_H
