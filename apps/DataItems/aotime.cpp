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
#include "aotime.h"
#include <QDateTime>

AOTime::AOTime( qint64 tm, typeCode_t typ, QObject *p )
  : Data64( tm, typ, p )
{ if ( tm == 0 )
    set( now() );
}

/**
 * @brief AOTime::now
 * @return seconds since epoch * 2^64, at millisecond resolution
 */
qint64 AOTime::now()
{ QDateTime now  = QDateTime::currentDateTime();
     qint64 sec  = now.toSecsSinceEpoch();
     qint64 ms   = now.time().msec();
  return ((sec * 1000) + ms) * AO_MILLISECOND;
}

/**
 * @brief AOTime::future
 * @return true if t is in the future
 */
bool AOTime::future()
{ return ( v > now() ); }

/**
 * @brief AOTime::past
 * @return true if t is in the past
 */
bool AOTime::past()
{ return ( v < now() ); }
