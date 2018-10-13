#include <unordered_map>
#include <iostream>

#include "transpositiontbl.hpp"

namespace tt {
  std::unordered_map<GameState, TTRec> tbl = {};

  void clear() {
    tbl.clear();
  }
  void setValue(const GameState* state, TTRec rec) {
    if (rec.val.tag != MMRet::ABORT
        && (tbl.find(*state) == tbl.end()
          || rec.depth > tbl[*state].depth)) {
      tbl[*state] = rec;
    }
  }
  bool hasValue(const GameState* state) {
    return tbl.find(*state) != tbl.end()
      && tbl[*state].val.tag != MMRet::ABORT;
  }
  TTRec getValue(const GameState* state) {
    //std::cout << "{";
    //for (auto const& pair: tbl) {
    //  std::cout << "{" << pair.first.pieces << ": " << pair.second.depth << "},";
    //}
    //std::cout << "}" << std::endl;
    return tbl[*state];
  }
}
