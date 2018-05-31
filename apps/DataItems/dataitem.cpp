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
#include "varsizecode.h"

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
    { case AO_SIZE_2BYTES  : return   2;
      case AO_SIZE_3BYTES  : return   3;
      case AO_SIZE_17BYTES : return  17;
      case AO_SIZE_34BYTES : return  34;
      case AO_SIZE_66BYTES : return  66;
      case AO_SIZE_388BYTES: return 388;
    }
  return -1;
}

/**
 * @brief DataItem::typeSize
 * @param di - byte array to interpret size of the first item in,
 *   byte array starts with the variable length type code and if the
 *   type code is not for a fixed length data item, then the variable
 *   length value is decoded and returned.
 */
qint32 DataItem::typeSize( const QByteArray &di ) const
{ if ( di.size() < 1 )
    return -1;
  int tcSz = 0;
  typeCode_t tc = bytesToCode( di, tcSz );
  qint32 sz = typeSizeTable( tc ); // See if fixed size, or variable, based on type
  if ( sz >= 0 )
    return sz; // fixed size
  int i = 0;
  return bytesToCode( di.mid( tcSz ), i ); // Variable size
}

/**
 * @brief DataItem::typeCodeOf
 * @param di - data item to interpret
 * @return typecode of the passed data item
 */
typeCode_t DataItem::typeCodeOf( const QByteArray &di )
{ if ( di.size() < 1 )
    return AO_UNDEFINED_DATAITEM;
  int i;
  return bytesToCode( di, i );
}

#include "aocoins.h"
#include "aotime.h"
#include "databytearray.h"
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
DataItem *DataItem::fromDataItem( const QByteArray &di, QObject *p )
{ switch ( typeCodeOf( di ) )
    { case AO_N_COINS:                 return new AOCoins( di, p );
      case AO_TIME_OF_SIG:
      case AO_TIME_RECORDED:
      case AO_RECORDING_DEADLINE:
      case AO_TIME_DIFF:
      case AO_UNDERWRITING_EXPIRATION: return new AOTime( di, p );
      case AO_DATABYTEARRAY:           return new DataByteArray( di, p );
      case AO_HASH256:                 return new Hash256( di, p );
      case AO_HASH512:                 return new Hash512( di, p );
      case AO_INDEX:                   return new Index( di, p );
      case AO_LISTSIZE:                return new ListSize( di, p );
      case AO_NETADDRESS:              return new NetAddress( di, p );
      case AO_NOTE:                    return new Note( di, p );
      case AO_ECDSA_PUB_KEY2:
      case AO_ECDSA_PUB_KEY3:          return new PublicKeyEcdsa( di, p );
      case AO_RSA3072_PUB_KEY:         return new PublicKeyRsa3072( di, p );
      case AO_SALT256:                 return new Salt256( di, p );
      case AO_ASSIGNMENT_AMT:
      case AO_UNDERWRITING_AMT:
      case AO_RECORDING_BID:           return new Shares( di, p );
      case AO_SHARES_OUT:              return new SharesOut( di, p );
      case AO_SHARE_STATE:             return new ShareState( di, p );
      case AO_ECDSA_SIG:               return new SigEcdsa( di, p );
      case AO_RSA3072_SIG:             return new SigRsa3072( di, p );
      case AO_ASSIGNMENT:              return new Assignment( di, p );
      case AO_ASSIGN_REF:              return new AssignRef( di, p );
      case AO_AUTHORIZATION:           return new Authorization( di, p );
      case AO_BLOCK_REF:               return new BlockRef( di, p );
      case AO_GENESIS_BLOCK:           return new GenesisBlock( di, p );
      case AO_GENESIS_REF:             return new GenesisRef( di, p );
      case AO_KEYPAIR:                 return new KeyPair( di, p );
      case AO_KEYVALUEPAIR:            return new KeyValuePair( di, p );
      case AO_PAGE_REF:                return new PageRef( di, p );
      case AO_PARTICIPANT_CF:
      case AO_PARTICIPANT:             return new Participant( di, p );
      case AO_SHARES_REF:              return new SharesRef( di, p );
      case AO_SIG_WITH_TIME:           return new Signature( di, p );
    }
  return new DataItem( AO_UNDEFINED_DATAITEM, p );
}

DataItem *DataItem::fromDataItem( const DataItem *ditm, QObject *p )
{ switch ( ditm->typeCode )
    { case AO_N_COINS:                 return new AOCoins( *((AOCoins *)ditm), p );
      case AO_TIME_OF_SIG:
      case AO_TIME_RECORDED:
      case AO_RECORDING_DEADLINE:
      case AO_TIME_DIFF:
      case AO_UNDERWRITING_EXPIRATION: return new AOTime( *((AOTime *)ditm), p );
      case AO_DATABYTEARRAY:           return new DataByteArray( *((DataByteArray *)ditm), p );
      case AO_HASH256:                 return new Hash256( *((Hash256 *)ditm), p );
      case AO_HASH512:                 return new Hash512( *((Hash512 *)ditm), p );
      case AO_INDEX:                   return new Index( *((Index *)ditm), p );
      case AO_LISTSIZE:                return new ListSize( *((ListSize *)ditm), p );
      case AO_NETADDRESS:              return new NetAddress( *((NetAddress*)ditm), p );
      case AO_NOTE:                    return new Note( *((Note *)ditm), p );
      case AO_ECDSA_PUB_KEY2:
      case AO_ECDSA_PUB_KEY3:          return new PublicKeyEcdsa( *((PublicKeyEcdsa *)ditm), p );
      case AO_RSA3072_PUB_KEY:         return new PublicKeyRsa3072( *((PublicKeyRsa3072 *)ditm), p );
      case AO_SALT256:                 return new Salt256( *((Salt256 *)ditm), p );
      case AO_ASSIGNMENT_AMT:
      case AO_UNDERWRITING_AMT:
      case AO_RECORDING_BID:           return new Shares( *((Shares *)ditm), p );
      case AO_SHARES_OUT:              return new SharesOut( *((SharesOut *)ditm), p );
      case AO_SHARE_STATE:             return new ShareState( *((ShareState *)ditm), p );
      case AO_ECDSA_SIG:               return new SigEcdsa( *((SigEcdsa *)ditm), p );
      case AO_RSA3072_SIG:             return new SigRsa3072( *((SigRsa3072 *)ditm), p );
      case AO_ASSIGNMENT:              return new Assignment( *((Assignment *)ditm), p );
      case AO_ASSIGN_REF:              return new AssignRef( *((AssignRef *)ditm), p );
      case AO_AUTHORIZATION:           return new Authorization( *((Authorization *)ditm), p );
      case AO_BLOCK_REF:               return new BlockRef( *((BlockRef *)ditm), p );
      case AO_GENESIS_BLOCK:           return new GenesisBlock( *((GenesisBlock *)ditm), p );
      case AO_GENESIS_REF:             return new GenesisRef( *((GenesisRef *)ditm), p );
      case AO_KEYPAIR:                 return new KeyPair( *((KeyPair *)ditm) );
      case AO_KEYVALUEPAIR:            return new KeyValuePair( *((KeyValuePair *)ditm), p );
      case AO_PAGE_REF:                return new PageRef( *((PageRef *)ditm), p );
      case AO_PARTICIPANT_CF:
      case AO_PARTICIPANT:             return new Participant( *((Participant *)ditm), p );
      case AO_SHARES_REF:              return new SharesRef( *((SharesRef *)ditm), p );
      case AO_SIG_WITH_TIME:           return new Signature( *((Signature *)ditm), p );
    }
  return new DataItem( AO_UNDEFINED_DATAITEM, p );
}
