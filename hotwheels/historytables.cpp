#include "historytables.hpp"
#include <algorithm>

namespace hl {
  thread_local signed long long historyTable[56 * 56] = { 0 };

  void incrementTable(const Move m, int amount) {
    historyTable[56 * m.fromIdx + m.toIdx] += amount;
  }

  void resetTable() {
    for (int i = 0; i < 56 * 56; ++i) {
      historyTable[i] = (int) (historyTable[i] * 0.8888);
    }
    //std::fill(historyTable, historyTable + (56*56), 0);
  }
}
