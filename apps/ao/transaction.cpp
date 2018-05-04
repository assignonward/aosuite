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
