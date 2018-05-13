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
#ifndef PUBLICKEYRSA3072_H
#define PUBLICKEYRSA3072_H

#include "datafixedlength.h"

class PublicKeyRsa3072 : public DataFixedLength
{
    Q_OBJECT
public:
    explicit  PublicKeyRsa3072( QObject *p = nullptr )
                : DataFixedLength( AO_RSA3072_PUB_KEY, p ) {}
              PublicKeyRsa3072( const PublicKeyRsa3072 &k, QObject *p = nullptr )
                : DataFixedLength( AO_RSA3072_PUB_KEY, k.ba, p ? p : k.parent() ) {}
              PublicKeyRsa3072( const QByteArray &di, QObject *p = nullptr )
                : DataFixedLength( di, p ) {}
        void  clear() { ba.clear(); }
     // bool  isValid() { return (ba.size() == 384); }  could do more, this is handled in the parent class
};

#endif // PUBLICKEYRSA3072_H
