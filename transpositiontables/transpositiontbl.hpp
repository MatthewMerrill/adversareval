#ifndef TRANSPOSITIONTABLES_HPP
#define TRANSPOSITIONTABLES_HPP
#include <utility>
#include "game.hpp"
#include "minimax.hpp"
namespace tt {

  struct TTRec {
    MMRet val;
    int depth;
    int bestMoveIdx;
  };

  void clear();
  void setValue(const GameState* state, TTRec rec);
  bool hasValue(const GameState* state);
  TTRec getValue(const GameState* state);

  bool cmp(const GameState* a, const GameState* b);
}
#endif

