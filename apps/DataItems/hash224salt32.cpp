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
#include "hash224salt32.h"
#include "random.h"
#include <QCryptographicHash>

/**
 * @brief Hash224Salt32::Hash224Salt32 - constructor.
 * @param text - text to hash, or empty.
 * @param p - object parent, if any
 */
Hash224Salt32::Hash224Salt32( QByteArray text, QObject *p )
  : DataFixedLength( AO_HASH224SALT32, p ), verified( false )
{ if ( text.size() < 1 )
    return;
  calculate( text );
}

/**
 * @brief Hash224Salt32::calculate
 * @param text
 */
Hash224Salt32 &Hash224Salt32::calculate( QByteArray text )
{ QCryptographicHash ho( QCryptographicHash::Sha224 );
  QByteArray salt = rng.rnd_bytes(4);
  ho.addData( salt+text );
  ba = salt+ho.result();
  verified = true;
  return *this;
}

/**
 * @brief Hash224Salt32::verify
 * @param text - check if this text matches the hash
 * @return true if hash of text matches this hash
 */
bool Hash224Salt32::verify( QByteArray text )
{ QCryptographicHash ho( QCryptographicHash::Sha224 );
  QByteArray salt = ba.mid(0,4);
  ho.addData( salt+text );
  verified = ( salt+ho.result() == ba );
  return verified;
}

/**
 * @brief Hash224Salt32::isValid
 * @return true if the data is the correct size
 */
bool Hash224Salt32::isValid() const
{ return (( ba.size() == 32 ) && ( typeCode == AO_HASH224SALT32 )); }
