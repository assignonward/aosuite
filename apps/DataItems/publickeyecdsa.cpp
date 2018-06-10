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
 * @return the key with the type code in front (standard 33 or 65 byte formats)
 */
QByteArray  PublicKeyEcdsa::get() const
{ QByteArray k;
  switch ( typeCode )
    { case AO_ECDSA_PUB_KEY2:
      case AO_ECDSA_PUB_KEY3:
      case AO_ECDSA_PUB_KEY4:
        k.append( typeCode );
        k.append( ba );
    }
  return k;
}

/**
 * @brief PublicKeyECDSA::set
 * @param k - 33 byte compressed or 65 byte uncompressed key
 */
void  PublicKeyEcdsa::set( QByteArray k )
{ switch ( k.at(0) )
    { case AO_ECDSA_PUB_KEY2:
      case AO_ECDSA_PUB_KEY3:
        if ( k.size() != 33 )
            { // TODO: log error
              return;
            }
        typeCode = k.at(0);
        ba = k.mid(1);
        return;

      case AO_ECDSA_PUB_KEY4:
        if ( k.size() != 65 )
            { // TODO: log error
              return;
            }
        typeCode = k.at(0);
        ba = k.mid(1);
        return;
    }
  // TODO: log error
}
