#include "historytables.hpp"
#include <algorithm>

namespace hl {
  int historyTable[56 * 56];

  void incrementTable(const Move m, int amount) {
    historyTable[56 * m.fromIdx + m.toIdx] += amount;
  }

  void resetTable() {
    std::fill(historyTable, historyTable + (56*56), 0);
  }
}
