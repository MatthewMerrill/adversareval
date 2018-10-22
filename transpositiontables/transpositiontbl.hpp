#ifndef TRANSPOSITIONTABLES_HPP
#define TRANSPOSITIONTABLES_HPP
#include <unordered_map>
#include <utility>
#include "bitscan.hpp"
#include "game.hpp"
#include "minimax.hpp"
#include "zobrist.hpp"

namespace std {
  template <> struct hash<GameState> {
    std::size_t operator()(const GameState& s) const noexcept {
      return s.hashCode;
    }
  };
}

using std::pair;

namespace tt {
  
  enum Bound{EXACT, UPPER, LOWER};
  struct TTRec {
    S16 val;
    signed char depth;
    signed char bestMoveIdx;
    Bound bound;

    TTRec() {
      val = 20000;
      depth = 0;
      bestMoveIdx = -1;
      bound = Bound::EXACT;
    }

    TTRec(S16 v, signed char d, signed char bmi, Bound b):
      val(v), depth(d), bestMoveIdx(bmi), bound(b) {}
  };
//*
  // bufs[16] holds the backing vector for GameStates with 16 pieces
  extern pair<U64, TTRec>* bufs[27];
  static U64 MOD_MASK = (1ULL << 20) - 1;

  static inline void init() {
    for (int pc = 2; pc < 27; ++pc) {
      //Let each piece-count buffer hold 2^30 values
      //bufs[pc] = (pair<U64, TTRec>*) malloc((MOD_MASK + 1) * sizeof(pair<U64, TTRec>));
      bufs[pc] = new pair<U64, TTRec>[MOD_MASK + 1];
    }
  }

  static inline void setValue(const GameState* state, TTRec rec) {
    int pieceCount = popcount(state->pieces);
    if (rec.val != 20000 && rec.val != -20000) {
      const auto optPair = bufs[pieceCount][state->hashCode & MOD_MASK];
      if (optPair.first != 0 && optPair.second.depth > rec.depth) {
        return; // Already something better there.
      }
      bufs[pieceCount][state->hashCode & MOD_MASK] = std::make_pair(state->hashCode, rec);
    }
  }

  static inline pair<U64, TTRec> getValue(const GameState* state) {
    int pieceCount = popcount(state->pieces);
    const auto p = bufs[pieceCount][state->hashCode & MOD_MASK];
    if (p.first == state->hashCode) {
      return p;
    }
    return std::make_pair(0, TTRec());
  }

  static inline void clear() {
  }
  static inline void cleanup(const GameState* state) {
    // noop. We already did all our allocing so freeing won't help.

  }
}


  /*/
  extern std::unordered_map<GameState, TTRec> tbl;

  static inline void clear() {
    tbl.clear();
  }

  static inline void init() {
    tbl.reserve(280000ULL);
  }

  void cleanup(const GameState* state);
  
  static inline void setValue(const GameState* state, TTRec rec) {
    if (rec.val.tag != MMRet::ABORT) {
      TTRec &existing = tbl[*state];
      if (rec.depth > existing.depth) {
        existing = rec;
      }
    }
  }
  static inline bool hasValue(const GameState* state) {
    return tbl.find(*state) != tbl.end();
  }
  static inline pair<U64, TTRec> getValue(const GameState* state) {
    return hasValue(state) ? std::make_pair(state->hashCode, tbl[*state]) : std::make_pair(0ULL, TTRec());
  }
}
//*/
#endif

