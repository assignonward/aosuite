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
#ifndef PRIVATEKEYECDSA_H
#define PRIVATEKEYECDSA_H

#include "datavarlength.h"

class PrivateKeyEcdsa : public DataVarLength
{
    Q_OBJECT
public:
    explicit  PrivateKeyEcdsa( QObject *p = NULL )
                : DataVarLength( AO_ECDSA_PRI_KEY, p ) {}
              PrivateKeyEcdsa( const PrivateKeyEcdsa &k, QObject *p = NULL )
                : DataVarLength( k.ba, AO_ECDSA_PRI_KEY, p ? p : k.parent() ) {}
              PrivateKeyEcdsa( const QByteArray &di, QObject *p = NULL )
                : DataVarLength( di, p ) {}
        void  clear() { ba.clear(); }
        bool  isValid() const { return (ba.size() > 0); } // TODO: really check
};

#endif // PUBLICKEYECDSA_H
