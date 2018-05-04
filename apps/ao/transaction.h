/*
 * MIT License
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
#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QObject>
#include "aotime.h"
#include "shares.h"
#include "pubkey.h"
#include "random.h"

/**
 * @brief The BlockRef class - identifies a block in the chain
 */
class BlockRef : public QObject
{
    Q_OBJECT
public:
    explicit  BlockRef( QObject *parent = nullptr) : QObject( parent ) {}
              BlockRef( const BlockRef &r )
                : QObject( r.parent() ), sequenceNumber( r.sequenceNumber ),
                  proposalTime( r.proposalTime ), hash( r.hash ) {}
      AOTime  time() { return proposalTime; }
  QByteArray  toByteArray() { return QByteArray(); }

private:
     quint32 sequenceNumber; // sequence number in the chain (Genesis==0) this page is recorded in
      AOTime proposalTime;   // time this block was proposed
  QByteArray hash;           // Hash of block - redundant check
};


/**
 * @brief The PageRef class - when the page is the chain-maker's signature page on
 *   a block in the chain, then this is also a reference to a block and chain.
 */
class PageRef : public QObject
{
    Q_OBJECT
public:
    explicit  PageRef( QObject *parent = nullptr) : QObject( parent ) {}
              PageRef( const PageRef &r )
                : QObject( r.parent() ), block( r.block ), sequenceNumber( r.sequenceNumber ), hash( r.hash ) {}
      AOTime  publicationTime() { return block.time(); }
  QByteArray  toByteArray() { return QByteArray(); }

private:
    BlockRef block;           // block this page is recorded in
      qint16 sequenceNumber;  // page's sequence number in the block
  QByteArray hash;            // Hash of page - redundant check
};

/**
 * @brief The ShareSource class - identifies a source of shares
 */
class ShareSource : public QObject
{
    Q_OBJECT
public:
    explicit  ShareSource( QObject *parent = nullptr) : QObject( parent ) {}
              ShareSource( const ShareSource &r )
                : QObject( r.parent() ), participantNumber( r.participantNumber ), page( r.page )  {}
  QByteArray  toByteArray() { return QByteArray(); }

private:
    qint16 participantNumber; // sequence number in the participant list
   PageRef page;              //
};

/**
 * @brief The Participant class - describes a participant's role in an Assignment contract
 */
class Participant : public QObject
{
    Q_OBJECT
public:
     explicit  Participant( QByteArray i, Shares a, QObject *parent = nullptr);
               Participant( const Participant &p );
   QByteArray  getId()      const { return id.getPublicKey(); }
       Shares  getAmount()  const { return amount;  }
       Shares  getMinUAmt() const { return minUAmt; }
   QByteArray  getNote()    const { return note;    }
         void  setId( QByteArray i )   { id.setPublicKey( i ); }
         void  setAmount( Shares v )   { amount  = v; }
         void  setMinUAmt( Shares v )  { minUAmt = v; }
         void  setNote( QByteArray n ) { note    = n; }
   QByteArray  toByteArray() { return QByteArray(); }

private:
      PubKey id;      // Public Key, corresponding to the private key used for signing
      Shares amount;  // Positive for receivers, negative for givers
      Shares minUAmt; // Positive, minimum acceptable amount for underwriting
  QByteArray note;    // Arbitrary data to record with the transaction
     PageRef page;    // Reference to a page in a block in the chain, for givers in the transaction
};

class Transaction : public QObject
{
    Q_OBJECT
public:
    explicit  Transaction(QObject *parent = nullptr);
      AOTime  proposalTime();
        void  randomizeSalt();
        bool  valid();
        bool  validSum();
        bool  validTimeline();
  QByteArray  toByteArray() { return QByteArray(); }

private:
             Random  rng;
         QByteArray  salt;
            PageRef  proposedChain;   // Reference to the signature page of a recent block in the chain this transaction is proposed to be recorded on
             AOTime  closingDeadline;
             Shares  recordingBid;    // Positive amount to bid for all underwriting, chain-making and recording taxes
  QList<Participant> participants;
};

class Signature : public QObject
{
    Q_OBJECT
public:
    explicit  Signature( AOTime t, QObject *parent = nullptr)
        : QObject( parent ), timeOfSignature( t ) {}
  QByteArray  toByteArray() { return QByteArray(); }

private:
      AOTime timeOfSignature;
  QByteArray signature;
};

/**
 * @brief The Authorization class - when
 *   complete and valid, contains a description of
 *   the basic transaction between all the participants
 *   without the underwriting and recording - only the RBID is specified
 *   which describes the maximum commission payable to
 *   the sum of all underwriters, chain-maker, and recording tax.
 */
class Authorization : public QObject
{
    Q_OBJECT
public:
    explicit Authorization( QObject *parent = nullptr) : QObject( parent ) {}
             Authorization( Transaction t, QObject *parent = nullptr);
 QByteArray  toByteArray() { return QByteArray(); }

private:
      Transaction  tran;
  QList<Signature> sig;  // Same length and order as the participants list in tran
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

#endif // TRANSACTION_H
