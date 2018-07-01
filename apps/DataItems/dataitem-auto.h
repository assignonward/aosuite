#define AO_INDEXV                       3 // (06+var) DataVbc64: Variable length unsigned integer using variable byte code up to 5 bytes in length, stored in an int32.  Meaning varies based on context, but generally used as a list index
#define AO_ECDSA_PUB_KEY2               1 // (02+32) PublicKeyEcdsa: ECDSA Public Key, type 2 compressed
#define AO_ECDSA_PUB_KEY3              -1 // (03+32) PublicKeyEcdsa: ECDSA Public Key, type 3 compressed
#define AO_ECDSA_PUB_KEY4               2 // (04+64) PublicKeyEcdsa: ECDSA Public Key, type 4 uncompressed
#define AO_HASH512                      5 // (0a+64) Hash512: SHA3-512 hash of some data
#define AO_HASH256                      6 // (0c+32) Hash256: SHA2-256 hash of some data
#define AO_SALT256                      7 // (0e+32) Salt256: 256 random bits
#define AO_PUB_RSA3072_ID               8 // (10+32) Hash256: Hash256 of a PublicKeyRsa3072
#define AO_HASH224SALT32               12 // (18+32) Hash224Salt32: Hash224 of some data including a 32 bit salt
#define AO_TIME_OF_SIG                 32 // (40+16) AOTime: UTC time (secs since epoch << 64) when a signature was made
#define AO_PROPOSAL_TIME             7692 // (9878+16) AOTime: UTC time (secs since epoch << 64) when a transaction was proposed (fixes coin-shares representations)
#define AO_RECORDING_DEADLINE        7564 // (9876+16) AOTime: UTC time (secs since epoch << 64) when a record is contracted to be recorded
#define AO_TIME_RECORDED             7500 // (9875+16) AOTime: UTC time (secs since epoch << 64) when a record was made
#define AO_TIME_DIFF                   35 // (46+16) AOTime: UTC time (secs since epoch << 64) defining a time interval, or difference
#define AO_UNDERWRITING_EXPIRATION     36 // (48+16) AOTime: UTC time (secs since epoch << 64) when underwriting shares are bound until
#define AO_AMT                         37 // (4a+16) Shares: 128 bit signed integer number of shares assigned
#define AO_UNDERWRITING_AMT            38 // (4c+16) Shares: 128 bit signed integer number of shares committed as underwriting
#define AO_RECORDING_BID             7620 // (8877+16) Shares: 128 bit signed integer number of shares bid for recording
#define AO_SHARES_OUT                  40 // (50+16) Shares: 128 bit signed integer number of shares outstanding (recorded on blocks)
#define AO_N_COINS                     44 // (58+16) AOCoins: Number of coins, as a fixed point 64.64 bit number
#define AO_SHARE_STATE                 47 // (5e+var) DataVbc64: 8 bit signed integer declares the state of shares (available, under contract, contract executed (assigned away), committed as underwriting)
#define AO_LISTSIZE                    48 // (60+var) DataVbc64: 32 bit unsigned integer stored as variable byte code, declares the size of a list, as a check that all intended data is present.
#define AO_INDEX                       49 // (62+var) DataVbc64: 16 bit signed integer declares position of an element in a list, used to check/correlate two lists with each other.
#define CB_FIRST_ID_SEQ_NUM            50 // (64+var) DataVbc64: First sequential ID number (of public keys) recorded in this block.
#define CB_N_ID_SEQ_NUM                51 // (66+var) DataVbc64: Number of sequential ID numbers (of public keys) recorded in this block, redundant check.
#define AO_ID_SEQ_NUM                  52 // (68+var) DataVbc64: 64 bit integer that identifies a particular public key in the chain.
#define AO_RSA3072_PUB_KEY             61 // (7a+384) PublicKeyRsa3072: an RSA3072 public key
#define AO_RSA3072_SIG                 62 // (7c+384) SigRsa3072: an RSA3072 signature
#define AO_ECDSA_SIG                   63 // (7e+var) SigEcdsa: An ECDSA signature
#define AO_ASSIGNMENT                  10 // (14+var) Assignment: Shares Assignment agreement before signatures
#define AO_PARTICIPANT                 42 // (54+var) Participant: Participant in a Shares Assignment agreement
#define AO_PARTICIPANT_CF              43 // (56+var) Participant: Participant in a Shares Assignment agreement, compact (aka chain) form
#define AO_AUTHORIZATION               31 // (3e+var) Authorization: An assignment plus a list of signatures on the authorization
#define AO_ASSIGN_REF                  30 // (3c+var) GenericCollection: Describes a record of shares when they were given in a recorded assignment agreement
#define AO_DATABYTEARRAY             4096 // <8040+var> DataByteArray:separable Arbitrary data of any form, may contain NULL bytes, any length (within reason)
#define AO_NOTE                     -4160 // <8141+var> Note:separable UTF-8 free text, no specific function, but recorded in the blockchain
#define AO_KEY_ASSET                 3765 // <ea3a+var> GenericCollection:separable Key pair reference coupled with the sharesRef they are associated with, if any.  Intended for use by Asset Organizers.
#define AO_BLOCK_REF                   29 // (3a+var) BlockRef: Describes a whole block in the chain
#define AO_PAGE_REF                    28 // (38+var) PageRef: UTF-8 free text, no specific function, but recorded in the blockchain
#define AO_GENESIS_REF                 27 // (36+var) GenesisRef: Uniquely describes a genesis block, includes list of properties used to calculate new blocks
#define AO_KEY_INDEX                   26 // (34+var) DataVbc64: Key/shares index in the blockchain, simple index number of a public key/shares receipt object in the blockchain
#define AO_SHARES_REF                  25 // (32+var) SharesRef: Reference to shares received
#define AO_ASSETS                    3856 // <a03c+var> Assets:separable A collection of lists of addresses for other asset organizers and recorders, references to shares, and unused keypairs
#define AO_ECDSA_PRI_KEY             3893 // <ea3c+var> PrivateKeyEcdsa:separable An ECDSA private key
#define AO_RSA3072_PRI_KEY           3829 // <ea3b+var> PrivateKeyRsa3072:separable An RSA3072 private key
#define AO_KEYPAIR                   3957 // <ea3d+var> KeyPair:separable A (hopefully matching) public-private key pair
#define AO_NETADDRESS                3125 // <ea30+var> NetAddress:separable IP4 or IP6 or named network contact address, potentially including :port number
#define AO_ORGANIZER                 2832 // (a02c+var) Organizer: Contact information for an asset organizer (user software)
#define AO_RECORDER                  2064 // (a020+var) Recorder: Contact information for a recorder (chainmaker software)
#define CB_CHAIN_BLOCK                 55 // (6e+var) GenericCollection: A full chain block, including all data - though potentially censored as required by local laws
#define CB_BLOCKMAKER                  56 // (70+var) PubKey: ID (public key) of the blockmaker, which holds claim on the residual shares in the block
#define CB_BLOCK_SIG                   57 // (72+var) Signature: Blockmaker's signature, includes a AO_TIME_OF_SIG time and CB_BLOCKMAKER PubKey
#define AO_AUTH_SIG                    58 // (74+var) Signature: Authorization signature, includes a AO_TIME_OF_SIG time and AO_INDEX index of the participant doing the signing
#define AO_SIG_WITH_TIME               59 // (76+var) Signature: Generic signature with All signatures include AO_TIME_OF_SIG time of signature, but no Index or PubKey
#define GB_GENESIS_BLOCK               80 // (a001+var) GenericCollection: A full Genesis block, including superfluous identifiers (text, images) to help brand/identify it
#define GB_PROTOCOL                   144 // (a002+var) DataVbc64: Basic level of protocol, identifies functionality
#define GB_PROTOCOL_REV               208 // (a003+var) DataVbc64: Revision of the protocol, may indicate additional types supported
#define GB_TEXT_SYMBOL                272 // (a004+var) Note: Short unique symbol that uniquely identifies the chain e.g. TⒶ1a
#define GB_DESCRIPTION               1040 // <a010+var> Note:separable Text description of the chain
#define GB_ICON                      1104 // <a011+var> DataByteArray:separable Image suitable for icon use to represent the chain
#define GB_IMAGE                     1168 // <a012+var> DataByteArray:separable Large format image to represent the chain
#define GB_STARTING_SHARES            336 // (a005+16) Shares: Starting number of shares
#define GB_MIN_BLOCK_INT              400 // (a006+16) AOTime: Minimuim block interval time
#define GB_N_COINS_TOTAL              464 // (a007+16) AOCoins: Number of coins that the sum of all shares outstanding represents
#define GB_RECORDING_TAX              528 // (a008+16) AOCoins: Recording Tax in coins per byte (usually a very small number)
#define AO_UNDEFINED_DATAITEM      977775 // (dead77+0) DataItem: An undefined data item, usually an error