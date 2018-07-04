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

#include <QPointer>
#include "datavbc64.h"
#include "publickeyecdsa.h"
#include "publickeyrsa3072.h"

/**
 * @brief The PubKey class - multi-container for various types of public keys
 */
class PubKey : public DataItem
{
    Q_OBJECT
public:
              PubKey( QObject *p = NULL )
                : DataItem( AO_UNDEFINED_DATAITEM, p ) {}
              PubKey( typeCode_t tc, QObject *p = NULL );
              PubKey( const DataItemBA &di, QObject *p = NULL );
              PubKey( const PubKey &pk, QObject *p = NULL );
              PubKey( PublicKeyEcdsa *pkp, QObject *p = NULL )
                : DataItem( AO_ECDSA_PUB_KEY4 , p ? p : pkp->parent() )
                { publicKeyEcdsa   = pkp; publicKeyEcdsa  ->setParent( this ); }
              PubKey( PublicKeyRsa3072 *pkp, QObject *p = NULL )
                : DataItem( AO_RSA3072_PUB_KEY, p ? p : pkp->parent() )
                { publicKeyRsa3072 = pkp; publicKeyRsa3072->setParent( this ); }
              PubKey( DataVbc64 *pkp, QObject *p = NULL )
                : DataItem( AO_ID_SEQ_NUM     , p ? p : pkp->parent() )
                { publicKeyIndex   = pkp; publicKeyIndex  ->setParent( this ); }
        void  operator = ( const PubKey &k )
                { typeCode         = k.typeCode;
                  publicKeyEcdsa   = k.publicKeyEcdsa;
                  publicKeyRsa3072 = k.publicKeyRsa3072;
                  publicKeyIndex   = k.publicKeyIndex;
                }
        void  operator = ( const DataItemBA &di );
  DataItemBA  toDataItem( bool cf = false ) const;
  QByteArray  get() const;
  QByteArray  getId( bool cf = true ) const;
        void  set( const QByteArray k );
        bool  isValid() const;

private:
QPointer<       DataVbc64> publicKeyIndex;  // AO_ID_SEQ_NUM index number of a public key on the blockchain
QPointer<  PublicKeyEcdsa> publicKeyEcdsa;
QPointer<PublicKeyRsa3072> publicKeyRsa3072;

//       DataVbc64 *publicKeyIndex = NULL;  // AO_ID_SEQ_NUM index number of a public key on the blockchain
//  PublicKeyEcdsa *publicKeyEcdsa = NULL;
//PublicKeyRsa3072 *publicKeyRsa3072 = NULL;
};

#endif // PUBKEY_H
