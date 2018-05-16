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
#ifndef BYTECODES_H
#define BYTECODES_H

#define AO_CODE_MASK           0xF8
#define AO_SIZE_MASK           0xF0
#define AO_VARSIZE_MASK        0xE0

// The atomic chunks:
// First bit, when 0, means fixed length, single byte checksum
// First 4 bits, when 0000 means: this is a 34 byte chunk, one for the code, 32 for the data (hash), one for the checkbyte
#define AO_SIZE_34BYTES        0x00
#define AO_HASH256             0x00
#define AO_SALT256             0x01
#define AO_ECDSA_PUB_KEY2      0x02  // Two types of public keys, only one used at a time
#define AO_ECDSA_PUB_KEY3      0x03
#define AO_PUB_RSA3072_ID      0x07  // Hash256 of the key itself, reducing size to 1/12th

// First 4 bits, when 0001 means: this is a 66 byte chunk, one for the code, 32 for the data (hash), one for the checkbyte
#define AO_SIZE_66BYTES        0x10
#define AO_HASH512             0x10

// First 4 bits, when 0010 means: this is a 18 byte chunk, one for the code, 16 for the data, one for the checkbyte
#define AO_SIZE_18BYTES        0x20
#define AO_TIME_CODE           0x20
#define AO_TIME_OF_SIG             ( 0x00 | AO_TIME_CODE )
#define AO_TIME_RECORDED           ( 0x01 | AO_TIME_CODE )
#define AO_RECORDING_DEADLINE      ( 0x02 | AO_TIME_CODE )
#define AO_UNDERWRITING_EXPIRATION ( 0x04 | AO_TIME_CODE )

#define AO_SHARES_CODE         0x28
#define AO_ASSIGNMENT_AMT    ( 0x00 | AO_SHARES_CODE )
#define AO_UNDERWRITING_AMT  ( 0x01 | AO_SHARES_CODE )
#define AO_RECORDING_BID     ( 0x02 | AO_SHARES_CODE )

// First 4 bits, when 0100 means: this is a 4 byte chunk, one for the code, 2 for the data, one for the checkbyte (generally a 16 bit int)
#define AO_SIZE_4BYTES         0x40
#define AO_LISTSIZE            0x40
#define AO_INDEX               0x41

// First 4 bits, when 0111 means: this is a 388 byte chunk, one for the code, 384 for the data, three for the checkbytes (generally an RSA3072 key)
#define AO_SIZE_388BYTES       0x70
#define AO_RSA3072_PUB_KEY     0x70
#define AO_RSA3072_SIG         0x71  // Just the signature

// Variable length items

// First 3 bits, when 101 means: this is a long variable length byte array, 3 byte data size, followed by 4 byte checksum
#define AO_SIZE_VARLENLONG     0xA0
#define AO_ASSIGNMENT          0xA0  // wrapper around the assignment data fields, which can come in any order
#define AO_PARTICIPANT         0xA1  // wrapper around the participant data fields, which can come in any order
#define AO_PARTICIPANT_CF      0xA2  // wrapper around the compact form participant data fields, which can come in any order
#define AO_SIGNATURE_LIST      0xA5  // starts with an AO_LISTSIZE, followed by that many signature objects
#define AO_AUTHORIZATION       0xA6  // an assignment, fully countersigned by all parties to the assignment
#define AO_NOTE                0xAA  // optional field in the participant item
#define AO_BLOCK_REF           0xAB  // reference to a block
#define AO_PAGE_REF            0xAC  // reference to a page in a block
#define AO_SIG_WITH_TIME       0xAE  // contains time of signature, and the signature itself
#define AO_ORGANIZER           0xB8  // collection of data to describe an Organizer entity
#define AO_RECORDER            0xB9  // collection of data to describe a Recorder entity
#define AO_NETADDRESS          0xBA  // string with an IP4, IP6 or FQDN address, optionally with :port number

// First 4 bits, when 1110 means: this is a short variable length byte array, up to 255 bytes
// Next byte tells the length of the following byte array
// Next bytes are the byte array
// Last two bytes are checkbytes - total length 4+length of data
#define AO_SIZE_VARLENSHORT    0xE0
#define AO_ECDSA_SIG           0xE0  // just the ECDSA signature itself

// Flag that somebody forgot to initialize the type
#define AO_UNDEFINED_DATAITEM  0xFF

#endif // BYTECODES_H
