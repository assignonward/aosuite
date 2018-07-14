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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    MainWinCommon(parent),
    ui(new Ui::MainWindow)
{ ui->setupUi(this);
  restoreConfig();
}

MainWindow::~MainWindow()
{ delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{ saveConfig();
  QApplication::processEvents( QEventLoop::AllEvents,100 );
  QMainWindow::closeEvent(event);
}

/**
 * @brief MainWindow::restoreConfig - in addition to the window size and
 *   placement saved in MainWinCommon, this app is also restoring assets.
 */
void MainWindow::restoreConfig()
{ MainWinCommon::restoreConfig();
  QSettings settings;
  assets = settings.value( "assets" ).toByteArray();
}

/**
 * @brief MainWindow::saveConfig - in addition to the window size and
 *   placement saved in MainWinCommon, this app is also saving assets.
 */
void MainWindow::saveConfig()
{ MainWinCommon::saveConfig();
  QSettings settings;
  settings.setValue( "assets", assets.toDataItem() );
}

/**********************************************************************
 * Assign Onward                                                      *
 *                                                                    *
 * Will Turner: This is either madness… or brilliance.                *
 * Jack Sparrow: It’s remarkable how often those two traits coincide. *
 * Pirates of the Caribbean: The Curse of the Black Pearl             *
 *                                                                    *
 **********************************************************************
 *
 * For Intra or Inter-ledger transfers:
 *
 * <PAGE> ledger entry page contents:
 *
 *   <AO_ASSIGNMENT> Coin transfer transaction contents:
 *     [AO_PROPOSAL_TIME] time of proposal
 *     [AO_RECORDING_DEADLINE] latest acceptable time of recording
 *     [AO_RECORDING_BID] maximum acceptable total recording fee
 *     <PART> List of each participant's:
 *       [KEY]  public key
 *       [AO_AMT]  amount to be transacted (signed 128 bit integer)
 *       [AO_NOTE] freeform binary data field (length+data), of any size
 *              but bear in mind that transaction size affects tax rate
 *              and there is a maximum block size of 1MB.
 *
 *   <AUTH> Authorization of transaction by the participants:
 *     <PART> List of each participant's:
 *       [TSIG] time of signing (approving) the proposed transaction <TRAN>
 *       [SIG]  signature on: TRAN+AUTH(PART(TSIG))
 *       For givers (empty for receivers):
 *       <PAGE> of participant's source of funds - actually redundant with the unique KEY
 *         [TREC] recording time of AUTH(PART(PAGE))
 *         [HASH] hash of AUTH(PART(PAGE)) - this page must show the giver as recipient of the AMT proposed in TRAN
 *
 *   <RECP> Recording proposal:
 *     <RCRD> List of each recorder's:
 *       [ADDR] current IP address (v4 or v6 with port)
 *       [KEY]  public key
 *       [AMT]  minimum amount to be taken as recording fee, sum of all RECP(RCRD(AMT)) <= TRAN(RBID)
 *       <PAGE> previous (most recent) ledger entry for RECP(RCRD(KEY))
 *         [TREC] time of RECP(RCRD(PAGE))
 *         [HASH] hash of RECP(RCRD(PAGE))
 *       [TSIG] time of signing [TSIG]
 *       [SIG]  signature on: TRAN+AUTH+RECP(RCRD(AMT))+TSIG indicating acceptance of proposal
 *     <PART> List of each participant's
 *       [TSIG] time of signing the recorder list
 *       [SIG]  signature on: TRAN+AUTH+RECP(RCRD)+TSIG indicating acceptance of the recorder list
 *
 *   <RECA> Recorders' acceptance:
 *     <RCRD> List of each recorder's:
 *       [AMT]  final commission amount
 *       [PBAL] post transaction balance after recording fee is transferred
 *       [TSIG] time of acceptance
 *       [SIG]  self-signature on: TRAN+AUTH+RECP+RECA(RCRD(AMT))+RECA(RCRD(PBAL))+RECA(RCRD(TSIG))
 *
 *   [TREC] Time of recording, recorders' last acceptance time
 *   [HASH] Hash of TRAN+AUTH+RECP+RECA+TREC performed by last recorder to accept
 *
 * ----------------------------
 *   Optional forward linking database to be stored by underwriters/chainmakers:
 *
 *   <NEXT> Next transactions:
 *     <PART> List of each participant's next transaction
 *       <PAGE> Descriptor of the next transaction page, which will be found in ledger AUTH(PART(PAGE(ROOT)))
 *         [TREC] time (zero filled until known)
 *         [HASH] (zero filled until known)
 *         <ROOT> the root page of the ledger the NEXT transaction is in for the corresponding TRAN(PART(KEY)),
 *                    must be the root of NEXT(PART(PAGE))'s ledger
 *           [TREC] time of NEXT(PART(PAGE(ROOT)))
 *           [HASH] hash of NEXT(PART(PAGE(ROOT)))
 *       <VAL>  Identity and proof of the recorder who is filling in this record
 *         [KEY]  public key of the recorder who is back-filling this next transaction TREC and HASH
 *         [TSIG] time of filling
 *         [SIG]  signature on: NEXT(PART(PAGE))+NEXT(PART(VAL(TSIG))) using private NEXT(PART(VAL(KEY)))
 *     <RCRD> List of each recorder's next transaction
 *       <PAGE> Descriptor of the next transaction page, which will be found in ledger RECP(RCRD(PAGE(ROOT)))
 *         [TREC] time (zero filled until known)
 *         [HASH] (zero filled until known)
 *       <VAL>  Identity and proof of the recorder who is filling in this record
 *         [KEY]  public key of the recorder who is back-filling this next transaction TREC and HASH
 *         [TSIG] time of filling
 *         [SIG]  signature on: NEXT(RCRD(PAGE))+NEXT(RCRD(VAL(TSIG))) using private NEXT(RCRD(VAL(KEY)))
 *
 ***********************************************************************************
 *
 * Formatting and validity:
 *   Times are all 128 bit unix times: TPRO, TRMX, TSIG, TREC
 *     64 bits for seconds since epoch,
 *     64 bits for fraction of second
 *   All ledgers use the same UTC clock
 *   Coin amounts: AMT, PBAL, RBID
 *     are all signed 128 bit integer fractions of the "one coin"
 *       Optionally, # of coins can float too, inflating when earned by
 *       transaction processors.  Not my favorite idea, but maybe a good
 *       way to "boot up" the ecosystem.
 *     givers show negative AMT
 *     receivers show positive AMT
 *     RBID is a 0 or positive amount
 *     PBAL is a 0 or positive amount
 *   KEY 2048 bit RSA? Lattice? Public Key
 *   SIG  signatures? Lattice?
 *   HASH SHA what?  Lattice?
 *
 *   Every KEY is associated with one, and only one, ledger ROOT
 *     even though it will appear in other ledgers when performing
 *     inter-ledger transfers, the KEY will always be associated
 *     with the same ledger ROOT in all transactions.
 *   PART participant lists are all in the same order
 *   PART participant lists must all be the same length
 *   Minimum participant list length: 2
 *   Maximum participant list length: 2048 - keeping a lid on transaction spam
 *   Each participant KEY may only appear in the list one time.
 *   RCRD recorder lists are all in the same order
 *   RCRD recorder lists must all be the same length
 *   Minimum number of unique ROOT in a transaction: 1
 *   Maximum number of unique ROOT in a transaction: 2
 *   Minimum recorder list length: 1 per unique ROOT
 *   Maximum recorder list length: 2000
 *   Each recorder KEY may only appear in the list one time.
 *   Each ROOT must have at least one recorder serving it
 *   Sum of transaction TRAN(PART(AMT)) amounts must equal TRAN(RBID)
 *   Sum of RECP(RCRD(AMT)) amounts must be less than or equal to TRAN(RBID)
 *   No RECP(RCRD(AMT)) may be less than 0
 *   Amounts found in RECA(RCRD(AMT)) may not be less than corresponding
 *     amounts found in RECP(RCRD(AMT)) and must not sum to more than TRAN(RBID).
 *     See below for suggested division of excess commission.
 *   No transaction TRAN(PART(AMT)) may be 0
 *   Post transaction balances PBAL must all be positive or 0
 *   Each participant's TRAN(PART(KEY)) must appear in their AUTH-ID entry's TRAN-KEY list
 *     Exception: for an initial transaction, the participant's AUTH- TREC, ID, and PBAL
 *       must all be 0, and their TRAN-KEY must not appear anywhere in the extant ledger.
 *   Each participant's AMT + their AUTH-ID entry's PBAL must be >= 0
 *   Each participant's PBAL must equal their AMT + their AUTH-ID entry's PBAL
 *   TIMA authorization times must follow the PROP transaction proposal time
 *     TIMA times may appear in any order
 *     TIMA times may be identical, but usually aren't
 *   TRAN(PART(REXC)) root times must all preceed or equal their corresponding AUTH(PART(TREC)) times
 *   AUTH(PART(TREC)) times must all precede TRAN(PROP) transaction proposal time
 *   Each AUTH-TREC time must match the TREC found in the AUTH-ID ledger page
 *   TREC finalization time must follow all TIMA authorization times
 *   Each TTAU and ID signature must validate with the KEY listed in the transaction TRAN
 *   No ledger shall accept a page with an TREC time in the future
 *   Transaction processors shall run NTP clients and maintain good synchronization
 *   NEXT-TREC and NEXT-ID are not involved in signature checks, but must either link
 *     properly to pages that point back to this page or be zero-filled
 *
 * Ledger synchronization:
 *   Only transactions which meet all of the above requirements are admitted to the ledger.
 *
 *   Transactions have a cooling off time [COOL] which corresponds to network communication and
 *     processing latency and syncronization skew.
 *
 *   Cooling pages are shared among all active transaction processors.
 *
 *   After [TREC]+[COOL] has passed, the proposed PAGE is compared against all other currently
 *     waiting proposed pages, any with KEYs that match KEYs in the proposed PAGE are checked to
 *     to ensure that they all would be valid if entered into the ledger.
 *
 *   Any transaction processor found to be signing a double-spend or otherwise invalid proposed
 *     ledger page is marked to HOLD future transactions, thier coin is effectively burned.
 *
 * Bonus:
 *   Automatic transaction handling between multiple networks, perhaps each has its own share
 *     of coin that it is responsible for real-time trading of, but the networks can also trade
 *     coin with each other when desired?  Yes.  For scalability, when a ledger reaches ~500 transactions
 *     per day, some transaction processors may establish a new sub-ledger.  When a sub-ledger falls below
 *     ~100 transactions per day for a period of 5 days or more, it should consolidate with another low-volume
 *     ledger.  All sub-ledgers must reference the root-ledger, but sub-ledger to sub-ledger transactions are
 *     permitted, and expected...  The root-ledger's primary purpose is to establish validity of sub-ledgers.
 *
 * Division of excess commission:
 *   When a transaction's TRAN(RBID) exceeds the sum of the minimum RECP(RCRD(AMT))s specified by all recorders
 *     there is excess commission to be divided.  When there is only one recorder (RCRD list length: 1), the excess
 *     commission all goes to them.  When there are multiple recorders, they may negotiate the division amongst
 *     themselves and arrive at any RECA(RCRD(AMT)) list that sums to TRAN(RBID) or less where each AMT is at least
 *     as much as the corresponding RECP(RCRD(AMT)), but... to encourage expedient transactions, it is suggested that
 *     excess commission be divided equally among all participating recorders, with any infinitesimal remainder given
 *     to the first recorder in the list.
 *   Note that if any selected recorder disagrees with the RECA(RCRD(AMT)), they have the option to not sign, rendering
 *     the PAGE invalid.  Parties to the transaction may then renegotiate, either adjusting the RECA(RCRD(AMT)) list or
 *     even proposing a new list of recorders to the participants.  If agreement cannot be reached by TRAN(TRMX), then
 *     the transaction fails and does not occur.
 *
 * Sample Transaction:
 *
 * Bob wants to give Alice some aoncoin.  Each has a wallet with multiple keys, each associated with a positive coin
 *   balance on one of several ledgers.  Bob sends a TRAN proposal to Alice:
 *
 *   <TRAN> Coin transfer transaction contents:
 *     [SALT] 256 bit random number included in all signatures
 *     [TPRO] time of proposal
 *     [TRMX] latest acceptable time of recording
 *     [RBID] maximum acceptable total recording fee
 *     <PART> List of each participant's:
 *       [KEY]  public key
 *       [AMT]  amount to be transacted (signed 128 bit integer)
 *
 * Bob runs a good wallet that makes a highly entropic SALT value, but it could as easily have been populated with
 *   any 256 bits - say instead of random bits, Bob's SALT said "Alice, please PM me your tits!" in UTF-8.  Oh dear,
 *   Bob's wallet (or poor judgement) has compromised the randomness of the SALT - this is not a problem the first
 *   time he does this, but Alice's wallet software might want to check on Bob's ledger transactions to make sure
 *   that there is at least variation in his SALT values - I mean, really Bob, can't you be more original than that?
 *   If Alice (or, more likely, her wallet software) has a problem with Bob's SALT, the transaction can be rejected
 *   and reproposed with a SALT of Alice's choosing.  Also, note that transaction recorders may refuse to approve
 *   transactions if they find a lack of randomness or repetition of SALT values in the ledger.
 *
 * Bob's wallet is also synchronized to a reasonably accurate NTP server, this is a requirement for all wallet software
 *   but the definition of "reasonably accurate" can vary.  When Alice receives TPRO, she should expect it to be within
 *   +/- 64 seconds of what time her wallet thinks it is, unless the proposal is travelling by e-mail or USB stick
 *   instead of a direct network connection.  In any event, TPRO must never be from Alice's concept of the future,
 *   while TRMX must be far enough in the future to reasonably allow the transaction to process and record using
 *   whatever communications method Alice, Bob and their Transaction Recorders have chosen.
 *
 * Bob is sort of a high roller in aoncoin, he presently holds 1/2^30 (about a millionth) of the total coin.  Consider
 *   that DogeCoin currently has a market cap of ~$613 million, assuming aoncoin has the same market valuation, Bob's
 *   aoncoin would be worth ~$613.  Bob is really horny, so he's going to send Alice 20% of his coin in a misguided
 *   attempt to impress her.  His wallet software uses an assumed value of $600M for the whole aoncoin and tells him
 *   that his transaction has an approximate market value of $120.  Maybe the world settles on the idea that there are
 *   8 billion aoncoin in total, since nobody has a very good grasp of what 2^127 means... in which case Bob's wallet
 *   might also note that he is transferring 1600 of the 8000 aoncoin he's holding.  Anyway... the wallet calculates
 *   an exact fraction of aoncoin which Bob is proposing to transfer and fills in the RBID and AMT fields, in human
 *   terms -8008 from Bob, +8000 to Alice, and an RBID of 8.  Transaction recorders are currently expecting about
 *   0.1% commission on transactions, so Bob's wallet is more or less on-target with the RBID of 8.
 *
 * Bob's wallet already knew one of Alice's public keys, by whatever method.  One of his keys with a current ledger
 *   balance of at least 8000 coin is populated in the transaction, and Alice's public key that Bob knows is populated
 *   in the slot that is receiving 7992 coin.  The proposed transaction is ready, Bob sends it to Alice, along with a
 *   plain-text note identifying himself and what this transfer is about; see, Bob, this - not the transaction SALT -
 *   is the right place to chat up your girl.
 *
 * Alice thinks Bob is more than a little crazy, that's too much money, but she does like aoncoin and could use a little
 *   more to buy a latte at the Geek Cafe next week, so she rejects his transaction and proposes a new one, this time
 *   with Bob's AMT -800 coin, Alice's AMT 799.2, the RBID at 0.8 and incidentally a random SALT from her wallet software.
 *   Alice's wallet also decides that it prefers to receive the coin from Bob under a newer KEY than the one he proposed,
 *   so it populates the receiver field with one of Alice's newer public KEYs.  Alice writes a short personal note to Bob
 *   describing her revision of the proposal and sends it to him.
 *
 * Bob is a little disappointed that he won't be able to go back in the ledger and show his grandkids how he did something
 *   clever with open source software hacking "back in the day," but... he accepts Alice's proposed transaction as is, which
 *   will prevent further back and forth for this step.  Bob's wallet now starts the AUTH block:
 *
 *   <AUTH> Authorization of transaction by the participants:
 *     <PART> List of each participant's:
 *       [TSIG] time of signing (approving) the propose transaction <TRAN>
 *       [SIG]  signature on: TRAN+AUTH(PART(TSIG))
 *       [PBAL] post transaction balance
 *       <PAGE> of participant's previous (most recent) ledger entry
 *         [TREC] recording time of AUTH(PART(PAGE))
 *         [HASH] hash of AUTH(PART(PAGE))
 *         <ROOT> root page of the ledger AUTH(PART(PAGE)) is found in
 *           [TREC] recording time of AUTH(PART(PAGE(ROOT)))
 *           [HASH] hash of AUTH(PART(PAGE(ROOT)))
 *
 *   by filling in the current time in TSIG, using his private key that corresponds to the public key in TRAN signing
 *   the TRAN & TSIG, and populating the most recent ledger page associated with that key (his key, so nobody else should
 *   be able to record transactions using it.)  This partial AUTH block is sent to Alice's wallet for completion.
 *
 * Alice can be a little snoopy at times, so she's using a wallet that shows the juicy details of the transaction.  All it
 *   has to tell her is whether or not there are enough funds in Bob's ledger entry for the transaction to proceed, but
 *   her wallet also shows her that Bob's most recent ledger entry shows a balance of 43276.284 aoncoin.  This isn't all
 *   the aoncoin that Bob is holding, he's got other keys on this same ledger, and a couple on other ledgers too, but this
 *   is his big account and now Alice has seen it.  If Bob wanted to be circumspect about it, he could have created a
 *   "throwaway" key with just enough coin in it to cover this transaction, and only given that key to Alice.  Alice's wallet
 *   could still have easily looked back in the ledger to see that that 800 coin came from an account with over 40000, and
 *   when that happened, but it would be more difficult (for a simple wallet program) to be sure that the bigger account was
 *   Bob's - all it would show was the time of the transaction, and a trace of previous transactions' amounts and times with
 *   related accounts.
 *   In any event, Alice gladly approves the transaction, signing it, and incidentally giving away to Bob that her balance
 *   in this account, after receiving Bob's 799.2 will be 819.2 aoncoin, and he can look back in the ledger and see regular
 *   transfers of ~250 aoncoin to the same account every Thursday night for the past 6 weeks - that would be the Geek Cafe,
 *   and from that he can deduce the public keys of the 3 other aoncoin using patrons who frequent the Geek Cafe on Thursday
 *   nights.  If the Geek Cafe wanted to make this harder, they, too, could use throwaway keys to collect aoncoin and be
 *   more circumspect about when they transfer those coins to use them for other things, but... why bother?  Their customers
 *   are Geeks, not drug dealers.
 *
 * So, the AUTH block is done, all signed.  Now, the transaction needs to be recorded, before the TRAN(TRMX) limit expires.
 *   Bob's wallet could have included a suggestion of recorder(s) it would prefer to use, but it didn't, and since the
 *   ball was in Alice's wallet's court when it came time to choose recorder agents, it's going to make its own decisions
 *   about which ones to try first.
 *
 *   <RECP> Recording proposal:
 *     <RCRD> List of each recorder's:
 *       [ADDR] current IP address (v4 or v6 with port)
 *       [KEY]  public key
 *       [AMT]  minimum amount to be taken as recording fee, sum of all RECP(RCRD(AMT)) <= TRAN(RBID)
 *       <PAGE> previous (most recent) ledger entry for RECP(RCRD(KEY))
 *         [TREC] time of RECP(RCRD(PAGE))
 *         [HASH] hash of RECP(RCRD(PAGE))
 *         <ROOT> the root page of the ledger this recorder is recording in, must be the root of RECP(RCRD(PAGE))'s ledger
 *           [TREC] time of RECP(RCRD(PAGE(ROOT)))
 *           [HASH] hash of RECP(RCRD(PAGE(ROOT)))
 *       [TSIG] time of signing [TSIG]
 *       [SIG]  signature on: TRAN+AUTH+RECP(RCRD(AMT))+TSIG indicating acceptance of proposal
 *     <PART> List of each participant's
 *       [TSIG] time of signing the recorder list
 *       [SIG]  signature on: TRAN+AUTH+RECP(RCRD)+TSIG indicating acceptance of the recorder list
 *
 * First, Alice and Bob's KEYs are from different ledger ROOTs, so they will need at least two recorders, one that works
 *   in each ledger.  Alice's wallet queries the ledger that Bob's key is in for the most recent 100 transactions, then
 *   looks through those to find the three most active recorder agents.  It also could have looked at Bob's last transaction
 *   and tried to use the same recorder again.  Anyway, it tries to contact the first, but that recorder signalled that it was
 *   currently busy with another transaction and suggested a different recorder key to try.  Alice's wallet has a contrary
 *   streak, so it tries the next recorder from it's analysis and this time gets a positive response, her wallet sends the TRAN
 *   and AUTH blocks to the recorder and it responds with a signed entry for the RCRD list, including a commission AMT of 0.2
 *   coin.  For Alice's ledger, her wallet contacts her usual recorder and gets lucky this time, again receiving a signed
 *   entry for the RCRD list.  Alice's wallet is satisfied with this recording arrangement, signs off on it in the RECP(PART)
 *   list, and sends the nearly filled out RECP to Bob's wallet for completion.
 *
 * Bob's wallet is a little more paranoid than Alice's - your coin is only as good as your ledger entry, and Bob has quite a
 *   bit more coin exposed in this transaction than Alice does, so... his wallet also selects different recording agents
 *   for each ledger for a total of four.  Each recording agent was demanding 0.2 coin, so this transaction with an RBID of
 *   0.8 will come in with zero excess commission for the recorders.  Bob's wallet signs this list and sends it back to Alice's
 *   wallet for confirmation.  Alice's wallet, seeing that the recorders it had selected are still present, has nothing to
 *   object to, so it resigns the new list of recorders and the transaction is released to all four recorders for validation and
 *   recording.
 *
 *   <RECA> Recorders' acceptance:
 *     <RCRD> List of each recorder's:
 *       [AMT]  final commission amount
 *       [PBAL] post transaction balance after recording fee is transferred
 *       [TSIG] time of acceptance
 *       [SIG]  self-signature on: TRAN+AUTH+RECP+RECA(RCRD(AMT))+RECA(RCRD(PBAL))+RECA(RCRD(TSIG))
 *
 * Each recorder verifies the transaction to its satisfaction.  While a recorder might try to cut corners and get sloppy
 *   with validations, there are a few factors to keep in mind about recorder selection:
 *
 *   Recorders are public entities who develop a reputation over time.  When a ledger entry is invalid, it is easily
 *   found by other recorders and even ordinary wallet users - a sloppy recorder's signature on an invalid ledger page
 *   is irrefutable. Recorders with long histories of timely and accurate dealings will be the first chosen for future
 *   business.
 *
 *   The absolute minimum number of recorders for an inter-ledger transaction is two, so unless both recorders
 *   are controlled by the same sloppy entity, there's a good chance that a sloppy recorder will get called out right away.
 *   As mentioned already, if wallet software is careful at all, it will double-check a recorder's work, and again
 *   discover an invalid ledger entry almost immediately.
 *
 *   Another factor that wallet software can use to select recorders is the amount of coin they show in the ledger, so
 *   recorders with more "skin in the game" will also get more business.  If a recorder approves and somehow manages to
 *   record an invalid ledger entry they will lose the coin that they are currently showing in the ledger.
 *
 *   This doesn't mean that new recorders with no coin can't find work.  Since all recorders on a transaction validate
 *   the transaction, adding a new recorder as the 3rd or 4th is no more risky than not using them.  New recorders are
 *   typically less busy and may be much faster to respond to validation requests.
 *
 * The last recorder to accept a transaction determines the page recording time, which is incorporated into the page hash.
 *
 *   [TREC] Time of recording, recorders' last acceptance time
 *   [HASH] Hash of TRAN+AUTH+RECP+RECA+TREC performed by last recorder to accept
 *
 * If a recorder messes up this very basic operation, first they and their IP address are blacklisted, and second another
 *  recorder on the transaction can easily fix it before recording the page to the shared ledger.
 *
 * A new page for the shared ledger is first committed for checking by the recording community, and then merged into the
 *   official ledger when sufficient cooldown time has passed to discourage double-spend attacks.  More analysis on this later.
 *
 * Meanwhile, Alice has received Bob's gift of coin, the transaction is propagating through both ledgers, and next Thursday
 *   night at Geek Cafe, when the barrista transacts 250 aoncoin away from Alice for her latte, that transaction will follow
 *   this one in the ledger.  For speedier processing of all four aoncoin users who sit at the same table and order their
 *   lattes at the same time, the Geek Cafe's wallet maintains multiple keys and each transaction goes to a different one,
 *   so no one transaction is "locked, waiting" for another to cooldown and complete.  Then, at close of business, the Geek
 *   Cafe consolidates the days receipts into a different single account to pay bigger bills with.  Note that the protocol
 *   allows many givers (up to 1023) to give some or all of their coin to a single receiver with a single transaction.
 *
 *****************************************************************************************************************************
 *
 * Core Concept: a scalable tree of ledgers
 *
 * What keeps the ledgers from growing to unmagageable sizes?  Well, rules, obviously, but what rules?  Just this one:
 *   When a ledger passes a certain number of incoming value transactions, it closes for new incoming transactions.
 *   Once a ledger is closed for new incoming transactions, any wallet that wants to receive value will have to do it
 *   on another ledger, possibly spawned off of the current one.
 *
 * Simulation and real-world live testing should show more effective break points, but a concept something like:
 *   The root of all 2^127-1 coin in the system ledger allows a single incoming transaction, the ICO which divides
 *   the coin into a number of accounts, perhaps as many as 1024.  All subsequent transactions in the root ledger are
 *   outgoing transactions to new ledgers that are designated "level 2" since they reference the root directly.
 *   Level 2 ledgers might allow up to 128 incoming value transactions before closing.  Ledgers spawned off of Level 2
 *   are designated level 3 ledgers.  To fully validate an entry in a Level 3 ledger, you would have to trace that entry
 *   back to the Level 3 root, then through the level 2 ledger to it's root, then the root ledger.  Still, since there will
 *   be hundreds, or maybe thousands of level 2 ledgers, this is much less data to plough through than if all transactions
 *   were contained in a single ledger.  Perhaps level 3 ledgers allow up to 256 incoming value transactions before closing,
 *   the level 4 ledgers might allow 512 incoming value transactions, and level 5 and higher ledgers might allow 1024 incoming
 *   value transactions - the thinking being that by the time you get out to level 5, there should be many millions of ledgers
 *   in the tree, and also opportunity to transfer value back down to a lower level ledger that is still accepting incoming
 *   value transactions.  A transaction between two unrelated level 5 ledgers might have to validate back through 4 ledgers each
 *   for a total of 8, but the transaction recorders working in each level 5 ledger should know that path well and be able to
 *   rely on previous knowledge for most transactions.
 *
 *   Or something like that.
 *
 * More on sub-ledger creation:
 *   What does a sub-ledger creation step look like?  A new sub-ledger starts with a single ordinary page.  The only thing that
 *   is special about the PAGE is that it refers to itself as ROOT.  When ledgers that a transaction processor is working with
 *   are all >50% "full," and a transaction is transferring coin to a new KEY (with zero value history), then that transaction
 *   processor may choose to instantiate the new KEY on a new sub-ledger instead of in one of the existing ledgers.  (When the
 *   existing ledgers are less than threshold full, it is preferable to keep the transactions on ledgers which are closer to
 *   the ultimate root ledger...)  The new sub-ledger has new keys for both the primary receiver of coin, and the transaction
 *   processor - with links back to the sources of coin in other sub-ledgers pages.  The transaction processor may choose to
 *   transfer some coin to this new sub-ledger from their entries on other sub-ledgers, both to distribute risk (the more value
 *   is spread around to different sub-ledgers, the less that will be lost in the event that a sub-ledger is lost), and also to
 *   increase transactant confidence in the transfer processor by showing that they have value invested in this ledger.  That
 *   transfer-in of value would be of the form: many sources of value from many other sub-ledgers, all transferred to one
 *   KEY on the newly created sub-ledger - in this way the transaction processor is also advertising other sub-ledgers they
 *   operate in, and total coin holdings in that collection of sub-ledgers.
 *
 ********************************************************************************************************************************
 *
 * Storing and sharing sub-ledgers:
 *   I keep getting the feeling that git would be a good tool to store and share ledgers with.
 *   Round numbers, a typical 4 processor, 2 participant transaction would include ~12 signatures, 14 hashes, and maybe 20
 *   coin amounts and times.  Put each signature at 2048 bits, hash at 256, and coin/time at 128, that's about 4K-bytes of
 *   data per ledger page.  So, if a typical sub-ledger has 1024 value-in pages and ~2048 value-out pages, that puts a
 *   sub-ledger at about 12 megabytes when done.  If each of the ~37,000 signatures takes about 5 milliseconds to compute,
 *   that's about 3 minutes to check every single signature in the sub-ledger, but most transactions will only look at a
 *   fraction of that number of signatures.
 *
 * In a git/ssh based structure, each sub-ledger could be a folder in a repository, and each page in that folder could be
 *   a file.  With a rough upper bound of 4000 files in the sub-ledger, the sub-ledger could itself have 64 sub-folders
 *   each with up to 64 files in them - more if necessary, but random distribution should scatter pages into the various
 *   sub-folders fairly well.  Then, each page in the sub-ledger would be its own file, which could be updated with NEXT
 *   pointers as they become available.
 *   The various transaction processor servers would each have their own local copy of the ledger repository, and would
 *   synchronize through the standard commit, push and pull operations on a regular basis.
 *
 * Locking account KEYs:
 *   While a transaction is in progress, that account KEY is effectively locked - prevented from performing other
 *   transactions.  This does not lock a user's entire wallet, just the value stored under that particular key.
 *   High volume merchants would set up a large-ish number of keys to receive value on their behalf, then bulk-transfer
 *   value from those "customer facing" keys into a larger store of value, or not depending on how they choose to run
 *   their operation.  This locking of a key starts as soon as a transaction processor is contacted requesting recording
 *   of a transaction - it continues for the needed cooling down period to check for incoming duplicate transactions,
 *   then the transaction is recorded on the ledger, another cooling down period ensues, and finally the keys for that
 *   transaction are relased to perform a new transfer.
 *
 ********************************************************************************************************************************
 *
 * Psychology of coin size:
 *   2^127 is not a human scale number.  8.6 billion (or 2^33) is just a bit over the current population of the earth, so that's
 *   sort-of inherently human-scale.  If everybody today had an equal share of aoncoin, that would be about 1.13 coin per person.
 *   In the not too distant future, world population should reach parity with the number of aoncoins - but, they are very
 *   unlikely to be evenly distributed.  I would consider an ICO to 1024 investors to be a reasonable launching point for "live"
 *   public use, hopefully enough to keep interest and activity up - supporting an active network of transaction processors - so,
 *   at ICO, each of the 1024 shares would receive 8388608 coins, and each coin would be sub-divisible up to 1.98x10^28 times.
 *   To be precise about it, one of the 1024 shares would be missing one infinitesimal slice since the total number of coin
 *   slices (ihsotas?) is 2^127-1.
 *   Now, if the ICO value is $1000 per share, for a total value of $1M, that would mean that each of the 8.6 billion coins starts
 *   out being worth 0.019 cents, or 8388 coins per dollar.  So, a latte might cost 30,000 coins - investors of course hope that
 *   it will inflate.   If the coin experiences a 10,000:1 increase in value, as some cryptocoins have, then at that point each
 *   coin would be worth about $1.20.
 *
 * Incentive to process transactions:
 *   Like miners, transaction processors provide the backbone of the exchange system and they are incentivised by taking a share
 *   of each transaction.  If the average daily volume exchanged is 30M coins (~1000 lattes at ICO), then 0.1% of that trading
 *   volume amounts to 30,000 coins - or about 1 latte for the transaction processor.  Again, investors hope for an increase in
 *   value, and processors hope for an increase in volume.  Still, processing 1000 transactions should be a relatively trivial
 *   amount of CPU invested and $3 per day should cover server/electricity costs.
 *
 * Incentive to use the system:
 *   A missing piece of the puzzle is: why would people exchange aoncoins?  If nobody is trading them, then the processors have
 *   no income - with no income the processors will shut down and then aoncoins will be less convenient to use and further lose
 *   value.  Who is going to accept aoncoin for valuable goods or services?  That's one that bitcoin is still struggling to
 *   answer definitively nearly 10 years after launch.  Some people do take it for some things - mostly speculation, but it's
 *   still very scattered for acceptance.
 *
 *************************************************************************************************************************
 *
 * Processing cost based tax:
 *   Even though the real-world cost to process a byte is extremely small, it can be quantified, and with each
 *   message being processed by a large parallel network of machines, that cost needs to be addressed.  Note:
 *   some businesses are developing to take advantage of the Bitcoin blockchain as a public recording medium
 *   by processing infinitesimally small transactions, deriving more value from the network than the cost to
 *   them to process the transaction.
 *
 *   This processing tax should not be awarded to any entity, to avoid such entities from processing un-necessary
 *   messages.  As such, eventually all the coin in the network will be "burned" in this manner.  If the tax is
 *   made proportional to the real-world cost of processing and the real-world trading value of the coin is low,
 *   it could quickly consume all the coin in the network, this tax should have some ceiling value to prevent
 *   frequent "split" operations from being necessary.  However, eventually, such a tax would "burn down" a
 *   significant fraction of coin in the network.  If the tax were the smallest possible 2^-127 fraction on each bit
 *   processed, and an average ledger page is 20KB, then the network could process a million pages an second for
 *   1.1*10^37 seconds or 3.3*10^30 years before burning 2^126 fractions (or, half the coin in the network), so,
 *   clearly the tax can be made small enough to not cause problems by burning "too much" coin, however - to have
 *   the desired psychological value, it would be desirable for the tax to be large enough to be felt by entities
 *   who process large volumes of data through the network.  This means an adjustable tax, preferably one that
 *   would "pay interest" at a realistic rate like 5% per year, this would mean "burning" half the coin roughly
 *   every 14 years, so the tax rate could be set like a trailing window average of transactions in the past
 *   30 days (2.592 million seconds), the total size of transactions during that period (1/170th of the desired
 *   half-period) should burn 1/340th of all coin.  So: the cost of transacting a single byte is 1/(340*N) where N
 *   is the number of bytes transacted in the previous 2.592 million seconds.  If volume is "light" at 20KB per
 *   second average, that's 1/1.7*10^13 of the total coin, or - if there are 2^33 (8B) "human scale" coins, that
 *   would be 0.0005 coin per byte, or 10 coin for a 20KB message.  Psychologically, I'd put a "floor" of 20KB
 *   per second on the message traffic measure, so during early days light traffic, the pinch of transaction tax
 *   doesn't seem too heavy.  This "inflation" for coins at rest could be realized by a split event once every
 *   14 years (or whenever the "burned" coin tally reaches 1/2 the total coin), such that all holdings from before
 *   the event are double counted.  Everybody's fraction of the total remains the same, but the numerical representation
 *   doubles.  Alternatively, and perhaps preferably, the tax could shrink as the pool of available coin shrinks,
 *   such that, after 14 years of transactions, the tax is taking 1/2 the numerical value it was at the start of
 *   the period, so the total pool of available coin shrinks by 1/2 every 14 years, instead of disappearing completely
 *   after 28 without a split event.  Still, should the tax on a single bit of data start to approach the 1/2^127
 *   threshold (by a wide margin - perhaps at 1/2^117), a split event would seem to be good to do... a simulation is
 *   called for, but - just swagging it, if the network is processing 200MB per second, that would be 1000th the
 *   tax calculated above, or 1/1.7*10^16, or 1/2^54, so there's still 63 bits of fidelity remaining, meaning
 *   63x14 = ~882 years of 5% inflation before a split event might be needed.
 *
 *   Early days, if 1000 ICO'ers buy equal shares of 8B coins, that's 8M coin each - a 1000 coin transaction would
 *   be giving 1 coin to the processor, and 10 coin to never-never land.  Wallet software might keep track of
 *   how much coin has been burned and report a consistent fraction of the remaining float, so that the number
 *   of "coins" in circulation remains constant - and people who don't move their coin would see the "interest"
 *   being paid.
 *
 * On the topic of miner incentives:
 *   https://themerkle.com/the-emergence-of-mining-as-an-ecosystem/
 *
 * It would seem, to me, that the whole coin eco-system might be realized as:
 *
 * Owners of coin - actual people who use:
 *   Lightweight wallet software, preferably mobile capable - possibly desktop based, which contracts with:
 *     Transaction processors - running servers, for example on AWS or Azure or a RaspberryPi on home internet, who use:
 *       Consensus processors who work out the content of the blocks in the block-chain.
 *
 *********************************************************************************************************************;'
 *
 * Name selection, some name confusion might be unavoidable?
 *
 * Assign Onward - like pay it forward, but different, and apparently unique - all the domain names were open.
 *
 *
 *********************************************************************************************************************;'
 *
 * Boiling down the parameters of the currency exchange rules:
 *
 * These parameters can be fixed, algorithmic, or voted on by the community and implemented by algorithm reading the
 *   votes, on a per-genesis block basis.
 *
 * Circulating Supply: nominal 8,000,000,000
 * Max annual interest rate for stationary currency: nominal +5%, could be anywhere from -99% to 1000% or more
 * Transaction commission: nominal 0.1% of transaction amount (given), could also be based on interest rate
 * Moving window for interest rate calculation: nominally based on trading volume for the past 30 days
 * Moving window for transaction commission calculation: nominal past 30 days
 * Block publishing minimum interval
 * Community based control of the above parameters:
 *
 * If the cryptocurrency markets are any indication, alt-coins are popular, and the above parameters are one way
 * to differentiate the various alt-coin exchanges by their rules of exchange.  A high interest alt-coin would
 * have high transaction fees (and presumably lower trading volume), but when there is trading volume that volume
 * is shared among holders.  A low interest, low commission alt-coin would have presumably higher trading volume
 * but might not attract enough transaction processsors to handle it.  The negative interest exchange is an
 * intriguing concept, perhaps as a short-term transfer medium - the exchange could give transaction processors
 * all of the fees, so the exchange of coin itself is no-fee or even "profitable", but while coin stays on the
 * ledger it devalues (presumably fairly rapidly) to fund the system.  This would make an interesting large scale
 * simulation to see if such a construct is viable... [HotCoin, sell it fast! before it devalues.  Would it be
 * valuable enough to transaction processors to pay for their efforts?  Could buyers of HotCoin sell it fast enough
 * to make the low transaction fees worthwhile.  It feels like, in an ideal market, all of these considerations
 * should cancel out, but markets are not ideal...]
 *
 * While on the topic of currency exchanges: https://themerkle.com/top-4-peer-to-peer-bitcoin-trading-platforms/
 *
 * It feels like, to succeed, the platform will need efficient and well accepted methods of exchanging the
 *   platform "coin" for other crypto-currencies, and of course real-world cash.  As such, it may be useful to
 *   build into the platform a bid/ask structure where holders of coin can advertise an ask price (pegged to
 *   other green-bits exchanges, maybe BTC, or (with much regulatory hassle) perhaps even real-world currency),
 *   and buyers can advertise a bid price for certain lot sizes of coin.
 *
 **********************************************************************************************************************
 *
 * The P2P network backbone.
 *   While RabbitMQ is an attractive message passing system that could be used to quickly build a network, I'm thinking
 *   that the problems faced by Bitcoin will be the same problems faced by this P2P network, and as such the Satoshi
 *   P2P network agent may be a more appropriate choice.
 *   https://en.bitcoin.it/wiki/Weaknesses#Denial_of_Service_.28DoS.29_attacks
 *   http://culubas.blogspot.com/2011/05/timejacking-bitcoin_802.html
 *   http://www.cs.bu.edu/~goldbe/NTPattack.html
 *   Note: GPS can be an alternate, reliable, accurate time source
 *     https://spectracom.com/resources/essential-education/gps-clock-synchronization
 *     https://learn.adafruit.com/adafruit-ultimate-gps-on-the-raspberry-pi?view=all
 *
 ***********************************************************************************************************************
 *
 * Spot the Merkle Tree application:
 *
 * The blockchain publishes a new block every minute.  Publication of this block may be delayed by ~10 minutes from the
 *   time a transaction is posted to the network.  If your transaction doesn't post to the network within ~15 minutes,
 *   you know it has failed and needs to be re-submitted.
 *
 * Block 0 TH(0) =         H(0) level 0 complete
 * Block 1 TH(1) =       H(H(0)+H(1)) level 1 complete
 * Block 2 TH(2) =     H(H(H(0)+H(1))+  H(2)     ) level 2 partial
 * Block 3 TH(3) =     H(H(H(0)+H(1))+H(H(2)+H(3)) level 2 complete
 * Block 4 TH(4) =   H(H(H(H(0)+H(1))+H(H(2)+H(3))+    H(4)                   ) level 3 partial
 * Block 5 TH(5) =   H(H(H(H(0)+H(1))+H(H(2)+H(3))+  H(H(4)+H(5))             ) level 3 partial
 * Block 6 TH(6) =   H(H(H(H(0)+H(1))+H(H(2)+H(3))+H(H(H(4)+H(5))+  H(6)      ) level 3 partial
 * Block 7 TH(7) =   H(H(H(H(0)+H(1))+H(H(2)+H(3))+H(H(H(4)+H(5))+H(H(6)+H(7))) level 3 complete
 * Block 8 TH(8) = H(H(H(H(H(0)+H(1))+H(H(2)+H(3))+H(H(H(4)+H(5))+H(H(6)+H(7)))+    H(8)       ) level 4 partial
 * Block 9 TH(9) = H(H(H(H(H(0)+H(1))+H(H(2)+H(3))+H(H(H(4)+H(5))+H(H(6)+H(7)))+  H(H(8)+H(9)) ) level 4 partial
 *
 * Note how level 2 always starts with the level 1 complete TH(1)
 *      and level 3 always starts with the level 2 complete TH(3)
 *      and level 4 always starts with the level 3 complete TH(7)
 * etc.  To compute a new tree-hash only requires Olog(n) hash computations, and the tree hash permits
 * a proof that any block exists in the latest tree with Olog(n) hash computations - so says Merkle, needs
 * some additional insight, says I.
 *
 **************************************************************************************************************************
 *
 * As tempting as the bitcoin source is to fork, I think I'm going with a de-novo implementation for the first cut.
 *   Full system image for the transaction processors (secured Linux, RabbitMQ) and Qt app for the wallet.
 *
 * For system topology, I still want to try to up the transaction per second capability to 100,000 per second if that's
 * in any way feasible (that would be over 8 billion per day).  This is going to require a multi-tiered block consolidation
 * scheme, which seems like a job for a clear head and a Merkle Tree.  First cut, I'll be thrilled if a minimal configuration'
 * can handle 30 per second, continuously.  Try to apply the lessons and strategies of Bitcoin, but don't get caught up
 * chasing compatibility - there are enough "me too" coin networks out there.
 *
 * Assign Onward is going to require positive identification of transactors, but more than that - it's going to require a more
 * refined trust profiling for transaction processors.  Wallets can (and should) obtain copies of the relevant ledger
 * pages from a trusted transaction processor.  Both wallets in a transaction should do their own validation of the
 * pages before they sign the AUTH block.  This begs the question: should the Tier 1 transaction processors be compensated
 * for processing invalid transactions?  Probably so, a much lower rate than a valid transaction which has to ripple through
 * many processors before being adopted - the first processor should be invalidating it right away.
 *
 * Wallets are tier 0, and they are the most likely to try to scam the system.  Tier 1 processors must be hypervigilent for
 * time spoof attacks, invalid signatures, fake blockchain page references, etc.
 *
 * To increase trust in the final block included in the chain, Tier 2 processors will only interact with Tier 1 processors
 * they should be able to establish better trust relationships and have more confidence when creating the blockchain.  Tier
 * 3 may facilitate scalability, both of transaction throughput and of trust in a busy blockchain.  Algorithmic determination
 * of processor topology would be preferable.
 *
 *****************************************************************************************************************************
 *
 * Microtransactions:
 *   Concept: everything has a cost.  That cost can be nano-pennies, but, if accounting only costs pico-pennies we're good.
 *   Prevent SPAM, require payment for data processing in proportion to bytes sent, processed and recorded - such payment
 *   should be provably not much higher than the actual cost of providing services.
 *
 * Analogy: Title Companies' Title Plants
 *   County halls of records hold the official record books, but title companies keep updated, indexed copies of the record
 *   books allowing rapid searches for ownership information.  Similarly, the block-chain is the official record, but
 *   transaction processors can take a copy of the whole block-chain and re-index it to make searches for proposed transaction
 *   validity much more efficient.  Mostly, this takes the form of adding forward links from old pages to new pages when the
 *   new pages take value away from the old pages.
 *
 * Design suggestion: organize title plant page references in a time index referenced folder tree
 *   Level 1 of the tree: 256 folders, each holding transactions from a different second (after 4.27 minutes return to first folder)
 *   Level 2 of the tree: 256 folders per folder, each holding transactions from different next most significant timestamps
 *     Level 2 folders are visited once every 18 hours
 *   Level 3 of the tree: 256 folders per folder, these are visited once every 194 days
 *   Level 4 of the tree: one folder per ledger page, holds the page itself plus any transactions that spend against it
 *     and any other notes the TP wants to make.
 *   Probably want to flip the directory tree upside-down to restrict activity to one root folder at a time
 *
 * A complete title plant may also want a record of every public key ever used in a similar storage tree with pointers
 *   to all the ledger pages on which they have transacted.
 *
 * KISS principle: overriding what is written above, givers in transactions always give the full value that they
 *   received on their reference page.  Any residual value not needed for this transaction is sent to a key under
 *   the giver's control on the new transaction page.  It can be the same key, but is good practice for it not to
 *   be the same key.  This simplifies validating value remaining in a given key on a given page because if any
 *   pages in the ledger refer to this page and key as a source of value, they are removing all the value from
 *   this page/recipient key combination.
 *
 * Another KISS principle: each new block in the block-chain includes a summary statement of the current float
 *   and other helpful information like the block tax rate.
 *
 *****************************************************************************************************************************
 *
 *  Rubber, meet road.  Top philosophy: it's the conceptual content of the blocks that matters, not the specific format or
 *    communication implementation - those should be modular and replaceable.
 *
 * Step 1: wallet to wallet negotiation of TRAN block.
 *
 *   A transaction (TRAN) block describes:
 *     givers and receivers by amounts (- for givers + for receivers) and identity (public keys)
 *     it also contains:
 *       a positive amount bid to pay transaction processors
 *       time of proposal and last acceptable time of transaction
 *       a random salt which ends up in every hash on the ledger page recording this transaction
 *
 *   Communication layer: modular.
 *     First module: .json files in a watched folder.
 *       Later modules:
 *         Binary files?
 *         Direct network transfer?
 *         NFC?  Very cool for phones.
 *         RabbitMQ mediated transfer?
 *           via paid transfer processor?
 *         Steganographic .png files?
 *
 *   Address book:
 *     ID by public key, correlate to human readable ID info (name, address, phone, photo, etc.)
 *
 *   Wallet function:
 *     List of Ledger pages with available funds to transfer
 *     Keys to said funds
 *       Secure keys with passphrase(s)
 *
 *   Transaction log:
 *     Proposals, acceptances.
 *
 *
 *   Step 2: wallet to wallet negotiation of AUTH block.
 *
 *     Timestamp and signatures.
 *
 *     Blockchain mirror:
 *       Partial copy of relevant records.
 *         Potentially queried from Level 1 transaction processor (for fee?)
 *         Potentially locally archived
 *
 *     AUTH negotiation:
 *       Timestamp and signatures.
 *
 *     Transaction log:
 *       Proposals, acceptances.
 *
 *   <TRAN> Coin transfer transaction contents:
 *     [SALT] 256 bit random number included in all signatures
 *     [TPRO] time of proposal
 *     [TRMX] latest acceptable time of recording
 *     [RBID] maximum acceptable total recording fee
 *     <PART> List of each participant's:
 *       [KEY]  public key
 *       [AMT]  amount to be transacted (signed 128 bit integer)
 *
 *   <AUTH> Authorization of transaction by the participants:
 *     <PART> List of each participant's:
 *       [TSIG] time of signing (approving) the propose transaction <TRAN>
 *       [SIG]  signature on: TRAN+AUTH(PART(TSIG))
 *       [PBAL] post transaction balance
 *       <PAGE> of participant's previous (most recent) ledger entry (only present if giver)
 *         [TREC] recording time of AUTH(PART(PAGE))
 *         [HASH] hash of AUTH(PART(PAGE))
 *
 *********************************************************************************************************************
 *
 * True cost of computations:
 *   If it takes 5ms to verify a digital signature, what's that actual cost in the real world?
 *   Assume 10W power consumption @ $0.11/kWh - that's 10/1000 kW, for 0.005/3600 hours, or about
 *   n$1.53 (nano-dollars).  Call the "fair" charge rate n$3 to account for equipment depreciation,
 *   housing costs, etc.  Now, if that signature is validated 8 billion times, n$3 * 8B = $24.
 *   Clearly, it's not practical for every person on the planet to validate every transaction.
 *   Some kind of transaction consolidation is called for.  If each blockchain serves 1 million
 *   people, then 8000 block chains can serve the world.  What is needed is a way to transfer
 *   between thousands of blockchains seamlessly.
 *
 * Scale target:
 *   8 billion simultaneous users on one network would seem.... ambitious.  If a single
 *   blockchain can scale to serve 100,000 active users (active = 6 transactions per day,
 *   or >10 transactions per second average on a peak day.)  Would be nice if the users
 *   were geographically distributed across time zones...
 *   100,000 users might average 600,000 transactions per day, each one a page (guesstimate 1KB)
 *   so after a year the blockchain would grow ~200GB.
 *   The real problem I see is: the graph is backwards.  When checking a transaction for
 *   validity, what you need to know is: has this page-key been spent between the time it was
 *   created with value and now?  So, an exhaustive search of the block-chain from the time of
 *   that page to the current time for a spend of that page-key is sufficient.  Is there a good
 *   way to make this search go faster?  Transaction processors could keep page-key reference
 *   lists for various time periods.  The block itself is the smallest list of page-key
 *   references, maybe every 8 blocks a bigger list is made sorted in page-key order, then
 *   every 64 blocks those lists are combined, then every 512 (8.5 minutes) then every 4096
 *   (just over an hour) 32768 (9.1 hours) 262144 (3 days) and 2097152 (24 days).  The 24 day
 *   record would be a searchable B-tree like index of ~14.4 million records - or ~14.4GB.
 *   The next level of this 8-tree would be 1677216, or 194 days / 111GB.  If the page-key
 *   records could be minimized to less than 1KB each, that would help... an abbreviated
 *   key perhaps 1/8 the real size would eventually get into collision issues, but those
 *   could be checked in exchange for the more compact and easily searched list.
 *   As a transaction ages, the cost of verifying its validity increases, thus there might be
 *   some kind of optimal age at which to "refresh" value to new pages, or perhaps an ideal
 *   interest rate at which the cost to process an older transaction is balanced out by the
 *   interest gained by the stationary currency.
 *
 *************************************************************************************************
 *
 *  Ascending levels of trust and commitment:
 *    A transaction proposal is just talk, nothing committed but an agreement in principle that
 *    such a transaction is desired between the parties.  It includes a proposal time (used, in
 *    part, to keep everybody on the same interpretation of valuation since coin is constantly
 *    inflating), and a closing deadline after which this transaction would be abandoned and
 *    a new one should be proposed, if still desired.
 *
 *    An authorization contains statements from the givers showing what ledger pages will
 *    be given, and a signature which basically promises that their funds on that page have not
 *    been previously given, nor will be given anywhere else on or before the closing deadline.
 *    Receivers also sign the authorization, indicating their acceptance of the proposed
 *    transaction.
 *
 *    This transaction and authorization is worthless without public recording, so... the next
 *    step is to secure agreement from a transaction processor to record the authorization.
 *    If no transaction processor is found which will record the authorization before the
 *    closing deadline (for the proposed bid), then the transaction is abandoned and a new
 *    one should be proposed, if still desired.
 *
 *    It is the duty of the transaction processor to ensure that there are no prior spends of
 *    the proposed giver amounts before submitting the transaction to be recorded in the block-chain.
 *
 * Underwriting, Chainmaking and Countersigning:
 *    Prefer the marketing image of "pay it forward" to greed.
 *    Proof Of Greed - the blockchain with the highest residual value gets picked up and extended.
 *    Transaction processors also act as underwriters, guaranteeing their work.  Receivers can
 *    stipulate an underwriting ratio, usually 1.0, but anything from 0 to >100x the value of the
 *    transaction (100x underwriting would likely require a higher bid to get picked up and processed.)
 *    And an escrow period (maybe 1 hour, maybe 30 days or more) during which the transaction processor
 *    is ineligible to spend the funds.  In the event that the chain proves the transaction processor
 *    incorrect before the expiration of the escrow period, the recipients are awarded the underwriting
 *    amount in proportion to the amounts they were supposed to receive in the original transaction
 *    (in the event of only a partial giver failure, then only the failed portion is awarded...)
 *    This underwriting amount is "skin in the game" for the transaction processor, they lose use of
 *    the funds during the escrow period, and they lose the funds if the blockchain proves their
 *    verification invalid.
 *
 *    The underwritten authorized transaction page has residual value from the processing bid - if it
 *    doesn't it's not likely to be included in the block, will be abandoned and must try again.
 *    Chainmakers assemble a block using all available pages with net residual value, then they
 *    attach a chainmaker's page with some transaction amount (positive, negative or zero) that will
 *    be given to the chainmaker who attaches to this block.  Presumably, the highest value block (with
 *    the most residual transaction value + chainmaker's gift to the next chainmaker to attach) will
 *    be the one to be included in the permanent chain.  Some additional incentives and assurances: a
 *    block is not valid until all underwriters have countersigned it, this implies that the underwriters
 *    should be checking every page proposed for the block to ensure that they do not lose use of their
 *    underwriting fee (either indefinitely, or for some period TBD somehow.)
 *
 *    Chainmakers attempt to assemble the highest value block they can, which basically includes all available
 *    valid (meaning, underwritten) pages.  There is a tax on page size, and thus an incentive to givers,
 *    receivers and underwriters to keep the page small.  There is also a cap on block size, so if too many
 *    pages are vying for a particular block-interval, only the highest value per byte ones (plus, possibly
 *    some that fit well into the final space) will be included by the presumably greedy chainmakers.
 *
 *    The tax per byte, mentioned far above as based on an inflationary rate of 5% per year and the moving
 *    window statistics of block sizes over the past whatever (1000 transactions? 30 days?) can be non-linear
 *    proportional by standard deviation of the observed block sizes.  An average sized block pays the nominal
 *    tax rate ber byte.  By 1SD below average size, the tax per byte is 1/2, by 2SD below average size, the
 *    tax per byte is 1/4, etc.  Meanwhile, by 1SD abve average size the tax per byte is 2x, 2SD 4x, 3SD 8x, etc.
 *    If this kind of trixy tax rate is to be implemented, a repeatable reference implementation using integer
 *    math or highly standard floating point should be provided.  A test server can qualify new software using
 *    test transactions to ensure exact computational accuracy.  Buggy software would lead to different answers
 *    and no matter how trivially small the difference from reference, that difference renders a page and its
 *    containing block invalid.  Remember, this tax is taken from the transaction bid and is given to noone, or
 *    rather, it is given to everyone in the form of inflation of their share of the whole circulating coin.
 *
 *    Tax break incentives might include: 1/2 of the total tax bill might be cut down by dividing by the number
 *    of pages included in a block.  1 page = full tax.  2 pages = 1/2 + 1/4 tax.  3 pages = 1/2 + 1/6 tax, etc.
 *    Transactions pay tax according to their underwritten page size, so spamming the block is not rewarding, but
 *    chainmakers can profit handsomly by bundling more pages into a block.  Remember the pay-it-forward incentive
 *    that a chainmaker has for including as much residual value as possible in a block so that that block is the
 *    one chosen for inclusion in the chain.
 *
 *    Another tax break might be provided according to the underwriting ratio of a transaction.  Receivers specify
 *    a minimum acceptable underwriting ratio, but 1/4 of the total tax bill might be cut down by dividing 1/4
 *    by the underwriting ratio, and when the resulting number is < 1/4 that can be the tax ratio - so a double
 *    underwritten transaction can save 1/8 of the tax (which can be collected by the underwriter as commission), or
 *    a triple underwritten transaction can save 1/4-1/12 of the tax, etc.  This tax break can't be too high because
 *    an underwriter could massively underwrite their own spam transaction in order to get it recorded on the chain
 *    while losing nothing but tax.
 *
 *    Only fully countersigned blocks are valid.  Underwriters have incentive to countersign any valid proposed block
 *    (that contains a page they have underwritten and collected a fee in) up until the cost of checking becomes too
 *    high to put their underwriting escrow at risk.  As a matter of practicality/courtesy an underwriter should also
 *    provide notice when they refuse to sign a block for whatever reason so that other underwriters do not waste their
 *    time validating it.  Note that once an underwriter has validated a page in any given proposed block, other proposed
 *    blocks containing that page do not need the page re-validated.
 *
 *    The chainmaker signature on a block includes a "gift" to the next chainmaker in the chain, to further incentivize
 *    that next chainmaker to choose this chain over another.  When a chain stabilizes, the chainmaker's signature in
 *    the a block gives them access to the residual value funds in the previous block, a sort of pay-it-forward arrangement.
 *
 *    Hopefully, not too many versions of fully countersigned blocks emerge from a given block proposal interval.  To
 *    break ties, a small proportion of the total block tax (1/128?) determined by the block hash may be gifted to the
 *    next block in the chain as incentive for that block to be the one picked up by the next block.  This may emerge
 *    as a high-energy competition between chainmakers to tweak the block for the highest tax break, but the problem
 *    is not easy since it involves computations based on every underwriter's unique signature.
 *
 *    Rule: no two signatures from a given public key may have the exact same time value.  This is to make it more
 *    costly for underwriter(s) to cooperate with chainmaker(s) to tweak hash values for advantage, and it's also
 *    just plain good practice to identify all transactions in-order rather than time-tied.
 *
 *    Enforcement: the first to find and successfully publish acceptable proof of this (before the underwriting escrow
 *    period has expired) is awarded the underwriting value of the duplicate time-stamps.  In the event of a tie, the
 *    bidder who offers the most residual value to the chain will be the winner (since no block may contain two
 *    duplicate transactions).  Note that, due to tax break, higher underwriting ratios are another way of upping
 *    the residual value.
 *
 *    Practice: independent processors may frequently accidentally collide on the same timestamp due to coarse clocks
 *    and frequent transactions.  It may eliminate the need for the two timestamp rule, or at least enforcement above if
 *    the effective comparison of two signature times xors the 32 MSB of the signature with the 32 LSB of the time value.
 *    The 33rd LSB has a value of ~0.5 nano seconds, and since processors are free to tweak their clock within +/- half
 *    of a processing interval anyway, 0.5ns won't be deciding any true time races, only providing random tiebreakers
 *    in the event of a tie.
 *
 * Democratic parameter adjustment:
 *
 *   Some arbitrary parameters may want to be tweakable within a range and the community members may (or may not) be the
 *   best judges of when and how to tweak.  As such, a running vote might determine some things, like: underwriters might
 *   vote on the block collection interval - longer means more pages per block, more countersignatures and bigger tax
 *   breaks, but at some point it might become unwieldy to have all underwriters successfully countersign a block before
 *   the next interval starts.  Longer block collection intervals also can mean longer transaction settling times.  Givers
 *   might vote to adjust the transaction tax base rate (which is directly tied to holder's interest growth.)  A Genesis
 *   block might establish the available window of values (say a range from 1-9% annual holder growth), and the community
 *   decides by voting with their transaction volume - this would tend to push value growth lower because it's the taxed
 *   transactions that are voting - so, the age of a giver's page in a transaction might also be weighted to give more
 *   voting power to longer holding owners.  Like the tax rate determination, a moving window of the previous 30 days or
 *   determined amount of voting power could determine the current parameter value, and interestingly that parameter could
 *   include the voting window rules to make a chain respond to votes more or less quickly.  Of course, alternate Genesis
 *   blocks can also establish alternate rule sets and people are free to trade and hold in whichever chain they wish.
 *
 * Welcome back, Alice and Bob:
 *
 *   This time, Alice has 504.023507972 coin under a single key in her wallet which has a total of 13 spendable keys and
 *   572.23980165 coin in it.  She wants to transfer 5 coin to Bob, and her wallet has selected the big key because it is
 *   getting older and will start to carry a higher commission as it ages and transaction processors want more commission
 *   to validate it.  Her wallet is also breaking up the remainder into 100 coin chunks because Alice's typical transactions
 *   are smaller, so future transactions will not have to carry as much underwriting - which also increases commissions.
 *   So, Alice's wallet proposes the following transaction to Bob's wallet:
 *
 *   <TRAN> Coin transfer transaction proposal:
 *     [SALT] 256 bit random number included in all signatures
 *     [TPRO] "now" from Alice's Wallet clock
 *     [TRMX] "now"+6 hours (closing deadline)
 *     [RBID] 3 coin (maximum recording fee)
 *     [MINU] 503.4 (minimum acceptable underwriting amount)
 *     <PART> List of each participant's:
 *       [KEY]  Alice's "504" public key
 *       [AMT]  -504.023507972
 *       [NOTE] (empty)
 *       [KEY]  Alice's first new public key
 *       [AMT]  100
 *       [NOTE] (empty)
 *       [KEY]  Alice's second new public key
 *       [AMT]  100
 *       [NOTE] (empty)
 *       [KEY]  Alice's third new public key
 *       [AMT]  100
 *       [NOTE] (empty)
 *       [KEY]  Alice's fourth new public key
 *       [AMT]  100
 *       [NOTE] (empty)
 *       [KEY]  Alice's fifth new public key
 *       [AMT]  96.023507972
 *       [NOTE] (empty)
 *       [KEY]  0 to be filled by the accepting recipient
 *       [AMT]  5
 *       [NOTE] Bob, you're weird.
 *   [TSIG] time of signing the transaction proposal
 *   [SIG]  signature on: TRAN+TSIG by Alice's published public ID key (different from any coin holding keys)
 *
 * Bob has his wallet set to "not snoopy, fuzzy time" so what he sees in the transaction proposal is:
 *
 *   "Proposed transfer of 5 coin just now,
 *    from Alice to close within 6 hours.
 *    Note: Bob, you're weird.
 *
 *    Approve Y/N?"
 *
 *   The just now is slightly important, because 5 coin just now will become 5.000166978 within 6 hours
 *   (due to ~5% CAGR from transaction tax reducing the circulating coin fraction), and if the transaction had a
 *   proposal time of 3 days ago, 5 coin today would have been 4.99799 then, for the same reasons.  In general, wallets
 *   might flag a proposal time more than 7 days old, and they should get snippy about any time signature that claims to
 *   be from the future, especially signature times.
 *
 *   Bob accepts right away.  His wallet does this by sending back the following to Alice's wallet:
 *
 *   <TRAN> Coin transfer transaction proposal:
 *     [SALT] 256 bit random number included in all signatures
 *     [TPRO] "now" from Alice's Wallet clock
 *     [TRMX] "now"+6 hours (closing deadline)
 *     [RBID] 3 coin (maximum recording fee)
 *     [MINU] 503.4 (minimum acceptable underwriting amount)
 *     <PART> List of each participant's:
 *       [KEY]  Alice's "504" public key
 *       [AMT]  -504.023507972
 *       [NOTE] (empty)
 *       [KEY]  Alice's first new public key
 *       [AMT]  100
 *       [NOTE] (empty)
 *       [KEY]  Alice's second new public key
 *       [AMT]  100
 *       [NOTE] (empty)
 *       [KEY]  Alice's third new public key
 *       [AMT]  100
 *       [NOTE] (empty)
 *       [KEY]  Alice's fourth new public key
 *       [AMT]  100
 *       [NOTE] (empty)
 *       [KEY]  Alice's fifth new public key
 *       [AMT]  96.023507972
 *       [NOTE] (empty)
 *       [KEY]  Bob's new public key
 *       [AMT]  5
 *       [NOTE] Bob, you're weird.
 *
 *   <AUTH> Authorization of transaction by the participants:
 *     <PART> List of each participant's:
 *       <PAGE> of participant's source of funds, to be filled by Alice's wallet
 *         [TREC] 0 recording time of AUTH(PART(PAGE))
 *         [HASH] 0 hash of AUTH(PART(PAGE)) - this page must show the giver as recipient of the AMT proposed in TRAN
 *       [TSIG] 0 time of signing, to be filled by Alice's wallet
 *       [SIG]  0 signature on: TRAN+AUTH(PART(PAGE))+AUTH(PART(TSIG)), to be filled by Alice's wallet using the private key from AUTH(PART(PAGE))
 *          Note: recipients (positive AMT) have no PAGE info
 *       [TSIG] 0 time of signing, to be filled by Alice's wallet
 *       [SIG]  0 signature on: TRAN+AUTH(PART(TSIG)), to be filled by Alice's wallet using Alice's first new public key
 *
 *       [TSIG] 0 time of signing, to be filled by Alice's wallet
 *       [SIG]  0 signature on: TRAN+AUTH(PART(TSIG)), to be filled by Alice's wallet using Alice's second new public key
 *
 *       [TSIG] 0 time of signing, to be filled by Alice's wallet
 *       [SIG]  0 signature on: TRAN+AUTH(PART(TSIG)), to be filled by Alice's wallet using Alice's third new public key
 *
 *       [TSIG] 0 time of signing, to be filled by Alice's wallet
 *       [SIG]  0 signature on: TRAN+AUTH(PART(TSIG)), to be filled by Alice's wallet using Alice's fourth new public key
 *
 *       [TSIG] 0 time of signing, to be filled by Alice's wallet
 *       [SIG]  0 signature on: TRAN+AUTH(PART(TSIG)), to be filled by Alice's wallet using Alice's fifth new public key
 *
 *       [TSIG] "new now" when Bob's wallet signs the AUTH
 *       [SIG]  TRAN+AUTH(PART(TSIG)), signed by Bob's new private key
 *
 * Alice's wallet, upon receipt, fills in the missing PAGE, times and signatures, and already having Bob's signature on
 * the transaction, this TRAN+AUTH is ready to go to Carol the underwriter.
 *
 * Carol's underwriting service gives Alice some sad news, market rates are up for this type of transaction, Carol's
 *   server rejects the recording request and bids 7.2385 coin to do the recording of this transaction, citing older
 *   than average source funds, and a slightly larger than average recording page size.  It even goes so far as to
 *   inform Alice that 0.1137 coin is being charged for the recording of her 20 byte message, and 0.2526 coin for the
 *   larger than average underwriting component.  Alice's wallet decides to shop around, so it sends the same TRAN+AUTH
 *   to Dave, Edgar, Francine and Gladys.  The underwriting market seems to be in a state of flux, Alice's wallet receives
 *   bids ranging from 3.4215 to 9.2414, every underwriter can make their own formula to bid on transactions, but all seem
 *   to agree that this one is more expensive than usual to record.  Since this is a larger transaction, Alice decides to
 *   revise her bid rather than keep looking for new underwriters that she has not dealt with in the past, her wallet
 *   recommends a new RBID of 3.5 and submits the revised transaction first to Bob for approval, since his signature was
 *   on the old TRAN it needs to be freshened.  Also, since there is a new time of proposal, the wallet recalculates the
 *   coin for Bob to be 5 exactly at the new time of proposal, it's an infinitesimal change, but that's what wallets do.
 *
 *   Alices' wallet pre-authorizes the transaction this time, since it already has Bob's public key to send to, and Bob's
 *   wallet is chill about the infinitesimal fraction of coin less this time around, Bob approves again and his wallet
 *   signs his line of the transaction.  Having a completely authorized transaction, Bob's wallet sends it out for bid to
 *   his familiar underwriters - this is O.K., all Alice and Bob want is for the transaction to get recorded as a page in
 *   some block in the chain sometime in the next 6 hours, and the authorized transaction will be circulating the chain
 *   makers momentarily, so many of them will pass it out to their familiar underwriters to bid on anyway.  In other words,
 *   it's public knowledge now.  To ensure that the underwriter does not hang on to the transaction until the last minute,
 *   Bob's wallet also publishes it to basically every underwriter service portal it knows.  Each underwriter prepares an
 *   underwriting block to go on the page, they employ varied strategies of increased underwriting amounts, and various
 *   proportions of residual value to try to get the highest profit for themselves that will still get the page successfully
 *   recorded in a block before the (generous in this case) closing deadline.  The underwriters (who may also be acting as
 *   chainmakers - this is unavoidable) will be proposing various forms of the page with various residual values.
 *
 *   Alice is running a "snoopy" wallet on her home PC, and it taps into the underwriters' network and sees the submitted
 *   transaction circulating.  Again, for thoroughness, Alice's wallet relays the transaction to the underwriters' portals
 *   that it knows about.
 *
 *   The chainmakers will be proposing various blocks which include only valid pages plus their chainmaker block which
 *   provides some value to the proposed block, as a bid to obtain the residual value in the block which they are
 *   chaining to, which should be the block for the previous timeslice - or if no block exists for the previous timeslice
 *   then the last block that does exist.  Blocks are published at regular intervals determined by the Genesis block and
 *   maybe some community adjustment by popular vote.
 *
 *   Empty block problem - what residual value is there in an empty block?  0 I assume - this seems like an opportunity for
 *   mischeif.
 *
 *   Back in the well behaved world, chain makers are proposing blocks with the maximum possible residual value now in a bid
 *   to obtain the residual value found in the last valid block of the chain for themselves.  This "pay it forward" is a
 *   tricky strategy, and the underwriters face a similar conundrum - balancing various bids for value they take from a
 *   transaction with underwriting they provide to the transaction to attempt to attract the most successful chainmaker to
 *   include their underwriting on the transaction instead of another's.
 *
 ******************************************************************************************************************************
 *
 *  Bad Actors, welcome!
 *
 *   The tax system is a control loop that is attempting to achieve ~5% annual tax on "the one coin", at that rate effectively
 *   "throwing away" 1/2 the value of the coin (one bit of transaction resolution) every ~14 years.  But, bits are cheap, and
 *   127 bits is arguably 63 more bits than a system shared by 8 billion transactors (2^43 - leaving 2^21, or
 *   2 million fractions per transactor) needs, so there's 63 bits available "to burn" and 63*14 = 882 years before this
 *   logic leads to the need to process some kind of split or just a migration to a new Genesis block - enough for me.
 *
 *   Note that the tax system is effectively inflating the share of "the one coin" owned by anyone who is not being taxed.
 *   One way to (mostly effectively) dodge tax is to hold your coin-share, not transact it.  TODO: some thinking about the
 *   implications of "under the table" transacting of private keys.  First one is: you're begging for a giver to burn you
 *   by spending that key in the block-chain before you do.  This might be mitigated by trust layering, but let's KISS for now.
 *
 *   The "best" source of tax revenue is buggy software.  Any underwriter who signs a bad proposal has effectively donated
 *   their entire underwriting backing fund to the tax pool, including payment to the vigilantes who catch them.  If there's
 *   enough tax from this source, transactions can actually be credited by the tax system instead of debited.  This kind of
 *   "tax holiday" would be a great time to freshen the signatures on old pages.
 *
 * Vigilante payment:
 *   If/when a malicious or buggy signed page appears, but that page's underwriting points to an actual valuable block, the
 *   first vigilante to claim that underwriting value in the block chain will get it.  Competing vigilantes presenting the
 *   claim for recording in the chain at the same time compete with each other by leaving more residual value in their claim
 *   transaction.  Note that vigilante claim blocks are larger than an ordinary transaction source, so they are (usually,
 *   depending on market conditions) a less efficient way to transfer coin as compared to a simple valid transaction.
 *   The Genesis block may also establish a ratio of Vigilante claim that is paid as tax.
 *
 * Quit your bullshit:
 *   More likely, malicious proposals from wallets to underwriters, or from underwriters to chain makers, will point to
 *   value sources which have already been spent elsewhere.  When this is the case, the only alternative a transaction
 *   processor has is to ignore the source of the malicious proposal.  If everyone ignores everyone, there's no network
 *   and no value, so it's up to each participant to decide how they will selectively listen to and ignore others, but,
 *   a common scheme would be a timeout penalty for proposals from the bad actor - first offense: ignore new proposals
 *   from that same network source for 1 minute, next offense within a 30 day period: 2 minute blackout, next one 4 minutes,
 *   next one 8, etc. until 30 days pass with no offenses, then the penalty time for a bad proposal cuts by half every 30
 *   days until it's back to 1.  Connections from unknown parties might start at 1 transaction per minute and cut by half
 *   after every valid transaction is recorded in the chain.  Persistent ssh keys might be used to establish this kind of
 *   credit between parties, the more good transaction history established, the faster new transactions are accepted.
 *   The system should ensure that being open to more new connections opens a processor to more profits.
 *
 *   Is 5% "interest" a pipe dream?  Well, yes, most likely.  To obtain 5% annual tax would require 5% average transaction
 *   tax on all 8 billion coins being transacted in that year, or 1% transaction tax on all 8 billion coins being traded
 *   5 times a year.  If trading volume can be pumped up high enough, then, sure, we can get it.  Current BTC daily volume
 *   is running $365M per day, or $133B per year, so, if transaction taxes collect 1%, or $1B per year, then that could
 *   support 5% interest on a total market cap as high as $20B, or just under $3 per coin.  I don't think trading volume
 *   will be that high on a market cap that low, but - buggy software and bad actors getting caught out exposing valid
 *   underwriting coin in attempted scam (invalid) transactions could increase the effective tax rate quite a lot, especially
 *   in early days.
 *
 *   Tax breaks (the world over) are attempts to shape behavior, identifiable, quantifiable behaviors that promote valid,
 *   rapid, secure, and efficient settlement of transactions are the ones that should get tax breaks.  A short list of things
 *   that might be given transaction tax breaks:
 *   - physically smaller (in bytes) recording pages
 *   - goldilocks (not too big, not too small)
 *     - page:
 *       - underwriting ratio
 *       - transaction closing time (between AUTH last signature time and recording block close time)
 *     - block:
 *      - size in bytes
 *      - size in pages
 *      - total residual value
 *      - ratio of residual value to previous block's residual value
 *
 ******************************************************************************************************************************
 *
 * (Near) zero assurance of independence:
 *    It can be assumed that most chain makers are also underwriters, and their proposed blocks include some (maybe all)
 *    pages which they underwrite, and that's O.K. The public keys involved will all be different, tracing to different
 *    source of value page-keys - the deciding factor in which block for a given time-slice is accepted into the stable
 *    chain is: how much residual value does it contain - the block with the highest residual value is the most desirable
 *    and therefore will be the one that the most new blocks are chained from - all blocks are working with a similar
 *    pool of available transactions (value) and so are attempting to build the most valuable next link in the chain.
 *
 * Rewrite of history:
 *    An underwriter and/or chainmaker might easily calculate and publish a new "end of chain" which has greater value than
 *    an established chain, cutting some transactions out of the chain (thereby erasing them from the accepted chain and
 *    making transactions based on them having been recorded invalid), but... those transactions have value, and another
 *    chainmaker can simply bundle them into a future block and recover that value.  There is a question of stability: what
 *    collection of incentive structures will make these kind of history rewrites as unlikely as possible to cut out
 *    transactions which once were considered stable.  I think the answer lies in goldilocks targets for tax credits - if
 *    these can be tuned to make it progressively more expensive to "cut out" an accepted transaction from the chain, then
 *    the chain will stabilize after a finite number of transactions.  Ideally, the cost to rewrite the chain might double
 *    every link in the chain, so after a certain number of chain steps, everyone can accept the chain as stable.  It also
 *    feels like higher residual value in a transaction will make it stabilize quicker.  We're gonna need a simulation.
 *
 *    Now, it feels like the transaction proposal's deadline for recording is just the start of the stabilization period,
 *    that a certain number of blocks must be added to the chain before that recording can be accepted as stable.
 *
 * Single actor's double-spend attack:
 *    1) Alice proposes a 1 coin transaction to Bob
 *    2) Alice runs an underwriter which underwrites this transaction
 *    3) Alice runs a chainmaker which enters this transaction into the chain at time t
 *    4) At time t+x Alice's chainmaker calculates a new chain which omits the transaction to Bob,
 *       but has more value than the current chain due to a marginal 0.1 bump in forward chain incentive,
 *       because the transaction to Bob only carried a less than 0.1 incentive to the chain.
 *
 *       This works because Alice's chainmaker is collecting all the transactions between t and t+x and
 *       repackaging them into a new valid chain with more forward chain incentive, Alice can do this because
 *       she is bumping the forward incentive by 0.01, but removing the transaction to Bob - regaining the
 *       ability to spend that coin.
 *
 *       This might be thwarted by Bob holding a secret that could increase the value of the chain with his
 *       transaction in it by more than Alice's scam differential?
 *
 *       It feels like Bob's wallet needs to trust his own underwriter who he can be reasonably assured is
 *       independent of Alice's.  The real key here is that Alice (the giver) should not control the underwriter.
 *       So, first, Bob's wallet must approve the underwriter it trusts - but how can it know?  In a previous
 *       transaction example, Alice had many receivers in the transaction and Bob only had one, but that could be
 *       sufficient if Bob's wallet would only sign the transaction if it had sufficient underwriting from an
 *       underwriter Bob can trust to cover his interests.
 *
 *   <TRAN> Coin transfer transaction proposal:
 *     [SALT] 256 bit random number included in all signatures
 *     [TPRO] "now" from Alice's Wallet clock
 *     [TRMX] "now"+6 hours (closing deadline)
 *     [RBID] 3 coin (maximum recording fee)
 *     <PART> List of each participant's:
 *       [KEY]  Alice's "504" public key
 *       [AMT]  -504.023507972
 *       [NOTE] (empty)
 *
 *       [KEY]  Alice's first new public key
 *       [AMT]  100
 *       [MINU] 100 (minimum acceptable underwriting amount to be countersigned by this key)
 *       [NOTE] (empty)
 *
 *       [KEY]  Alice's second new public key
 *       [AMT]  100
 *       [MINU] 100 (minimum acceptable underwriting amount)
 *       [NOTE] (empty)
 *
 *       [KEY]  Alice's third new public key
 *       [AMT]  100
 *       [MINU] 100 (minimum acceptable underwriting amount)
 *       [NOTE] (empty)
 *
 *       [KEY]  Alice's fourth new public key
 *       [AMT]  100
 *       [MINU] 100 (minimum acceptable underwriting amount)
 *       [NOTE] (empty)
 *
 *       [KEY]  Alice's fifth new public key
 *       [AMT]  96.023507972
 *       [MINU] 96.023507972 (minimum acceptable underwriting amount)
 *       [NOTE] (empty)
 *
 *       [KEY]  Bob's new public key
 *       [AMT]  5
 *       [MINU] 5 (minimum acceptable underwriting amount)
 *       [NOTE] Bob, you're weird.
 *
 * So, once the AUTH has a countersignature from Bob's new public key on underwriting >= 5, (and similar signatures from all of
 * Alice's new public keys), we can proceed with the assumption that "Bob is covered."  If both Alice and Bob trust the same
 * underwriter, then they can all just countersign one underwriter with sufficient coverage for the sum of all.
 *
 * What does this do for Bob in the above scam?  First, it means that Bob trusts the underwriter to not try to scam him, this
 * may or may not be a smart decision on Bob's part, but it's a start.  Second, it means that the underwriter makes a promise
 * to Bob that it will protect Bob's interests and not cooperate in rewrites of history.  The underwriter will use the same
 * private key to validate all transactions up through block approval into the chain.
 *
 * Does this prevent Alice's agents from re-writing history to remove this block?  It feels like, not.  All that Bob's
 * underwriter can really guarantee is that he has gotten this transaction into a valid end-block of the chain, not the
 * chain that will be picked up and continued.  What Bob needs is assurance that the chain his transaction appears in is
 * the one-true chain.  This certainty increases as additional blocks are added to the chain and the page showing Bob's
 * source of value gets deeper into the most valuable chain, but when is it certain enough?
 *
 * An outside "source of time" might solve this?  How about the underwriter staking their coin on the fact that the chain
 * is the "one true chain" - how do they enforce that when Alice does a rewrite?
 *
 * Stages of confidence:
 *   1) here's a valid chain with your page
 *   2) here's my assurance that chain is the "one true chain," backed by my coin which comes from a time before
 *      your giver's source coin (does that help? I think it does) - for a limited period of time I promise this
 *      coin to you in any chain that does not include your page.  Deeper underwriting is better (additional
 *      requirements of underwriting, minimum age of guarantee page, length of guarantee into the future).
 *   3) Bob spends the coin and has no further interest in it, transaction is underwritten by others
 *
 * There may be "amplification" of trust when underwriting multiples are >1.0.
 *
 * So, this protects recievers at the wallet level - what protects underwriters?
 *   Quick thoughts:
 *     countersigning additional blocks in the future to re-affirm the existing transaction
 *       these additional countersignatures might add residual value to the block, encouraging their adoption
 *         if this type of countersignature derives its value from "tax" then we're lifting all boats
 *     protection from attackers: deep secrets, the private key used to underwrite the transaction comes from
 *       deep in the chain, therefore the attacker cannot capture the value of transactions signed by the deepest
 *         secrets in the chain
 *
 *   Deep secrets:
 *   Perhaps: divide the "one coin" by 300, then divide one of those by 300, then divide several of those by 300,
 *     then divide those by 300, 300^4 = 8100000000, so each of the bottom level would be 0.9876 of one of the
 *     8 billion original coin shares, as interest accrues they will soon rise above 1.0.  These coin shares are only 4
 *     steps removed from the Genesis block, and they appear on ridiculously low page numbers like < 10.  The private
 *     keys behind these coins make some of the best underwriting secrets, since a chain in which an underwriter using
 *     one of these coins is signing the transaction, nobody can realistically hijack the value of the transaction
 *     by rebuilding history.
 *   Is a recent coin transacted from a deep secret coin any more valuable for underwriting than a standard coin?
 *     Maybe only if that transaction is underwritten by a deep secret?
 *
 * Underwriters approved by transaction participants:
 *   In another "evil Alice" scenario, if Bob approves an underwriter (who Alice does not control, and) who uses a
 *   deep secret to underwrite the transaction, then, Alice cannot rewrite history and use this transaction to build
 *   a higher value chain-end.  The deep secret protects Bob from underwriter failure by Alice rewriting history
 *   around the source of the underwriter's value, and this transaction being unavailable to Alice for use in
 *   construction of an alternate chain protects the "true chain" by locking up the residual value in this transaction.
 *
 * Wally the Whale:
 *   If Wally wants to rewrite history, he can simply dump a ton of value into his version of the chain-end and effectively
 *   erase history from the point he forked it and started rewriting it.  It will be obvious to honest participants what
 *   happend, but how do they prove it?
 *   Underwriters will be obligated to give value payouts in Wally's chain due to the erasure of the transactions they
 *   guaranteed, how do they prove Wally is a Pirate?  Upset receivers can register objections in Wally's chain, but
 *   this doesn't seem very effective, Wally can also register objections in what was the "true chain."
 *   One exremely lame alternative is to lean on Bitcoin - put periodic hashes of the "true chain" in the Bitcoin blockchain
 *   as proof that this chain was accepted.  Wally might also register his own alternate history in the BTC blockchain...
 *   Maybe effective:
 *   Participants can boycott Wally's chain, ignoring its higher value because they know their chain is the honest one.
 *   Instead of registering protests in Wally's chain, they can register protests in their own chain, alerting others to
 *   the hijack.
 *   Note that anybody can publish their own Genesis block, what Wally is doing by forking the chain is attempting to
 *   entice participants to follow his version of the value history.  There might be legitimate forks (as BTC has done),
 *   but "real world" value will likely suffer due to the dilution.
 *   Philosophically, Wally might have a legitimate reason to fork, particularly if he wants a change in the rules that
 *   would invalidate transactions on his chain in the "main chain."  The problem comes if Wally's fork is hidden and
 *   surfaces later with no visible rules changes, but missing transactions that underwriters would be liable for on
 *   Wally's chain.  This would cause the underwriters to reject Wally's chain and point to the "true chain" which
 *   they validated on.  This might actually be the answer: triggering of underwriter liability - the more underwriter
 *   liability on a given chain, the less that chain will be accepted by the underwriters, they will point instead to
 *   the chain they have been building.  Wally can make his chain, but he has alienated any participants he has cheated.
 *   If Wally is above-board when making his fork, then there should be no problems.  Wally is acknowledging everyone
 *   else's secret-held shares as having value in his chain, but denying transactions on the main chain from the point
 *   of the fork.  One does not need be a whale to do an honest fork, but one does need a network of underwriters and
 *   chainmakers to make the fork work.
 *   To prevent sneaky Wallys from erasing small pieces of history, underwriters and chainmakers need to stay current on
 *   which blocks "win" in real-time, and reject any higher value chain-ends that appear after the deadline.  If the
 *   chainmakers ignore this agreement, there can be trouble.  A sequence might be:
 *
 *   Block/Step
 *
 *   0f existing chain-end decided (Genesis block, or any later one agreed upon by the community)
 *
 *   1a fully underwritten transactions, which do not appear in the existing chain, are collected and proposed for
 *      inclusion in block 1 (sequence number and chain-end embedded in signed underwriting) - cooperation of underwriters
 *      might get ticklish.  Each transaction page should reference the highest value, fully signed chain-end's hash in
 *      each underwriter's signed data.
 *
 *   1b cutoff time, real-time, any transactions received after this time should be proposed for a later block.  It
 *      becomes harder for block 0 snipers to get in now.
 *
 *   1c new block proposal phase, each chain-maker public key may submit at most one block for consideration as the new
 *      chain-end.  Here, some conflicting opinions may arise as to what was the accepted chain-end last time, the next
 *      step may help.
 *
 *   1d cutoff time, real-time, new block 1 proposals received after this _should_ be ignored.
 *
 *   1e countersigning phase, for a block to be a valid chain-end, it must be countersigned by all underwriters in the
 *      pages of the block.  This is where underwriters have a check/balance on chain makers who might be attempting to
 *      snipe or rewrite history.  When an underwriter considers a proposed block for acceptance, they should read into
 *      its chain, on the simple side to see if it agrees with their memory of what was agreed to - but really for the
 *      reason of validating that this chain does not put them at risk of liability for loss of transactions they have
 *      underwritten.
 *
 *      As long as at least one block proposal is countersigned by all underwriters in the pages in the block, it can be
 *      the basis for the next block in the chain, higher and higher residual blocks may be proposed until roughly 2b, at
 *      which point it is unlikely that a "sniping block" will be countersigned in time to be included in the later chain.
 *
 *   1f cutoff time, real-time, transactions for block 2 need to be prepared and transmitted.  Underwriters more or less
 *      control late entries from chain-makers by not countersigning the late entries.  If all underwriters in the block
 *      do countersign after deadline, that block may or may not be used as the new chain-end depending on
 *
 *   2a Fully underwritten transactions proposed for inclusion in block 2 can start being posted to the network after
 *      the 1f cutoff time has produced an apparent "winning chain-end" with the highest residual value.
 *
 *   2b transaction submission cutoff time, late posted or late arriving transactions have a decreasing chance of
 *      inclusion in their specified block.  Late posted block 1 chain-ends also have rapidly diminishing chances of
 *      being considered after this point since clock abiding underwriters won't have a chance to include their signature
 *      in their posted transactions.
 *
 *   2c new block 2 proposal (bidding) phase.  Note that all pages in a valid block proposal must reference the same block
 *      1 chain-end, which is presumably the highest residual value block 1 since that value will be given to the
 *      chain maker.
 *
 *   2d cutoff time, real-time, new block 1 proposals received after this _should_ be ignored.
 *
 *   etc. continue the pattern a-f on each new block.
 *
 * Some caveats:
 *   Even though each chain-maker public key may only be submitted once per N blocks in the chain (N to be decided by
 *   Genesis block parameter selection, I'm thinking an N of 1000 might be good), and that may be enforced by vigilantes
 *   detecting a violation, and proving it by showing the repeat use of a single public key inside the limits, and then
 *   claiming the value stored in the page referenced by that public key, that doesn't mean that evil Alice doesn't have
 *   1,000,000 pages (which can be obtained with just 1001 transactions, in just a few blocks, each of these pages with
 *   sufficient value to place a chain-maker bid, and thus evil Alice can place 1000 bids on each new block in the chain
 *   in an attempt to win the chain-maker award from the previous chain-end.  Evil Carol can do the same thing, so each can
 *   take 1000 shots at being the winning chain-maker bidder on every block, and all they are losing is use of their funds
 *   while they play this game.  It is probably beneficial to let evil Alice and Carol play at this since it will result
 *   in the lowest possible residual value payout to the chain-maker as each progressively under-bids the other.
 *
 *   Underwriters cannot play a similar game so easily, unless they control all parties in a proposed transaction (which
 *   they easily can.)  When an underwriter does not control all parties in a transaction, they are limited to a single
 *   public key with which they can bid their entry into a particular block.  Massive underwriter page proposal spam
 *   can be throttled at the network connection level like any other form of DoS attack.  Underwriters do control
 *   which chain-maker proposed blocks are acceptable, so if evil Alice is underwriting a page in the block, she might
 *   be tempted to not counter-sign blocks proposed by other chain-makers, same for evil Carol.
 *   What we have to hope for here is a Glenda the good chain-maker who proposes a variety of page collections including
 *   some that don't have Alice or Carol as underwriters, and that the collection of all good underwriters sign off on
 *   those blocks.  If two legitimate pages have been submitted for inclusion in the block, plus 4 pages with Alice as
 *   underwriter and 4 more pages with Carol as underwriter, at first Alice is only countersigning blocks proposed by
 *   herself, and Carol is only countersigning blocks proposed by herself - this monkey business isn't provable because
 *   all the public keys are different, though network connection sleuths could infer what's going on pretty quickly and
 *   possibly throttle the sources as DoS attempts.  Anyway, Glenda the good chain-maker starts proposing some other
 *   permutations of the 10 available pages, starting with each page individually - and sees that 2 of those proposed
 *   blocks are fully counter-signed by their underwriters, exposing Alice and Carol's shenanigans. Then Glenda
 *   (or any other good chain-maker) would propose a combined block with all the already fully countersigned pages,
 *   hopefully producing a valid chain-end that includes as many legitimate transactions as possible.  If some of Alice
 *   or Carol's transactions they created for shenanigans do get accepted to the chain, that's o.k. - the tax on those
 *   transactions lifts all boats.
 *
 *   Late entries:
 *     If a late entry appears, it can only include transactions that are fully controlled by "bad actors" who are
 *   producing the late entry for shenanigans.  Legitimate transactions cannot be hijacked because that would require
 *   access to the underwriters' private keys.  Still, a late entry might appear with a higher residual value in the
 *   chain-end, why wouldn't "legitimate" underwriters and chain-makers play along and accept it, abandoning the pages
 *   that arrived on-time?  Well, all the underwriters and their clients who were participating in the on-time chain
 *   would get erased out of the chain, so they're not going to be happy, and the residual value they provided to the
 *   chain would be lost.  The late entry can easily provide more residual value just with a single bid.  Tax incentives
 *   for more pages to be included in a block, and consecutive blocks can help a little, but only so much, a whale can
 *   always out-bid a tax incentive.  What a late entry is, in effect, is a fork of the on-time chain.  It burns any
 *   underwriters who got their transactions erased.  Now, from the perspective of a "legitimate" late entry due to
 *   islands of processing coming together, the late chain's underwriters are getting burned because their transactions
 *   are not represented in the on-time chain.  How do legitimate underwriters prevent this?  The key would seem to be
 *   preventing islands of processing.
 *
 *   Islands of processing:
 *     The whole block-chain structure assumes a monolithic community, all with real-time access to the official block-
 *   chain, so they can verify legitimacy of proposed transactions and get legitimate transactions recorded as a page
 *   in a block in "the chain."  If this community gets split, and starts processing two separate block-chains, can those
 *   chains ever be reconcile-merged?  I believe the answer is: yes, iff no source of value is consumed in both streams
 *   while they are separated.  With all good actors this shouldn't happen.  With bad actors it is a definite possibility.
 *   Even with "good" actors, if a husband and wife both have copies of the same wallet, and the husband travels overseas,
 *   then the chain bifurcates, and both copies of a page are spent in separate islands of processing, that double-spend
 *   won't be exposed until the chains come back together.  It could have been an innocent, if somewhat sloppy, mistake.
 *   What to do?  Well, simple justice could involve the owners of the double-spent page providing value from some other
 *   page to cover the shortfall.  Until that happens, the chain-merge operation could include a double (or more) spend
 *   reconcilliation method which divides the original page value and makes that partial value available for claim by the
 *   down-chain receivers of value from that page, while invalidating any transactions that received value from the page.
 *   Underwriters might provide relief, if every transaction is fully underwritten, then the chain-merge process can
 *   restore all value to downstream recipients from the underwriting, so that's a good thing, unless you are an
 *   underwriter, but that's part of the risk that you get paid for...  Legitimate underwriters thus have an incentive
 *   to prevent islands of processing from happening, and to promote chain-merge as soon as possible after they do.
 *   Multi-spends should be limited to one spend per island of processing, thus the ideal number of islands is only one.
 *
 * Politically driven forking:
 *   Say that there is division in the community, one significant faction wants to move to shorter recording intervals
 *   and another wants to move to longer recording intervals.  The built-in quasi-democratic voting system is following
 *   one group and the other is unsatisfied by the results.  The unsatisfied group may intentionally fork the block-chain
 *   to follow a new set of rules - which are acceptable to their members but not to the others.  This is fine, the forked
 *   group may proceed on their own fork, but as soon as a pre-fork page gets spent in both post-fork chains, the fork is
 *   semi-permanent, a re-merge would require significant negotiation and tweaking.  There is quite a bit of precident for
 *   this in existing cryptocurrency communities.
 *
 *****************************************************************************************************************************
 *
 *  Chain validation (and coin inflation reduction) by "free" lottery consensus:
 *    To play the lottery a holder of a valid page-key countersigns a recent (3-5 cycles old?) chain-end and registers
 *    this signature with a lottery processor.  The page-key holder dare not play the lottery on two different chain-ends
 *    for the same cycle because doing so would put the entire value of their page-key at risk to vigilantes who could
 *    present the double signature as a cheat-claim and take their value.  A lottery play only has value in the chain
 *    which contains the block (recent chain-end) which was countersigned.  Periodically (weekly?) a lottery drawing is
 *    held in which the hash of the lottery drawing chain-end is partially matched to all the countersignatures registered
 *    with lottery processors.  Lottery processors register the hash of all their entries in the chain some interval before
 *    the drawing, and make all the entries available for public inspection (by vigilantes who can look for cheat-claims)
 *    at least by drawing time, and preferably all the time.  The lottery drawing is performed by a computationally intense
 *    hash of the drawing chain-end, and a sufficiently long match to the hash generated as the entry counter-signature will
 *    result in a payout reward to be shared between the entrant and the lottery processors.  Payouts may be linearly or
 *    logarithmically proportional to the amount of the page-key used to enter.  Winners are not guaranteed every drawing,
 *    but, the payouts come from the collected tax pool.  1/6 of the tax collected might be at 33% risk of claim in the linear
 *    pool and 1/6 of the tax collected might be 33% at risk of claim in the logarithmic pool, and 1/6 of the tax collected
 *    might be at risk in the claim per entry pool, meaning, on average, there is one winner in one of the 3 pools - though
 *    multiple winners in every pool are possible.  Note that players need-not hold their page-key value until the drawing,
 *    possession of the secret key from that page-key is enough to claim the prize.  Lottery processors countersign players'
 *    entries to assure recording of the players' entry ticket and later publication.  If a lottery processor's registered
 *    publication of entries not contain a player's entry, any players who register a cheat-claim against the lottery processor
 *    may collect their share of the prize winnings (further assurance of accurate reporting by lottery processors.)
 *
 *    The primary purpose of the lottery is to obtain additional endorsements of the true chain, incentivising all players
 *    to indicate which chain they believe in via a signed public record.  Lottery processor's big list of entries need not
 *    be stored long term, after perhaps 90 days it is of little or no future value, and only tiny hash-evidence of it gets
 *    recorded in the block-chain.
 *
 ******************************************************************************************************************************
 *
 * Rewarding error-checking:
 *   Errors can happen, whether malicious or accidental, and erroneous pages in the block-chain can devalue future
 *   transactions based on those pages.  Part of the reasoning behind requiring underwriting is to provide valid value
 *   basis with which to repair erroneously recorded pages.  To motivate the rapid discovery and repair of mistakes, the
 *   first agent to find and record proof of an error is entitled to any residual value after correction of the problem.
 *   This reward to error/cheat catchers is one reason to demand underwriting in excess of 100% of the receivers' value
 *   in a transaction.  It is expected that the reward level would at least cover recording costs for the error discovery
 *   claim(s), plus a flat incentive amount commensurate with the effort required to discover the error, perhaps plus
 *   a percentage of the receiver value which was corrected (shifted to underwriting) by demonstration of the error.
 *   These levels and percentages can be set out in the Genesis block and perhaps guided by algorithm that may or may not
 *   include community input into the decisions.
 *
 *   Responsibility breaks down like this:
 *
 *   - Receivers have no responsibility in a transaction, they will receive value when a valid page with their public
 *     key is recorded in the block chain.  Their value in the page-key is primarily sourced from the giver's page-key,
 *     and also protected by the associated underwriter's given page-key.  This is why receivers should not sign an
 *     underwriting proposal until they are satisfied that they trust the underwriting that is assoicated with thier
 *     receiver participation.
 *
 *   - Givers are responsible in the amount of 100% of the page-key they are giving.  Once they provide a valid signature
 *     on a transaction authorization, they have surrendered all the value of the associated giver's page-key in the
 *     transaction.  Note, that same transaction may often name new receiver(s) which recover part of the value that is
 *     given into a new page-key controlled by the same entity identified as giver in the transaction.
 *
 *   - Underwriters are responsible to the receivers they identify in the transaction, up to the total amount they have
 *     signed up for in the underwriting section of the page.  Receivers may recover the full amount they were entitled
 *     to in the authorized transaction and any additional underwriting amount is awarded to the parties which record
 *     a valid proof of error in the chain.  If the error does not involve loss of value from the giver(s), then the
 *     underwriters' responsibility is reduced by the amount of value successfully sourced from the giver(s).  Note that
 *     all value in block-chain computation is stated in fraction of the "one coin" rather than current adjustments of
 *     coin-share based on circulating fraction of the "one coin."  It is expected that underwriting ratios will be >100%
 *     of the received value and that full restoration of receiver value (and thus, repair of value chain to recipients
 *     of value from those receivers).
 *     Underwriters' period of liability is limited, as defined in the underwriting agreement.  The Genesis block
 *     may establish value, or algorithm, possibly including community vote input, to determine required underwiting
 *     multiples and periods of responsibility.  Underwriter responsibility may also be algorithmically decayed over time
 *     so that errors caught quickly are of more value to the error catcher than errors caught at the last minute before
 *     underwriting expires.  Full underwriting may extend for a period of days, then decay linearly or negative
 *     exponentially stepwise or continuously until a cutoff date where discovered errors are no longer rewarded according
 *     to algorithm and the underwriter's source funds are free to recycle and use to underwrite other transactions.
 *     It may be that the system algorithms allow a certain multiple use of underwriting value, for instance a single large
 *     underwriting source-value page may be used to guarantee several smaller transactions, even up to an oversubscription
 *     limit - though this might encourage cheating on the underwriters' part, again, a community decision - personally I
 *     prefer the KISS approach where the entire page-key is committed to underwriting the transaction until such time
 *     as the underwriting guarantee runs out - but it may be desirable to have a short fully backed underwriting time period
 *     backed up by a larger, longer-term pool backing up transactions after the short-term guarantee runs out.  All a matter
 *     of inspiring sufficient confidence and providing sufficient time for error catching.
 *
 *   - Error finders are responsible to identify the error in the block-chain and post a proof of the declared error,
 *     obtain (presumably compensated) underwriting for their error-claim, and record it similarly to a normal page in
 *     the block-chain, all normal recording taxes and fees apply.  Residual value after 100% reparation to the
 *     underwritten receiver and all fees and taxes associated with underwriting and recording the error claim is awarded
 *     to the error finder as a receiver in the error-recording page.  A cooling-off period following the error-recording
 *     applies to the value in the error-recording page, after which time recipients in the page may spend their key-value
 *     as usual.  The most unusual thing about the value chain is that the covered receiver may have already spent
 *     their value before the error was discovered - any recipients of value from the covered reciever still have their
 *     value, but it is sourced from the receivers' underwriter via this error correction record instead of from the
 *     erroneous transaction (in the case that there was a problem with giver value in the erroneous page).
 *
 *   - of course, error finder pages may themselves have errors.  To discourage this, underwriting requirements for
 *     error-recording pages may be considerably higher than for normal transactions, and underwriters are expected
 *     to be extra careful when verifying error-recording pages - they may also be exposed to multiple-of damage
 *     claims, for example: an error found in an error-recording page might be awarded 3x the value of the error
 *     from underwriting (and if that error-recording is found to be in-error, it would be awarded 3x again the value
 *     to its error finder, and thus require sufficient underwriting to cover that claim.)
 *
 *     Fairly heavy tax (lifting all boats) on error-recording can discourage intentional abuse of the system.
 *     Abuse being described as an actor "playing with themselves" proposing a transaction, providing the underwriting,
 *     intentionally slipping an erroneous page into the block-chain and serving as chain-maker and counter-signer of
 *     the block, then "discovering" the error later to claim the error-recording fee.  At the end of this process,
 *     the player should lose a significant amount of coin-share as compared to simply leaving their coin stationary.
 *     On the topic of varying tax collection - baseline recording tax fees can be adjusted to provide a target
 *     CAGR to stationary holdings, so this means that unusual taxes collected on things like error-correction will
 *     be compensated by reduced taxes on ordinary transactions while holding long term CAGR steady.
 *
 *     Bottom line: software validation is important.  Underwriting provides a route to value-chain repair when
 *     mistakes are made (malicious or not).
 *
 *     As for errors in the block-chain itself (not just specific page(s)), it would be nice to catch these before too
 *     many additional blocks are added to the chain.  Basic chain-making is a pretty easy job, you rely on the
 *     underwriters to validate the pages they are underwriting against the chain-history, select a combination of
 *     pages that you think represents optimal residual value offer to the next chain-maker, add a final page to the
 *     block including any give-value you are placing as a boost to the residual value in the chain, and receive-value
 *     where this page will claim the residual value in the previous block in the chain, underwriting as appropriate
 *     if there is a give in the transaction, and it's all done except for the taxes.
 *     Taxes are going to end up being a complex calculation, and it's the one that makes a chain community what it is.
 *     Fixes adopted against abuses will usually show up in the tax code, and without consistent computation of taxes
 *     chain-end proposals' residual value is deceptive.   This is also a concern about malicious erroneous pages
 *     within a block, which is why all underwriters countersigning a new chain-end proposal should also do some kind
 *     of checking of the validity of the other pages in the chain-end, not just their own - they aren't liable for
 *     their underwriting on the other pages (or, are they?), but they will effectively lose their earned underwriting
 *     fees if the block is later found to be invalid and the main chain back-forks and abandons this block.
 *
 *     The summary information in a chain end: shares locked by tax, algorithm state calculations pre and post block
 *     computation (hint: pre must match post of the previous block), etc. is all part of the tricky tax computation.
 *
 ***********************************************************************************************************************
 *
 * Fast transactions:
 *   Say Alice gets off a plane in Morocco and wants to pay her cab driver with coin shares, can she do it?
 *   Perhaps Alice recently received some coin in the "sweet spot" of the chain, not too fresh that the chain-end
 *   isn't highly trusted yet, but also not so old that an underwriter will need to do extensive research to
 *   verify a lack of double-spend on the source value.  The cabbie, Bob, of course works with a fast settlement
 *   specialist underwriter who can estimate time to record a transaction in the chain for him, so when Alice
 *   proposes the transaction to Bob to pay for the cab ride, Bob checks with his underwriter to get an estimate
 *   of risk vs. settlement time.  Due to the favorable recording time of Alice's funds source page, the underwriter
 *   can answer within seconds showing settlement time probabilities and a risk vs time estimate.  Due to the shorter
 *   than average settlement time, the underwriter is asking for a higher than average fee, which Alice and Bob
 *   agree to because Alice wants a ride and Bob wants a paying fare.  During the ride, the transaction is recorded
 *   in the chain (note, that a 10 minute block interval isn't likely to work for this, but a 1 minute block interval
 *   probably would), and some time later the cabbie receives an update from his underwriter that the transaction is
 *   now guaranteed.
 *
 **************************************************************************************************************************
 *
 * Forks happen:
 *   Block chains, distribution agreements, proof of work, it doesn't matter.  The value of any currency is determined
 *   by what people are willing to give for it, and if the whole world decides that it doesn't want to give anything
 *   for the currency, but with a few changes in the rules they would, then the rules will change or currency holders
 *   will lose their store of value.  Forks have been proposed and executed in Bitcoin.  Alt-coins run parallel systems
 *   each with their own set of rules.  All cryptocurrencies are based on a public record of share transfers, secrets
 *   which are required to release shares for transfer, and a community which continues to extend the block-chain with
 *   new records allowing continued transfers of shares.
 *
 *   A community of transaction processors are required.  Technically, a single person could run all the software
 *   required to give, underwrite, record, and receive shares stored under one secret into another.  Due to underwriting
 *   requirements, that one person couldn't transfer all their shares this way, but they could transfer a significant
 *   fraction of them, until flat recording fees would consume the whole remaining value.  If the community agrees to
 *   recognize these transactions (and, why wouldn't they?  The recording fees benefit all other players), then the
 *   self-made transaction becomes part of the permanent chain.
 *
 *   As with other forking block chains, the outside world value going forward may be similar or very different between
 *   the forked chains.  As long as nobody spends in both chains, an accidentally forked chain can be remerged smoothly
 *   and continue on.  Double-spends would require some kind of repair to avoid creation of shares (deflation of everyone
 *   elses' shares) after the merge.
 *
 * Woven blockchains?
 *   Some of the single-signature rules proposed would likely result in many chain-ends per cycle as different
 *   underwriters and chainmakers failed to concur on the "best" next block for the chain.  Chainmakers from
 *   the next cycle might "pick up the threads" of the separate chain-ends and weave them back together by
 *   validating that doing so would not create any double spends.  This raises an interesting question for:
 *   how does the residual value of a chain-end get divided among chainmakers who share it going forward in
 *   this woven arrangement?  Needs analysis, but seems like "chainweaving" might be a good solution to the
 *   problems raised by the otherwise seemingly good "single signature per timeslice" idea.
 *
 **************************************************************************************************************************
 *
 *  Applications beyond value transfer:
 *    The blockchain is a public record, and anything can be recorded in it with payment of the recording fee.  Interesting?
 *    note: if the item proposed for recording ends up forked out of the long-term adopted blockchain, the recording
 *    fee payment is also forked out of the chain and effectively refunded in the new chain branch.
 *
 *    By explicitly charging a per-byte recording fee for both transfer of value and arbitrary data recording (incidentally
 *    paid for by a traditional, underwritten, value transfer with no recipients other than the recording agents), this
 *    makes a "fair" way for entities to get their data permanently, publically, irrefutably recorded and distributed for
 *    as long as that blockchain is updated and distributed, which it must be for shareholders to conduct transactions.
 *
 *    Smart Contracts, public records, and all manner of other applications can be executed in this public record.  It may
 *    make sense for counties to duplicate their official public record books in such a blockchain, incidentally providing
 *    a locally maintained and trusted blockchain for businesses and residents to transact value exchanges on, both locally
 *    and with the world.  There would come a question: how much daily trading volume is required to make a blockchain
 *    sustainable, economically?  That might be answered by: cost to run the server (just guess $200 per month for leased
 *    AWS or Azure isntances) divided by the transaction overhead, call that 2%, so if $10,000 per month is transacted through
 *    the system, transaction recording fees from that could pay for server costs.  In a county with population 25,000 or more
 *    it is not too unrealistic to think that >1% of them might transact >$40 per month through the system.  Could the county
 *    level systems "peg the value" of shares?  Only if they are willing to back that value by buying shares at that value
 *    when any holder of shares wants to sell them.  They could, of course, also sell shares to anybody willing to buy them
 *    at that price.  The whole state could organize with a state level exchange, conducting open, trusted and verifiable
 *    transactions with the county exchanges, and, similarly, a national level exchange could conduct transfers between the
 *    states.  As transaction volume increases, the cost per transaction could come down, considerably.  A holder of local county
 *    shares could transfer that holding to state shares for the recording fees (state level recording fees might be set
 *    intentionally higher than county level to encourage distribution of transactions to the lower levels), and similarly to
 *    the national level.  Counties might support direct county to county transfers of value, establishing some level of trust
 *    through the state and national connections, but avoiding having to step up and down the pyramid net over 4 transactions
 *    for every transfer of value.
 *
 ********************************************************************************************************************************
 *
 * Smart Contracts:
 *   So, I thought Smart Contracts were this complicated thing like: https://ciyam.org/at/at.html and many are, but...
 *   apparently the transaction processing system I am describing might also qualify as a smart contract since it can
 *   be automatically underwritten and recorded by computers.  I thought I would avoid Smart Contracts in the first
 *   iteration of (now named, domain name parked and all) Assign Onward, but the incentive systems described so far
 *   feel like they want to be specified in the genesis block, and that would seem to be an ideal application for
 *   programmable "smart contracts" since chainmakers, at the least, will need to know how the incentive taxes work so
 *   they can calculate accurate residual values for the blocks.  Underwriters probably also want to know how the
 *   smart contracts are working so they can more intelligently bid their commission in pages they are underwriting, not
 *   to mention interpreting underwriting ratio and duration requirements, etc.
 *   And, finally, error checkers will definitely have to know how the algorithms work if they are going to doublecheck
 *   the chainmaker's math, get their error catching commission collections properly underwritten, etc.
 *
 *   First feelings: since these contracts are kind of one-off per blockchain, I'm leaning in the direction of a
 *   standard scripting language like (gasp) Python, with some kind of hooks into the transaction and underwriting
 *   bytecode definitions.  The smart contracts don't have a reason to be efficiently stored in bytecode, nor copied
 *   in essentially the same form millions of times, one definition per blockchain seems enough.
 *   As for how to deal with Python script smart contracts: https://docs.python.org/3/extending/embedding.html
 *
 *   On a tangent, the example use case cited for Automated Transactions in Wikipedia: atomic cross-chain trading, sounds
 *   really exciting, but looking at their bytecode specification, I'm less enthused.
 *
 *******************************************************************************************************************************
 *
 * Rewriting history for personal, or community profit:
 *   Say there's a well established blockchain with a large community of shareholders with a wide distribution of
 *   shareholder stakes.  One shareholder develops an alternate blockchain which omits some transactions, and declares
 *   to the community "this chain is better!" and tries to get the community to adopt their chain.
 *
 *   A) nothing is stopping him - it is not computationally infeasible to develop a valid, alternate chain.
 *
 *   However, his alternate chain will be missing all transactions which he does not know the secret keys for.
 *   Each transaction was not only approved by the givers, directly benefiting the receivers, and also indirectly
 *   benefiting the whole community through retirement of shares, increasing stationary shares' fraction of ownership.
 *
 *   This alternate chain could only benefit the givers whose transactions were erased, and only if they did not mind
 *   breaking their contracts with the outside world.
 *
 *   Posit a world of many Assign Onward blockchains interacting.  When an open exchange is made between two blockchains,
 *   shares move between parties in both chains, and a record of the exchange agreement is recorded on both chains.  That
 *   exchange agreement: 1) exposes a relative value of coins in the two exchanges at the time, although value not disclosed
 *   in the record may influence this value ratio, and 2) creates a record of a recent block in each chain in the other.
 *
 *   It is possible that one (or both) chains then "fork" before these transactions were recorded and move forward on the
 *   alternate branch, in which case this transaction will have no validity in the alternate fork.
 *
 *   Receivers of shares are acting on faith that the chain they received those shares in will continue to be an actively
 *   extended chain with value to the participants.  Philosophically, this is no different from receiving fiat currency
 *   backed by a national government.
 *
 *   It is feeling like the key to transactional security, to not having the chain forked out from under you, is to be
 *   a small fish in a big pond of many small fishes.  The collective value of all the small fish transactions, both to
 *   the receivers, underwriters, chainmakers and to the passive share holders, is their incentive to ignore forks which
 *   abandon these transactions.
 *
 *   Note that proof-of-work networks are also subject to forking by a defection of a majority of the community.  In practice
 *   this does not happen to the "big ones" like Bitcoin because the community is so large that it is too costly in the real
 *   world to independently overpower majority opinion of the active miners.
 *
 *   To overpower community opinion, and get a majority to follow a forked chain, would be to propose an alternate future
 *   which benefits the majority.  The undermining of trust in the community _should_ be a powerful demotivator against
 *   defection to an alternate fork of the chain, as long as the community is robustly engaged and committed to the future
 *   value of their established transactions.
 *
 * Poison pill contracts?
 *   This may, or may not help - I suspect in some situations it could backfire, badly, simulation is needed, but... the
 *   concept would be to include in the genesis block an irrevocable poison pill contract.  When an aggreived party feels
 *   that the blockchain has forked away from them and their interests, they may register a complaint on the new blockchain
 *   which aggregates the value of all transactions appearing in the chain they support and locks that value in the competing
 *   chain.  In this way, a single plaintiff can devalue alternate histories for all parties who have transacted since the
 *   fork in question.  A gamer creating an alternate history cannot know the secrets of others' transactions, but a valid
 *   complaint against the gamer would shut down the value of all those transactions - restoring their transaction taxes
 *   (small hit to everyone) and removing value from... what, exactly?  The alternate chain has already implicitly invalidated
 *   all those transactions, the pre-fork history would not be affected.... The plaintiff is saying "I have proof that all
 *   these alternate transactions happened in my chain, therefore the page-keys these transactions came from in your chain
 *   are invalid, and now locked in your chain."  That's what the complaint is doing, locking up value in the alternate chain
 *   with proof of transactions in this chain.
 *
 * Forkers beware: if your fork does not work, all the value given in pages on your fork can be similarly locked, and even
 *   claimed, by plaintiffs in the "one true chain."  This is different from the poison pill which devalues forked chains,
 *   this is saying: I have proof that someone has signed their secrets into an alternate chain, and therefore abandoned
 *   this one-true chain.  This would not work on wallet level transactants, primary givers and receivers, since they do
 *   not sign off on the blocks in the chain, but underwriters _do_ sign off on blocks in the chain, and they are pledging
 *   even more share than the givers and receivers, so that value is at risk... how?  Earlier proposals have suggested that
 *   underwriters might sign multiple proposed blocks.  Perhaps there is a contract that underwriters will only sign off once?
 *   This might work if each block in the chain only has one page, good for transaction settlement speed, but bad for
 *   collective reassurance.  Can we at least burn the chainmakers in the fork?  I think that's an easy one, any chainmaker
 *   who signs a different fork is losing the shares pledged into that fork's chain in this chain.  Now, how do we reach
 *   down and prove that the underwriters are willfully participating in the fork?  When they sign a proposed block on the
 *   "one true chain" that does not ultimately make it into the chain, that can't expose them to liability because they have
 *   no way to know if that signature will get into the chain or not - bad actors could then take that signature and try to use
 *   it against them, so that can't be a violation.  However, underwriters who sign a block that traces down to another block
 *   that is not in the "one true chain" _have_ now exposed themselves as colluding with the fork.  If the fork becomes the
 *   "one true chain" then they have lost nothing, but... that deeper following of the fork allows their pledged shares to be
 *   claimed by others in the "one true chain."  This feels powerful, very powerful.
 *
 *   Mix this with "chainweaving" by defining a fork as an alternate chain with a "double spend" of shares
 *   when compared with this chain.  If there are no double spends, the split chains can be rewoven into one,
 *   but as soon as one player spends the same funds on both chains, that is irreconcilable and one of the
 *   chains is going to need to lose.  Meanwhile, if the doublespender was an underwriter or chainmaker,
 *   their shares can be returned to the community on both chains due to rule-breaking.  The thornier problem
 *   is to determine which fork should continue and which is declared invalid - possibly by a residual value
 *   comparison.
 *
 * "with great power there must also come -- great responsibility!"  Marvel Comics Amazing Fantasy #15
 *
 * In plain terms: you want to follow a fork?  Fine, follow your fork, but by so doing you have surrendered the value you
 *   sign in that fork to any other chain which wishes to claim it.
 *
 *   Giant hole?  Chain forks, transactions proceed on both sides.  Transactions found double spending on either side
 *   of the fork liquidate their underwriting on the opposite side?  Alice gives shares to Bob, underwritten by Carol
 *   on one side.  Then Alice gives the same shares to Dan, underwritten by Erin on the other side.  On the side where
 *   Alice gave to Bob, can we liquidate Carol's underwriting because we see that Alice gave those shares to Dan on another
 *   chain?  Not really, Carol can't be expected to know about the Dan chain.  Alice, Bob and Dan are users, too ignorant
 *   to know if their underwriters have put them on a fork or not, they shouldn't lose their shares.  However, Carol and
 *   Erin are underwriters, and they should know better, so when Carol proceeds down the Bob chain, she is abandoning
 *   the Dan chain.  Assuming symmetric anti-forking rules: any shares Carol underwrites with Bob chain go up for grabs in
 *   the Dan chain - mostly liquidated as tax, but also quite the party for the error checkers - woo, hoo, and since Dan
 *   chain can make up all their own rules, they can give everybody free unicorns with every transaction too.  Back in
 *   boring Bob chain, error checkers see Erin deep-fork underwriting in Dan chain.  This is a pretty heavy commitment
 *   by Erin to the Dan chain, because error checkers in Bob chain are busy liquidating Erin's underwriting shares in Bob
 *   chain.  If Dan chain doesn't continue, Erin's shares will be decimated back in Bob chain.  I don't think this is a
 *   giant hole.
 *
 *   The real question is: can we expect "professionals" like Carol and Erin to recognize malicious forking attempts and
 *   not follow them?
 *   I think the answer is: yes.  The genuine chain ends will appear on the chainmaker network at approximately the correct
 *   times, there are clear rules about which proposed chain end is the one to use, and if things get fuzzy for a few
 *   cycles with chainmakers proposing competing forks, the underwriters should probably step back and not countersign
 *   their transactions until the dust settles.  Malicious chainmakers might try to sew this confusion intentionally,
 *   but underwriters won't be countersigning blocks that don't contain their transactions, and sooner or later the backlog
 *   of legitimate transactions' residual value should overwhelm any small-time monkey business.
 *
 * Cheat-catchers profit, and a large portion of the
 *   forked value is returned as tax, making transactions cheaper in this chain, possibly to the level of fully funding
 *   coin inflation.  Underwriters would have to choose a fork and stick with it, and if all transactions are required to
 *   be underwritten, this would be a large source of income to both forks while they continue (and a rather exicting
 *   frenzy for cheat catchers).   But, could ordinary givers and receivers participate on both forks?  I think it could
 *   go either way.  Givers source of funds come from before the fork.  If we force givers to sign a recent block in the
 *   chain (say: the proposal time) and require (by genesis parameter) a proposal to be within a certain time window of
 *   recording, then after that window has elapsed we can definitely lock a giver into the chain they have chosen after
 *   that time window has elapsed.  Now this is feeling like an incentive for forking frenzies, split your shares to both
 *   forks while you can...  I think it's in the hands of the underwriters, again more power.  If your underwriter signs
 *   your value into a fork, then you're stuck there.  I think we might be able to get out of this by filing a complaint:
 *   "I didn't want to be on that fork, I wanted to be on this one true fork."  No, no, rabbit hole rapidly approaching.
 *   How to handle the forking frenzy?  Your underwriter can always sign your wallet-give transaction into a number of
 *   short forks, that is beyond your control.  You can sign a block in your chosen chain at proposal time, this could
 *   or could not burn your value on other chains.
 *
 *   Say Wally the Whale aggregates 80% of a chain's shares, then does whatever transactions transferring them in a short
 *   time window.  Alt-chains could claim Wally's shares in their chain.  The shares don't all have to be Wally's, any
 *   Alt-chain can claim the "one true chain"'s transaction values and offer them up across whatever period of time.
 *   I want to see some simulations.  Psychologically, it might be beneficial to lock some (all? probably most) coin when
 *   making these claims, reducing the outstanding coin from 8 billion by some fraction of the cheat-claim.
 *
 *   So, what if an underwriter signs your page into an Alt-Chain?  The underwriter is the one doing the signing, so
 *   their underwriting amount of coin (after cheat catcher fees) is burned in the one chain, but the giver didn't sign
 *   any such thing, so is not exposed.  We can say that a giver should not sign the same page-key into two different
 *   chains, we can catch them in the act when they do it, but what can we do about it?  I don't see anything at the
 *   moment.  But maybe...
 *
 *   Speaking of good behavior, what a share holder should do when abandoning "the one true chain" for a legitimate
 *   fork is file a declaration of forking, which retires the shares and coin from this side of the fork because the
 *   share holder has decided to follow the other fork.  Failing to do so, and double-signing a page-key (once in each
 *   fork) is then a cheat, and when caught by a cheat catcher, does what?  The "one true chain" has a rule against
 *   signing into forks, but the fork double signed into may have erased that rule - what happens in the fork is no
 *   concern of the one true chain.  In the one chain, the cheat catcher is entitled to some form of bounty (max flat fee
 *   plus percentage of remaining?) and the balance of the double-signed page-key value is retired from coin and shares.
 *   Oops, no.  A give transaction already transfers all value to receivers, and they are not liable... enforcement
 *   doesn't have any teeth or reward - no cheat catching here, and if you can't catch cheats why have a rule?  Underwriters
 *   can't really search for records on other blockchains, seems like the better approach is to embrace the fact that
 *   fork-gives happen.  Still, I do think it's a good idea to include the proposal time block hash in the transaction.
 *
 *   So what?  So, there's a forking frenzy.  There's nothing lost in the one true chain when page-keys are double-signed
 *   during the fork-frenzy time window.  Underwriters and chainmakers who move to the forked chain lose their shares in
 *   the one chain, even during the fork-frenzy time window.  The forked chain can do whatever it wants, new perfect clone
 *   chains can start from scratch, too.  The goal is to preserve the share value of the one chain, and diminish a forked
 *   chain's incentives to dilute the one chain's resources of underwriters, chain makers and cheat catchers.  Burning
 *   forker's shares in the one true chain is one way to disincentivize the fork.
 *
 *   Fooling underwriters to sign on to a fork: This is where underwriters need to earn their keep.  As long as underwriters
 *   contact a majority of network sources and verify that they are signing "the one true chain" according to the phase gate
 *   timing rules, then they cannot be burned by a small-time fool's fork gambit.  Ordinary share holders have a much wider
 *   time window between proposal and when the transaction must record, but underwriters need to be on-top of the current
 *   real-time situation.  If they cannot contact a sufficient number of network nodes during the chain extending phase,
 *   they should delay and record the transaction in a later block to be safe.  It might be possible to give underwriters
 *   a smaller, but still > 1, grace period where it is O.K. for them to sign into a fork - but this seems messy.  Better
 *   to require them (by smart contract) to only sign one chainmaker block proposal per phase.  If not all underwriters
 *   sign the block proposal by the deadline, that's o.k. - the block can still be incorporated into the chain, but the
 *   pages not countersigned by their (malicious?) underwriters are not legally recorded in the chain.  Their hashes appear
 *   in the hash-chain, but their content is censored by the chainmaker, which both increases the residual value of the block,
 *   and avoids recording data in the chain for free.
 *
 *   Refined chain-making process:
 *
 *   Underwriters propose pages to chainmakers
 *
 *   Chainmakers assemble proposed blocks
 *
 *   Underwriters validate proposed blocks, maybe starting with the ones with the highest residual value,
 *     possibly ignoring proposed blocks from "fishy" sources.
 *
 *     Nice underwriters share information in the community, identifying problems with proposed blocks so other
 *     underwriters may quickly confirm those problems and move on to verifying other proposed blocks.  Problems
 *     should always be verified independently, because not all underwriters are guaranteed to play nice.
 *
 *     Nice underwriters also share their opinion of the best proposed block they have fully validated so far, not
 *     that an underwriter should trust what a "nice" underwriter says, they should do the validation themselves
 *     too, but if an underwriter is not going to get a chance to validate all proposed blocks, they might try
 *     trusting this information enough to lead them to validate this perhaps "best block" before a random one
 *     from the list.  Of course, spreaders of misinformation need to be ignored rather than trusted, optimization
 *     for another day.
 *
 *     Early days, underwriters will probably fully verify all proposed blocks to determine which one has the most
 *     residual value, then sign that one.  When it starts getting hairy because there's not enough time to verify
 *     them all, that's when some kind of cooperative trust network should emerge to optimize recording times without
 *     compromising security.
 *
 *   When evaluation is done, the choice should be clear, the proposed block with the highest residual value is the
 *     winner.  Underwriters all countersign that one (except when they don't).
 *
 *   Now, it's chainmaker's turn to decide which countersigned block has the highest value (to them), if none of their
 *     block proposals received countersignatures from underwriters, they have no incentive to continue - wait for next
 *     phase and maybe adjust their bidding strategy.  If the chainmaker did "win the bid" from at least one underwriter
 *     then they censor any pages not countersigned by their underwriters (as required by smart contract for this block
 *     to be valid), and sign their chainmaker page that wraps up the block.  Rather than waiting for late arrival
 *     underwriter countersignatures, I think we require chainmakers to only sign once like so many other chaos control
 *     situations.  Whichever chainmaker's final block has the highest residual value will be the one which is picked up
 *     by the following chainmaking process.  With underwriters and chainmakers only signing once per phase (or virtually
 *     guaranteeing loss of the shares they put up in the pages), the apparent winner should settle out pretty quickly.
 *     Malicious players might attempt to split the chain into nearly even parts, or flood the process with confusing
 *     proposals that lead most legitimate underwriters to sign separate block proposals - these strategies will be
 *     "wasting shares" for the attackers in recording fees, and delaying legitimate transactions, but they can only
 *     play with each proposed transaction one time per phase.  It is feeling like some kind of side-channel reputation
 *     or trust system would help to keep churners out of the chain making process.
 *
 *   As a malicious actor, I can "hijack" this chainmaking process by throwing high residual value transactions into
 *     the proposal process and only signing the block proposals that do not contain transactions I want to block.
 *     My high residual value transactions will cost me share, but I am "winning" delay of one or more legitimate
 *     transactions by not countersigning them.  Eventually, the residual value of transactions not included in the
 *     chain will build up as they get resubmitted for inclusion in future blocks and it will get more and more expensive
 *     to continue to block a particular transaction.  Also, the control algorithm for residual value tax incentive
 *     might develop a "sweet spot" such that perturbation of the normal flow of transactions becomes much more costly
 *     very quickly.  Through simulation there should emerge other ways to identify this kind of malicious actor and
 *     make it quickly more expensive to continue the ploy.
 *
 *   The important lesson here is the ability to add and tweak tax and other incentive algorithms on the fly as problems
 *     emerge.  Past attacks should be retained as test cases to check new algorithm proposals against in simulation to
 *     ensure that the algorithm revision doesn't re-open old vulnerabilities.  The (one?) primary philosophical focus of
 *     the incentive system is: shares are power.  Fair play is rewarded, unfair play is costly, thereby reducing share
 *     holding and eventually power to play unfairly.  Fair play amounts to fast processing of legitimate transactions
 *     at minimal cost by any entity who wants to connect to the network and play by the rules.
 *
 *   Weirdly enough: it might be more stable to develop a rule like 3 or 4 signatures maximum for underwriters on new
 *     blocks in the chain.  The countersignature could include a counter number to make it clear how many proposed
 *     blocks the underwriter is signing, but by allowing legitimate underwriters to sign more than one proposed
 *     block, it might be easier to thwart high residual blockers attempting to stall legitimate transactions.  Again,
 *     the call for simulation is strong in this one.
 *
 ******************************************************************************************************************
 *
 * Chain endorsemenents:
 *   Crazy-weird thought, a public key can "endorse" a blockchain by receiving shares on it with a note of
 *   endorsement.  Why would a blockchain want to give away shares?  Well, maybe because the endorsing
 *   party has run some sort of validation suite of tests on the chain, more likely as a celebrity promotional
 *   thing, who knows.  Anyway, it might be a way for new genesis blocks to gain some traction in the market -
 *   underwriters, chainmakers and error checkers might choose to only work on chains with a certain set of
 *   endorsements.  It's a way for a chain to get started without endorsements in the genesis block, but acquire
 *   them later.
 *
 */
