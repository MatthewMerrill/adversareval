#include "evaluate.hpp"
#include "minimax.hpp"

float MinimaxMin() {
  return evaluate();
}

float MinimaxMax() {
  return 1 - evaluate();
}
