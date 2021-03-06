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
#ifndef PRIVATEKEYRSA3072_H
#define PRIVATEKEYRSA3072_H

#include "datavarlength.h"

class PrivateKeyRsa3072 : public DataVarLength
{
    Q_OBJECT
public:
    explicit  PrivateKeyRsa3072( QObject *p = nullptr )
                : DataVarLength( AO_RSA3072_PRI_KEY, p ) {}
              PrivateKeyRsa3072( const PrivateKeyRsa3072 &k, QObject *p = nullptr )
                : DataVarLength( k.ba, AO_RSA3072_PRI_KEY, p ? p : k.parent() ) {}
              PrivateKeyRsa3072( const DataItemBA &di, QObject *p = nullptr )
                : DataVarLength( di, p ) {}
              PrivateKeyRsa3072( const QByteArray &iba, QObject *p = nullptr )
                : DataVarLength( iba, AO_RSA3072_PRI_KEY, p ) {}
        void  clear() { ba.clear(); }
        bool  isValid() const { return true; }  // TODO: really check
};

#endif // PRIVATEKEYRSA3072_H
