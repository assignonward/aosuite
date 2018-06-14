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
#include "hash.h"
#include "varsizecode.h"

/**
 * @brief DataItem::typeCodeOf
 * @param di - data item to interpret
 * @return typecode of the passed data item
 */
typeCode_t DataItem::typeCodeOf( const DataItemBA &di )
{ if ( di.size() < 1 )
    return AO_UNDEFINED_DATAITEM;
  int i;
  return bytesToCode( di, i );
}

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

/**
 * @brief DataItem::typeSizeTable - for fixed size types, return the number of data bytes that follow the typeCode
 * @param tc - typecode to interpret
 * @return size of type, if it is a known fixed size type, otherwise return -1
 */
qint32 DataItem::typeSizeTable( typeCode_t tc )
{ switch ( tc )
  { case AO_ECDSA_PUB_KEY2:          return 32;
    case AO_ECDSA_PUB_KEY3:          return 32;
    case AO_ECDSA_PUB_KEY4:          return 64;
    case AO_HASH512:                 return 64;
    case AO_HASH256:                 return 32;
    case AO_SALT256:                 return 32;
    case AO_PUB_RSA3072_ID:          return 32;
    case AO_HASH224SALT32:           return 32;
    case AO_TIME_OF_SIG:             return 16;
    case AO_TIME_RECORDED:           return 16;
    case AO_RECORDING_DEADLINE:      return 16;
    case AO_TIME_DIFF:               return 16;
    case AO_UNDERWRITING_EXPIRATION: return 16;
    case AO_ASSIGNMENT_AMT:          return 16;
    case AO_UNDERWRITING_AMT:        return 16;
    case AO_RECORDING_BID:           return 16;
    case AO_SHARES_OUT:              return 16;
    case AO_N_COINS:                 return 16;
    case AO_SHARE_STATE:             return 1;
    case AO_LISTSIZE:                return 2;
    case AO_INDEX:                   return 2;
    case CB_FIRST_ID_SEQ_NUM:        return 8;
    case CB_N_ID_SEQ_NUM:            return 2;
    case AO_ID_SEQ_NUM:              return 8;
    case AO_RSA3072_PUB_KEY:         return 384;
    case AO_RSA3072_SIG:             return 384;
    case AO_INDEX32:                 return 4;
    case AO_KEY_INDEX:               return 8;
    case GB_PROTOCOL:                return 2;
    case GB_PROTOCOL_REV:            return 2;
    case GB_STARTING_SHARES:         return 16;
    case GB_MIN_BLOCK_INT:           return 16;
    case GB_N_COINS_TOTAL:           return 16;
    case GB_RECORDING_TAX:           return 16;
    case AO_UNDEFINED_DATAITEM:      return 0;
  }
  return -1;
}

/**
 * @brief DataItem::typeSize
 * @param di - byte array to interpret size of the first item in,
 *   byte array starts with the variable length type code and if the
 *   type code is not for a fixed length data item, then the variable
 *   length value is decoded and returned.
 * @return total size of this data item, including typeCode and size data, if any
 */
qint32 DataItem::typeSize( const DataItemBA &di ) const
{ if ( di.size() < 1 )
    return -1;
  qint32 tcSz = 0; // Size of the typeCode
  typeCode_t tc = bytesToCode( di, tcSz );
  qint32 fdSz = typeSizeTable( tc ); // See if fixed size, or variable, based on type
  if ( fdSz >= 0 )
    return tcSz+fdSz; // Total size of the data item's byte array, including type code and its fixed size data
  qint32 scSz = 0; // Size of the size code
  qint32 vdSz = bytesToCode( di.mid( tcSz ), scSz ); // Size of the variable data
  return tcSz+scSz+vdSz; // Total size of the data item's byte array, including type and size codes
}

#include "aocoins.h"
#include "aotime.h"
#include "assets.h"
#include "databytearray.h"
#include "data32.h"
#include "data64.h"
#include "hash224salt32.h"
#include "hash256.h"
#include "hash512.h"
#include "netaddress.h"
#include "note.h"
#include "publickeyecdsa.h"
#include "publickeyrsa3072.h"
#include "salt256.h"
#include "shares.h"
#include "sigecdsa.h"
#include "sigrsa3072.h"
#include "assignment.h"
#include "assignref.h"
#include "authorization.h"
#include "blockref.h"
#include "genericcollection.h"
#include "genesisref.h"
#include "keypair.h"
#include "pageref.h"
#include "participant.h"
#include "sharesref.h"
#include "signature.h"

/**
 * @brief fromDataItem
 * @param di - serialized form of a data item
 * @return a de-serialized "live" DataItem of the type found in the byte array
 */
DataItem *DataItem::fromDataItem( const DataItemBA &di, QObject *p )
{ switch ( typeCodeOf( di ) )
  { case AO_ECDSA_PUB_KEY2:          return new PublicKeyEcdsa( di, p );
    case AO_ECDSA_PUB_KEY3:          return new PublicKeyEcdsa( di, p );
    case AO_ECDSA_PUB_KEY4:          return new PublicKeyEcdsa( di, p );
    case AO_HASH512:                 return new Hash512( di, p );
    case AO_HASH256:                 return new Hash256( di, p );
    case AO_SALT256:                 return new Salt256( di, p );
    case AO_PUB_RSA3072_ID:          return new Hash256( di, p );
    case AO_HASH224SALT32:           return new Hash224Salt32( di, p );
    case AO_TIME_OF_SIG:             return new AOTime( di, p );
    case AO_TIME_RECORDED:           return new AOTime( di, p );
    case AO_RECORDING_DEADLINE:      return new AOTime( di, p );
    case AO_TIME_DIFF:               return new AOTime( di, p );
    case AO_UNDERWRITING_EXPIRATION: return new AOTime( di, p );
    case AO_ASSIGNMENT_AMT:          return new Shares( di, p );
    case AO_UNDERWRITING_AMT:        return new Shares( di, p );
    case AO_RECORDING_BID:           return new Shares( di, p );
    case AO_SHARES_OUT:              return new Shares( di, p );
    case AO_N_COINS:                 return new AOCoins( di, p );
    case AO_SHARE_STATE:             return new Data8( di, p );
    case AO_LISTSIZE:                return new Data16( di, p );
    case AO_INDEX:                   return new Data16( di, p );
    case CB_FIRST_ID_SEQ_NUM:        return new Data64( di, p );
    case CB_N_ID_SEQ_NUM:            return new Data16( di, p );
    case AO_ID_SEQ_NUM:              return new Data64( di, p );
    case AO_RSA3072_PUB_KEY:         return new PublicKeyRsa3072( di, p );
    case AO_RSA3072_SIG:             return new SigRsa3072( di, p );
    case AO_ECDSA_SIG:               return new SigEcdsa( di, p );
    case AO_ASSIGNMENT:              return new Assignment( di, p );
    case AO_PARTICIPANT:             return new Participant( di, p );
    case AO_PARTICIPANT_CF:          return new Participant( di, p );
    case AO_AUTHORIZATION:           return new Authorization( di, p );
    case AO_ASSIGN_REF:              return new GenericCollection( di, p );
    case AO_DATABYTEARRAY:           return new DataByteArray( di, p );
    case AO_NOTE:                    return new Note( di, p );
    case AO_KEY_ASSET:               return new GenericCollection( di, p );
    case AO_INDEX32:                 return new Data32( di, p );
    case AO_BLOCK_REF:               return new BlockRef( di, p );
    case AO_PAGE_REF:                return new PageRef( di, p );
    case AO_GENESIS_REF:             return new GenesisRef( di, p );
    case AO_KEY_INDEX:               return new Data64( di, p );
    case AO_SHARES_REF:              return new SharesRef( di, p );
    case AO_ASSETS:                  return new Assets( di, p );
    case AO_ECDSA_PRI_KEY:           return new PrivateKeyEcdsa( di, p );
    case AO_RSA3072_PRI_KEY:         return new PrivateKeyRsa3072( di, p );
    case AO_KEYPAIR:                 return new KeyPair( di, p );
    case AO_NETADDRESS:              return new NetAddress( di, p );
    case AO_ORGANIZER:               return new Organizer( di, p );
    case AO_RECORDER:                return new Recorder( di, p );
    case CB_CHAIN_BLOCK:             return new GenericCollection( di, p );
    case CB_BLOCKMAKER:              return new PubKey( di, p );
    case CB_BLOCK_SIG:               return new Signature( di, p );
    case AO_AUTH_SIG:                return new Signature( di, p );
    case AO_SIG_WITH_TIME:           return new Signature( di, p );
    case GB_GENESIS_BLOCK:           return new GenericCollection( di, p );
    case GB_PROTOCOL:                return new Data16( di, p );
    case GB_PROTOCOL_REV:            return new Data16( di, p );
    case GB_TEXT_SYMBOL:             return new Note( di, p );
    case GB_DESCRIPTION:             return new Note( di, p );
    case GB_ICON:                    return new DataByteArray( di, p );
    case GB_IMAGE:                   return new DataByteArray( di, p );
    case GB_STARTING_SHARES:         return new Shares( di, p );
    case GB_MIN_BLOCK_INT:           return new AOTime( di, p );
    case GB_N_COINS_TOTAL:           return new AOCoins( di, p );
    case GB_RECORDING_TAX:           return new AOCoins( di, p );
  }
  return new DataItem( AO_UNDEFINED_DATAITEM, p );
}

DataItem *DataItem::fromDataItem( const DataItem *ditm, QObject *p )
{ switch ( ditm->typeCode )
  { case AO_ECDSA_PUB_KEY2:          return new PublicKeyEcdsa( *((PublicKeyEcdsa *)ditm), p );
    case AO_ECDSA_PUB_KEY3:          return new PublicKeyEcdsa( *((PublicKeyEcdsa *)ditm), p );
    case AO_ECDSA_PUB_KEY4:          return new PublicKeyEcdsa( *((PublicKeyEcdsa *)ditm), p );
    case AO_HASH512:                 return new Hash512( *((Hash512 *)ditm), p );
    case AO_HASH256:                 return new Hash256( *((Hash256 *)ditm), p );
    case AO_SALT256:                 return new Salt256( *((Salt256 *)ditm), p );
    case AO_PUB_RSA3072_ID:          return new Hash256( *((Hash256 *)ditm), p );
    case AO_HASH224SALT32:           return new Hash224Salt32( *((Hash224Salt32 *)ditm), p );
    case AO_TIME_OF_SIG:             return new AOTime( *((AOTime *)ditm), p );
    case AO_TIME_RECORDED:           return new AOTime( *((AOTime *)ditm), p );
    case AO_RECORDING_DEADLINE:      return new AOTime( *((AOTime *)ditm), p );
    case AO_TIME_DIFF:               return new AOTime( *((AOTime *)ditm), p );
    case AO_UNDERWRITING_EXPIRATION: return new AOTime( *((AOTime *)ditm), p );
    case AO_ASSIGNMENT_AMT:          return new Shares( *((Shares *)ditm), p );
    case AO_UNDERWRITING_AMT:        return new Shares( *((Shares *)ditm), p );
    case AO_RECORDING_BID:           return new Shares( *((Shares *)ditm), p );
    case AO_SHARES_OUT:              return new Shares( *((Shares *)ditm), p );
    case AO_N_COINS:                 return new AOCoins( *((AOCoins *)ditm), p );
    case AO_SHARE_STATE:             return new Data8( *((Data8 *)ditm), p );
    case AO_LISTSIZE:                return new Data16( *((Data16 *)ditm), p );
    case AO_INDEX:                   return new Data16( *((Data16 *)ditm), p );
    case CB_FIRST_ID_SEQ_NUM:        return new Data64( *((Data64 *)ditm), p );
    case CB_N_ID_SEQ_NUM:            return new Data16( *((Data16 *)ditm), p );
    case AO_ID_SEQ_NUM:              return new Data64( *((Data64 *)ditm), p );
    case AO_RSA3072_PUB_KEY:         return new PublicKeyRsa3072( *((PublicKeyRsa3072 *)ditm), p );
    case AO_RSA3072_SIG:             return new SigRsa3072( *((SigRsa3072 *)ditm), p );
    case AO_ECDSA_SIG:               return new SigEcdsa( *((SigEcdsa *)ditm), p );
    case AO_ASSIGNMENT:              return new Assignment( *((Assignment *)ditm), p );
    case AO_PARTICIPANT:             return new Participant( *((Participant *)ditm), p );
    case AO_PARTICIPANT_CF:          return new Participant( *((Participant *)ditm), p );
    case AO_AUTHORIZATION:           return new Authorization( *((Authorization *)ditm), p );
    case AO_ASSIGN_REF:              return new GenericCollection( *((GenericCollection *)ditm), p );
    case AO_DATABYTEARRAY:           return new DataByteArray( *((DataByteArray *)ditm), p );
    case AO_NOTE:                    return new Note( *((Note *)ditm), p );
    case AO_KEY_ASSET:               return new GenericCollection( *((GenericCollection *)ditm), p );
    case AO_INDEX32:                 return new Data32( *((Data32 *)ditm), p );
    case AO_BLOCK_REF:               return new BlockRef( *((BlockRef *)ditm), p );
    case AO_PAGE_REF:                return new PageRef( *((PageRef *)ditm), p );
    case AO_GENESIS_REF:             return new GenesisRef( *((GenesisRef *)ditm), p );
    case AO_KEY_INDEX:               return new Data64( *((Data64 *)ditm), p );
    case AO_SHARES_REF:              return new SharesRef( *((SharesRef *)ditm), p );
    case AO_ASSETS:                  return new Assets( *((Assets *)ditm), p );
    case AO_ECDSA_PRI_KEY:           return new PrivateKeyEcdsa( *((PrivateKeyEcdsa *)ditm), p );
    case AO_RSA3072_PRI_KEY:         return new PrivateKeyRsa3072( *((PrivateKeyRsa3072 *)ditm), p );
    case AO_KEYPAIR:                 return new KeyPair( *((KeyPair *)ditm), p );
    case AO_NETADDRESS:              return new NetAddress( *((NetAddress *)ditm), p );
    case AO_ORGANIZER:               return new Organizer( *((Organizer *)ditm), p );
    case AO_RECORDER:                return new Recorder( *((Recorder *)ditm), p );
    case CB_CHAIN_BLOCK:             return new GenericCollection( *((GenericCollection *)ditm), p );
    case CB_BLOCKMAKER:              return new PubKey( *((PubKey *)ditm), p );
    case CB_BLOCK_SIG:               return new Signature( *((Signature *)ditm), p );
    case AO_AUTH_SIG:                return new Signature( *((Signature *)ditm), p );
    case AO_SIG_WITH_TIME:           return new Signature( *((Signature *)ditm), p );
    case GB_GENESIS_BLOCK:           return new GenericCollection( *((GenericCollection *)ditm), p );
    case GB_PROTOCOL:                return new Data16( *((Data16 *)ditm), p );
    case GB_PROTOCOL_REV:            return new Data16( *((Data16 *)ditm), p );
    case GB_TEXT_SYMBOL:             return new Note( *((Note *)ditm), p );
    case GB_DESCRIPTION:             return new Note( *((Note *)ditm), p );
    case GB_ICON:                    return new DataByteArray( *((DataByteArray *)ditm), p );
    case GB_IMAGE:                   return new DataByteArray( *((DataByteArray *)ditm), p );
    case GB_STARTING_SHARES:         return new Shares( *((Shares *)ditm), p );
    case GB_MIN_BLOCK_INT:           return new AOTime( *((AOTime *)ditm), p );
    case GB_N_COINS_TOTAL:           return new AOCoins( *((AOCoins *)ditm), p );
    case GB_RECORDING_TAX:           return new AOCoins( *((AOCoins *)ditm), p );
    case AO_UNDEFINED_DATAITEM:      return new DataItem( *((DataItem *)ditm), p );
  }
  return new DataItem( AO_UNDEFINED_DATAITEM, p );
}

DataItemBA  DataItem::toDataItem( bool cf ) const
{ qDebug( "DataItem:toDataItem ERROR!" );
  (void)cf; return DataItemBA();
} // Implemented in all child classes

/**
 * @brief DataItem::toHashData
 * @param cf - chain/compact form?
 * @return the data item as a type identified byte array, with separable items replaced
 *   with a salted hash.
 */
DataItemBA DataItem::toHashData( bool cf ) const
{ //qDebug( "DataItem:toHashData" );
  if ( typeCode & AO_SEPARABLE_TYPE )
    { Hash256 h;
      return h.calculate( toDataItem(cf) ).toDataItem(cf);
    }
  return toDataItem(cf);
}

/**
 * @brief DataItem::getHash - return the hash of this item
 * @param ht - type of hash to calculate
 * @return The hash value in a hash type identifying wrapper
 */
DataItemBA  DataItem::getHash( typeCode_t ht ) const
{ Hash hi( ht );
  hi.calculate( toHashData() );
  return hi.toDataItem();
}

/**
 * @brief DataItem::verifyHash
 * @param hdi - hash in a type identifying wrapper to verify against this item's current text
 * @return
 */
bool  DataItem::verifyHash( const DataItemBA &hdi ) const
{ Hash hi( hdi );
  return hi.verify( toHashData() );
}

/////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef INCLUDE_TESTS
/**
 * @brief DataItem::testHashVerify - create and verify all kinds of hashes, then modify the hash
 *   and verify that it does not successfully verify.
 * @return true if all tests pass
 */
bool DataItem::testHashVerify()
{ if ( !testHashVerifyType( AO_HASH256       ) ) return false;
  if ( !testHashVerifyType( AO_HASH512       ) ) return false;
  if ( !testHashVerifyType( AO_HASH224SALT32 ) ) return false;
  qDebug( "testHashVerify() PASS all types" );
  return true;
}

/**
 * @brief DataItem::testHashVerifyType - create and verify one kind of hash, then modify the hash
 *   and verify that it does not successfully verify.
 * @param ht - type of hash to test
 * @return true if all tests pass
 */
bool DataItem::testHashVerifyType( typeCode_t ht )
{ // qDebug( "testHashVerifyType( %x )", ht );
  // qDebug( "  data:%s",qPrintable( QString::fromUtf8( toDataItem().toHex() ) ) );
  // qDebug( "  sepr:%s",qPrintable( QString::fromUtf8( toHashData().toHex() ) ) );
  DataItemBA hdi = getHash( ht );
  // qDebug( "  hash:%s",qPrintable( QString::fromUtf8( hdi.toHex() ) ) );
  if ( verifyHash( hdi ) )
    { // qDebug( "  verified when hash is unaltered." );
    }
   else
    { qDebug( "  FAIL, did not verify." );
      return false;
    }
  // Single bit-flip tests
  for ( int i = 0 ; i < hdi.size() ; i++ )
    { for ( int j = 0; j < 8 ; j++ )
        { QByteArray hda = hdi;
          hda[i] = hda.at(i) ^ (0x01 << j);
          if ( verifyHash( hda ) )
            { qDebug( "  FAIL, verified with altered hash %d %d.",i,j );
              qDebug( "  altered hash:%s",qPrintable( QString::fromUtf8( hda.toHex() ) ) );
              return false;
            }
        }
    }
  // qDebug( "  no single bit flips verified." );
  if ( verifyHash( hdi ) )
    { // qDebug( "  re-verified unaltered hash." );
    }
   else
    { qDebug( "  FAIL, unaltered hash did not re-verify." );
      qDebug( "  hash:%s",qPrintable( QString::fromUtf8( hdi.toHex() ) ) );
      return false;
    }
  // qDebug( "  PASS type %x", ht );
  return true;
}

#endif // #ifdef INCLUDE_TESTS
