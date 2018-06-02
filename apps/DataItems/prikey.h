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

/**
 * @brief The PriKey class - multi-container for various types of private keys
 */
class PriKey : public DataItem
{
    Q_OBJECT
public:
    explicit  PriKey( typeCode_t tc = AO_UNDEFINED_DATAITEM, QObject *p = NULL );
              PriKey( const DataItemBA &di, QObject *p = NULL );
              PriKey( const PriKey &pk, QObject *p = NULL );
        void  operator = ( const PriKey &k ) { typeCode = k.typeCode; privateKeyEcdsa = k.privateKeyEcdsa; privateKeyRsa3072 = k.privateKeyRsa3072; }
        void  operator = ( const DataItemBA &di );
  DataItemBA  toDataItem( bool cf = false ) const;
  QByteArray  get() const;
        void  set( const QByteArray k );
        bool  isValid() const;

private:
    PrivateKeyEcdsa  privateKeyEcdsa;
  PrivateKeyRsa3072  privateKeyRsa3072;
};

#endif // PRIKEY_H
