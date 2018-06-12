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
#ifndef KEYASSET_H
#define KEYASSET_H

#include "datavarlength.h"
#include "keypair.h"
#include "sharesref.h"

/**
 * @brief The KeyAsset class - contains a key pair and if
 *   the keys are associated with shares, then populates the
 *   sharesRef as fully as needed too.
 */
class KeyAsset : public DataVarLength
{
    Q_OBJECT
public:
      explicit  KeyAsset( DataItemBA di = DataItemBA(), QObject *p = NULL );
                KeyAsset( const KeyAsset &k, QObject *p = NULL )
                  : DataVarLength( AO_KEY_ASSET, p ? p : k.parent() ),
                    keyPair( k.keyPair ), sharesRef( k.sharesRef ) {}
          void  operator = ( const DataItemBA &di );
    DataItemBA  toDataItem( bool cf = false );
          bool  isValid() { return keyPair.isValid(); }

private:
       KeyPair  keyPair;
     SharesRef  sharesRef;
};

#endif // KEYASSET_H
