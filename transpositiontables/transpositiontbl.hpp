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
    Move bestMove;
    Bound bound;
    signed char pieceCount;

    TTRec() {
      val = 20000;
      depth = 0;
      bestMove = Move();
      bound = Bound::EXACT;
    }

    TTRec(S16 v, signed char d, Move bm, Bound b):
      val(v), depth(d), bestMove(bm), bound(b) {}
  };
//*
  // bufs[16] holds the backing vector for GameStates with 16 pieces
  thread_local extern pair<U64, TTRec>* bufs;
  extern U64 MOD;

  static inline void init() {
   /* printf("How many GB Ram can you burn? ");
    double inp;
    std::cin >> inp;
    inp *= 1000000000.0;
    inp /= sizeof(pair<U64, TTRec>);
    MOD = (U64) inp;

    bufs = new pair<U64, TTRec>[MOD + 1];
    for (unsigned long long idx = 0; idx < MOD + 1; idx += 256) {
      *(volatile char*) (bufs + idx);
    }*/
  }

  static inline void setValue(const GameState* state, TTRec rec) {
    if (rec.val != 20000 && rec.val != -20000) {
      const auto optPair = bufs[state->hashCode % MOD];
      if (optPair.first != 0 && optPair.second.depth > rec.depth) {
        return; // Already something better there.
      }
      rec.pieceCount = popcount(state->pieces);
      bufs[state->hashCode % MOD] = std::make_pair(state->hashCode, rec);
    }
  }

  static inline pair<U64, TTRec> getValue(const GameState* state) {
    const auto p = bufs[state->hashCode % MOD];
    if (p.first == state->hashCode) {
      return p;
    }
    return std::make_pair(0, TTRec());
  }

  static inline void clear() {
    for (U64 i = 0; i < MOD; ++i) {
      bufs[i].first = 0;
    }
  }
  static inline void cleanup(const GameState* state) {
    // noop. We already did all our allocing so freeing won't help.
    int pieceCount = popcount(state->pieces);
    for (U64 i = 0; i < MOD; ++i) {
      if (bufs[i].first != 0 && pieceCount < bufs[i].second.pieceCount) {
        bufs[i].first = 0;
      }
    }
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

