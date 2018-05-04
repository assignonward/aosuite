#include "aotime.h"
#include <QDateTime>

AOTime::AOTime( __int128 tm, QObject *parent) : QObject(parent)
{ if ( tm == 0 )
    set( now() );
   else
    set( tm );
}

/**
 * @brief AOTime::now
 * @return seconds since epoch * 2^64, at millisecond resolution
 */
__int128 AOTime::now()
{ QDateTime now  = QDateTime::currentDateTime();
   __int128 sec  = now.toSecsSinceEpoch();
   __int128 ms   = now.time().msec();
  return (sec << 64) + ((ms << 64)/1000);
}

__int128 AOTime::shiftUp64( __int128 v )
{ return v << 64; }

/**
 * @brief AOTime::future
 * @return true if t is in the future
 */
bool AOTime::future()
{ return ( t > now() ); }

/**
 * @brief AOTime::past
 * @return true if t is in the past
 */
bool AOTime::past()
{ return ( t < now() ); }
