#include "aocoins.h"
#include "aotime.h"
#include "assets.h"
#include "databytearray.h"
#include "datavbc64.h"
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
{ // qDebug( "making data item type 0x%x size %d", typeCodeOf(di), di.size() );
  switch ( typeCodeOf( di ) )
  { 
      case AO_INDEXV:                  return new DataVbc64( di, p );
      case AO_ECDSA_PUB_KEY2:          return new PublicKeyEcdsa( di, p );
      case AO_ECDSA_PUB_KEY3:          return new PublicKeyEcdsa( di, p );
      case AO_ECDSA_PUB_KEY4:          return new PublicKeyEcdsa( di, p );
      case AO_HASH512:                 return new Hash512( di, p );
      case AO_HASH256:                 return new Hash256( di, p );
      case AO_SALT256:                 return new Salt256( di, p );
      case AO_PUB_RSA3072_ID:          return new Hash256( di, p );
      case AO_HASH224SALT32:           return new Hash224Salt32( di, p );
      case AO_TIME_OF_SIG:             return new AOTime( di, p );
      case AO_PROPOSAL_TIME:           return new AOTime( di, p );
      case AO_RECORDING_DEADLINE:      return new AOTime( di, p );
      case AO_TIME_RECORDED:           return new AOTime( di, p );
      case AO_TIME_DIFF:               return new AOTime( di, p );
      case AO_UNDERWRITING_EXPIRATION: return new AOTime( di, p );
      case AO_AMT:                     return new Shares( di, p );
      case AO_UNDERWRITING_AMT:        return new Shares( di, p );
      case AO_RECORDING_BID:           return new Shares( di, p );
      case AO_SHARES_OUT:              return new Shares( di, p );
      case AO_N_COINS:                 return new AOCoins( di, p );
      case AO_SHARE_STATE:             return new DataVbc64( di, p );
      case AO_LISTSIZE:                return new DataVbc64( di, p );
      case AO_INDEX:                   return new DataVbc64( di, p );
      case CB_FIRST_ID_SEQ_NUM:        return new DataVbc64( di, p );
      case CB_N_ID_SEQ_NUM:            return new DataVbc64( di, p );
      case AO_ID_SEQ_NUM:              return new DataVbc64( di, p );
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
      case AO_BLOCK_REF:               return new BlockRef( di, p );
      case AO_PAGE_REF:                return new PageRef( di, p );
      case AO_GENESIS_REF:             return new GenesisRef( di, p );
      case AO_KEY_INDEX:               return new DataVbc64( di, p );
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
      case GB_PROTOCOL:                return new DataVbc64( di, p );
      case GB_PROTOCOL_REV:            return new DataVbc64( di, p );
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
  { 
      case AO_INDEXV:                  return new DataVbc64( *((DataVbc64 *)ditm), p );
      case AO_ECDSA_PUB_KEY2:          return new PublicKeyEcdsa( *((PublicKeyEcdsa *)ditm), p );
      case AO_ECDSA_PUB_KEY3:          return new PublicKeyEcdsa( *((PublicKeyEcdsa *)ditm), p );
      case AO_ECDSA_PUB_KEY4:          return new PublicKeyEcdsa( *((PublicKeyEcdsa *)ditm), p );
      case AO_HASH512:                 return new Hash512( *((Hash512 *)ditm), p );
      case AO_HASH256:                 return new Hash256( *((Hash256 *)ditm), p );
      case AO_SALT256:                 return new Salt256( *((Salt256 *)ditm), p );
      case AO_PUB_RSA3072_ID:          return new Hash256( *((Hash256 *)ditm), p );
      case AO_HASH224SALT32:           return new Hash224Salt32( *((Hash224Salt32 *)ditm), p );
      case AO_TIME_OF_SIG:             return new AOTime( *((AOTime *)ditm), p );
      case AO_PROPOSAL_TIME:           return new AOTime( *((AOTime *)ditm), p );
      case AO_RECORDING_DEADLINE:      return new AOTime( *((AOTime *)ditm), p );
      case AO_TIME_RECORDED:           return new AOTime( *((AOTime *)ditm), p );
      case AO_TIME_DIFF:               return new AOTime( *((AOTime *)ditm), p );
      case AO_UNDERWRITING_EXPIRATION: return new AOTime( *((AOTime *)ditm), p );
      case AO_AMT:                     return new Shares( *((Shares *)ditm), p );
      case AO_UNDERWRITING_AMT:        return new Shares( *((Shares *)ditm), p );
      case AO_RECORDING_BID:           return new Shares( *((Shares *)ditm), p );
      case AO_SHARES_OUT:              return new Shares( *((Shares *)ditm), p );
      case AO_N_COINS:                 return new AOCoins( *((AOCoins *)ditm), p );
      case AO_SHARE_STATE:             return new DataVbc64( *((DataVbc64 *)ditm), p );
      case AO_LISTSIZE:                return new DataVbc64( *((DataVbc64 *)ditm), p );
      case AO_INDEX:                   return new DataVbc64( *((DataVbc64 *)ditm), p );
      case CB_FIRST_ID_SEQ_NUM:        return new DataVbc64( *((DataVbc64 *)ditm), p );
      case CB_N_ID_SEQ_NUM:            return new DataVbc64( *((DataVbc64 *)ditm), p );
      case AO_ID_SEQ_NUM:              return new DataVbc64( *((DataVbc64 *)ditm), p );
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
      case AO_BLOCK_REF:               return new BlockRef( *((BlockRef *)ditm), p );
      case AO_PAGE_REF:                return new PageRef( *((PageRef *)ditm), p );
      case AO_GENESIS_REF:             return new GenesisRef( *((GenesisRef *)ditm), p );
      case AO_KEY_INDEX:               return new DataVbc64( *((DataVbc64 *)ditm), p );
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
      case GB_PROTOCOL:                return new DataVbc64( *((DataVbc64 *)ditm), p );
      case GB_PROTOCOL_REV:            return new DataVbc64( *((DataVbc64 *)ditm), p );
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

/**
 * @brief DataItem::typeSizeTable - for fixed size types, return the number of data bytes that follow the typeCode
 * @param tc - typecode to interpret
 * @return size of type, if it is a known fixed size type, otherwise return -1
 */
qint32 DataItem::typeSizeTable( typeCode_t tc )
{ switch ( tc )
  {
      case AO_INDEXV:                  return -2;
      case AO_ECDSA_PUB_KEY2:          return 32;
      case AO_ECDSA_PUB_KEY3:          return 32;
      case AO_ECDSA_PUB_KEY4:          return 64;
      case AO_HASH512:                 return 64;
      case AO_HASH256:                 return 32;
      case AO_SALT256:                 return 32;
      case AO_PUB_RSA3072_ID:          return 32;
      case AO_HASH224SALT32:           return 32;
      case AO_TIME_OF_SIG:             return 16;
      case AO_PROPOSAL_TIME:           return 16;
      case AO_RECORDING_DEADLINE:      return 16;
      case AO_TIME_RECORDED:           return 16;
      case AO_TIME_DIFF:               return 16;
      case AO_UNDERWRITING_EXPIRATION: return 16;
      case AO_AMT:                     return 16;
      case AO_UNDERWRITING_AMT:        return 16;
      case AO_RECORDING_BID:           return 16;
      case AO_SHARES_OUT:              return 16;
      case AO_N_COINS:                 return 16;
      case AO_SHARE_STATE:             return -2;
      case AO_LISTSIZE:                return -2;
      case AO_INDEX:                   return -2;
      case CB_FIRST_ID_SEQ_NUM:        return -2;
      case CB_N_ID_SEQ_NUM:            return -2;
      case AO_ID_SEQ_NUM:              return -2;
      case AO_RSA3072_PUB_KEY:         return 384;
      case AO_RSA3072_SIG:             return 384;
      case AO_KEY_INDEX:               return -2;
      case GB_PROTOCOL:                return -2;
      case GB_PROTOCOL_REV:            return -2;
      case GB_STARTING_SHARES:         return 16;
      case GB_MIN_BLOCK_INT:           return 16;
      case GB_N_COINS_TOTAL:           return 16;
      case GB_RECORDING_TAX:           return 16;
      case AO_UNDEFINED_DATAITEM:      return 0;
  }
  return -1;
}
