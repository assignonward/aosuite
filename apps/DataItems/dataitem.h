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
#ifndef DATAITEM_H
#define DATAITEM_H

#define INCLUDE_TESTS

#include <QObject>
#include "dataitemba.h"
#include "varsizecode.h"

#define typeCode_t quint32
#define AO_SEPARABLE_TYPE          0x40 // Types with this bit set are separable

#define AO_ECDSA_PUB_KEY2          0x02 // (02+32) PublicKeyEcdsa: ECDSA Public Key, type 2 compressed
#define AO_ECDSA_PUB_KEY3          0x03 // (03+32) PublicKeyEcdsa: ECDSA Public Key, type 3 compressed
#define AO_HASH256                 0x06 // (06+32) Hash256: SHA2-256 hash of some data
#define AO_SALT256                 0x07 // (07+32) Salt256: 256 random bits
#define AO_PUB_RSA3072_ID          0x08 // (08+32) Hash256: Hash256 of a PublicKeyRsa3072
#define AO_HASH224SALT32           0x0c // (0c+32) Hash224Salt32: Hash224 of some data including a 32 bit salt
#define AO_HASH512                 0x10 // (10+64) Hash512: SHA3-512 hash of some data
#define AO_TIME_OF_SIG             0x20 // (20+16) AOTime: UTC time (secs since epoch << 64) when a signature was made
#define AO_TIME_RECORDED           0x21 // (21+16) AOTime: UTC time (secs since epoch << 64) when a record was made
#define AO_RECORDING_DEADLINE      0x22 // (22+16) AOTime: UTC time (secs since epoch << 64) when a record is contracted to be recorded
#define AO_TIME_DIFF               0x23 // (23+16) AOTime: UTC time (secs since epoch << 64) defining a time interval, or difference
#define AO_UNDERWRITING_EXPIRATION 0x24 // (24+16) AOTime: UTC time (secs since epoch << 64) when underwriting shares are bound until
#define AO_ASSIGNMENT_AMT          0x25 // (25+16) Shares: 128 bit signed integer number of shares assigned
#define AO_UNDERWRITING_AMT        0x26 // (26+16) Shares: 128 bit signed integer number of shares committed as underwriting
#define AO_RECORDING_BID           0x27 // (27+16) Shares: 128 bit signed integer number of shares bid for recording
#define AO_SHARES_OUT              0x28 // (28+16) Shares: 128 bit signed integer number of shares outstanding (recorded on blocks)
#define AO_N_COINS                 0x2c // (2c+16) AOCoins: Number of coins, as a fixed point 64.64 bit number
#define AO_SHARE_STATE             0x2f // (2f+1) ShareState: 8 bit signed integer declares the state of shares (available, under contract, contract executed (assigned away), committed as underwriting)
#define AO_LISTSIZE                0x30 // (30+2) Data16: 16 bit signed integer declares the size of a list, as a check that all intended data is present.
#define AO_INDEX                   0x31 // (31+2) Data16: 16 bit signed integer declares position of an element in a list, used to check/correlate two lists with each other.
#define AO_RSA3072_PUB_KEY         0x34 // (34+384) PublicKeyRsa3072: an RSA3072 public key
#define AO_RSA3072_SIG             0x35 // (35+384) SigRsa3072: an RSA3072 signature
#define AO_ECDSA_SIG               0x36 // (36+var) SigEcdsa: An ECDSA signature
#define AO_ASSIGNMENT              0x0a // (0a+var) Assignment: Shares Assignment agreement before signatures
#define AO_PARTICIPANT             0x2a // (2a+var) Participant: Participant in a Shares Assignment agreement
#define AO_PARTICIPANT_CF          0x2b // (2b+var) Participant: Participant in a Shares Assignment agreement, compact (aka chain) form
#define AO_AUTHORIZATION           0x1f // (1f+var) Authorization: An assignment plus a list of signatures on the authorization
#define AO_ASSIGN_REF              0x1e // (1e+var) AssignRef: Describes a record of shares when they were signed away in a binding contract
#define AO_DATABYTEARRAY           0x4a // (4a+var) DataByteArray:separable Arbitrary data of any form, may contain NULL bytes, any length (within reason)
#define AO_NOTE                    0x4b // (4b+var) Note:separable UTF-8 free text, no specific function, but recorded in the blockchain
#define AO_BLOCK_REF               0x1d // (1d+var) BlockRef: Describes a whole block in the chain
#define AO_PAGE_REF                0x1c // (1c+var) PageRef: UTF-8 free text, no specific function, but recorded in the blockchain
#define AO_GENESIS_REF             0x1b // (1b+var) GenesisRef: Uniquely describes a genesis block, includes list of properties used to calculate new blocks
#define AO_SHARES_REF              0x19 // (19+var) SharesRef: Reference to shares received potentially including info on their current state in the chain
#define AO_ASSETS                  0x18 // (18+var) Assets: A collection of lists of addresses for other asset organizers and recorders, references to shares, and unused keypairs
#define AO_ECDSA_PRI_KEY           0x60 // (60+var) PrivateKeyEcdsa:separable An ECDSA private key
#define AO_RSA3072_PRI_KEY         0x61 // (61+var) PrivateKeyRsa3072:separable An RSA3072 private key
#define AO_KEYPAIR                 0x62 // (62+var) KeyPair:separable A (hopefully matching) public-private key pair
#define AO_NETADDRESS              0x6a // (6a+var) NetAddress:separable IP4 or IP6 or named network contact address, potentially including :port number
#define AO_ORGANIZER               0x80 // (8001+var) Organizer: Contact information for an asset organizer (user software)
#define AO_RECORDER                0x81 // (8101+var) Recorder: Contact information for a recorder (chainmaker software)
#define CB_CHAIN_BLOCK             0x37 // (37+var) GenericCollection: A full chain block, including all data - though potentially censored as required by local laws
#define CB_BLOCKMAKER              0x38 // (38+var) PubKey: ID (public key) of the blockmaker, which holds claim on the residual shares in the block
#define CB_BLOCK_SIG               0x39 // (39+var) Signature: Blockmaker's signature, includes a AO_TIME_OF_SIG time and CB_BLOCKMAKER PubKey
#define AO_AUTH_SIG                0x3a // (3a+var) Signature: Authorization signature, includes a AO_TIME_OF_SIG time and AO_INDEX index of the participant doing the signing
#define AO_SIG_WITH_TIME           0x3b // (3b+var) Signature: Generic signature with All signatures include AO_TIME_OF_SIG time of signature, but no Index or PubKey
#define GB_GENESIS_BLOCK           0x87 // (8701+var) GenericCollection: A full Genesis block, including superfluous identifiers (text, images) to help brand/identify it
#define GB_PROTOCOL                0x107 // (8702+2) Data16: Basic level of protocol, identifies functionality
#define GB_PROTOCOL_REV            0x187 // (8703+2) Data16: Revision of the protocol, may indicate additional types supported
#define GB_TEXT_SYMBOL             0x1807 // (8730+var) Note: Short unique symbol that uniquely identifies the chain e.g. TⒶ1a
#define GB_DESCRIPTION             0x18c7 // (c731+var) Note:separable Text description of the chain
#define GB_ICON                    0x2047 // (c740+var) DataByteArray:separable Image suitable for icon use to represent the chain
#define GB_IMAGE                   0x20c7 // (c741+var) DataByteArray:separable Large format image to represent the chain
#define GB_STARTING_SHARES         0x807 // (8710+16) Shares: Starting number of shares
#define GB_MIN_BLOCK_INT           0x887 // (8711+16) AOTime: Minimuim block interval time
#define GB_N_COINS_TOTAL           0x907 // (8712+16) AOCoins: Number of coins that the sum of all shares outstanding represents
#define GB_RECORDING_TAX           0x987 // (8713+16) AOCoins: Recording Tax in coins per byte (usually a very small number)
#define AO_UNDEFINED_DATAITEM      0x3f // (3f+0) DataItem: An undefined data item, usually an error

class DataItem : public QObject, public VarSizeCode
{
    Q_OBJECT
public:
           explicit  DataItem( typeCode_t tc = AO_UNDEFINED_DATAITEM, QObject *p = NULL )
                       : QObject(p), typeCode( tc ) {}
                     DataItem( const DataItem &i, QObject *p = NULL )
                       : QObject(p ? p : i.parent()), typeCode( i.typeCode ) {}
               void  operator = ( const DataItem &i )
                       { typeCode = i.typeCode; }
       virtual void  operator = ( const DataItemBA &di ) { typeCode = typeCodeOf(di); }
             qint32  typeSize( typeCode_t tc = AO_UNDEFINED_DATAITEM ) const;
             qint32  typeSize( const DataItemBA &di ) const;
  static   DataItem *fromDataItem( const DataItemBA &di, QObject *p = NULL );
  static   DataItem *fromDataItem( const DataItem *di, QObject *p = NULL );
  static     qint32  typeSizeTable( typeCode_t tc );
  static typeCode_t  typeCodeOf( const DataItemBA &di );
 virtual DataItemBA  toDataItem( bool cf = false ) const;
 virtual DataItemBA  toHashData( bool cf = false ) const;
         DataItemBA  getHash( typeCode_t ht = AO_HASH256 ) const;
               bool  verifyHash( const DataItemBA &hdi ) const;
         typeCode_t  getTypeCode() const { return typeCode; }
               void  setTypeCode( const typeCode_t &tc ) { typeCode = tc; }

// Testing code
#ifdef INCLUDE_TESTS
               bool  testHashVerify();
               bool  testHashVerifyType( typeCode_t ht );
#endif

protected:
  typeCode_t  typeCode; // what kind of data item is this?
};

#endif // DATAITEM_H
