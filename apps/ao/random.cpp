#include "random.h"
// Intended to be a replacable, improvable module.
// Good enough for the moment, and I prefer a specified
// PRNG instead of relying on system libraries.

Random::Random( __int128 sd )
{ seed( sd ); }

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

float Random::rnd_float( float min, float max )
{ uint64_t x = s[0];
  uint64_t const y = s[1];
  s[0] = y;
  x ^= x << 23; // a
  s[1] = x ^ y ^ (x >> 17) ^ (y >> 26); // b, c
  uint64_t r = (s[1] + y) & 0xFFFFFFFF; // restricted to 32 bits
  return min + ((max - min) * (float)r / (float)0xFFFFFFFF);
}

