#include "game.hpp"
#include "evaluate.hpp"


float evaluate(const GameState* state) {
  if (state->XWins()) {
    return -1;
  }
  else if (state->OWins()) {
    return 1;
  }
  else {
    return 0;
  }
}
