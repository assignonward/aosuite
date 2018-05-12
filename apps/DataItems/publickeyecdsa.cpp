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
#include "publickeyecdsa.h"

/**
 * @brief PublicKeyECDSA::get
 * @return the key with the type code in front (standard 33 byte compressed format)
 */
QByteArray  PublicKeyECDSA::get() const
{ QByteArray k;
  switch ( typeCode )
    { case AO_PUB_ECDSA_KEY2:
      case AO_PUB_ECDSA_KEY3:
        k.append( typeCode );
        k.append( ba );
    }
  return k;
}

/**
 * @brief PublicKeyECDSA::set
 * @param k - 33 byte compressed key
 */
void  PublicKeyECDSA::set( QByteArray k )
{ if ( k.size() != 33 )
    { // TODO: log error
      return;
    }
  switch ( k.at(0) )
    { case AO_PUB_ECDSA_KEY2:
      case AO_PUB_ECDSA_KEY3:
        typeCode = k.at(0);
        ba = k.mid(1);
        return;
    }
  // TODO: log error
}
