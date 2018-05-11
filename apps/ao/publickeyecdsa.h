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
#ifndef PUBLICKEYECDSA_H
#define PUBLICKEYECDSA_H

#include "datafixedlength.h"

class PublicKeyECDSA : public DataFixedLength
{
    Q_OBJECT
public:
      explicit  PublicKeyECDSA( QObject *p = nullptr )
                  : DataFixedLength( AO_PUB_ECDSA_KEY2, QByteArray(), p ) {}
                PublicKeyECDSA( const PublicKeyECDSA &k, QObject *p = nullptr )
                  : DataFixedLength( AO_PUB_ECDSA_KEY2, k.ba, p ? p : k.parent() ) {}
                PublicKeyECDSA( const QByteArray &di, QObject *p = nullptr )
                  : DataFixedLength( di, p ) {}
    QByteArray  get() const;
          void  set( QByteArray k );
          void  clear() { ba.clear(); }
};

#endif // PUBLICKEYECDSA_H
