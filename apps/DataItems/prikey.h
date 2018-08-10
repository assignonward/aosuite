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
#ifndef PRIKEY_H
#define PRIKEY_H

#include "dataitem.h"
#include "privatekeyecdsa.h"
#include "privatekeyrsa3072.h"
#include <QPointer>

/**
 * @brief The PriKey class - multi-container for various types of private keys
 */
class PriKey : public DataItem
{
    Q_OBJECT
public:
              PriKey( QObject *p = nullptr )
                : DataItem( AO_UNDEFINED_DATAITEM, p ) {}
              PriKey( typeCode_t tc, QObject *p = nullptr );
              PriKey( typeCode_t tc, const QByteArray &kd, QObject *p = nullptr )
                : DataItem( tc, p ) { set(kd); }
              PriKey( const DataItemBA &di, QObject *p = nullptr );
              PriKey( const PriKey &pk, QObject *p = nullptr );
              PriKey( PrivateKeyEcdsa *pkp, QObject *p = nullptr )
                : DataItem( AO_ECDSA_PRI_KEY, p ? p : pkp->parent() )
                { priKeyEcdsa = pkp; if ( priKeyEcdsa ) priKeyEcdsa->setParent( this ); }
              PriKey( PrivateKeyRsa3072 *pkp, QObject *p = nullptr )
                : DataItem( AO_RSA3072_PRI_KEY, p ? p : pkp->parent() )
                { priKeyRsa3072 = pkp; if ( priKeyRsa3072 ) priKeyRsa3072->setParent( this ); }
        void  operator = ( const PriKey &k )
          { typeCode      = k.typeCode;
            priKeyEcdsa   = k.priKeyEcdsa;   if ( priKeyEcdsa   ) priKeyEcdsa  ->setParent( this );
            priKeyRsa3072 = k.priKeyRsa3072; if ( priKeyRsa3072 ) priKeyRsa3072->setParent( this );
          }
        void  operator = ( const DataItemBA &di );
  DataItemBA  toDataItem( bool cf = false ) const;
  QByteArray  get() const;
        void  set( const QByteArray &k );
        bool  isValid() const;

private:
  QPointer<  PrivateKeyEcdsa> priKeyEcdsa;
  QPointer<PrivateKeyRsa3072> priKeyRsa3072;
};

#endif // PRIKEY_H
