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
#ifndef RICEYCODES_H
#define RICEYCODES_H
/*
List ordering: every O list that includes a zzzO is "Ascending sorted"
by the bsonish representation of the key-value of each object in the
list, with most significant byte first.  The zzzO comes last,
regardless of sort order.
*/


// Ricey data types - the 4 lsb of a Ricey key
#define RDT_OBJECT          0x0 // o - list of zero or more key-value pairs json encased in {} items separated by commas, in bson a key of Obterm ends the current object
#define RDT_INT64           0x1 // i - 64 bit signed integer - json in decimal string, bson as 8 bytes using void qToLittleEndian(qint64 src, uchar *dest)
#define RDT_INT32           0x2 // l - 32 bit signed integer - json in decimal string, bson as 4 bytes using void qToLittleEndian(qint32 src, uchar *dest)
#define RDT_MPZ             0x3 // n - libGMP signed integer - json as decimal string, bson as??? native GMP
#define RDT_MPQ             0x4 // r - libGMP rational fraction - json as decimal string/decimal string, bson as??? native GMP
#define RDT_RCODE           0x5 // y - Ricey code - json as the table defined name, bson as a list of bytes: last byte has 0 in most significant bit.
#define RDT_STRING          0x6 // s - UTF8 encoded string - json as UTF8 string, bson as int32_t length (includes terminator) followed by UTF8 string plus null terminator
#define RDT_BYTEARRAY       0x7 // b - ByteArray (aka octet stream) - json as hex string, bson as int32_t length followed by length octets
#define RDT_OBJECT_ARRAY    0x8 // O - array of lists of zero or more key-value pairs json encased in {} items separated by commas, in bson a key of Obterm ends the current object
#define RDT_INT64_ARRAY     0x9 // I - array of 64 bit signed integers - json in decimal string, bson as 8 bytes using void qToLittleEndian(qint64 src, uchar *dest)
#define RDT_INT32_ARRAY     0xA // L - array of 32 bit signed integers - json in decimal string, bson as 4 bytes using void qToLittleEndian(qint32 src, uchar *dest)
#define RDT_MPZ_ARRAY       0xB // N - array of libGMP signed integers - json as decimal string, bson as??? native GMP
#define RDT_MPQ_ARRAY       0xC // R - array of libGMP rational fractions - json as decimal string/decimal string, bson as??? native GMP
#define RDT_RCODE_ARRAY     0xD // Y - array of Ricey codes - json as the table defined name, bson as a list of bytes: last byte has 0 in most significant bit.
#define RDT_STRING_ARRAY    0xE // S - array of UTF8 encoded strings - json as UTF8 string, bson as int32_t length (includes terminator) followed by UTF8 string plus null terminator
#define RDT_BYTEARRAY_ARRAY 0xF // B - array of ByteArrays (aka octet stream) - json as hex string, bson as int32_t length followed by length octets
#define RDT_NULL           0xDD // z - A null block value type, used as an error flag

// Ricey key types         bsonish key    description
#define KTC_ObTerm_o              0x00 // Object terminator, indicates no more items in the object in bsonish lists
#define KTC_int64_i               0x01 // Generic signed int64, use depends on context.
#define KTC_int32_l               0x02 // Generic signed int32, use depends on context.
#define KTC_mpz_n                 0x03 // Generic MPZ large integer, use depends on context.
#define KTC_mpq_r                 0x04 // Generic MPQ large integer fraction, use depends on context.
#define KTC_type_y                0x05 // Identifies an algorithm type for hash or crypto key, or other types depending on context
#define KTC_text_s                0x06 // Generic string, use depends on context, for instance in an image object this would be the alt text
#define KTC_data_b                0x07 // Generic data block, use determined by the object which contains it.
#define KTC_chainBlock_o          0x10 // A Chain Block always contains a signedBlock_O which includes a time_i, and a signature_O.
#define KTC_signedBlock_o         0x20 // A Signed Block in the chain always contains a parentSignature_O a time_i of when the block was signed, the signature_O is found in the object which contains the signedBlock_O.
#define KTC_parentSignature_o     0x30 // Contains a copy of the signature_O of one or more (more in the case of merging forked chains) parent blocks.
#define KTC_signature_o           0x40 // Hash object, only found in chainBlock_O and similar objects which contain a signedBlock_O, contains an algorithm id (type_y) and the hash by that algorithm of the binary representation of the objects in the signedBlock_O (data_b) and a timestamp (time_i) matching the time_i in the signedBlock_O and which must not be as old or older than any parent timestamp and also should not be in the future, these time_i are unique identifiers for chainBlock_O objects.
#define KTC_time_i                0x11 // UTC time of block creation (microseconds since epoch x 189).  May never be less than or equal to the timei of any parent block.  Serves as a unique id for AOBO blocks.
#define KTC_AOShares_n            0x13 // A quantity of shares - meaning depends on context
#define KTC_SHA256_y              0x15 // algorithm ID (algor) for SHA2 - 256
#define KTC_ECB256_y              0x25 // algorithm ID (algor) for Elliptic Curve ECDSA curve BrainpoolP256r1
#define KTC_RSA3072_y           0x8025 // algorithm ID (algor) for 3072 bit RSA - less commonly used algorithms get longer ricey codes
#define KTC_SHA3b512_y          0x8015 // algorithm ID (algor) for SHA3 - 512
#define KTC_jpg_y               0x9015 // Found in image objects, tells what format the data_b is to be interpreted as
#define KTC_png_y               0x9025 // Found in image objects, tells what format the data_b is to be interpreted as
#define KTC_AOGenesisBlock_o  0xA0B000 // An Assign Onward Genesis Block. With a zzz0 which includes a timei, and other definitions for the chain.
#define KTC_AOChainDesc_o     0xA0CD00 // Chain Description, generally only found in the Genesis Block - descriptive items like name, symbol, number of coins represented by all shares, etc.
#define KTC_AOChainFunc_o     0xA0CF00 // Chain Functional items description, generally only found in the Genesis Block - functional items like number of shares, expiration rates, recording fees, etc.
#define KTC_AOEndChain_o      0xA0EC00 // Used when reporting the last blocks in a chain, this block is a signal that all known blocks are received.  Has a pzzzO and zzzO and timei of when it was reported, but contains no transaction information and will (usually) be discarded when appending new transaction blocks to its parent.
#define KTC_Symbol_s          0xA0CD06 // A short string like ITC representing the chain.
#define KTC_CdName_s          0xA0CD16 // A longer string like Itcoin representing the chain.
#define KTC_Tagline_s         0xA0CD26 // A brief sentence like "The greatest blockchain ever" describing the chain.
#define KTC_Description_s     0xA0CD36 // A paragraph (say 1000-ish characters) describing the chain.
#define KTC_Icon_o            0xA0CD10 // A square-ish image that looks good small, representing the chain.
#define KTC_Banner_o          0xA0CD20 // A wide image that might be displayed near the top of a page, representing the chain.
#define KTC_Image_o           0xA0CD30 // A more detailed image representing the chain.
#define KTC_CfShares_n        0xA0CF03 // The number of shares recorded in the Genesis block.
#define KTC_CfCoins_n         0xA0CF13 // The number of "coins" all the shares in this chain represent.
#define KTC_CfRecFee_r        0xA0CF04 // The number of coins per byte charged to record data on this chain (usually 1/a very large number).

#endif // RICEYCODES_H
