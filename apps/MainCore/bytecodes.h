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

#define AO_ECDSA_PUB_KEY2        0x02 // PublicKeyEcdsa (02)32: ECDSA Public Key, type 2 compressed
#define AO_ECDSA_PUB_KEY3        0x03 // PublicKeyEcdsa (03)32: ECDSA Public Key, type 3 compressed
#define AO_HASH256               0x06 // Hash256 (06)32: SHA2-256 hash of some data
#define AO_SALT256               0x09 // Salt256 (09)32: 256 random bits
#define AO_PUB_RSA3072_ID        0x0c // Hash256 (0c)32: Hash256 of a PublicKeyRsa3072
#define AO_HASH224SALT32         0x0e // Hash224Salt32 (0e)32: Hash224 of some data including a 32 bit salt
#define AO_HASH512               0x0f // Hash512 (0f)64: SHA3-512 hash of some data
#define AO_TIME_OF_SIG           0x12 // AOTime (12)16: UTC time (secs since epoch << 64) when a signature was made
#define AO_TIME_RECORDED         0x15 // AOTime (15)16: UTC time (secs since epoch << 64) when a record was made
#define AO_RECORDING_DEADLINE    0x18 // AOTime (18)16: UTC time (secs since epoch << 64) when a record is contracted to be recorded
#define AO_TIME_DIFF             0x1b // AOTime (1b)16: UTC time (secs since epoch << 64) defining a time interval, or difference
#define AO_UNDERWRITING_EXPIRATION 0x1e // AOTime (1e)16: UTC time (secs since epoch << 64) when underwriting shares are bound until
#define AO_ASSIGNMENT_AMT        0x21 // Shares (21)16: 128 bit signed integer number of shares assigned
#define AO_UNDERWRITING_AMT      0x24 // Shares (24)16: 128 bit signed integer number of shares committed as underwriting
#define AO_RECORDING_BID         0x27 // Shares (27)16: 128 bit signed integer number of shares bid for recording
#define AO_SHARES_OUT            0x2a // Shares (2a)16: 128 bit signed integer number of shares outstanding (recorded on blocks)
#define AO_SHARE_STATE           0x2d // ShareState (2d)1: 8 bit signed integer declares the state of shares (available, under contract, contract executed (assigned away), committed as underwriting)
#define AO_N_COINS               0x2e // AOCoins (2e)1: Number of coins, as a fixed point 64.64 bit number
#define AO_LISTSIZE              0x30 // Data16 (30)2: 16 bit signed integer declares the size of a list, as a check that all intended data is present.
#define AO_INDEX                 0x33 // Data16 (33)2: 16 bit signed integer declares position of an element in a list, used to check/correlate two lists with each other.
#define AO_RSA3072_PUB_KEY       0x36 // PublicKeyRsa3072 (36)384: an RSA3072 public key
#define AO_RSA3072_SIG           0x39 // SigRsa3072 (39)384: an RSA3072 signature
#define AO_ASSIGNMENT            0x3c // Assignment (3c)-1: Shares Assignment agreement before signatures
#define AO_PARTICIPANT           0x3f // Participant (3f)-1: Participant in a Shares Assignment agreement
#define AO_PARTICIPANT_CF        0x42 // Participant (42)-1: Participant in a Shares Assignment agreement, compact (aka chain) form
#define AO_AUTHORIZATION         0x45 // Authorization (45)-1: An assignment plus a list of signatures on the authorization
#define AO_ASSIGN_REF            0x48 // AssignRef (48)-1: Describes a record of shares when they were signed away in a binding contract
#define AO_DATABYTEARRAY         0x4a // DataByteArray (4a)-1: Arbitrary data of any form, may contain NULL bytes, any length (within reason)
#define AO_NOTE                  0x4b // Note (4b)-1: UTF-8 free text, no specific function, but recorded in the blockchain
#define AO_BLOCK_REF             0x4e // BlockRef (4e)-1: Describes a whole block in the chain
#define AO_PAGE_REF              0x51 // PageRef (51)-1: UTF-8 free text, no specific function, but recorded in the blockchain
#define AO_GENESIS_REF           0x54 // GenesisRef (54)-1: Uniquely describes a genesis block, includes list of properties used to calculate new blocks
#define AO_SIG_WITH_TIME         0x57 // Signature (57)-1: All signatures include the time of signature
#define AO_SHARES_REF            0x5a // SharesRef (5a)-1: Reference to shares received potentially including info on their current state in the chain
#define AO_ASSETS                0x5d // Assets (5d)-1: A collection of lists of addresses for other asset organizers and recorders, references to shares, and unused keypairs
#define AO_ECDSA_PRI_KEY         0x60 // PrivateKeyEcdsa (60)-1: An ECDSA private key
#define AO_RSA3072_PRI_KEY       0x63 // PrivateKeyRsa3072 (63)-1: An RSA3072 private key
#define AO_KEYPAIR               0x66 // KeyPair (66)-1: A (hopefully matching) public-private key pair
#define AO_LONGBYTEARRAY         0x69 // ByteArrayLong (69)-1: Arbitrary data, potentially lots of it
#define AO_ORGANIZER             0x6c // Organizer (6c)-1: Contact information for an asset organizer (user software)
#define AO_RECORDER              0x6f // Recorder (6f)-1: Contact information for a recorder (chainmaker software)
#define AO_NETADDRESS            0x72 // NetAddress (72)-1: IP4 or IP6 or named network contact address, potentially including :port number
#define AO_GENESIS_BLOCK         0x75 // GenesisBlock (75)-1: A full Genesis block, including superfluous identifiers (text, images) to help brand/identify it
#define AO_CHAIN_BLOCK           0x78 // TBD (78)-1: A full chain block, including all data - though potentially censored as required by local laws
#define AO_ECDSA_SIG             0x7b // SigEcdsa (7b)-1: An ECDSA signature
#define AO_PROTOCOL              0x12c // Data16 (ac02)2: Basic level of protocol, identifies functionality
#define AO_PROTOCOL_REV          0x12f // Data16 (af02)2: Revision of the protocol, may indicate additional types supported
#define AO_TEXT_SYMBOL           0x132 // Note (b202)-1: Short unique symbol that uniquely identifies the chain e.g. TⒶ1a
#define AO_DESCRIPTION           0x135 // Note (b502)-1: Text description of the chain
#define AO_ICON                  0x138 // DataByteArray (b802)-1: Image suitable for icon use to represent the chain
#define AO_IMAGE                 0x13b // DataByteArray (bb02)-1: Large format image to represent the chain
#define AO_STARTING_SHARES       0x13e // Shares (be02)16: Starting number of shares
#define AO_MIN_BLOCK_INT         0x8765 // AOTime (e58e02)16: Minimuim block interval time
#define AO_N_COINS_TOTAL         0x144 // AOCoins (c402)16: Number of coins that the sum of all shares outstanding represents
#define AO_RECORDING_TAX         0x147 // AOCoins (c702)16: Recording Tax in coins per byte (usually a very small number)
#define AO_UNDEFINED_DATAITEM    0x7f // DataItem (7f)0: An undefined data item, usually an error

#endif // BYTECODES_H
