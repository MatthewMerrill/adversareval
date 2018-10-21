#ifndef MINIMAX_H
#define MINIMAX_H

#include <cassert>

#include "game.hpp"
#include "evaluate.hpp"

// https://en.cppreference.com/w/cpp/language/union
struct MMRet {
  enum{ABORT, LOSE, NORMAL, WIN} tag;
  union {
    float eval;
    int depth;
  };

  bool operator ==(MMRet other) const {
    return tag == other.tag
      && eval == other.eval
      && eval == other.depth;
  }
  bool operator >(MMRet other) const {
    return tag > other.tag
      || (tag == NORMAL && eval > other.eval)
      || (tag == LOSE && depth > other.depth)
      || (tag == WIN && depth < other.depth);
  }
  bool operator <(MMRet other) const {
    return tag < other.tag
      || (tag == NORMAL && eval < other.eval)
      || (tag == LOSE && depth < other.depth)
      || (tag == WIN && depth > other.depth);
  }

  MMRet operator -() const {
    switch(tag) {
      case MMRet::NORMAL:
        return {MMRet::NORMAL, -eval};
      case MMRet::WIN: {
        MMRet ret = {MMRet::LOSE};
        ret.depth = depth;
        return ret;
      }
      case MMRet::LOSE: {
        MMRet ret = {MMRet::WIN};
        ret.depth = depth;
        return ret;
      }
      case MMRet::ABORT:
        assert(false && "No ABORTs in - op!");
        return {MMRet::ABORT};
    }
    assert(false && "IDK this tag!" && tag);
    return {MMRet::ABORT};
  }

  MMRet InvertIn() {
    MMRet ret;
    switch (tag) {
      case MMRet::NORMAL:
        ret.tag = MMRet::NORMAL;
        ret.eval = -eval;
        break;

      case MMRet::WIN:
        ret.tag = MMRet::LOSE;
        ret.depth = depth-1;
        break;

      case MMRet::LOSE:
        ret.tag = MMRet::WIN;
        ret.depth = depth-1;
        break;

      case MMRet::ABORT:
        ret.tag = MMRet::ABORT;
        break;
    }
    return ret;
  }

  MMRet InvertOut() {
    MMRet ret;
    switch (tag) {
      case MMRet::NORMAL:
        ret.tag = MMRet::NORMAL;
        ret.eval = -eval;
        break;

      case MMRet::WIN:
        ret.tag = MMRet::LOSE;
        ret.depth = depth+1;
        break;

      case MMRet::LOSE:
        ret.tag = MMRet::WIN;
        ret.depth = depth+1;
        break;

      case MMRet::ABORT:
        ret.tag = MMRet::ABORT;
        break;
    }
    return ret;
  }
};


Move MyBestMove(const GameState* state);
Move MyBestMoveAtDepth(const GameState* state, int depth);

#endif

