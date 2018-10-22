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


S16 negamax(const GameState* state, int depth, int moveBufBase,
            S16 alpha, S16 beta);

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
    if (p.second == 20000 || p.second == -20000) {
      printf("ABORT! Interrupted at depth %d\n", depth);
      break;
    }

    printf("Performed %9llu evals at depth %2d. Best is now: ", evalsDone, depth);
    best = p.first;
    best.Print();
    std::cout << ".";
    // We Win!
    if (p.second >= 10000) {
      printf("\nThis is going to be very painful......       for you!.\n");
      std::cout << std::flush;
      break;
    }
    // They Win!
    else if (p.second <= -10000) {
      printf("\nThis is going to be very painful......\n");
      std::cout << std::flush;
      break;
    }
    // No Force Wins Found
    else {
      printf(" Evaluated at: %7d.\n", p.second);
      std::cout << std::flush;
      ++depth;
    }
  }
  start = 0;
  return best;
}

pair<Move, S16> MyBestMoveAtDepth(const GameState* state, int depth) {
  S16 value = -20000;
  Move* childMoves = (Move*) &moveBuf;
  int nmoves = GetMoves(state, childMoves);
  Move best;
  if (nmoves == 1) {
    return std::make_pair(childMoves[0], 0);
  }
  int numEq = 0;
  for (int moveIdx = 0; moveIdx < nmoves; ++moveIdx) {
    Move move = childMoves[moveIdx];
    GameState newState = state->ApplyMove(move).Invert();
    S16 nm = -negamax(&newState, depth-1, nmoves, -19999, 19999);
    if (nm == 20000 || nm == -20000) {
      return std::make_pair(Move(), nm);
    }
    else if (nm >= 10000) {
      nm += 1;
    }
    else if (nm <= -10000) {
      nm -= 1;
    }

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
  }
  return std::make_pair(best, value);
}

using tt::TTRec;

// https://en.wikipedia.org/wiki/Negamax
// https://en.wikipedia.org/wiki/Negamax#Negamax_with_alpha_beta_pruning
S16 negamax(const GameState* state, int depth, int moveBufBase,
            S16 alpha, S16 beta) {
  int winner = state->GetWinner();
  if (winner == 1) {
    return 10000 + depth;
  }
  else if (winner == -1) {
    return -10000 - depth;
  }

  if (start == 1 || (start != 0 && time(NULL) - start > MINIMAX_TIME_CUTOFF)) {
    return 20000;
  }

  Move* childMoves = ((Move*) &moveBuf) + moveBufBase;
  int nmoves = GetMoves(state, childMoves);

  // We have TT value!
  const auto ttrecPair = std::make_pair(0, TTRec());//tt::getValue(state);
  if (ttrecPair.first == state->hashCode) {
    TTRec ttrec = ttrecPair.second;
    //https://webdocs.cs.ualberta.ca/~tony/OldPapers/icca.Mar1986.pp3-18.pdf
    if (depth == 0) {
      return ttrec.val;
    }
    if (ttrec.val >= 10000 || ttrec.val <= -10000) {
        return ttrec.val;
    }
    if (ttrec.depth >= depth) {
      if (ttrec.bound == tt::Bound::EXACT) {
        return ttrec.val;
      }
      else if (ttrec.bound == tt::Bound::LOWER) {
        alpha = (alpha > ttrec.val) ? alpha : ttrec.val;
      }
      else if (ttrec.bound == tt::Bound::UPPER) {
        // min(beta, ttrec.val)
        beta = (beta < ttrec.val) ? beta : ttrec.val;
      }
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
    return evaluate(state);
  }
  // Okay we don't have a TT and we're not done yet. Just use HistTbl.
  else {
    std::sort(childMoves, childMoves + nmoves, hl::keyCmp);
  }

  tt::Bound bound = tt::Bound::EXACT;
  S16 value = -20000;
  S16 oldAlpha = alpha;
  signed char bestIdx = -1;
  if (nmoves == 0) {
    std::cerr << "WARN: Umm this shouldn't be possible! Err: 012938";
  }
  for (signed char idx = 0; idx < nmoves; ++idx) {
    Move move = childMoves[idx];
    GameState newState = state->ApplyMove(move).Invert();
    // TODO: Fix depth changes
    S16 nm = -negamax(&newState, depth-1, moveBufBase + nmoves,
                       -beta, -alpha);
    // ABORT!
    if (nm == 20000 || nm == -20000) {
      return nm;
    }
    else if (nm >= 10000) {
      nm += 1;
    }
    else if (nm <= -10000) {
      nm -= 1;
    }
    if (nm > value) {
      hl::incrementTable(move, 10 * idx);
      value = nm;
      bestIdx = idx;
    }
    alpha = (alpha > value) ? alpha : value;
    if (alpha > beta || alpha == beta) {
      bound = tt::Bound::LOWER;
      hl::incrementTable(move, 20000);
      break;
    }
  }
  if (value != 20000 && value != -20000) {
    hl::incrementTable(childMoves[bestIdx], (int) (10 * value));
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
