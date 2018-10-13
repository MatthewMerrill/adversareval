#include <limits>
#include <list>
#include <chrono>

#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <utility>

#include "game.hpp"
#include "movegen.hpp"
#include "evaluate.hpp"
#include "minimax.hpp"
#include "historytables.hpp"
#include "transpositiontbl.hpp"

MMRet negamax(const GameState* state, int depth, MMRet alpha, MMRet beta);

U64 evalsDone = 0;
time_t start = 0;

static float randomUniform01() {
  // https://stackoverflow.com/a/9879024/3188059
  return ((float) rand() / (RAND_MAX));
}

Move MyBestMove(const GameState* state) {
  // Seed with current time https://stackoverflow.com/a/14914657/3188059
  srand(time(NULL));
  int depth = 2;
  Move best;
  hl::resetTable();
  //tt::clear();
  start = time(NULL); 
  while (time(NULL) - start < 5) {
    evalsDone = 0;
    Move m = MyBestMoveAtDepth(state, depth);
    if (m.from == 0) {
      printf("ABORT! Interrupted at depth %d\n", depth);
      break;
    }
    std::cout << "Performed " << evalsDone << " evals at depth "
      << depth << "." << std::endl;
    best = m;
    ++depth;
  }
  start = 0;
  return best;
}

Move MyBestMoveAtDepth(const GameState* state, int depth) {
  MMRet value = {MMRet::ABORT};
  Move best;
  std::vector<Move> childMoves = GetMoves(state);
  if (childMoves.size() == 1) {
    return childMoves.at(0);
  }
  int numEq = 0;
  for (const Move move : childMoves) {
    GameState newState = state->ApplyMove(move).Invert();
    MMRet nm = negamax(&newState, depth-1, {MMRet::LOSE}, {MMRet::WIN}).InvertOut();
    switch (nm.tag) {
      case MMRet::ABORT:
        return Move();

      default:
        if (nm > value) {
          value = nm;
          best = move;
          numEq = 1;
        }
        // Uniformly select from moves with same evaluation
        else if (nm == value && randomUniform01() * ++numEq < 1) {
          value = nm;
          best = move;
        }
        break;
    }
  }
  return best;
}

// https://en.wikipedia.org/wiki/Negamax
// https://en.wikipedia.org/wiki/Negamax#Negamax_with_alpha_beta_pruning
MMRet negamax(const GameState* state, int depth, MMRet alpha, MMRet beta) {
  tt::TTRec ttrec = tt::getValue(state);
  switch (ttrec.val.tag) {
    case MMRet::WIN:
    case MMRet::LOSE:
      return ttrec.val;

    case MMRet::NORMAL:
      if (ttrec.depth >= depth) {
        return ttrec.val;
      } else {
        // TODO: is this a good idea?
        //alpha = (alpha < ttrec.val) ? alpha : ttrec.val;
      }
      break;
    default:
      break;
  }
  int winner = state->GetWinner();
  if (winner == 1) {
    MMRet ret = {MMRet::WIN};
    ret.depth = depth;
    tt::setValue(state, {ret, depth, 0});
    return ret;
  }
  else if (winner == -1) {
    MMRet ret = {MMRet::LOSE};
    ret.depth = depth;
    tt::setValue(state, {ret, depth, 0});
    return ret;
  }
  if (start != 0 && time(NULL) - start > 5) {
    return {MMRet::ABORT};
  }
  if (depth == 0) {
    if (tt::hasValue(state)) {
      printf("WOO\n");
      return tt::getValue(state).val;
    }
    evalsDone++;
    return {MMRet::NORMAL, evaluate(state)};
  }
  std::vector<Move> childMoves = GetMoves(state);
  if (ttrec.val.tag != MMRet::ABORT) {
    //printf("%d\n", ttrec.bestMoveIdx);
    std::iter_swap(childMoves.begin(), childMoves.begin() + ttrec.bestMoveIdx);
  }
  std::sort(childMoves.begin() + 1, childMoves.end(), hl::keyCmp);
  MMRet value = {MMRet::ABORT};
  int idx = 0;
  int bestIdx = -1;
  if (childMoves.size() == 0) {
    std::cerr << "WARN: Umm this shouldn't be possible! Err: 012938";
  }
  for (const Move move : childMoves) {
    GameState newState = state->ApplyMove(move).Invert();
    MMRet nm = negamax(&newState, depth-1, -beta, -alpha).InvertOut();
    switch (nm.tag) {
      case MMRet::ABORT:
        return nm;

      default:
        if (nm > value) {
          value = nm;
          bestIdx = idx;
        }
        alpha = (alpha > value) ? alpha : value;
        if (alpha > beta || alpha == beta) {
          hl::incrementTable(move, 300);
          tt::setValue(state, {value, depth, idx});
          return value;
        }
    }
    idx += 1;
  }
  if (value.tag != MMRet::ABORT) {
    hl::incrementTable(childMoves[bestIdx], 1);
    tt::setValue(state, {value, depth, bestIdx});
  }
  return value;
}
