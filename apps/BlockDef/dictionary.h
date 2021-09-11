/* MIT License
 *
 * Copyright (c) 2021 Assign Onward
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
#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "riceyCodes.h"
#include <QJsonArray>
#include <QHash>

class Dictionary
{
public:
                           Dictionary();
                     void  read();
                     void  clear();
                     void  reread();
                     void  interpret( const QByteArray & );
                     bool  codesContainName( QByteArray n ) { return ciByName .contains(n); }
                     bool  codesContainCode( QByteArray c ) { return ciByRicey.contains(c); }
                     bool  codesContainCode( qint64 c )     { return ciByNum  .contains(c); }
               QByteArray  nameFromCode( QByteArray );
               QByteArray  nameFromCode( qint64 );
               QByteArray  riceyFromCodeName( QByteArray );
                   qint64  codeFromCodeName( QByteArray );

               QJsonArray  codes;
  QHash<QByteArray,qint32> ciByName;
  QHash<QByteArray,qint32> ciByRicey;
  QHash<qint64    ,qint32> ciByNum;
               QJsonArray  types;
};

#endif // DICTIONARY_H
