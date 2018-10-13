#include "evaluate.hpp"
#include <iostream>

float evaluate(const GameState* state) {
  U64 bit = 1;
  float eval = 0;
  int idx; 
  U64 pieces = state->pieces;
  while ((idx = bitscanll(pieces))) {
    bit = 1ULL << (idx-1);
    pieces ^= bit;
    if (state->teams & bit) {
      if (state->cars & bit) {
        eval -= 20 * (idx % 7);
      }
      eval -= 1;
    }
    else {
      if (state->cars & bit) {
        eval += 20 * (idx % 7);
      }
      eval += 1;
    }
  }
  return eval;
}
