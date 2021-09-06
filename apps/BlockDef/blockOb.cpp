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
#include "blockOb.h"

QHash<QByteArray,QByteArray> keyNames;  // key

void initKeyNames()
{ keyNames.insert( QByteArray::fromHex(   "00" ), "ObTermO"         );
  keyNames.insert( QByteArray::fromHex(   "10" ), "AOChainBlockO"   );
  keyNames.insert( QByteArray::fromHex(   "60" ), "pzzzO"           );
  keyNames.insert( QByteArray::fromHex(   "70" ), "zzzO"            );
  keyNames.insert( QByteArray::fromHex(   "01" ), "timei"           );
  keyNames.insert( QByteArray::fromHex(   "07" ), "datab"           );
  keyNames.insert( QByteArray::fromHex(   "04" ), "algorthmy"       );
  keyNames.insert( QByteArray::fromHex(   "14" ), "SHA256y"         );
  keyNames.insert( QByteArray::fromHex(   "24" ), "ECB256y"         );
  keyNames.insert( QByteArray::fromHex( "A024" ), "RSA3072y"        );
  keyNames.insert( QByteArray::fromHex( "A014" ), "SHA3b512y"       );
  keyNames.insert( QByteArray::fromHex( "A000" ), "AOGenesisBlockO" );
  keyNames.insert( QByteArray::fromHex( "A002" ), "AOSharesn"       );
  keyNames.insert( QByteArray::fromHex( "AEC0" ), "AOEndChainO"     );
}

BlockValueObjectList::~BlockValueObjectList()
{ foreach( QPointer<BlockOb> blockOb, obList )
    if ( blockOb )
      delete( blockOb );
}

