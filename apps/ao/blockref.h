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
#ifndef BLOCKREF_H
#define BLOCKREF_H

#include "aotime.h"

/**
 * @brief The BlockRef class - identifies a block in the chain
 */
class BlockRef : public QObject
{
    Q_OBJECT
public:
    explicit  BlockRef( QObject *parent = nullptr) : QObject( parent ) {}
              BlockRef( const BlockRef &r )
                : QObject( r.parent() ), sequenceNumber( r.sequenceNumber ),
                  proposalTime( r.proposalTime ), hash( r.hash ) {}
      AOTime  time() { return proposalTime; }
  QByteArray  toByteArray() { return QByteArray(); }

private:
     quint32 sequenceNumber; // sequence number in the chain (Genesis==0) this page is recorded in
      AOTime proposalTime;   // time this block was proposed
  QByteArray hash;           // Hash of block - redundant check
};

#endif // BLOCKREF_H
