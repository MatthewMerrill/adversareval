#include "evaluate.hpp"
#include "minimax.hpp"

#include <limits>

float MinimaxMin(const GameState* state, int depth) {
  return evaluate(state);
}

float MinimaxMax(const GameState* state, int depth) {
  return 1 - evaluate(state);
}
