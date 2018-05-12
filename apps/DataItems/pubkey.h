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
#ifndef PUBKEY_H
#define PUBKEY_H

#include "publickeyecdsa.h"
#include "publickeyrsa3072.h"

/**
 * @brief The PubKey class - unnecessary? QByteArray wrapper.
 *   future plans include compressing 64-33 and expanding 33-64
 */
class PubKey : public QObject
{
    Q_OBJECT
public:
    explicit  PubKey( typeCode_t tc = AO_PUB_ECDSA_KEY2, QObject *p = nullptr );
              PubKey( const QByteArray &di, QObject *p = nullptr );
              PubKey( const PubKey &pk, QObject *p = nullptr ) : QObject( p ? p : pk.parent() ),
                  typeCode( pk.typeCode ), publicKeyECDSA( pk.publicKeyECDSA ), publicKeyRsa3072( pk.publicKeyRsa3072 ) {}
        void  operator = ( const PubKey &k ) { typeCode = k.typeCode; publicKeyECDSA = k.publicKeyECDSA; publicKeyRsa3072 = k.publicKeyRsa3072; }
        void  operator = ( const QByteArray &di );
  QByteArray  toDataItem() const;
  QByteArray  get() const;
  QByteArray  getId() const;
        void  set( const QByteArray k );
        bool  isValid() const;

private:
        typeCode_t  typeCode;
    PublicKeyECDSA  publicKeyECDSA;
  PublicKeyRsa3072  publicKeyRsa3072;
};

#endif // PUBKEY_H
