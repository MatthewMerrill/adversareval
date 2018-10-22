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

#include <mutex>

#define MINIMAX_TIME_CUTOFF 5


MMRet negamax(const GameState* state, int depth, int moveBufBase,
              MMRet alpha, MMRet beta);

U64 evalsDone = 0;
time_t start = 0;
Move moveBuf[70 * 30];

std::mutex mm_mutex;

static float randomUniform01() {
  // https://stackoverflow.com/a/9879024/3188059
  return ((float) rand() / (RAND_MAX));
}

void Ponder(GameState state) {
  int depth = 5;
  std::lock_guard<std::mutex> guard(mm_mutex);
  while (start == 0) {
    MyBestMoveAtDepth(&state, depth);
    ++depth;
  }
}
void StopPondering() {
  start = 1;
}

Move MyBestMove(const GameState* state) {
  // Seed with current time https://stackoverflow.com/a/14914657/3188059
  srand(time(NULL));
  int depth = 2;
  Move best;
  hl::resetTable();
  //tt::clear();
  start = 1;
  std::lock_guard<std::mutex> guard(mm_mutex);
  start = time(NULL); 
  while (depth < 30 && time(NULL) - start < MINIMAX_TIME_CUTOFF) {
    evalsDone = 0;
    const auto p = MyBestMoveAtDepth(state, depth);
    if (p.second.tag == MMRet::ABORT) {
      printf("ABORT! Interrupted at depth %d\n", depth);
      start = 0;
      break;
    }

    printf("Performed %9llu evals at depth %2d. Best is now: ", evalsDone, depth);
    best = p.first;
    best.Print();
    std::cout << ".";
    switch (p.second.tag) {
      case MMRet::WIN:
        printf("\nThis is going to be very painful......       for you!.\n");
        start = 0;
        std::cout << std::flush;
        return best;

      case MMRet::LOSE:
        printf("\nThis is going to be very painful......\n");
        start = 0;
        std::cout << std::flush;
        return best;

      default:
        printf(" Evaluated at: %7.0f.\n", p.second.eval);
        ++depth;
        break;
    }
    std::cout << std::flush;
  }
  start = 0;
  return best;
}

pair<Move, MMRet> MyBestMoveAtDepth(const GameState* state, int depth) {
  MMRet value = {MMRet::ABORT};
  Move* childMoves = (Move*) &moveBuf;
  int nmoves = GetMoves(state, childMoves);
  Move best;
  if (nmoves == 1) {
    return std::make_pair(childMoves[0], MMRet{MMRet::NORMAL});
  }
  int numEq = 0;
  for (int moveIdx = 0; moveIdx < nmoves; ++moveIdx) {
    Move move = childMoves[moveIdx];
    GameState newState = state->ApplyMove(move).Invert();
    MMRet nm = negamax(&newState, depth-1, nmoves, MMRet{MMRet::LOSE}, MMRet{MMRet::WIN}).InvertOut();
    switch (nm.tag) {
      case MMRet::ABORT:
        return std::make_pair(Move(), nm);

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
  return std::make_pair(best, value);
}

using tt::TTRec;

// https://en.wikipedia.org/wiki/Negamax
// https://en.wikipedia.org/wiki/Negamax#Negamax_with_alpha_beta_pruning
MMRet negamax(const GameState* state, int depth, int moveBufBase,
              MMRet alpha, MMRet beta) {
  int winner = state->GetWinner();
  if (winner == 1) {
    MMRet ret = MMRet{MMRet::WIN};
    ret.depth = depth;
    return ret;
  }
  else if (winner == -1) {
    MMRet ret = {MMRet::LOSE};
    ret.depth = depth;
    return ret;
  }

  if (start == 1 || (start != 0 && time(NULL) - start > MINIMAX_TIME_CUTOFF)) {
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
          }/*
          else if (ttrec.bound == tt::Bound::LOWER) {
            // max(alpha, ttrec.val)
            alpha = (alpha > ttrec.val) ? alpha : ttrec.val;
          }
          else if (ttrec.bound == tt::Bound::UPPER) {
            // min(beta, ttrec.val)
            beta = (beta < ttrec.val) ? beta : ttrec.val;
          }*/
        }
        break;
      default:
        break;
    }
    if (ttrec.bestMoveIdx < nmoves) {
      std::swap(childMoves[0], childMoves[ttrec.bestMoveIdx]);
      std::sort(childMoves + 1, childMoves + nmoves, hl::keyCmp);
    }
    else {
      //printf("Hmm ttrec has an invalid move idx?\n");
      std::sort(childMoves, childMoves + nmoves, hl::keyCmp);
    }
    
    //std::sort(childMoves, childMoves + nmoves, hl::keyCmp);
  }
  // We don't, but that's fine. We're done anyway.
  else if (depth == 0) {
    evalsDone++;
    return {MMRet::NORMAL, evaluate(state)};
  }
  // Okay we don't have a TT and we're not done yet. Just use HistTbl.
  else {
    //std::sort(childMoves, childMoves + nmoves, hl::keyCmp);
  }

  tt::Bound bound = tt::Bound::EXACT;
  MMRet value = {MMRet::ABORT};
  MMRet oldAlpha = alpha;
  signed char bestIdx = -1;
  if (nmoves == 0) {
    std::cerr << "WARN: Umm this shouldn't be possible! Err: 012938";
  }
  for (signed char idx = 0; idx < nmoves; ++idx) {
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
          bound = tt::Bound::LOWER;
          hl::incrementTable(move, 200);
          goto done;
        }
    }
  }
done:
  if (value.tag != MMRet::ABORT) {
    switch (value.tag) {
      case MMRet::WIN:
        hl::incrementTable(childMoves[bestIdx], 100000);
        break;
      case MMRet::LOSE:
        hl::incrementTable(childMoves[bestIdx], -100000);
        break;
      case MMRet::NORMAL:
        hl::incrementTable(childMoves[bestIdx], (int) (10 * value.eval));
        break;
      default:
        hl::incrementTable(childMoves[bestIdx], 100);
        break;
    }
    if (depth > 4) {
      if (value < oldAlpha || value == oldAlpha) {
        bound = tt::Bound::UPPER;
      }
      if (value > beta || value == beta) {
        bound = tt::Bound::LOWER;
      }
      tt::setValue(state, TTRec(value, depth, bestIdx, bound));
    }
  }
  return value;
}
