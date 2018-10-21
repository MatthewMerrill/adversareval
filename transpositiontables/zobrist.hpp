// Matthew Merrill mattmerr.com
//
// Generates a bunch of random U64s in an array for use in Zobrist hashing
#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <random>

#include <game.hpp>

namespace tt {

  extern U64 ZOBRIST_KEYS[];

  // https://stackoverflow.com/a/18726221/3188059
  static inline void generateZobristKeysFromSeed(U64* keys, int n, int seed) {
    std::mt19937_64 mtRand{ (U64) seed };
    for (int i = 0; i < n; ++i) {
      keys[i] = (mtRand() & 255);
      keys[i] = (keys[i] << 8) | (mtRand() & 255);
      keys[i] = (keys[i] << 8) | (mtRand() & 255);
      keys[i] = (keys[i] << 8) | (mtRand() & 255);
      keys[i] = (keys[i] << 8) | (mtRand() & 255);
      keys[i] = (keys[i] << 8) | (mtRand() & 255);
      keys[i] = (keys[i] << 8) | (mtRand() & 255);
      keys[i] = (keys[i] << 8) | (mtRand() & 255);
    }
  }
  static inline void generateZobristKeys(U64* keys, int ntypes, int ntiles) {
    std::random_device r;
    generateZobristKeysFromSeed(keys, ntypes * ntiles, r());
  }

  U64 hash(const GameState* state);
}

#endif
