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
#ifndef INDEX_H
#define INDEX_H

#include "data16.h"

class Index : public Data16
{
    Q_OBJECT
public:
    explicit  Index( qint16 val = 0, QObject *p = nullptr );
              Index( const Index &f, QObject *p = nullptr )
                : Data16( AO_INDEX, f.v, p ? p : f.parent() ) {}
        void  operator = ( const QByteArray &di ) { Data16::operator = ( di  ); }
        void  operator = ( const qint16    &val ) { Data16::operator = ( val ); }
};

#endif // INDEX_H
