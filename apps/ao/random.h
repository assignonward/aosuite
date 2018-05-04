#ifndef RANDOM_H
#define RANDOM_H
#include <stdint.h>

class Random
{
public:
           Random( __int128 seed = 42 );
     void  seed( __int128 seed );
 uint64_t  rnd_uint64();
  int64_t  rnd_int64();
    float  rnd_float( float min, float max );

private:
    /* The state must be seeded so that it is not all zero */
    uint64_t s[2];
};

#endif // RANDOM_H
