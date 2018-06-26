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
#include "datavarint32.h"
#include "datavarlength.h"
#include "hash.h"

/**
 * @brief The PageRef class - when the page is the chain-maker's signature page on
 *   a block in the chain, then this is also a reference to a block and chain.
 */
class PageRef : public DataVarLength
{
    Q_OBJECT
public:
    explicit  PageRef( QObject *p = NULL )
                : DataVarLength( QByteArray(), AO_PAGE_REF, p ), sequenceNumber( -1 ) {}
              PageRef( const PageRef &r, QObject *p = NULL )
                : DataVarLength( r.ba, r.typeCode, p ? p : r.parent() ), block( r.block ), sequenceNumber( r.sequenceNumber ), hash( r.hash ) {}
              PageRef( const DataItemBA &di, QObject *p = NULL );
        void  operator = ( const DataItemBA &di );
      AOTime  publicationTime() { return block.getTime(); }
  DataItemBA  toDataItem( bool cf = false );
        bool  isValid() { return block.isValid(); }

private:
    BlockRef block;           // block this page is recorded in
DataVarInt32 sequenceNumber;  // page's sequence number in the block
        Hash hash;            // Hash of page - redundant check
};

#endif // PAGEREF_H
