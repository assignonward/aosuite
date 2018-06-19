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
#include "random.h"
#include <QFile>

// Intended to be a replacable, improvable module.
// Good enough for the moment, and I prefer a specified
// PRNG instead of relying on system libraries.

Random rng; // Global object

Random::Random( __int128 sd )
{ seed( sd );
  namesInitialized = false;
}

void Random::seed( __int128 sd )
{ s[0] = sd & 0xFFFFFFFFFFFFFFFF;
  s[1] = (sd >> 64) & 0xFFFFFFFFFFFFFFFF;
}

uint64_t Random::rnd_uint64()
{ uint64_t x = s[0];
  uint64_t const y = s[1];
  s[0] = y;
  x ^= x << 23; // a
  s[1] = x ^ y ^ (x >> 17) ^ (y >> 26); // b, c
  return s[1] + y;
}

int64_t Random::rnd_int64()
{ uint64_t x = s[0];
  uint64_t const y = s[1];
  s[0] = y;
  x ^= x << 23; // a
  s[1] = x ^ y ^ (x >> 17) ^ (y >> 26); // b, c
  uint64_t r = s[1] + y;
   int64_t v = *((int64_t *)&r);
  return v;
}

QByteArray Random::rnd_bytes( qint32 n )
{ QByteArray ba;
  union _64_as_8
    { qint64 i;
      unsigned char d[8];
    } u;
  while ( ba.size() < n )
    { u.i = rnd_int64();
      int j = 0;
      while (( ba.size() < n ) && ( j < 8 ))
        ba.append( u.d[j++] );
    }
  return ba;
}

float Random::rnd_float( float min, float max )
{ uint64_t x = s[0];
  uint64_t const y = s[1];
  s[0] = y;
  x ^= x << 23; // a
  s[1] = x ^ y ^ (x >> 17) ^ (y >> 26); // b, c
  uint64_t r = (s[1] + y) & 0xFFFFFFFF; // restricted to 32 bits
  return min + ((max - min) * (float)r / (float)0xFFFFFFFF);
}

/**
 * @brief Random::rnd_nameAndEmail
 * @return A random name and matching random email address
 */
QStringList Random::rnd_nameAndEmail()
{ if ( !namesInitialized )
    initializeNames();
  bool male = ( rnd_int64() & 1 );
  QString firstName  = male ?   maleNames.at( rnd_uint64() %   maleNames.size() ) :
                              femaleNames.at( rnd_uint64() % femaleNames.size() ) ;
  QString middleName = male ?   maleNames.at( rnd_uint64() %   maleNames.size() ) :
                              femaleNames.at( rnd_uint64() % femaleNames.size() ) ;
  while ( middleName == firstName )
    { middleName = male ?   maleNames.at( rnd_uint64() %   maleNames.size() ) :
                          femaleNames.at( rnd_uint64() % femaleNames.size() ) ;
    }
  QString lastName    =    lastNames.at( rnd_uint64() %    lastNames.size() );
  QString emailDomain = emailDomains.at( rnd_uint64() % emailDomains.size() );

  QString fullName;
  int nameForm = rnd_uint64() % 100;
  if ( nameForm < 20 )
    fullName = firstName+" "+middleName+" "+lastName;
   else if ( nameForm < 35 )
    fullName = firstName+" "+middleName.left(1)+". "+lastName;
   else if ( nameForm < 45 )
    fullName = firstName.left(1)+". "+middleName+" "+lastName;
   else
    fullName = firstName+" "+lastName;

  QString emailAddress;
  nameForm = rnd_uint64() % 100;
  if ( nameForm < 20 )
    emailAddress = firstName.left(1)+lastName+"@"+emailDomain;
   else if ( nameForm < 40 )
    emailAddress = firstName.left(1)+middleName.left(1)+lastName+"@"+emailDomain;
   else if ( nameForm < 60 )
    emailAddress = firstName+middleName.left(1)+lastName+"@"+emailDomain;
   else if ( nameForm < 80 )
    emailAddress = firstName+lastName.left(1)+"@"+emailDomain;
   else
    emailAddress = firstName.left(1)+middleName.left(1)+lastName.left(1)+QString::number( rnd_uint64() % 50 + 50 )+"@"+emailDomain;

  QStringList ret;
  ret.append( fullName );
  ret.append( emailAddress.toLower() );
  return ret;
}

void Random::initializeNames()
{ QFile file( ":/files/malenames.txt" );
  file.open( QIODevice::ReadOnly );
  QString data = QString::fromUtf8( file.readAll() );
  file.close();
  maleNames = data.split( QRegExp("\\s"), QString::SkipEmptyParts );

  file.setFileName( ":/files/femalenames.txt" );
  file.open( QIODevice::ReadOnly );
  data = QString::fromUtf8( file.readAll() );
  file.close();
  femaleNames = data.split( QRegExp("\\s"), QString::SkipEmptyParts );

  file.setFileName( ":/files/lastnames.txt" );
  file.open( QIODevice::ReadOnly );
  data = QString::fromUtf8( file.readAll() );
  file.close();
  lastNames = data.split( QRegExp("\\s"), QString::SkipEmptyParts );

  file.setFileName( ":/files/emaildomains.txt" );
  file.open( QIODevice::ReadOnly );
  data = QString::fromUtf8( file.readAll() );
  file.close();
  emailDomains = data.split( QRegExp("\\s"), QString::SkipEmptyParts );

  namesInitialized = true;
}

