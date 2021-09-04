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
#define RDT_OBJECT    0x0 // O - list of zero or more key-value pairs json encased in {} items separated by commas, in bson a key of Obterm ends the current object
#define RDT_INT64     0x1 // i - 64 bit signed integer - json in decimal string, bson as 8 bytes using void qToLittleEndian(qint64 src, uchar *dest)
#define RDT_MPZ       0x2 // n - libGMP signed integer - json as decimal string, bson as??? native GMP
#define RDT_MPQ       0x3 // r - libGMP rational fraction - json as decimal string/decimal string, bson as??? native GMP
#define RDT_RCODE     0x4 // y - Ricey code - json as the table defined name, bson as a list of bytes: last byte has 0 in most significant bit.
#define RDT_STRING    0x6 // s - UTF8 encoded string - json as UTF8 string, bson as int32_t length (includes terminator) followed by UTF8 string plus null terminator
#define RDT_BYTEARRAY 0x7 // b - ByteArray (aka octet stream) - json as hex string, bson as int32_t length followed by length octets

// Ricey key types     bsonish key    json key        description
#define KTC_ObTermO           0x00 // ObTermO       - Object terminator, indicates no more items in the object
#define KTC_AOChainBlockO     0x10 // AOChainBlockO - contains an Assign Onward Chain Block. Always contains a pzzz0 and a zzz0 which includes a timei.
#define KTC_pzzzO             0x60 // pzzzO         - Contains a copy of the zzzO of one or more (more in the case of merging forked chains) parent blocks.
#define KTC_zzzO              0x70 // zzzO          - Hash object, only found in O lists of objects, contains al algorithm id (algor) and the hash by that algorithm of the binary representation of the rest of the objects in the list (datab) and a timestamp (timei) which must not be older than any parent timestamp and also should not be in the future.
#define KTC_timei             0x01 // timei         - UTC time of block creation (microseconds since epoch x 189).  May never be less than or equal to the timei of any parent block.  Serves as a unique id for AOBO blocks.
#define KTC_datab             0x07 // datab         - generic data block, use determined by the object which contains it.
#define KTC_algorthmy         0x04 // algorthmy     - Identifies an algorithm type for hash or crypto key.
#define KTC_SHA256y           0x14 // SHA256y       - algorithm ID (algor) for SHA2 - 256
#define KTC_ECB256y           0x24 // ECB256y       - algorithm ID (algor) for Elliptic Curve ECDSA curve BrainpoolP256r1
#define KTC_RSA3072y        0xA024 // RSA3072y      - algorithm ID (algor) for 3072 bit RSA
#define KTC_SHA3b512y       0xA014 // SHA3b512y     - algorithm ID (algor) for SHA3 - 512
#define KTC_AOGenesisBlockO 0xA000 // AOGenBO       - An Assign Onward Genesis Block. With a zzz0 which includes a timei, and other definitions for the chain.
#define KTC_AOSharesn       0xA002 // AOSharesn     - A quantity of shares
#define KTC_AOEndChainO     0xAEC0 // AOEndChainO   - Used when reporting the last blocks in a chain, this block is a signal that all known blocks are received.  Has a pzzzO and zzzO and timei of when it was reported, but contains no transaction information and will (usually) be discarded when appending new transaction blocks to its parent.

#endif // RICEYCODES_H
