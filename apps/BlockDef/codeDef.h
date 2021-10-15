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

#define RDT_NULL            0x00 // z - A null block value type, used as an error flag 
#define RDT_MPZ             0x01 // n - libGMP signed integer - json as a quoted ASCII decimal string, bson as an object carrying the native GMP values
#define RDT_MPQ             0x02 // r - libGMP rational fraction - json as a quoted ASCII decimal string/ASCII decimal string, bson as an object carrying the native GMP
#define RDT_STRING          0x05 // s - UTF8 encoded string - json as UTF8 string, bson as int32_t length (includes terminator) followed by UTF8 string plus null terminator
#define RDT_INT64           0x06 // i - 64 bit signed integer - json in an unquoted ASCII decimal string, bson as 8 bytes using void qToLittleEndian(qint64 src, uchar *dest)
#define RDT_BYTEARRAY       0x0B // b - ByteArray (aka octet stream) - json as hex string, bson as int32_t length followed by length octets
#define RDT_RCODE           0x0C // c - Ricey code - json as the table defined name, bson as a list of bytes: last byte has 0 in most significant bit.
#define RDT_OBJECT          0x0F // o - in json: a comma separated list of zero or more key-value pairs json encases each pair in {}, all keys unique within the object, in bson there are no separators, a key of Obterm ends the current object
#define RDT_MPZ_ARRAY       0x11 // N - array of zero or more libGMP signed integers stored as described above
#define RDT_MPQ_ARRAY       0x12 // R - array of zero or more libGMP rational fractions stored as described above
#define RDT_STRING_ARRAY    0x15 // S - array of zero or more UTF-8 encoded strings - json as quoted, escaped UTF-8 string, bson as int32_t length followed by UTF-8 string, no null terminator
#define RDT_INT64_ARRAY     0x16 // I - array of zero or more 64 bit signed integers stored as above, comma separated in json, rice code size followed by the array in bson
#define RDT_BYTEARRAY_ARRAY 0x1B // B - ByteArray (aka octet stream) - json as hex string, bson as int32_t length followed by length octets
#define RDT_RCODE_ARRAY     0x1C // C - array of zero or more Ricey codes, quoted hexadecimal and comma separated in json, starts with a rice code size followed by the array in bson
#define RDT_OBJECT_ARRAY    0x1F // O - array of zero or more objects encased in []
#define RDT_ARRAY           0x10 // * - this bit is always set in array types
#define RDT_TYPEMASK        0x0F // * - used to get the contained value type out of an array type 
#define RDT_OBTYPEMASK      0x1F // * - used to get the contained value type out of an object type

#define RCD_riceyTypes_O                         1336223 // dictionaryGroup An array object which contains the description, name and ricey code 4 LSB for each defined ricey code type in the system.
#define RCD_riceyCodes_O                         1336255 // dictionaryGroup An array object which contains the description, name and ricey code for each defined ricey code in the system.
#define RCD_type_b                               1336203 // dictionaryGroup In the dictionary definition context, _y is not defined yet, use this instead of type_y
#define RCD_group_s                              1336197 // dictionaryGroup A logical collecting of code names, can become a folder structure if necessary.
#define RCD_null_z                                     0 // dataGroup This key indicates the value is empty, has no value - usually indicates an error
#define RCD_mpz_n                                      1 // dataGroup Generic MPZ large integer, use depends on context.
#define RCD_mpq_r                                      2 // dataGroup Generic MPQ large integer fraction, use depends on context.
#define RCD_text_s                                     5 // dataGroup Generic string, use depends on context, for instance in an image object this would be the alt text
#define RCD_int64_i                                    6 // dataGroup Generic signed int64, use depends on context.
#define RCD_data_b                                    11 // dataGroup Generic data block, use determined by the object which contains it.
#define RCD_type_c                                    12 // dataGroup Identifies an algorithm type for hash or crypto key, or other types depending on context
#define RCD_ob_o                                      15 // dataGroup Generic object, use depends on context
#define RCD_mpzArray_N                                17 // dataGroup A generic array of MPZ elements
#define RCD_mpqArray_R                                18 // dataGroup A generic array of MPQ elements
#define RCD_stringArray_S                             21 // dataGroup A generic array of string elements
#define RCD_int64Array_I                              22 // dataGroup A generic array of int64 elements
#define RCD_byteArrayArray_B                          27 // dataGroup A generic array of byte array element
#define RCD_riceyArray_C                              28 // dataGroup A generic array of Ricey elements
#define RCD_objectArray_O                             31 // dataGroup A generic array of object elements
#define RCD_AOShares_n                                33 // dataGroup A quantity of shares - meaning depends on context
#define RCD_name_s                                    37 // dataGroup Name as a string, use depends on context, often a key name
#define RCD_time_i                                    38 // dataGroup UTC time of block creation (microseconds since epoch x 189). May never be less than or equal to the timei of any parent block. Serves as a unique id for AOBO blocks.
#define RCD_desc_s                                    69 // dataGroup Description of the current object, specific use depends on context
#define RCD_code_s                                   101 // dataGroup Code as a string, use depends on context, often a key type
#define RCD_userId_b                                  43 // dataGroup Unique user identifier, possibly the 256bit hash of a userID_o that has all kinds of stuff in it
#define RCD_chainBlock_o                            9615 // chainGroup An Assign Onward Chain Block. With a hash_o which includes a timei, and a hashedOb_o which includes exact copied of the hash_o(s) of the parent(s)
#define RCD_hashedOb_o                                47 // chainGroup An object which has its hash computed. When found in a chainBlock_o always contains a parentHash_O a time_i of when the block was signed, the hash_o is found in the object which contains the hashedOb_o.
#define RCD_hash_o                                   111 // chainGroup Found in chainBlock_O and similar objects which contain a signedBlock_O, contains an algorithm id (type_y) and the hash by that algorithm of the binary representation of the objects in the signedBlock_O (data_b) and a timestamp (time_i) matching the time_i in the signedBlock_O and which must not be as old or older than any parent timestamp and also should not be in the future, these time_i are unique identifiers for chainBlock_O objects.
#define RCD_parentHash_O                             127 // chainGroup Contains copies of the hash_o of one or more (more in the case of merging forked chains) parent blocks.
#define RCD_separableItems_O                         863 // chainGroup List of items which are only indirectly included in the hash chain
#define RCD_separableItemsHashes_O                   831 // chainGroup Hashes of separable items which do get included in the hash chain
#define RCD_SHA256_c                                 140 // algorithmIdGroup algorithm ID for SHA2 - 256
#define RCD_ECB256_c                                 172 // algorithmIdGroup algorithm ID for Elliptic Curve ECDSA curve BrainpoolP256r1
#define RCD_RSA3072_c                                204 // algorithmIdGroup algorithm ID for 3072 bit RSA - less commonly used algorithms get longer ricey codes
#define RCD_SHA3b512_c                               236 // algorithmIdGroup algorithm ID for SHA3 - 512
#define RCD_jpg_c                                    268 // algorithmIdGroup Found in image objects, tells what format the data_b is to be interpreted as
#define RCD_png_c                                    332 // algorithmIdGroup Found in image objects, tells what format the data_b is to be interpreted as
#define RCD_success_c                               8204 // dataGroup success code
#define RCD_failure_c                              14348 // dataGroup failure code
#define RCD_ProtocolDAO0_c                     195559436 // protocolIdGroup Development protocol, symbol: DⒶ0
#define RCD_ProtocolDAO1_c                     195559468 // protocolIdGroup Development protocol, symbol: DⒶ1
#define RCD_ProtocolDAO2_c                     195559500 // protocolIdGroup Development protocol, symbol: DⒶ2
#define RCD_ProtocolDAO3_c                     195559532 // protocolIdGroup Development protocol, symbol: DⒶ3
#define RCD_ProtocolDAO4_c                     195559564 // protocolIdGroup Development protocol, symbol: DⒶ4
#define RCD_ProtocolDAO5_c                     195559596 // protocolIdGroup Development protocol, symbol: DⒶ5
#define RCD_ProtocolDAO6_c                     195559628 // protocolIdGroup Development protocol, symbol: DⒶ6
#define RCD_ProtocolDAO7_c                     195559660 // protocolIdGroup Development protocol, symbol: DⒶ7
#define RCD_ProtocolDAO8_c                     195559692 // protocolIdGroup Development protocol, symbol: DⒶ8
#define RCD_ProtocolDAO9_c                     195559724 // protocolIdGroup Development protocol, symbol: DⒶ9
#define RCD_ProtocolDAO10_c                    195559756 // protocolIdGroup Development protocol, symbol: DⒶ10
#define RCD_ProtocolDAO11_c                    195559788 // protocolIdGroup Development protocol, symbol: DⒶ11
#define RCD_ProtocolDAO12_c                    195559948 // protocolIdGroup Development protocol, symbol: DⒶ12
#define RCD_ProtocolDAO13_c                    195559980 // protocolIdGroup Development protocol, symbol: DⒶ13
#define RCD_ProtocolTAO1_c                      82313228 // protocolIdGroup Test protocol, symbol: TⒶ2021.9.22
#define RCD_ProtocolTAO2_c                      82313260 // protocolIdGroup Test protocol, symbol: TⒶ2021.10.13
#define RCD_ProtocolA00_c                           4108 // protocolIdGroup Live protocol, symbol: Ⓐ2021.11.15 Blockchain signing only, no shares or coin
#define RCD_ProtocolA01_c                           4140 // protocolIdGroup Live protocol, symbol: Ⓐ2021.12.15 Blockchain plus simple shares and coin, no fees
#define RCD_actorWriterClient_c                   729996 // protocolActorGroup Actor definition: the actor who wants to create a record (in a blockchain)
#define RCD_actorWriterServer_c                   730028 // protocolActorGroup Actor definition: the actor who wants to retrieve a record (from a blockchain)
#define RCD_actorReaderClient_c                   730060 // protocolActorGroup Actor definition: the actor who handles requests to create records (in a blockchain)
#define RCD_actorReaderServer_c                   730092 // protocolActorGroup Actor definition: the actor who handles requests to retrieve records (from a blockchain)
#define RCD_ProtocolList_O                          4223 // protocolDefGroup An Assign Onward Protocol List - should contain one or more ProtocolDef_o describing the protocols an actor is equipped to handle
#define RCD_ProtocolDef_o                           4207 // protocolDefGroup An Assign Onward Protocol Definition - defines containers and their contents including operation requirements (hashes, signatures, order of timestamps, etc.)
#define RCD_ProtocolSig_o                           4175 // protocolDefGroup A signature on an Assign Onward Protocol Definition, indicating the protocol is "approved" by the signer
#define RCD_ItemDefList_O                           4159 // protocolDefGroup A list of Assign Onward Protocol Item Definitions
#define RCD_ItemDef_o                               4143 // protocolDefGroup An Assign Onward Protocol Item Definition - overall, defines containers and their contents including operation requirements (hashes, signatures, order of timestamps, etc.)
#define RCD_ActorDefList_O                          4127 // protocolDefGroup A list of Actor definitions, a valid protocol definition should contain one or more actor definitions
#define RCD_ActorDef_o                              4111 // protocolDefGroup A single Actor definition
#define RCD_OutgoingItemsList_O                     4351 // protocolDefGroup A list items this actor publishes
#define RCD_IncomingItemsList_O                     4319 // protocolDefGroup A list items this actor consumes
#define RCD_SubItemDefs_O                           4191 // protocolDefGroup inside _o or _O items, SubItemDefs_O defines the requirements for sub-items which may appear
#define RCD_DefinedSubItems_C                       4220 // protocolDefGroup DEPRECATED USE SubItemDefs_O Items that may appear in a container (to fulfill the protocol requirements)
#define RCD_DefinedSubItemsDesc_S                   4213 // protocolDefGroup DEPRECATED USE SubItemDefs_O Optional, description of what the items represent in this context
#define RCD_OperReqList_O                           4447 // protocolDefGroup USE inside SubItemDefs_O Operational requirements / constraints on values found in Required or Optional items
#define RCD_OperReq_o                               4431 // protocolDefGroup USE inside SubItemDefs_O A single operational requirement / constraint on value found in an item
#define RCD_OpMemberOf_C                            4124 // operationGroup List of possible values for an RCODE
#define RCD_OpOneOrMoreOf_C                         4380 // operationGroup One or more objects of the listed types are required to appear
#define RCD_OpGreaterThan_C                         4156 // operationGroup Path defining a value, or array of values, to compare - the targeted value must be greater than all
#define RCD_OpLessThan_C                            4188 // operationGroup Path defining a value, or array of values, to compare - the targeted value must be less than all
#define RCD_OpTimeValue_c                           4172 // operationGroup The specified object should be assigned a time value, RCD_time_i indicates the time should be the current time - other values may be used in the future to indicate other types of time values.
#define RCD_OpHash_O                                4287 // operationGroup Three parameters follow, describing inputs to the hash function
#define RCD_itemRelAddr_C                        2095836 // structNavGroup Navigation from the current itemDef to an operand in an operation, sequence of item types when navigating deeper, relative step back or to root to get higher in the hiearchy
#define RCD_navUpOne_c                           2095852 // structNavGroup Navigate 'up one' level in a nested block, used in operand relative addressing.
#define RCD_navChainBlock_c                      2095756 // structNavGroup Navigate to the 'root' of this chain block, used in operand absolute addressing.
#define RCD_navJumpToBlock_c                     2095788 // structNavGroup The next item in the itemRelAddr_Y array contains a chainBlock index, navigation will continue from that block.
#define RCD_navIfPresent_c                       2095820 // structNavGroup If the next item is present, then proceed - otherwise this condition is satisfied by the absence of the optional item.
#define RCD_requestAvailServices_o                692943 // serviceDescriptors Object contains the protocol name and/or uid (some protocols will require uid, early ones will go by name).
#define RCD_availServicesList_O                   692959 // serviceDescriptors List of services provided, objects contain a type_y: service descriptor and possibly additional info like lists of items, database names, etc.
#define RCD_requestRecordStorage_o                692911 // serviceDescriptors Object describes the id of the requestor, id of the database to record in, and the data to record
#define RCD_recordStorageResult_o                 692879 // serviceDescriptors Object describes success or failure of a record storage attempt
#define RCD_requestRecordRetrieval_o              693039 // serviceDescriptors Object describes the id of the requestor, id of the database to record in, and id of the record to retrieve
#define RCD_recordRetrievalResult_o               693007 // serviceDescriptors Object describes success (with data) or failure of a record retrieval attempt
#define RCD_RangeBounds_O                           4255 // dataGroup Boundaries for valid values
#define RCD_min_i                                     70 // dataGroup Used in range boundaries
#define RCD_min_n                                     65 // dataGroup Used in range boundaries
#define RCD_min_r                                     66 // dataGroup Used in range boundaries
#define RCD_max_i                                    102 // dataGroup Used in range boundaries
#define RCD_max_n                                     97 // dataGroup Used in range boundaries
#define RCD_max_r                                     98 // dataGroup Used in range boundaries
#define RCD_enum_C                                    92 // dataGroup Used to define available choices
#define RCD_enum_S                                    85 // dataGroup Used to define available choices
#define RCD_GenesisBlock_o                        530447 // dataGroup An Assign Onward Genesis Block. With a signature_o which includes a timei, and other definitions for the chain.
#define RCD_ChainDesc_o                           534159 // dataGroup Chain Description, generally only found in the Genesis Block - descriptive items like name, symbol, number of coins represented by all shares, etc.
#define RCD_ChainFunc_o                           534415 // dataGroup Chain Functional items description, generally only found in the Genesis Block - functional items like number of shares, expiration rates, recording fees, etc.
#define RCD_EndChain_o                            538127 // dataGroup Used when reporting the last blocks in a chain, this block is a signal that all known blocks are received. Has a pzzzO and zzzO and timei of when it was reported, but contains no transaction information and will (usually) be discarded when appending new transaction blocks to its parent.
#define RCD_Symbol_s                              534149 // dataGroup A short string like ITC representing the chain.
#define RCD_CdName_s                              534181 // dataGroup A longer string like Itcoin representing the chain.
#define RCD_Tagline_s                             534213 // dataGroup A brief sentence like "The greatest blockchain ever" describing the chain
#define RCD_Description_s                         534245 // dataGroup A paragraph (say 1000-ish characters) describing the chain.
#define RCD_Icon_o                                534191 // dataGroup A square-ish image that looks good small, representing the chain.
#define RCD_Banner_o                              534223 // dataGroup A wide image that might be displayed near the top of a page, representing the chain.
#define RCD_Image_o                               534255 // dataGroup A more detailed image representing the chain.
#define RCD_CfShares_n                            534401 // dataGroup The number of shares recorded in the Genesis block.
#define RCD_CfCoins_n                             534433 // dataGroup The number of "coins" all the shares in this chain represent.
#define RCD_CfRecFee_r                            534402 // dataGroup The number of coins per byte charged to record data on this chain (usually 1/a very large number).
#define RCD_ExchAdvert_o                            8207 // contractGroup An exchange advertisement for cross chain exchange swaps (expressed in coins for coins)
#define RCD_Proposal_o                              8239 // contractGroup A share swap proposal
#define RCD_Offer_o                                 8271 // contractGroup A share swap offer
#define RCD_Acceptance_o                            8399 // contractGroup A share swap acceptance of offer
#define RCD_Cancellation_o                          9327 // contractGroup A share swap notice of cancellation (for cause, usually time expiration)
#define RCD_Identity_o                              8335 // contractGroup A cross chain identity, may be tied to share ownership but carries its own key pair
#define RCD_NineCode_z                 72057594037927936 // dataGroup A test code to check a 9 byte ricey code
#define RCD_MaxiCode_z               9223372036854775776 // dataGroup A test code to check a 9 byte ricey code


#endif // CODEDEF_H