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
#include "dataitem.h"

/**
 * @brief DataItem::typeSize
 * @param tc - type code to interpret, or 0xFF (default) to use this object's typeCode
 *  current typeCode,
 *   or -1 if the typeCode doesn't have a defined fixed length.
 */
qint32 DataItem::typeSize( typeCode_t tc ) const
{ if ( tc == AO_UNDEFINED_DATAITEM )
    tc = typeCode;
  return typeSizeTable( tc );
}

qint32 DataItem::typeSizeTable( typeCode_t tc )
{ switch ( tc & AO_SIZE_MASK )
    { case AO_SIZE_3BYTES  : return   3;
      case AO_SIZE_4BYTES  : return   4;
      case AO_SIZE_18BYTES : return  18;
      case AO_SIZE_34BYTES : return  34;
      case AO_SIZE_66BYTES : return  66;
      case AO_SIZE_388BYTES: return 388;
    }
  return -1;
}

/**
 * @brief DataItem::typeSize
 * @param di - byte array to interpret size of the first item in
 */
qint32 DataItem::typeSize( const QByteArray &di ) const
{ if ( di.size() < 1 )
    return -1;
  typeCode_t tc = di.at(0);
  switch ( tc & AO_VARSIZE_MASK )
    { case AO_SIZE_VARLENLONG:
        if ( di.size() < 4 )
          return -1;
        return (((qint32)di.at(1)) * 256 + ((qint32)di.at(2))) * 256 + ((qint32)di.at(3));

      case AO_SIZE_VARLENSHORT:
        if ( di.size() < 2 )
          return -1;
        return (qint32)di.at(1);
    }
  return typeSizeTable( tc );
}

/**
 * @brief DataItem::typeCodeOf
 * @param di - data item to interpret
 * @return typecode of the passed data item
 */
typeCode_t DataItem::typeCodeOf( const QByteArray &di )
{ if ( di.size() < 1 )
    return AO_UNDEFINED_DATAITEM;
  char tc = di.at(0);
  return reinterpret_cast<typeCode_t &>( tc );
}

#include "aotime.h"
#include "bytearraylong.h"
#include "bytearrayshort.h"
#include "hash256.h"
#include "hash512.h"
#include "index.h"
#include "listsize.h"
#include "netaddress.h"
#include "note.h"
#include "publickeyecdsa.h"
#include "publickeyrsa3072.h"
#include "salt256.h"
#include "shares.h"
#include "sharesout.h"
#include "sharestate.h"
#include "sigecdsa.h"
#include "sigrsa3072.h"
#include "assignment.h"
#include "assignref.h"
#include "authorization.h"
#include "blockref.h"
#include "genesisblock.h"
#include "genesisref.h"
#include "keypair.h"
#include "keyvaluepair.h"
#include "pageref.h"
#include "participant.h"
#include "sharesref.h"
#include "signature.h"

/**
 * @brief fromDataItem
 * @param di - serialized form of a data item
 * @return a de-serialized "live" DataItem of the type found in the byte array
 */
DataItem DataItem::fromDataItem( const QByteArray &di )
{ switch ( typeCodeOf( di ) )
    { case AO_TIME_OF_SIG:
      case AO_TIME_RECORDED:
      case AO_RECORDING_DEADLINE:
      case AO_UNDERWRITING_EXPIRATION: return AOTime( di );
      case AO_LONGBYTEARRAY:           return ByteArrayLong( di );
      case AO_SHORTBYTEARRAY:          return ByteArrayShort( di );
      case AO_HASH256:                 return Hash256( di );
      case AO_HASH512:                 return Hash512( di );
      case AO_INDEX:                   return Index( di );
      case AO_LISTSIZE:                return ListSize( di );
      case AO_NETADDRESS:              return NetAddress( di );
      case AO_NOTE:                    return Note( di );
      case AO_ECDSA_PUB_KEY2:
      case AO_ECDSA_PUB_KEY3:          return PublicKeyEcdsa( di );
      case AO_RSA3072_PUB_KEY:         return PublicKeyRsa3072( di );
      case AO_SALT256:                 return Salt256( di );
      case AO_ASSIGNMENT_AMT:
      case AO_UNDERWRITING_AMT:
      case AO_RECORDING_BID:           return Shares( di );
      case AO_SHARES_OUT:              return SharesOut( di );
      case AO_SHARE_STATE:             return ShareState( di );
      case AO_ECDSA_SIG:               return SigEcdsa( di );
      case AO_RSA3072_SIG:             return SigRsa3072( di );
      case AO_ASSIGNMENT:              return Assignment( di );
      case AO_ASSIGN_REF:              return AssignRef( di );
      case AO_AUTHORIZATION:           return Authorization( di );
      case AO_BLOCK_REF:               return BlockRef( di );
      case AO_GENESIS_BLOCK:           return GenesisBlock( di );
      case AO_GENESIS_REF:             return GenesisRef( di );
      case AO_KEYPAIR:                 return KeyPair( di );
      case AO_KEYVALUEPAIR:            return KeyValuePair( di );
      case AO_PAGE_REF:                return PageRef( di );
      case AO_PARTICIPANT_CF:
      case AO_PARTICIPANT:             return Participant( di );
      case AO_SHARES_REF:              return SharesRef( di );
      case AO_SIG_WITH_TIME:           return Signature( di );
    }
  return DataItem();
}

