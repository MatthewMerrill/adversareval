#include <limits>
#include <list>
#include <chrono>

#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <utility>

#include "ansi.hpp"
#include "game.hpp"
#include "movegen.hpp"
#include "evaluate.hpp"
#include "minimax.hpp"
#include "historytables.hpp"
#include "transpositiontbl.hpp"

#include <mutex>

#define MINIMAX_TIME_CUTOFF 5

S16 negamax(const GameState* state, int depth,
            S16 alpha, S16 beta, int epoch);

int epoch = 0;
S16 fBuf[MOVE_ARR_LEN];

U64 evalsDone = 0;
time_t start = 0;
S16 initialDelta = 20;

std::mutex mm_mutex;

static float randomUniform01() {
  // https://stackoverflow.com/a/9879024/3188059
  return ((float) rand() / (RAND_MAX));
}

void populateFBuf(const GameState* state) {
  Move childMoves[MOVE_ARR_LEN];
  int nmoves = GetMoves(state, childMoves);
  for (int i = 0; i < nmoves; ++i) {
    GameState after = state->ApplyMove(childMoves[i]);
    const auto ttPair = tt::getValue(&after);
    if (ttPair.first == after.hashCode) {
      fBuf[i] = ttPair.second.val;
    }
    else {
      fBuf[i] = evaluate(&after);
    }
  }
}

void Ponder(GameState state) {
  S16 windowBase = evaluate(&state);
  S16 windowDelta = initialDelta;
  int depth = 5;
  std::lock_guard<std::mutex> guard(mm_mutex);
  populateFBuf(&state);
  while (start == 0) {
    const auto p = MyBestMoveAtDepth(&state, depth,
        windowBase - windowDelta, windowBase + windowDelta);
    if (p.second == 30000 || p.second == -30000) {
      windowDelta += 5;
    }
    else {
      windowBase = p.second;
      windowDelta = initialDelta;
      ++depth;
    }
  }
}
void StopPondering() {
  start = 1;
}

//*
Move MyBestMove(const GameState* state) {
  // Seed with current time https://stackoverflow.com/a/14914657/3188059
  srand(time(NULL));
  int depth = 3;
  Move best;
  hl::resetTable();
  //tt::clear();
  start = 1;
  std::lock_guard<std::mutex> guard(mm_mutex);
  populateFBuf(state);

  start = time(NULL); 
  while (depth < 30 && time(NULL) - start < MINIMAX_TIME_CUTOFF) {
    printf("\nDepth %2d: ", depth);
    evalsDone = 0;
    const auto p = MyBestMoveAtDepthMTDF(state, depth);
    if (p.second == 20000 || p.second == -20000) {
      textfg(RED);
      printf("          ABORT! Interrupted at depth %d\n", depth);
      resettext();
      break;
    }
    textfg(GREEN);
    printf("          Performed %9llu evals. Best: ", evalsDone);
    best = p.first;
    best.Print();
    std::cout << ".";
    // We Win!
    if (p.second >= 10000) {
      textfg(BLUE);
      printf("\n          This is going to be very painful......       for you!.\n");
      resettext();
      std::cout << std::flush;
      break;
    }
    // They Win!
    else if (p.second <= -10000) {
      textfg(BLUE);
      printf("\n          This is going to be very painful......\n");
      resettext();
      std::cout << std::flush;
      break;
    }
    // No Force Wins Found
    else {
      printf(" Evaluated at: %7d.\n", p.second);
      resettext();
      std::cout << std::flush;
      ++depth;
    }
  }
  start = 0;
  return best;
}
/*/
Move MyBestMove(const GameState* state) {
  // Seed with current time https://stackoverflow.com/a/14914657/3188059
  srand(time(NULL));
  int depth = 5;
  S16 windowBase = evaluate(state);
  S16 deltaLow = initialDelta;
  S16 deltaHigh = initialDelta;
  Move best;
  hl::resetTable();
  //tt::clear();
  start = 1;
  std::lock_guard<std::mutex> guard(mm_mutex);
  start = time(NULL); 
  while (depth < 30 && time(NULL) - start < MINIMAX_TIME_CUTOFF) {
    if (deltaLow == initialDelta && deltaHigh == initialDelta) {
      printf("\nDepth %2d: ", depth);
    }
    else {
      printf("          ");
    }
    printf("Window: (%7d, %7d)\n", windowBase - deltaLow, windowBase + deltaHigh);
    evalsDone = 0;
    const auto p = MyBestMoveAtDepth(state, depth,
                                     windowBase - deltaLow,
                                     windowBase + deltaHigh);
    if (p.second == 30000) {
      initialDelta += 5;
      deltaHigh += 20;
      textfg(RED);
      printf("          Too greedy with my window! Failed HIGH.\n");
      resettext();
      continue;
    }
    if (p.second == -30000) {
      initialDelta += 5;
      deltaLow += 20;
      textfg(RED);
      printf("          Too greedy with my window! Failed LOW.\n");
      resettext();
      continue;
    }
    if (p.second == 20000 || p.second == -20000) {
      textfg(RED);
      printf("          ABORT! Interrupted at depth %d\n", depth);
      resettext();
      break;
    }

    textfg(GREEN);
    printf("          Performed %9llu evals. Best: ", evalsDone);
    best = p.first;
    best.Print();
    std::cout << ".";
    // We Win!
    if (p.second >= 10000) {
      textfg(BLUE);
      printf("\n          This is going to be very painful......       for you!.\n");
      resettext();
      std::cout << std::flush;
      break;
    }
    // They Win!
    else if (p.second <= -10000) {
      textfg(BLUE);
      printf("\n          This is going to be very painful......\n");
      resettext();
      std::cout << std::flush;
      break;
    }
    // No Force Wins Found
    else {
      printf(" Evaluated at: %7d.\n", p.second);
      if (initialDelta == deltaLow && initialDelta == deltaHigh) {
        initialDelta -= 4;
      }
      windowBase = (p.second + windowBase) / 2;
      deltaLow = initialDelta;
      deltaHigh = initialDelta;
      resettext();
      std::cout << std::flush;
      ++depth;
    }
  }
  start = 0;
  return best;
}
//*/

pair<Move, S16> MyBestMoveAtDepth(const GameState* state, int depth, S16 alpha, S16 beta) {
  S16 value = -30000;
  S16 oldAlpha = alpha;
  Move childMoves[MOVE_ARR_LEN];
  int nmoves = GetMoves(state, childMoves);
  Move best;
  if (nmoves == 1) {
    return std::make_pair(childMoves[0], 0);
  }
  int numEq = 0;
  for (int moveIdx = 0; moveIdx < nmoves; ++moveIdx) {
    Move move = childMoves[moveIdx];
    GameState newState = state->ApplyMove(move).Invert();
    S16 nm = -negamax(&newState, depth-1, -beta, -alpha, epoch + 1);
    // ABORT
    if (nm == 20000 || nm == -20000) {
      return std::make_pair(Move(), nm);
    }
    // PRUNED: Ignore node completely
    else if (nm == 30000 || nm == -30000) {
      continue;
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

    alpha = (alpha > value) ? alpha : value;
    if (value > -10000 && value < oldAlpha) {
      return std::make_pair(best, -30000);
    }
    if (value < 10000 && value > beta) {
      return std::make_pair(best, 30000);
    }
  }
  return std::make_pair(best, value);
}

pair<Move, S16> MyBestMoveAtDepthMTDF(const GameState* state, int depth) {
  S16 value = -20000;
  Move childMoves[MOVE_ARR_LEN];
  int nmoves = GetMoves(state, childMoves);
  Move best;
  int numEq = 0;
  for (int moveIdx = 0; moveIdx < nmoves; ++moveIdx) {
    Move move = childMoves[moveIdx];
    GameState newState = state->ApplyMove(move).Invert();
    S16 f = fBuf[moveIdx];
    f += (depth & 1) ? -30 : 30;
    //printf("expected: %3d ", -f);
    f = -MTDF(&newState, -f, depth - 1, epoch + 1);
    //printf("actual:   %3d\n", -f);
    fBuf[moveIdx] = f;

    // ABORT
    if (f == 20000 || f == -20000) {
      return std::make_pair(Move(), f);
    }
    // PRUNED: Ignore node completely
    else if (f == 30000 || f == -30000) {
      printf("MTDF shouldn't be pruned like that?\n");
      continue;
    }

    if (f > value) {
      value = f;
      best = move;
      numEq = 1;
    }
    // Uniformly select from moves with same evaluation
    else if (f == value && randomUniform01() * ++numEq < 1) {
      value = f;
      best = move;
    }
  }
  return std::make_pair(best, value);
}

//https://www.chessprogramming.org/MTD(f)#C_Pseudo_Code
S16 MTDF(const GameState* state, S16 f, int depth, int epoch) {
  S16 bound[2] = { -19000, +19000 };
  //S16 f = evaluate(state) / 2;
  do {
    //printf("MTDF: { %7d, %7d }\n          ", bound[0], bound[1]);
    S16 beta = f + (f == bound[0]);
    f = negamax(state, depth, beta - 4, beta, epoch);
    bound[f < beta] = f;
  } while (f != 20000 && f != -20000 && bound[0] < bound[1]);
  return f;
}

using tt::TTRec;

// https://en.wikipedia.org/wiki/Negamax
// https://en.wikipedia.org/wiki/Negamax#Negamax_with_alpha_beta_pruning
// https://webdocs.cs.ualberta.ca/~tony/OldPapers/icca.Mar1986.pp3-18.pdf
S16 negamax(const GameState* state, int depth,
            S16 alpha, S16 beta, int epoch) {
  int winner = state->GetWinner();
  if (winner == 1) {
    return 10099 - epoch;
  }
  else if (winner == -1) {
    return -10099 + epoch;
  }

  if (start == 1 || (start != 0 && time(NULL) - start > MINIMAX_TIME_CUTOFF)) {
    return 20000;
  }

  S16 value = -30000;
  Move* bestMove = NULL;
  Move childMoves[MOVE_ARR_LEN];
  int nmoves;
  S16 oldAlpha = alpha;

  // We have TT value!
  const auto ttrecPair = tt::getValue(state);
  if (ttrecPair.first == state->hashCode) {
    TTRec ttrec = ttrecPair.second;
    if (ttrec.depth >= depth) {
      if (ttrec.bound == tt::Bound::EXACT) {
        return ttrec.val;
      }/*
      else if (ttrec.bound == tt::Bound::LOWER) {
        alpha = (alpha > ttrec.val) ? alpha : ttrec.val;
      }
      else if (ttrec.bound == tt::Bound::UPPER) {
        beta = (beta < ttrec.val) ? beta : ttrec.val;
      }*/
      if (alpha >= beta) {
        return ttrec.val;
      }
    }
    if (depth == 0) {
      evalsDone++;
      return evaluate(state);
    }
    if (ttrec.depth >= 0) {
      GameState newState = state->ApplyMove(ttrec.bestMove).Invert();
      value = -negamax(&newState, depth-1, -beta, -alpha, epoch + 1);
      // ABORT
      if (value == 20000 || value == -20000) {
        return value;
      }
      // PRUNED: Ignore node completely
      else if (value == 30000 || value == -30000) {
        value = -30000;
        bestMove = &(ttrec.bestMove);
      }
      if (value >= beta) {
        goto done;
      }
    }
  }
  // We don't, but that's fine. We're done anyway.
  else if (depth == 0) {
    evalsDone++;
    return evaluate(state);
  }

  nmoves = GetMoves(state, childMoves);
  std::sort(childMoves, childMoves + nmoves, hl::keyCmp);

  if (nmoves == 0) {
    std::cerr << "WARN: Umm this shouldn't be possible! Err: 012938";
  }
  for (signed char idx = 0; idx < nmoves; ++idx) {
    Move move = childMoves[idx];
    GameState newState = state->ApplyMove(move).Invert();
    // TODO: Fix depth changes
    S16 nm = -negamax(&newState, depth-1,
                      -beta, -alpha, epoch + 1);
    // ABORT
    if (nm == 20000 || nm == -20000) {
      return nm;
    }

    if (nm >= beta) {
      hl::incrementTable(move, 2000);
      return nm;
    }
    if (nm > value) {
      value = nm;
      bestMove = &move;
      //alpha = (alpha > value) ? alpha : value;
      //if (beta - alpha <= (epoch + 8) / 4) {
      //if (value >= beta) {
      //  return value;
      //}
    }
  }
done:
  if (20000 > value && value > -20000 && bestMove != NULL) {
    hl::incrementTable(*bestMove, (int) (10 * value));
    if (depth > 4) {
      /*tt::Bound bound = tt::Bound::EXACT;
      if (value <= oldAlpha) {
        bound = tt::Bound::UPPER;
      }
      if (value >= beta) {
        bound = tt::Bound::LOWER;
      }*/
      tt::setValue(state, TTRec(value, depth, *bestMove, tt::Bound::EXACT));
    }
  }
  return value;
}
