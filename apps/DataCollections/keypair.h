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
#ifndef KEYPAIR_H
#define KEYPAIR_H

#include "datavarlength.h"
#include "data8.h"
#include "prikey.h"
#include "pubkey.h"

// AO_SHARE_STATE values, stored in the shareState member
#define KEYS_UNUSED             0x00
#define KEYS_CONTROL_SHARES     0x02
#define KEYS_SHARES_ASSIGNED    0x05
#define KEYS_ASSIGNMENT_PENDING 0x06
#define KEYS_SHARES_ESCROWED    0x0A  // For underwriting, similar to assignment pending but not expected to result in assignment

/**
 * @brief The KeyPair class - contains a (hopefully matching) public/private key pair
 */
class KeyPair : public DataVarLength
{
    Q_OBJECT
public:
      explicit  KeyPair( DataItemBA di = DataItemBA(), QObject *p = NULL );
                KeyPair( const KeyPair &k, QObject *p = NULL )
                  : DataVarLength( AO_KEYPAIR, p ? p : k.parent() ),
                    shareState( k.shareState ), pubKey( k.pubKey ), priKey( k.priKey ) {}
          void  operator = ( const DataItemBA &di );
    DataItemBA  toDataItem( bool cf = false );
          bool  isValid() { return pubKey.isValid() && priKey.isValid(); }

private:
         Data8  shareState;
        PubKey  pubKey;
        PriKey  priKey;
};

#endif // KEYPAIR_H
