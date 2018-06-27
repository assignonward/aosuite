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
#include "datavbc64.h"
#include "datavarlength.h"
#include "note.h"
#include "pageref.h"
#include "participant.h"
#include "pubkey.h"
#include "random.h"
#include "salt256.h"
#include "shares.h"
#include "signature.h"

/**
 * @brief The Assignment class - for negotiation and recording of a shares assignment contract
 */
class Assignment : public DataVarLength
{
    Q_OBJECT
public:
    explicit  Assignment( const DataItemBA &di = QByteArray(), QObject *p = NULL );
              Assignment( const Assignment &a, QObject *p = NULL )
                : DataVarLength( AO_ASSIGNMENT, p ? p : a.parent() ), proposedChain( a.proposedChain ),
                  recordingDeadline( a.recordingDeadline ), recordingBid( a.recordingBid ), note( a.note ),
                  participants( a.participants ) {}
        void  operator = ( const DataItemBA &di );
  DataItemBA  toDataItem( bool cf = false );
      AOTime  proposalTime();
        void  randomizeSalt();
        bool  valid();
        bool  validSum();
        bool  validTimeline();
        void  append( Participant p ) { participants.append(p); nParticipants = participants.size(); }
      qint32  getNParticipants() { return nParticipants.value(); }

private:
            PageRef  proposedChain;     // Reference to the signature page of a recent block in the chain this assignment is proposed to be recorded on, and valid child of this block should also be acceptable
             AOTime  recordingDeadline; // When the assignment contract is expected to be recorded in the chain
             Shares  recordingBid;      // Positive amount to bid for all underwriting and recording taxes
               Note  note;              // top level note, applies to all participants whereas participant level notes may only apply to that participant
       DataVbc64  nParticipants;
  QList<Participant> participants;
};

/* A structure to hold:
 *   <TRAN> Coin transfer transaction proposal:
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
