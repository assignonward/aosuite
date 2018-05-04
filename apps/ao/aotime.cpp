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
