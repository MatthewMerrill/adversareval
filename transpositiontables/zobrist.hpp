// Matthew Merrill mattmerr.com
//
// Generates a bunch of random U64s in an array for use in Zobrist hashing
#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <random>

// https://stackoverflow.com/a/18726221/3188059
void generateZobristKeysFromSeed(
  unsigned long long** keys,
  int ntypes, int ntiles, int seed) {
  
  std::mt19937_64 mtRand{ (unsigned long long) seed };
  
  for (int itype = 0; itype < ntypes; ++itype) {
    for (int itile = 0; itile < ntiles; ++itile) {
      keys[itype][itile] = (keys[itype][itile] << 8) | (mtRand() & 255);
      keys[itype][itile] = (keys[itype][itile] << 8) | (mtRand() & 255);
      keys[itype][itile] = (keys[itype][itile] << 8) | (mtRand() & 255);
      keys[itype][itile] = (keys[itype][itile] << 8) | (mtRand() & 255);
      keys[itype][itile] = (keys[itype][itile] << 8) | (mtRand() & 255);
      keys[itype][itile] = (keys[itype][itile] << 8) | (mtRand() & 255);
      keys[itype][itile] = (keys[itype][itile] << 8) | (mtRand() & 255);
      keys[itype][itile] = (keys[itype][itile] << 8) | (mtRand() & 255);
    }
  }
}
void generateZobristKeys(unsigned long long** keys, int ntypes, int ntiles) {
  std::random_device r;
  generateZobristKeysFromSeed(keys, ntypes, ntiles, r());
}

#endif
