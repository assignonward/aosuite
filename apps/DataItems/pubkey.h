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

#include "data64.h"
#include "publickeyecdsa.h"
#include "publickeyrsa3072.h"

/**
 * @brief The PubKey class - multi-container for various types of public keys
 */
class PubKey : public DataItem
{
    Q_OBJECT
public:
    explicit  PubKey( typeCode_t tc = AO_UNDEFINED_DATAITEM, QObject *p = NULL );
              PubKey( const DataItemBA &di, QObject *p = NULL );
              PubKey( const PubKey &pk, QObject *p = NULL );
        void  operator = ( const PubKey &k ) { typeCode = k.typeCode; publicKeyEcdsa = k.publicKeyEcdsa; publicKeyRsa3072 = k.publicKeyRsa3072; }
        void  operator = ( const DataItemBA &di );
  DataItemBA  toDataItem( bool cf = false ) const;
  QByteArray  get() const;
  QByteArray  getId( bool cf = true ) const;
        void  set( const QByteArray k );
        bool  isValid() const;

private:
            Data64  publicKeyIndex;  // AO_ID_SEQ_NUM index number of a public key on the blockchain
    PublicKeyEcdsa  publicKeyEcdsa;
  PublicKeyRsa3072  publicKeyRsa3072;
};

#endif // PUBKEY_H
