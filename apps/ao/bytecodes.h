#ifndef BYTECODES_H
#define BYTECODES_H

// The atomic chunks: fixed length, single byte checksum
#define AO_CODE_MASK 0xF0

// First 3 bits, when 001 means: this is a 18 byte chunk, one for the code, 16 for the data, one for the checkbyte
#define AO_TIME_CODE          0x20
#define AO_TIME_RECORDED    ( 0x00 | AO_TIME_CODE )
#define AO_CLOSING_DEADLINE ( 0x01 | AO_TIME_CODE )
#define AO_TIME_OF_SIG      ( 0x02 | AO_TIME_CODE )

#define AO_SHARES_CODE        0x30
#define AO_TRANSACTION_AMT  ( 0x00 | AO_SHARES_CODE )
#define AO_MINU_AMT         ( 0x01 | AO_SHARES_CODE )
#define AO_RECORDING_BID    ( 0x02 | AO_SHARES_CODE )

// First 2 bits, when 010 means: this is a 34 byte chunk, one for the code, 32 for the data (hash), one for the checkbyte
#define AO_HASH256          0x40
#define AO_SALT             0x41
#define AO_PUBLIC_KEY2      0x42  // Two types of public keys, only one used at a time
#define AO_PUBLIC_KEY3      0x43

// First 2 bits, when 011 means: this is a 42 byte chunk, one for the code, 40 for the data (time+hash), one for the checkbyte
#define AO_PAGEREF          0x60 // 8 bytes time + 32 bytes hash

// First 2 bits, when 000 means: this is a 4 byte chunk, one for the code, 2 for the data, one for the checkbyte (generally a 16 bit int)
#define AO_LISTSIZE         0x00

// Variable length items

// First 2 bits, when 100 means: this is a short variable length byte array
// Next byte tells the length of the following byte array
// Next bytes are the byte array
// Last byte is the checkbyte - total length 3+length of data
#define AO_ECDSA_SIG        0x80  // starts with 8 bytes for time of signature, followed by the ECDSA signature itself

// First 2 bits, when 101 means: this is a long variable length byte array, 4 byte data size, followed by 4 byte checksum
#define AO_TRANSACTION      0xA0  // wrapper around the transaction data fields, which can come in any order
#define AO_PARTICIPANT      0xA1  // wrapper around the participant data fields, which can come in any order
#define AO_PARTICIPANT_LIST 0xA2  // starts with an AO_LISTSIZE, followed by that many participant objects
#define AO_SIGNATURE_LIST   0xA3  // starts with an AO_LISTSIZE, followed by that many signature objects
#define AO_NOTE             0xAA  // optional field in the participant item

#endif // BYTECODES_H
