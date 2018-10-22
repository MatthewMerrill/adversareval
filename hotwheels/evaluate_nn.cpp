#include <iostream>
#include <vector>

#include <fdeep/fdeep.hpp>

#include "bitscan.hpp"
#include "evaluate.hpp"
#include "nn.hpp"

const auto model = fdeep::load_model("models/fdeep_model2.json");

signed short evaluate(const GameState* state) {
  fdeep::tensor3 tensor(fdeep::shape_hwc(8, 7, 10), 0.0);
  
  U64 pieces = state->pieces;
  int idx;
  U64 bit;
  
  while ((idx = bitscanll(pieces))) {
    bit = 1ULL << (idx - 1);
    pieces ^= bit;
    int r = (idx-1) / 7;
    int c = (idx-1) % 7;
    int base = (bit & state->teams) ? 5 : 0;
    if (bit & state->cars) {
      tensor.set_yxz(r, c, base + 0, 1.0);
    }
    else if (bit & state->knights) {
      tensor.set_yxz(r, c, base + 1, 1.0);
    } 
    else if (bit & state->rooks) {
      tensor.set_yxz(r, c, base + 2, 1.0);
    }
    else if (bit & state->bishops) {
      tensor.set_yxz(r, c, base + 3, 1.0);
    }
    else if (bit & state->pawns) {
      tensor.set_yxz(r, c, base + 4, 1.0);
    }
    else {
      std::cerr << "Err: 12931" << std::endl;
    }
  }
  //*/

  const auto result_vec = model.predict({tensor});
  const auto result_val = 1.0 * result_vec[0].get_yxz(0, 0, 0);
  //std::cout << result_val << std::endl;
  return (signed short) (result_val - 50);
}

