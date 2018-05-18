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
#include "bytecodes.h"
#include "datavarlenlong.h"
#include "genesisref.h"
#include "hash.h"
#include "sharesout.h"

/**
 * @brief The BlockRef class - identifies a block in the chain
 */
class BlockRef : public DataVarLenLong
{
    Q_OBJECT
public:
    explicit  BlockRef( QByteArray di = QByteArray(), QObject *p = nullptr );
              BlockRef( const BlockRef &r )
                : DataVarLenLong( AO_BLOCK_REF, QByteArray(), r.parent() ),
                  propTime( r.propTime ), shOut( r.shOut ), blkHash( r.blkHash ), genesis( r.genesis ) {}
              BlockRef( const Hash &h, const AOTime t, const SharesOut &s, const GenesisRef &r, QObject *p = nullptr )
                : DataVarLenLong( AO_BLOCK_REF, QByteArray(), p ),
                  propTime( t ), shOut( s ), blkHash( h ), genesis( r ) {}
        void  operator = ( const QByteArray &di );
      AOTime  getTime()    const { return propTime; }
        Hash  getHash()    const { return  blkHash; }
   SharesOut  getShOut()   const { return    shOut; }
  GenesisRef  getGenesis() const { return  genesis; }
        void  setTime( const AOTime &t )        { propTime = t; }
        void  setHash( const Hash &h )          {  blkHash = h; }
        void  setShOut( const SharesOut &s )    {    shOut = s; }
        void  setGenesis( const GenesisRef &r ) {  genesis = r; }
  QByteArray  toDataItem( bool cf = false );
        bool  isValid() { return propTime.past() && blkHash.isValid() && genesis.isValid(); }

private:
      AOTime  propTime;  // time this block was proposed (should fit it into a specific time-layer)
   SharesOut  shOut;     // shares outstanding after this block is recorded
        Hash  blkHash;   // Whole block hash signature (unique ID)
  GenesisRef  genesis;   // the genesis block (chain ID)
};

#endif // BLOCKREF_H
