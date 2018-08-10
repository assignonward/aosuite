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
#include "hash256.h"
#include <QCryptographicHash>

/**
 * @brief Hash256::Hash256 - constructor.
 * @param text - text to hash, or empty.
 * @param p - object parent, if any
 */
Hash256::Hash256( QByteArray text, QObject *p )
        : DataFixedLength( AO_HASH256, p ), verified( false )
{ // qDebug( "Hash256 construct from text" );
  if ( text.size() < 1 )
    return;
  QCryptographicHash ho( QCryptographicHash::Sha256 );
  ho.addData( text );
  ba.append( ho.result() );
}

/**
 * @brief Hash256::calculate
 * @param text
 */
Hash256 &Hash256::calculate( QByteArray text )
{ QCryptographicHash ho( QCryptographicHash::Sha256 );
  ho.addData( text );
  ba = ho.result();
  verified = true;
  return *this;
}

/**
 * @brief Hash256::verify
 * @param text - check if this text matches the hash
 * @return true if hash of text matches this hash
 */
bool Hash256::verify( QByteArray text )
{ // qDebug( "  text:%s",qPrintable( QString::fromUtf8( text.toHex() ) ) );
  QCryptographicHash ho( QCryptographicHash::Sha256 );
  ho.addData( text );
  verified = (ho.result() == ba);
//  qDebug( "  result:%s",qPrintable( QString::fromUtf8( ho.result().toHex() ) ) );
//  qDebug( "      ba:%s",qPrintable( QString::fromUtf8( ba.toHex() ) ) );
  return verified;
}

/**
 * @brief Hash256::isValid
 * @return true if the data is the correct size
 */
bool Hash256::isValid() const
{ return ((ba.size() == 32) && ( typeCode == AO_HASH256 )); }
