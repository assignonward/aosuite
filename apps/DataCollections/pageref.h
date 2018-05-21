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
#ifndef PAGEREF_H
#define PAGEREF_H

#include "aotime.h"
#include "blockref.h"
#include "datavarlenlong.h"
#include "hash.h"
#include "index.h"

/**
 * @brief The PageRef class - when the page is the chain-maker's signature page on
 *   a block in the chain, then this is also a reference to a block and chain.
 */
class PageRef : public DataVarLenLong
{
    Q_OBJECT
public:
    explicit  PageRef( QObject *p = NULL )
                : DataVarLenLong( AO_PAGE_REF, QByteArray(), p ) {}
              PageRef( const PageRef &r )
                : DataVarLenLong( r.typeCode, r.ba, r.parent() ), block( r.block ), sequenceNumber( r.sequenceNumber ), hash( r.hash ) {}
              PageRef( const QByteArray &di, QObject *p = NULL );
        void  operator = ( const QByteArray &di );
      AOTime  publicationTime() { return block.getTime(); }
  QByteArray  toDataItem( bool cf = false );
        bool  isValid() { return block.isValid(); }

private:
    BlockRef block;           // block this page is recorded in
       Index sequenceNumber;  // page's sequence number in the block
        Hash hash;            // Hash of page - redundant check
};

#endif // PAGEREF_H
