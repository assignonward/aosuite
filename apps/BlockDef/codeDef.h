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
#ifndef CODEDEF_H
#define CODEDEF_H

#define RDT_OBJECT 0x00 // o - in json: a comma separated list of zero or more key-value pairs json encases each pair in {}, all keys unique within the object, in bson there are no separators, a key of Obterm ends the current object
#define RDT_INT64 0x01 // i - 64 bit signed integer - json in an unquoted ASCII decimal string, bson as 8 bytes using void qToLittleEndian(qint64 src, uchar *dest)
#define RDT_RCODE 0x02 // y - Ricey code - json as the table defined name, bson as a list of bytes: last byte has 0 in most significant bit.
#define RDT_MPZ 0x03 // n - libGMP signed integer - json as a quoted ASCII decimal string, bson as an object carrying the native GMP values
#define RDT_MPQ 0x04 // r - libGMP rational fraction - json as a quoted ASCII decimal string/ASCII decimal string, bson as an object carrying the native GMP
#define RDT_BYTEARRAY 0x05 // b - ByteArray (aka octet stream) - json as hex string, bson as int32_t length followed by length octets
#define RDT_STRING 0x06 // s - UTF8 encoded string - json as UTF8 string, bson as int32_t length (includes terminator) followed by UTF8 string plus null terminator
#define RDT_NULL 0x0F // z - A null block value type, used as an error flag 
#define RDT_OBJECT_ARRAY 0x10 // O - array of zero or more objects encased in []
#define RDT_INT64_ARRAY 0x11 // I - array of zero or more 64 bit signed integers stored as above, comma separated in json, rice code size followed by the array in bson
#define RDT_RCODE_ARRAY 0x12 // Y - array of zero or more Ricey codes, quoted hexadecimal and comma separated in json, starts with a rice code size followed by the array in bson
#define RDT_MPZ_ARRAY 0x13 // N - array of zero or more libGMP signed integers stored as described above
#define RDT_MPQ_ARRAY 0x14 // R - array of zero or more libGMP rational fractions stored as described above
#define RDT_BYTEARRAY_ARRAY 0x15 // B - ByteArray (aka octet stream) - json as hex string, bson as int32_t length followed by length octets
#define RDT_STRING_ARRAY 0x16 // S - array of zero or more UTF-8 encoded strings - json as quoted, escaped UTF-8 string, bson as int32_t length followed by UTF-8 string, no null terminator
#define RDT_ARRAY 0x10 // * - this bit is always set in array types
#define RDT_TYPEMASK 0x0F // * - used to get the contained value type out of an array type 
#define RDT_OBTYPEMASK 0x1F // * - used to get the contained value type out of an object type

#define RCD_group_y                                11522 // groupDefGroup_y Identifies what group a definition belongs to
#define RCD_groupDefGroup_y                        11554 // groupDefGroup_y These codes are used to identify what group a code belongs to
#define RCD_dataGroup_y                            11586 // groupDefGroup_y Generic data group - many items simply not yet classified
#define RCD_protocolIdGroup_y                      11618 // groupDefGroup_y These codes identify established protocols
#define RCD_protocolActorGroup_y                   11650 // groupDefGroup_y These codes identify actors within protocols
#define RCD_algorithmIdGroup_y                     11682 // groupDefGroup_y These codes identify algorithms
#define RCD_structNavGroup_y                       11714 // groupDefGroup_y These codes direct protocol operations to operands
#define RCD_dictionaryGroup_y                      11746 // groupDefGroup_y These codes are used specifically for dictionary containers
#define RCD_chainGroup_y                           11778 // groupDefGroup_y These codes are used for chain structure items
#define RCD_operationGroup_y                       11810 // groupDefGroup_y These codes define calculation/operation types
#define RCD_protocolDefGroup_y                     11842 // groupDefGroup_y These codes are used in protocol definition
#define RCD_serviceDescriptors_y                   11874 // groupDefGroup_y These codes are used for service requests and responses
#define RCD_riceyTypes_O                         1336208 // dictionaryGroup_y An array object which contains the description, name and ricey code 4 LSB for each defined ricey code type in the system.
#define RCD_riceyCodes_O                         1336240 // dictionaryGroup_y An array object which contains the description, name and ricey code for each defined ricey code in the system.
#define RCD_ObTerm_o                                   0 // dataGroup_y Object terminator, indicates no more items in the object in bsonish lists
#define RCD_int64_i                                    1 // dataGroup_y Generic signed int64, use depends on context.
#define RCD_type_y                                     2 // dataGroup_y Identifies an algorithm type for hash or crypto key, or other types depending on context
#define RCD_mpz_n                                      3 // dataGroup_y Generic MPZ large integer, use depends on context.
#define RCD_mpq_r                                      4 // dataGroup_y Generic MPQ large integer fraction, use depends on context.
#define RCD_data_b                                     5 // dataGroup_y Generic data block, use determined by the object which contains it.
#define RCD_text_s                                     6 // dataGroup_y Generic string, use depends on context, for instance in an image object this would be the alt text
#define RCD_name_s                                    38 // dataGroup_y Name as a string, use depends on context, often a key name
#define RCD_desc_s                                    70 // dataGroup_y Description of the current object, specific use depends on context
#define RCD_code_s                                   102 // dataGroup_y Code as a string, use depends on context, often a key type
#define RCD_userId_b                                  37 // dataGroup_y Unique user identifier, possibly the 256bit hash of a userID_o that has all kinds of stuff in it
#define RCD_objectArray_O                             16 // dataGroup_y A generic array of object elements
#define RCD_int64Array_I                              17 // dataGroup_y A generic array of int64 elements
#define RCD_riceyArray_Y                              18 // dataGroup_y A generic array of Ricey elements
#define RCD_mpzArray_N                                19 // dataGroup_y A generic array of MPZ elements
#define RCD_mpqArray_R                                20 // dataGroup_y A generic array of MPQ elements
#define RCD_byteArrayArray_B                          21 // dataGroup_y A generic array of byte array element 
#define RCD_stringArray_S                             22 // dataGroup_y A generic array of string elements
#define RCD_time_i                                    33 // dataGroup_y UTC time of block creation (microseconds since epoch x 189). May never be less than or equal to the timei of any parent block. Serves as a unique id for AOBO blocks.
#define RCD_AOShares_n                                35 // dataGroup_y A quantity of shares - meaning depends on context
#define RCD_null_z                                    15 // dataGroup_y This key indicates the value is empty, has no value
#define RCD_chainBlock_o                            9600 // chainGroup_y An Assign Onward Chain Block. With a hash_o which includes a timei, and a hashedOb_o which includes exact copied of the hash_o(s) of the parent(s) 
#define RCD_hashedOb_o                                32 // chainGroup_y An object which has its hash computed. When found in a chainBlock_o always contains a parentHash_O a time_i of when the block was signed, the hash_o is found in the object which contains the hashedOb_o.
#define RCD_hash_o                                    96 // chainGroup_y Found in chainBlock_O and similar objects which contain a signedBlock_O, contains an algorithm id (type_y) and the hash by that algorithm of the binary representation of the objects in the signedBlock_O (data_b) and a timestamp (time_i) matching the time_i in the signedBlock_O and which must not be as old or older than any parent timestamp and also should not be in the future, these time_i are unique identifiers for chainBlock_O objects.
#define RCD_parentHash_O                             112 // chainGroup_y Contains copies of the hash_o of one or more (more in the case of merging forked chains) parent blocks.
#define RCD_separableItems_O                          48 // chainGroup_y List of items which are only indirectly included in the hash chain
#define RCD_separableItemsHashes_O                    80 // chainGroup_y Hashes of separable items which do get included in the hash chain
#define RCD_separatedItem_o                       131936 // chainGroup_y Included separable items are contained in hashedOb_o, when they have been separated this object container replaces them, along with optional information about the separated item (where it might be found, why it was removed, when it was removed, on whose authority it was removed, etc.)
#define RCD_SHA256_y                                  34 // algorithmIdGroup_y algorithm ID (algor) for SHA2 - 256
#define RCD_ECB256_y                                  66 // algorithmIdGroup_y algorithm ID (algor) for Elliptic Curve ECDSA curve BrainpoolP256r1
#define RCD_RSA3072_y                               4290 // algorithmIdGroup_y algorithm ID (algor) for 3072 bit RSA - less commonly used algorithms get longer ricey codes
#define RCD_SHA3b512_y                              4258 // algorithmIdGroup_y algorithm ID (algor) for SHA3 - 512
#define RCD_jpg_y                                   2082 // algorithmIdGroup_y Found in image objects, tells what format the data_b is to be interpreted as
#define RCD_png_y                                   2114 // algorithmIdGroup_y Found in image objects, tells what format the data_b is to be interpreted as
#define RCD_success_y                               8194 // dataGroup_y success code
#define RCD_failure_y                              14338 // dataGroup_y failure code
#define RCD_ProtocolDAO0_y                     195559426 // protocolIdGroup_y Development protocol, symbol: DⒶ0
#define RCD_ProtocolDAO1_y                     195559458 // protocolIdGroup_y Development protocol, symbol: DⒶ1
#define RCD_ProtocolDAO2_y                     195559490 // protocolIdGroup_y Development protocol, symbol: DⒶ2
#define RCD_ProtocolDAO3_y                     195559522 // protocolIdGroup_y Development protocol, symbol: DⒶ3
#define RCD_ProtocolDAO4_y                     195559554 // protocolIdGroup_y Development protocol, symbol: DⒶ4
#define RCD_ProtocolTAO1_y                      82313218 // protocolIdGroup_y Test protocol, symbol: TⒶ2021.9.22
#define RCD_ProtocolTAO2_y                      82313250 // protocolIdGroup_y Test protocol, symbol: TⒶ2021.10.13
#define RCD_ProtocolA00_y                           4098 // protocolIdGroup_y Live protocol, symbol: Ⓐ2021.11.15 Blockchain signing only, no shares or coin
#define RCD_ProtocolA01_y                           4130 // protocolIdGroup_y Live protocol, symbol: Ⓐ2021.12.15 Blockchain plus simple shares and coin, no fees
#define RCD_actorWriterClient_y                   729986 // protocolActorGroup_y Actor definition: the actor who wants to create a record (in a blockchain)
#define RCD_actorWriterServer_y                   730018 // protocolActorGroup_y Actor definition: the actor who wants to retrieve a record (from a blockchain)
#define RCD_actorReaderClient_y                   730050 // protocolActorGroup_y Actor definition: the actor who handles requests to create records (in a blockchain)
#define RCD_actorReaderServer_y                   730082 // protocolActorGroup_y Actor definition: the actor who handles requests to retrieve records (from a blockchain)
#define RCD_ProtocolList_O                          4208 // protocolDefGroup_y An Assign Onward Protocol List - should contain one or more ProtocolDef_o describing the protocols an actor is equipped to handle 
#define RCD_ProtocolDef_o                           4192 // protocolDefGroup_y An Assign Onward Protocol Definition - defines containers and their contents including operation requirements (hashes, signatures, order of timestamps, etc.) 
#define RCD_ProtocolSig_o                           4160 // protocolDefGroup_y A signature on an Assign Onward Protocol Definition, indicating the protocol is "approved" by the signer
#define RCD_ItemDefList_O                           4144 // protocolDefGroup_y A list of Assign Onward Protocol Item Definitions
#define RCD_ItemDef_o                               4128 // protocolDefGroup_y An Assign Onward Protocol Item Definition - overall, defines containers and their contents including operation requirements (hashes, signatures, order of timestamps, etc.) 
#define RCD_ActorDefList_O                          4112 // protocolDefGroup_y A list of Actor definitions, a valid protocol definition should contain one or more actor definitions
#define RCD_ActorDef_o                              4096 // protocolDefGroup_y A single Actor definition
#define RCD_OutgoingItemsList_O                     4336 // protocolDefGroup_y A list items this actor publishes
#define RCD_IncomingItemsList_O                     4304 // protocolDefGroup_y A list items this actor consumes
#define RCD_SubItemDefs_O                           4176 // protocolDefGroup_y inside _o or _O items, SubItemDefs_O defines the requirements for sub-items which may appear
#define RCD_DefinedSubItems_Y                       4210 // protocolDefGroup_y DEPRECATED USE SubItemDefs_O Items that may appear in a container (to fulfill the protocol requirements)
#define RCD_DefinedSubItemsDesc_S                   4214 // protocolDefGroup_y DEPRECATED USE SubItemDefs_O Optional, description of what the items represent in this context
#define RCD_OperReqList_O                           4432 // protocolDefGroup_y USE inside SubItemDefs_O Operational requirements / constraints on values found in Required or Optional items
#define RCD_OperReq_o                               4416 // protocolDefGroup_y USE inside SubItemDefs_O A single operational requirement / constraint on value found in an item
#define RCD_OpMemberOf_Y                            4114 // operationGroup_y List of possible values for an RCODE
#define RCD_OpOneOrMoreOf_Y                         4370 // operationGroup_y One or more objects of the listed types are required to appear
#define RCD_OpGreaterThan_Y                         4146 // operationGroup_y Path defining a value, or array of values, to compare - the targeted value must be greater than all
#define RCD_OpLessThan_Y                            4178 // operationGroup_y Path defining a value, or array of values, to compare - the targeted value must be less than all
#define RCD_OpTimeValue_y                           4162 // operationGroup_y The specified object should be assigned a time value, RCD_time_i indicates the time should be the current time - other values may be used in the future to indicate other types of time values.
#define RCD_OpHash_O                                4272 // operationGroup_y Three parameters follow, describing inputs to the hash function
#define RCD_itemRelAddr_Y                        2095826 // structNavGroup_y Navigation from the current itemDef to an operand in an operation, sequence of item types when navigating deeper, relative step back or to root to get higher in the hiearchy
#define RCD_navUpOne_y                           2095842 // structNavGroup_y Navigate 'up one' level in a nested block, used in operand relative addressing.
#define RCD_navChainBlock_y                      2095746 // structNavGroup_y Navigate to the 'root' of this chain block, used in operand absolute addressing.
#define RCD_navJumpToBlock_y                     2095778 // structNavGroup_y The next item in the itemRelAddr_Y array contains a chainBlock index, navigation will continue from that block.
#define RCD_navIfPresent_y                       2095810 // structNavGroup_y If the next item is present, then proceed - otherwise this condition is satisfied by the absence of the optional item.
#define RCD_requestAvailServices_o                692928 // serviceDescriptors_y Object contains the protocol name and/or uid (some protocols will require uid, early ones will go by name).
#define RCD_availServicesList_O                   692944 // serviceDescriptors_y List of services provided, objects contain a type_y: service descriptor and possibly additional info like lists of items, database names, etc.
#define RCD_requestRecordStorage_o                692896 // serviceDescriptors_y Object describes the id of the requestor, id of the database to record in, and the data to record
#define RCD_recordStorageResult_o                 692864 // serviceDescriptors_y Object describes success or failure of a record storage attempt
#define RCD_requestRecordRetrieval_o              693024 // serviceDescriptors_y Object describes the id of the requestor, id of the database to record in, and id of the record to retrieve
#define RCD_recordRetrievalResult_o               692992 // serviceDescriptors_y Object describes success (with data) or failure of a record retrieval attempt
#define RCD_RangeBounds_O                           4240 // dataGroup_y Boundaries for valid values
#define RCD_min_i                                     65 // dataGroup_y Used in range boundaries
#define RCD_min_n                                     67 // dataGroup_y Used in range boundaries
#define RCD_min_r                                     68 // dataGroup_y Used in range boundaries
#define RCD_max_i                                     97 // dataGroup_y Used in range boundaries
#define RCD_max_n                                     99 // dataGroup_y Used in range boundaries
#define RCD_max_r                                    100 // dataGroup_y Used in range boundaries
#define RCD_enum_Y                                   114 // dataGroup_y Used to define available choices
#define RCD_enum_S                                    86 // dataGroup_y Used to define available choices
#define RCD_GenesisBlock_o                        530432 // dataGroup_y An Assign Onward Genesis Block. With a signature_o which includes a timei, and other definitions for the chain.
#define RCD_ChainDesc_o                           534144 // dataGroup_y Chain Description, generally only found in the Genesis Block - descriptive items like name, symbol, number of coins represented by all shares, etc.
#define RCD_ChainFunc_o                           534400 // dataGroup_y Chain Functional items description, generally only found in the Genesis Block - functional items like number of shares, expiration rates, recording fees, etc.
#define RCD_EndChain_o                            538112 // dataGroup_y Used when reporting the last blocks in a chain, this block is a signal that all known blocks are received. Has a pzzzO and zzzO and timei of when it was reported, but contains no transaction information and will (usually) be discarded when appending new transaction blocks to its parent.
#define RCD_Symbol_s                              534150 // dataGroup_y A short string like ITC representing the chain.
#define RCD_CdName_s                              534182 // dataGroup_y A longer string like Itcoin representing the chain.
#define RCD_Tagline_s                             534214 // dataGroup_y A brief sentence like "The greatest blockchain ever" describing the chain.
#define RCD_Description_s                         534246 // dataGroup_y A paragraph (say 1000-ish characters) describing the chain.
#define RCD_Icon_o                                534176 // dataGroup_y A square-ish image that looks good small, representing the chain.
#define RCD_Banner_o                              534208 // dataGroup_y A wide image that might be displayed near the top of a page, representing the chain.
#define RCD_Image_o                               534240 // dataGroup_y A more detailed image representing the chain.
#define RCD_CfShares_n                            534403 // dataGroup_y The number of shares recorded in the Genesis block.
#define RCD_CfCoins_n                             534435 // dataGroup_y The number of "coins" all the shares in this chain represent.
#define RCD_CfRecFee_r                            534404 // dataGroup_y The number of coins per byte charged to record data on this chain (usually 1/a very large number).
#define RCD_ExchAdvert_o                          692224 // dataGroup_y An exchange advertisement for cross chain exchange swaps (expressed in coins for coins)
#define RCD_Proposal_o                            692256 // dataGroup_y A share swap proposal
#define RCD_Offer_o                               692288 // dataGroup_y A share swap offer
#define RCD_Acceptance_o                          692320 // dataGroup_y A share swap acceptance of offer
#define RCD_Cancellation_o                        694144 // dataGroup_y A share swap notice of cancellation (for cause, usually time expiration)
#define RCD_Identity_o                              4224 // dataGroup_y A cross chain identity, may be tied to share ownership but carries its own key pair
#define RCD_NineCode_z                 72057594037927951 // dataGroup_y A test code to check a 9 byte ricey code
#define RCD_MaxiCode_z               9223372036854775791 // dataGroup_y A test code to check a 9 byte ricey code


#endif // CODEDEF_H