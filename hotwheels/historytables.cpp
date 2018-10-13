#include "historytables.hpp"
#include <algorithm>

namespace hl {
  int historyTable[56 * 56];

  bool keyCmp(const Move a, const Move b) {
    int av = historyTable[56 * (bitscanll(a.from)-1) + bitscanll(a.to)-1];
    int bv = historyTable[56 * (bitscanll(b.from)-1) + bitscanll(b.to)-1];
    return av > bv;
  }

  void incrementTable(const Move m, int amount) {
    historyTable[56 * (bitscanll(m.from)-1) + bitscanll(m.to)-1] += amount;
  }

  void resetTable() {
    std::fill(historyTable, historyTable + (56*56), 0);
  }
}
