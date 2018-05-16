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
#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include <QObject>
#include "aotime.h"
#include "blockref.h"
#include "datavarlenlong.h"
#include "pageref.h"
#include "participantlist.h"
#include "pubkey.h"
#include "random.h"
#include "salt256.h"
#include "shares.h"
#include "signature.h"

/**
 * @brief The Assignment class - for negotiation and recording of a shares assignment contract
 */
class Assignment : public DataVarLenLong
{
    Q_OBJECT
public:
    explicit  Assignment(const QByteArray &di = QByteArray(), QObject *p = nullptr);
              Assignment( const Assignment &a )
                : DataVarLenLong( AO_ASSIGNMENT, a.parent() ), salt( a.salt ), proposedChain( a.proposedChain ),
                  recordingDeadline( a.recordingDeadline ), recordingBid( a.recordingBid ), note( a.note ),
                  participants( a.participants ) {}
        void  operator = ( const QByteArray &di );
  QByteArray  toDataItem();
      AOTime  proposalTime();
        void  randomizeSalt();
        bool  valid();
        bool  validSum();
        bool  validTimeline();

private:
             Random  rng;
            Salt256  salt;
            PageRef  proposedChain;     // Reference to the signature page of a recent block in the chain this assignment is proposed to be recorded on
             AOTime  recordingDeadline; // When the assignment contract is expected to be recorded in the chain
             Shares  recordingBid;      // Positive amount to bid for all underwriting and recording taxes
               Note  note;              // top level note, applies to all participants whereas participant level notes may only apply to that participant
    ParticipantList  participants;
};

/**
 * @brief The Authorization class - when
 *   complete and valid, contains a description of
 *   the basic assignment contract between all the participants
 *   without the underwriting and recording - only the RBID is specified
 *   which describes the maximum commission payable to
 *   the sum of all underwriters, chain-maker, and recording tax.
 */
class Authorization : public QObject
{
    Q_OBJECT
public:
    explicit Authorization( QObject *parent = nullptr) : QObject( parent ) {}
 //            Authorization( Assignment t, QObject *parent = nullptr);
 QByteArray  toByteArray() { return QByteArray(); }

private:
         Assignment  tran;
    QList<Signature> sigs;  // Same length and order as the participants list in tran
};

/* A structure to hold:
 *   <TRAN> Coin transfer transaction proposal:
 *     [SALT] 256 bit random number included in all signatures
 *     [TPRO] "now" from Alice's Wallet clock
 *     [TRMX] "now"+6 hours (closing deadline)
 *     [RBID] 3 coin (maximum recording fee)
 *     <PART> List of each participant's:
 *       [KEY]  Alice's "504" public key
 *       [AMT]  -504.023507972
 *       [NOTE] (empty)
 *       <PAGE> giver's source of funds
 *         [TREC] recording time of PAGE
 *         [HASH] hash of PAGE (found in BLOCK at TREC)
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
 * <AUTH> A fully described and authorized transaction:
 *   <PART> List of each participant's:
 *     [TSIG] time of signing (approving) the proposed transaction <TRAN>
 *     [SIG]  signature on: TRAN+AUTH(PART(TSIG)) using corresponding private TRAN(PART(KEY))
 *   <TRAN> as above, now wrapped in the AUTH structure
 *
 * AUTH(PART) and AUTH(TRAN(PART)) lists must be the same length and in the same order
 *
 * PAGE is only needed for givers, and is not necessary in initial transaction
 *   proposal, but must be accurately recorded before AUTH is signed
 * Perhaps: when MINU is absent, it is assumed to be AMT (by rules of the Genesis block)
 *
 */

#endif // ASSIGNMENT_H
