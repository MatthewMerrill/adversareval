#include "evaluate.hpp"
#include <iostream>

float evaluate(const GameState* state) {
  U64 bit = 1;
  float eval = 0;
  for (int r = 0; r < 8; ++r) {
    for (int c = 0; c < 7; ++c) {
      if (state->pieces & bit) {
        if (state->teams & bit) {
          if (state->cars & bit) {
            eval -= 20 * c;
          }
          //eval -= 1;
        }
        else {
          if (state->cars & bit) {
            eval += 20 * c;
          }
          //eval += 1;
        }
      }
      bit <<= 1;
    }
  }
  return rand();
}
