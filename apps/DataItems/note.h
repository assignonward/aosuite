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
#ifndef NOTE_H
#define NOTE_H

#include "datavarlength.h"

/**
 * @brief The Note class - implies UTF8 encoding, but no enforcement as of yet.
 */
class Note : public DataVarLength
{
    Q_OBJECT
public:
    explicit  Note( QByteArray ba = QByteArray(), QObject *p = nullptr )
                : DataVarLength( ba, AO_NOTE, p ) {}
              Note( const Note &n, QObject *p = nullptr )
                : DataVarLength( n.ba, AO_NOTE, p ? p : n.parent() ) {}
        void  setNote( const QString &s ) { ba = s.toUtf8(); }
     QString  getNote() { return QString::fromUtf8( ba ); }
      qint32  size() { return ba.size(); }
        void  operator = ( const DataItemBA &di ) { DataVarLength::operator = ( di ); }
};

#endif // NOTE_H
