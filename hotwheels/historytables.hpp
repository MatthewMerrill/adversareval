#ifndef HISTORYTABLES_HPP
#define HISTORYTABLES_HPP

#include "bitscan.hpp"
#include "game.hpp"

namespace hl {

extern int historyTable[56 * 56];

static inline bool keyCmp(const Move a, const Move b) {
  int av = historyTable[56 * a.fromIdx + a.toIdx];
  int bv = historyTable[56 * b.fromIdx + b.toIdx];
  return av > bv;
}

void incrementTable(const Move m, int amount);
void resetTable();

}
#endif

