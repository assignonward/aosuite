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

#define RDT_OBJECT          0x00 // o - in json: a comma separated list of zero or more key-value pairs json encases each pair in {}, all keys unique within the object, in bson there are no separators, a key of Obterm ends the current object
#define RDT_INT64           0x01 // i - 64 bit signed integer - json in an unquoted ASCII decimal string, bson as 8 bytes using void qToLittleEndian(qint64 src, uchar *dest)
#define RDT_INT32           0x02 // l - 32 bit signed integer - json in an unquoted ASCII decimal string, bson as 4 bytes using void qToLittleEndian(qint32 src, uchar *dest)
#define RDT_MPZ             0x03 // n - libGMP signed integer - json as a quoted ASCII decimal string, bson as an object carrying the native GMP values
#define RDT_MPQ             0x04 // r - libGMP rational fraction - json as a quoted ASCII decimal string/ASCII decimal string, bson as an object carrying the native GMP
#define RDT_RCODE           0x05 // y - Ricey code - json as the table defined name, bson as a list of bytes: last byte has 0 in most significant bit.
#define RDT_STRING          0x06 // s - UTF8 encoded string - json as UTF8 string, bson as int32_t length (includes terminator) followed by UTF8 string plus null terminator
#define RDT_BYTEARRAY       0x07 // b - ByteArray (aka octet stream) - json as hex string, bson as int32_t length followed by length octets
#define RDT_NULL            0x0F // z - A null block value type, used as an error flag 
#define RDT_OBJECT_ARRAY    0x10 // O - array of zero or more objects encased in []
#define RDT_INT64_ARRAY     0x11 // I - array of zero or more 64 bit signed integers stored as above, comma separated in json, rice code size followed by the array in bson
#define RDT_INT32_ARRAY     0x12 // L - array of zero or more 32 bit signed integers stored as above, comma separated in json, rice code size followed by the array in bson
#define RDT_MPZ_ARRAY       0x13 // N - array of zero or more libGMP signed integers stored as described above
#define RDT_MPQ_ARRAY       0x14 // R - array of zero or more libGMP rational fractions stored as described above
#define RDT_RCODE_ARRAY     0x15 // Y - array of zero or more Ricey codes, quoted hexadecimal and comma separated in json, starts with a rice code size followed by the array in bson
#define RDT_STRING_ARRAY    0x16 // S - array of zero or more UTF-8 encoded strings - json as quoted, escaped UTF-8 string, bson as int32_t length followed by UTF-8 string, no null terminator
#define RDT_BYTEARRAY_ARRAY 0x17 // B - ByteArray (aka octet stream) - json as hex string, bson as int32_t length followed by length octets
#define RDT_ARRAY           0x10 // * - this bit is always set in array types
#define RDT_TYPEMASK        0x0F // * - used to get the contained value type out of an array type 
#define RDT_OBTYPEMASK      0x1F // * - used to get the contained value type out of an object type

#define RCD_ObTerm_o               0 // Object terminator, indicates no more items in the object in bsonish lists
#define RCD_int64_i                1 // Generic signed int64, use depends on context.
#define RCD_int32_l                2 // Generic signed int32, use depends on context.
#define RCD_mpz_n                  3 // Generic MPZ large integer, use depends on context.
#define RCD_mpq_r                  4 // Generic MPQ large integer fraction, use depends on context.
#define RCD_type_y                 5 // Identifies an algorithm type for hash or crypto key, or other types depending on context
#define RCD_text_s                 6 // Generic string, use depends on context, for instance in an image object this would be the alt text
#define RCD_name_s                38 // Name as a string, use depends on context, often a key name
#define RCD_code_s               102 // Code as a string, use depends on context, often a key type
#define RCD_data_b                 7 // Generic data block, use determined by the object which contains it.
#define RCD_objectArray_O         16 // A generic array of object elements
#define RCD_int64Array_I          17 // A generic array of int64 elements
#define RCD_int32Array_L          18 // A generic array of int32 elements
#define RCD_mpzArray_N            19 // A generic array of MPZ elements
#define RCD_mpqArray_R            20 // A generic array of MPQ elements
#define RCD_riceyArray_Y          21 // A generic array of Ricey elements
#define RCD_stringArray_S         22 // A generic array of string elements
#define RCD_byteArrayArray_B      23 // A generic array of byte array elementschainBlock_o 0xA000 // A Chain Block always contains a signedBlock_O which includes a time_i, and a signature_O that signs the whole signedBlock_O.
#define RCD_signedBlock_o         96 // A Signed Block in the chain always contains a parentSignature_O a time_i of when the block was signed, the signature_O is found in the object which contains the signedBlock_O.
#define RCD_parentSignature_o     64 // Contains a copy of the signature_O of one or more (more in the case of merging forked chains) parent blocks.
#define RCD_signature_o           32 // Hash object, only found in chainBlock_O and similar objects which contain a signedBlock_O, contains an algorithm id (type_y) and the hash by that algorithm of the binary representation of the objects in the signedBlock_O (data_b) and a timestamp (time_i) matching the time_i in the signedBlock_O and which must not be as old or older than any parent timestamp and also should not be in the future, these time_i are unique identifiers for chainBlock_O objects.
#define RCD_time_i                33 // UTC time of block creation (microseconds since epoch x 189). May never be less than or equal to the timei of any parent block. Serves as a unique id for AOBO blocks.
#define RCD_AOShares_n            35 // A quantity of shares - meaning depends on context
#define RCD_SHA256_y              37 // algorithm ID (algor) for SHA2 - 256
#define RCD_ECB256_y              69 // algorithm ID (algor) for Elliptic Curve ECDSA curve BrainpoolP256r1
#define RCD_RSA3072_y           4293 // algorithm ID (algor) for 3072 bit RSA - less commonly used algorithms get longer ricey codes
#define RCD_SHA3b512_y          4261 // algorithm ID (algor) for SHA3 - 512
#define RCD_jpg_y               2085 // Found in image objects, tells what format the data_b is to be interpreted as
#define RCD_png_y               2117 // Found in image objects, tells what format the data_b is to be interpreted as
#define RCD_riceyTypes_O       10256 // An array object which contains the description, name and ricey code 4 LSB for each defined ricey code type in the system.
#define RCD_riceyCodes_O       10288 // An array object which contains the description, name and ricey code for each defined ricey code in the system.
#define RCD_PcolTA01_y        600069 // Test protocol, symbol: TⒶ2021.9.7
#define RCD_PcolTA02_y        600101 // Test protocol, symbol: TⒶ2021.10.13
#define RCD_PcolA00_y         616453 // Live protocol, symbol: Ⓐ2021.11.15 Blockchain signing only, no shares or coin
#define RCD_PcolA01_y         616485 // Live protocol, symbol: Ⓐ2021.12.15 Blockchain plus simple shares and coin, no fees
#define RCD_ProtocolId_y      528453 // A shorthand Assign Onward Protocol identifier.
#define RCD_ProtocolSig_o     528480 // A signature on an Assign Onward Protocol Definition, indicating the protocol is "approved" by the signer
#define RCD_ProtocolDef_o     528448 // An Assign Onward Protocol Definition - overall, defines containers and their contents including operation requirements (hashes, signatures, order of timestamps, etc.) 
#define RCD_ProtocolList_O    528464 // An Assign Onward Protocol List - should contain one or more ProtocolDef_o describing the protocols an actor is equipped to handle 
#define RCD_RequiredItems_Y     9877 // Items that must appear in a container (to fulfill the protocol requirements)
#define RCD_OptionalItems_Y     9909 // Items that may optionally appear in a container, do not violate protocol if they are missing.
#define RCD_RangeBounds_O       9872 // Boundaries for valid values
#define RCD_min_i                 65 // Used in range boundaries
#define RCD_min_l                 66 // Used in range boundaries
#define RCD_min_n                 67 // Used in range boundaries
#define RCD_min_r                 68 // Used in range boundaries
#define RCD_max_i                 97 // Used in range boundaries
#define RCD_max_l                 98 // Used in range boundaries
#define RCD_max_n                 99 // Used in range boundaries
#define RCD_max_r                100 // Used in range boundaries
#define RCD_enum_Y                85 // Used to define available choices
#define RCD_enum_S                86 // Used to define available choices
#define RCD_GenesisBlock_o    530432 // An Assign Onward Genesis Block. With a signature_o which includes a timei, and other definitions for the chain.
#define RCD_ChainDesc_o       534144 // Chain Description, generally only found in the Genesis Block - descriptive items like name, symbol, number of coins represented by all shares, etc.
#define RCD_ChainFunc_o       534400 // Chain Functional items description, generally only found in the Genesis Block - functional items like number of shares, expiration rates, recording fees, etc.
#define RCD_EndChain_o        538112 // Used when reporting the last blocks in a chain, this block is a signal that all known blocks are received. Has a pzzzO and zzzO and timei of when it was reported, but contains no transaction information and will (usually) be discarded when appending new transaction blocks to its parent.
#define RCD_Symbol_s          534150 // A short string like ITC representing the chain.
#define RCD_CdName_s          534182 // A longer string like Itcoin representing the chain.
#define RCD_Tagline_s         534214 // A brief sentence like "The greatest blockchain ever" describing the chain.
#define RCD_Description_s     534246 // A paragraph (say 1000-ish characters) describing the chain.
#define RCD_Icon_o            534176 // A square-ish image that looks good small, representing the chain.
#define RCD_Banner_o          534208 // A wide image that might be displayed near the top of a page, representing the chain.
#define RCD_Image_o           534240 // A more detailed image representing the chain.
#define RCD_CfShares_n        534403 // The number of shares recorded in the Genesis block.
#define RCD_CfCoins_n         534435 // The number of "coins" all the shares in this chain represent.
#define RCD_CfRecFee_r        534404 // The number of coins per byte charged to record data on this chain (usually 1/a very large number).
#define RCD_chainBlock_o        9600 // An Assign Onward Chain Block. With a signature_o which includes a timei, and the signature of the parent(s).
#define RCD_ExchAdvert_o      692224 // An exchange advertisement for cross chain exchange swaps (expressed in coins for coins)
#define RCD_Proposal_o        692256 // A share swap proposal
#define RCD_Offer_o           692288 // A share swap offer
#define RCD_Acceptance_o      692320 // A share swap acceptance of offer
#define RCD_Cancellation_o    694144 // A share swap notice of cancellation (for cause, usually time expiration)
#define RCD_Identity_o          4224 // A cross chain identity, may be tied to share ownership but carries its own key pair


#endif // CODEDEF_H