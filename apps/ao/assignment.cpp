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
#include "assignment.h"
#include <QCryptographicHash>
#include <QSettings>

/**
 * @brief Assignment::Assignment - describes the exchange of value
 * @param parent
 */
Assignment::Assignment(QObject *parent) : QObject(parent)
{ // proposalTime.set( AOTime::now() );  Need to populate proposedChain, instead.
  finalRecordingDeadline.set( proposalTime().get() + AOTime::shiftUp64( AO_DAY ) ); // default, TBD by wallet
  randomizeSalt();
}

AOTime Assignment::proposalTime()
{ return proposedChain.publicationTime(); }

/**
 * @brief Assignment::randomizeSalt - using the last salt and
 *   the current time, come up with a new salt.
 */
void Assignment::randomizeSalt()
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
 * @brief Assignment::valid
 * @return true if the Assignment internal checks are all valid
 */
bool Assignment::valid()
{ return validSum() & validTimeline(); }

/**
 * @brief Assignment::validTimeline
 * @return true if the timeline is in-order
 *   Will want to add some additional sanity checks
 */
bool Assignment::validTimeline()
{ return ( finalRecordingDeadline.future() && proposalTime().past() ); }

/**
 * @brief Assignment::validSum
 * @return true if the give, take and bid sum correctly
 */
bool Assignment::validSum()
{ Shares total(0);
  foreach( Participant p, participants.list )
    total += p.getAmount();
  return ( recordingBid == total );
}
