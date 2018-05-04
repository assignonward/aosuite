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
#include "transaction.h"
#include <QCryptographicHash>
#include <QSettings>

Participant::Participant(QByteArray i, Shares a, QObject *parent) : QObject(parent)
{ setId( i );
  setAmount( a );
  setMinUAmt( a ); // Default, can be adjusted - usually higher
  // note to be set later if desired, usually empty.
}

Participant::Participant( const Participant &p ) : QObject(p.parent())
{ id.setPublicKey( p.getId() );
  amount  = p.getAmount();
  minUAmt = p.getMinUAmt();
  note    = p.getNote();
}

/**
 * @brief Transaction::Transaction - describes the exchange of value
 * @param parent
 */
Transaction::Transaction(QObject *parent) : QObject(parent)
{ // proposalTime.set( AOTime::now() );  Need to populate proposedChain, instead.
  closingDeadline.set( proposalTime().get() + AOTime::shiftUp64( AO_DAY ) ); // default, TBD by wallet
  randomizeSalt();
}

AOTime Transaction::proposalTime()
{ return proposedChain.publicationTime(); }

/**
 * @brief Transaction::randomizeSalt - using the last salt and
 *   the current time, come up with a new salt.
 */
void Transaction::randomizeSalt()
{ QSettings setting;
  QByteArray ls = setting.value( "lastSalt" ).toByteArray();
  rng.seed( *((__int128 *)ls.data()) ^ AOTime::now() ); // LAME, but good enough for now.
  QByteArray ba;
  while ( ba.length() < 32 )
    ba.append( rng.rnd_uint64() & 0xFF );
  salt = QCryptographicHash::hash( ba, QCryptographicHash::Sha3_256 );
  setting.setValue( "lastSalt", salt );
}

/**
 * @brief Transaction::valid
 * @return true if the transaction internal checks are all valid
 */
bool Transaction::valid()
{ return validSum() & validTimeline(); }

/**
 * @brief Transaction::validTimeline
 * @return true if the timeline is in-order
 *   Will want to add some additional sanity checks
 */
bool Transaction::validTimeline()
{ return ( closingDeadline.future() && proposalTime().past() ); }

/**
 * @brief Transaction::validSum
 * @return true if the give, take and bid sum correctly
 */
bool Transaction::validSum()
{ Shares total(0);
  foreach( Participant p, participants )
    total += p.getAmount();
  return ( recordingBid == total );
}
