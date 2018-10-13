#ifndef HISTORYTABLES_HPP
#define HISTORYTABLES_HPP

#include "bitscan.hpp"
#include "game.hpp"

namespace hl {

extern int historyTable[56 * 56];

bool keyCmp(const Move a, const Move b);
void incrementTable(const Move m, int amount);
void resetTable();

}
#endif

