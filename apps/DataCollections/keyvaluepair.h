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
#ifndef KEYVALUEPAIR_H
#define KEYVALUEPAIR_H

#include "datavarlenlong.h"
#include "datavarlenshort.h"

/**
 * @brief The KeyValuePair class - A (short) arbitrary length key and a
 *   (long) arbitrary length value
 */
class KeyValuePair : public DataVarLenLong
{
    Q_OBJECT
public:
    explicit  KeyValuePair( QByteArray di = QByteArray(), QObject *p = NULL );
              KeyValuePair( const KeyValuePair &k )
                : DataVarLenLong( AO_KEYVALUEPAIR, k.parent() ),
                  key( k.key ), value( k.value ) {}
        void  operator = ( const QByteArray &di );
  QByteArray  toDataItem( bool cf = false );

private:
    DataVarLenShort  key;
     DataVarLenLong  value;
};

#endif // KEYVALUEPAIR_H