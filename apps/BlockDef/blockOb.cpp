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

/**
 * @brief initKeyNames - easy code generator output, TODO: initialize from the json definition instead.
 */
void initKeyNames()
{ keyNames.insert( QByteArray::fromHex(     "00" ), "ObTerm_o"          );
  keyNames.insert( QByteArray::fromHex(     "01" ), "int64_i"           );
  keyNames.insert( QByteArray::fromHex(     "02" ), "int32_l"           );
  keyNames.insert( QByteArray::fromHex(     "03" ), "mpz_n"             );
  keyNames.insert( QByteArray::fromHex(     "04" ), "mpq_r"             );
  keyNames.insert( QByteArray::fromHex(     "05" ), "type_y"            );
  keyNames.insert( QByteArray::fromHex(     "06" ), "text_s"            );
  keyNames.insert( QByteArray::fromHex(     "07" ), "data_b"            );
  keyNames.insert( QByteArray::fromHex(     "10" ), "chainBlock_o"      );
  keyNames.insert( QByteArray::fromHex(     "20" ), "signedBlock_o"     );
  keyNames.insert( QByteArray::fromHex(     "30" ), "parentSignature_o" );
  keyNames.insert( QByteArray::fromHex(     "40" ), "signature_o"       );
  keyNames.insert( QByteArray::fromHex(     "11" ), "time_i"            );
  keyNames.insert( QByteArray::fromHex(     "13" ), "AOShares_n"        );
  keyNames.insert( QByteArray::fromHex(     "15" ), "SHA256_y"          );
  keyNames.insert( QByteArray::fromHex(     "25" ), "ECB256_y"          );
  keyNames.insert( QByteArray::fromHex(   "8025" ), "RSA3072_y"         );
  keyNames.insert( QByteArray::fromHex(   "8015" ), "SHA3b512_y"        );
  keyNames.insert( QByteArray::fromHex(   "9015" ), "jpg_y"             );
  keyNames.insert( QByteArray::fromHex(   "9025" ), "png_y"             );
  keyNames.insert( QByteArray::fromHex( "A0B000" ), "AOGenesisBlock_o"  );
  keyNames.insert( QByteArray::fromHex( "A0CD00" ), "AOChainDesc_o"     );
  keyNames.insert( QByteArray::fromHex( "A0CF00" ), "AOChainFunc_o"     );
  keyNames.insert( QByteArray::fromHex( "A0EC00" ), "AOEndChain_o"      );
  keyNames.insert( QByteArray::fromHex( "A0CD06" ), "Symbol_s"          );
  keyNames.insert( QByteArray::fromHex( "A0CD16" ), "CdName_s"          );
  keyNames.insert( QByteArray::fromHex( "A0CD26" ), "Tagline_s"         );
  keyNames.insert( QByteArray::fromHex( "A0CD36" ), "Description_s"     );
  keyNames.insert( QByteArray::fromHex( "A0CD10" ), "Icon_o"            );
  keyNames.insert( QByteArray::fromHex( "A0CD20" ), "Banner_o"          );
  keyNames.insert( QByteArray::fromHex( "A0CD30" ), "Image_o"           );
  keyNames.insert( QByteArray::fromHex( "A0CF03" ), "CfShares_n"        );
  keyNames.insert( QByteArray::fromHex( "A0CF13" ), "CfCoins_n"         );
  keyNames.insert( QByteArray::fromHex( "A0CF04" ), "CfRecFee_r"        );
}

BlockValueObject::~BlockValueObject()
{ foreach( QPointer<BlockObject> blockOb, m_obMap )
    if ( blockOb )
      delete( blockOb );
}

