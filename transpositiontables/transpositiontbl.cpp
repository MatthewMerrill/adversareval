#include <unordered_map>
#include <iostream>
#include <stdio.h>
#include <vector>

#include "bitscan.hpp"
#include "transpositiontbl.hpp"

namespace tt {
  //*
  pair<U64, TTRec>* bufs;
  U64 MOD;
  /*/
  std::unordered_map<GameState, TTRec> tbl = {};

  inline static int countPieces(const GameState* state) {
    return popcount(state->pieces);
  }
  void cleanup(const GameState* state) {
    unsigned int numPieces = popcount(state->pieces);
    std::vector<const GameState*> rm;
    for (auto& rec: tbl) {
      if (popcount(rec.first.pieces) > numPieces) {
        rm.push_back(&rec.first);
      }
      if (rm.size() > 100000000ULL) {
        printf("That's a lot of cleaning!");
        break;
      }
    }
    printf("Cleaning %llu elems...", rm.size());
    for (auto& s : rm) {
      tbl.erase(*s);
    }
    //tbl.rehash(0);
  }
  //*/
}

