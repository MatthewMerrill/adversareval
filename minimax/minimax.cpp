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

#define MINIMAX_TIME_CUTOFF 5


MMRet negamax(const GameState* state, int depth, int moveBufBase,
              MMRet alpha, MMRet beta);

U64 evalsDone = 0;
time_t start = 0;
Move moveBuf[70 * 30];

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
  while (depth < 30 && time(NULL) - start < MINIMAX_TIME_CUTOFF) {
    evalsDone = 0;
    Move m = MyBestMoveAtDepth(state, depth);
    if (m.fromIdx == -1) {
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
  Move* childMoves = (Move*) &moveBuf;
  int nmoves = GetMoves(state, childMoves);
  Move best;
  if (nmoves == 1) {
    return childMoves[0];
  }
  int numEq = 0;
  for (int moveIdx = 0; moveIdx < nmoves; ++moveIdx) {
    Move move = childMoves[moveIdx];
    GameState newState = state->ApplyMove(move).Invert();
    MMRet nm = negamax(&newState, depth-1, nmoves, {MMRet::LOSE}, {MMRet::WIN}).InvertOut();
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

using tt::TTRec;

// https://en.wikipedia.org/wiki/Negamax
// https://en.wikipedia.org/wiki/Negamax#Negamax_with_alpha_beta_pruning
MMRet negamax(const GameState* state, int depth, int moveBufBase,
              MMRet alpha, MMRet beta) {
  int winner = state->GetWinner();
  if (winner == 1) {
    MMRet ret = {MMRet::WIN};
    ret.depth = depth;
    return ret;
  }
  else if (winner == -1) {
    MMRet ret = {MMRet::LOSE};
    ret.depth = depth;
    return ret;
  }

  if (start != 0 && time(NULL) - start > MINIMAX_TIME_CUTOFF) {
    return {MMRet::ABORT};
  }

  Move* childMoves = ((Move*) &moveBuf) + moveBufBase;
  int nmoves = GetMoves(state, childMoves);

  // We have TT value!
  const auto ttrecPair = tt::getValue(state);
  if (ttrecPair.first == state->hashCode) {
    TTRec ttrec = ttrecPair.second;
    //https://webdocs.cs.ualberta.ca/~tony/OldPapers/icca.Mar1986.pp3-18.pdf
    if (depth == 0) {
      return ttrec.val;
    }
    switch (ttrec.val.tag) {
      case MMRet::WIN:
      case MMRet::LOSE:
        return ttrec.val;

      case MMRet::NORMAL:
        if (ttrec.depth >= depth) {
          if (ttrec.bound == tt::Bound::EXACT) {
            return ttrec.val;
          }
          else if (ttrec.bound == tt::Bound::LOWER) {
            // max(alpha, ttrec.val)
            alpha = (alpha > ttrec.val) ? alpha : ttrec.val;
          }
          else if (ttrec.bound == tt::Bound::UPPER) {
            // min(beta, ttrec.val)
            beta = (beta < ttrec.val) ? beta : ttrec.val;
          }
        }
        break;
      default:
        break;
    }
    childMoves[nmoves] = ttrec.bestMove;
    std::swap(childMoves[0], childMoves[nmoves]);
    nmoves += 1;
    std::sort(childMoves + 1, childMoves + nmoves, hl::keyCmp);
  }
  // We don't, but that's fine. We're done anyway.
  else if (depth == 0) {
    evalsDone++;
    return {MMRet::NORMAL, evaluate(state)};
  }
  // Okay we don't have a TT and we're not done yet. Just use HistTbl.
  else {
    std::sort(childMoves, childMoves + nmoves, hl::keyCmp);
  }

  MMRet value = {MMRet::ABORT};
  MMRet oldAlpha = alpha;
  int bestIdx = -1;
  if (nmoves == 0) {
    std::cerr << "WARN: Umm this shouldn't be possible! Err: 012938";
  }
  for (int idx = 0; idx < nmoves; ++idx) {
    Move move = childMoves[idx];
    GameState newState = state->ApplyMove(move).Invert();
    MMRet nm = negamax(&newState, depth-1, moveBufBase + nmoves,
                       beta.InvertIn(), alpha.InvertIn()).InvertOut();
    switch (nm.tag) {
      case MMRet::ABORT:
        return nm;

      default:
        if (nm > value) {
          hl::incrementTable(move, idx);
          value = nm;
          bestIdx = idx;
        }
        alpha = (alpha > value) ? alpha : value;
        if (alpha > beta || alpha == beta) {
          hl::incrementTable(move, 200);
          goto done;
        }
    }
  }
done:
  if (value.tag != MMRet::ABORT) {
    hl::incrementTable(childMoves[bestIdx], 1);
    tt::Bound bound = tt::Bound::EXACT;
    if (value < oldAlpha || value == oldAlpha) {
      bound = tt::Bound::UPPER;
    }
    if (value > beta || value == beta) {
      bound = tt::Bound::LOWER;
    }
    tt::setValue(state, {value, depth, childMoves[bestIdx], bound});
  }
  return value;
}
