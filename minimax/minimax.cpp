#include <limits>
#include <chrono>
#include <stdio.h>

#include <algorithm>
#include <iostream>
#include <thread>
#include <utility>
#include <vector>

#include "ansi.hpp"
#include "game.hpp"
#include "movegen.hpp"
#include "evaluate.hpp"
#include "minimax.hpp"
#include "historytables.hpp"
#include "transpositiontbl.hpp"

#include <mutex>

#define MINIMAX_TIME_CUTOFF 5

using std::vector;

S16 negamax(const GameState* state, int depth,
            S16 alpha, S16 beta, int epoch, int thIdx);

int epoch = 0;

unsigned long long evalsDone = 0;
time_t start = 0;
S16 initialDelta = 20;

std::mutex mm_mutex;

static float randomUniform01() {
  // https://stackoverflow.com/a/9879024/3188059
  return ((float) rand() / (RAND_MAX));
}

std::thread mtdfThreads[MOVE_ARR_LEN];
S16 fBuf[MOVE_ARR_LEN];
unsigned long thEvalsDone[MOVE_ARR_LEN];

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
  int depth = 5;
  std::lock_guard<std::mutex> guard(mm_mutex);
  while (start == 0) {
    MyBestMoveAtDepthMTDF(&state, depth++);
  }
}
void StopPondering() {
  start = 1;
}

//*
Move MyBestMove(const GameState* state) {
  // Seed with current time https://stackoverflow.com/a/14914657/3188059
  srand(time(NULL));
  int depth = 5;
  Move best;
  hl::resetTable();
  start = 1;
  std::lock_guard<std::mutex> guard(mm_mutex);
  populateFBuf(state);

  start = time(NULL); 
  while (depth < 50 && time(NULL) - start < MINIMAX_TIME_CUTOFF) {
    printf("\nDepth %2d: ", depth);
    evalsDone = 0;
    const auto p = MyBestMoveAtDepthMTDF(state, depth);
    if (p.second == 20000 || p.second == -20000) {
      textfg(RED);
      printf("ABORT! Interrupted at depth %d", depth);
      resettext();
      break;
    }
    textfg(GREEN);
    printf("Performed %12llu evals. Best: ", evalsDone);
    best = p.first;
    best.Print();
    std::cout << ".";
    // We Win!
    if (p.second >= 10000) {
      textfg(BLUE);
      printf("\n          This is going to be very painful......       for you!.");
      resettext();
      std::cout << std::flush;
      break;
    }
    // They Win!
    else if (p.second <= -10000) {
      textfg(BLUE);
      printf("\n          This is going to be very painful......");
      resettext();
      std::cout << std::flush;
      break;
    }
    // No Force Wins Found
    else {
      printf(" Evaluated at: %7d.", p.second);
      resettext();
      std::cout << std::flush;
      ++depth;
    }
  }
  printf("\n");
  start = 0;
  tt::clear();
  return best;
}

void launchAlphaBeta(const GameState* state, Move move, int moveIdx, int depth) {
  GameState newState = state->ApplyMove(move).Invert();
  thEvalsDone[moveIdx] = 0;
  S16 f = -negamax(&newState, depth - 1, -20000, 20000, epoch + 1, moveIdx);
  fBuf[moveIdx] = f;
}
void launchAlphaBetaItr(const GameState* state, Move* childMoves, int depth, int from, int to, int by) {
  for (int moveIdx = from; moveIdx < to; moveIdx += by) {
    launchAlphaBeta(state, childMoves[moveIdx], moveIdx, depth);
  }
}

pair<Move, S16> MyBestMoveAtDepthAB(const GameState* state, int depth) {
  S16 value = -20000;
  Move childMoves[MOVE_ARR_LEN];
  int nmoves = GetMoves(state, childMoves);
  std::sort(childMoves, childMoves + nmoves, hl::keyCmp);
  int bestIdx = 0;
  int numEq = 0;

  for (int moveIdx = 0; moveIdx < nmoves; ++moveIdx) {
    GameState newState = state->ApplyMove(childMoves[moveIdx]).Invert();
    thEvalsDone[moveIdx] = 0;
    S16 f = -negamax(&newState, depth - 1, -20000, 20000, epoch + 1, moveIdx);
    fBuf[moveIdx] = f;
    evalsDone += thEvalsDone[moveIdx];

    // ABORT
    if (f == 20000 || f == -20000) {
      return std::make_pair(Move(), f);
    }

    if (f > value) {
      value = f;
      bestIdx = moveIdx;
      numEq = 1;
    }
    // Uniformly select from moves with same evaluation
    else if (f == value && randomUniform01() * ++numEq < 1) {
      value = f;
      bestIdx = moveIdx;
    }
  }
  return std::make_pair(childMoves[bestIdx], value);
}

pair<Move, S16> MyBestMoveAtDepthABParallel(const GameState* state, int depth) {
  S16 value = -20000;
  Move childMoves[MOVE_ARR_LEN];
  int nmoves = GetMoves(state, childMoves);
  int bestIdx = 0;
  int numEq = 0;

  for (int moveIdx = 0; moveIdx < nmoves; ++moveIdx) {
    mtdfThreads[moveIdx] = (std::thread(launchAlphaBeta, state, childMoves[moveIdx], moveIdx, depth));
  }

  for (int moveIdx = 0; moveIdx < nmoves; ++moveIdx) {
    mtdfThreads[moveIdx].join();
    evalsDone += thEvalsDone[moveIdx];
    S16 f = fBuf[moveIdx];

    // ABORT
    if (f == 20000 || f == -20000) {
      while (++moveIdx < nmoves) {
        mtdfThreads[moveIdx].join();
        evalsDone += thEvalsDone[moveIdx];
      }
      return std::make_pair(Move(), f);
    }

    if (f > value) {
      value = f;
      bestIdx = moveIdx;
      numEq = 1;
    }
    // Uniformly select from moves with same evaluation
    else if (f == value && randomUniform01() * ++numEq < 1) {
      value = f;
      bestIdx = moveIdx;
    }
  }
  return std::make_pair(childMoves[bestIdx], value);
}

void launchMTDF(const GameState* state, Move move, int moveIdx, int depth) { 
  GameState newState = state->ApplyMove(move).Invert();
  S16 f = fBuf[moveIdx];
  thEvalsDone[moveIdx] = 0;
  f = -MTDF(&newState, -f, depth - 1, epoch + 1, moveIdx);
  fBuf[moveIdx] = f;
}
void launchBatchMTDF(const GameState* state, Move* childMoves, int depth, int from, int to, int by) {
  for (int moveIdx = from; moveIdx < to; moveIdx += by) {
    launchMTDF(state, childMoves[moveIdx], moveIdx, depth);
  }
}

pair<Move, S16> MyBestMoveAtDepthMTDF(const GameState* state, int depth) {
  S16 value = -20000;
  Move childMoves[MOVE_ARR_LEN];
  int nmoves = GetMoves(state, childMoves);
  int bestIdx = 0;
  int numEq = 0;

  for (int moveIdx = 0; moveIdx < nmoves; ++moveIdx) {
    GameState newState = state->ApplyMove(childMoves[moveIdx]).Invert();
    thEvalsDone[moveIdx] = 0;
    S16 f = fBuf[moveIdx];
    f = -MTDF(&newState, -f, depth - 1, epoch + 1, moveIdx);
    fBuf[moveIdx] = f;
    evalsDone += thEvalsDone[moveIdx];

    // ABORT
    if (f == 20000 || f == -20000) {
      return std::make_pair(Move(), f);
    }

    if (f > value) {
      value = f;
      bestIdx = moveIdx;
      numEq = 1;
    }
    // Uniformly select from moves with same evaluation
    else if (f == value && randomUniform01() * ++numEq < 1) {
      value = f;
      bestIdx = moveIdx;
    }
  }
  return std::make_pair(childMoves[bestIdx], value);
}

pair<Move, S16> MyBestMoveAtDepthMTDFParallel(const GameState* state, int depth) {
  S16 value = -20000;
  Move childMoves[MOVE_ARR_LEN];
  int nmoves = GetMoves(state, childMoves);
  int bestIdx = 0;
  int numEq = 0;

  for (int moveIdx = 0; moveIdx < nmoves; ++moveIdx) {
    mtdfThreads[moveIdx] = (std::thread(launchMTDF, state, childMoves[moveIdx], moveIdx, depth));
  }

  for (int moveIdx = 0; moveIdx < nmoves; ++moveIdx) {
    mtdfThreads[moveIdx].join();
    evalsDone += thEvalsDone[moveIdx];
    S16 f = fBuf[moveIdx];

    // ABORT
    if (f == 20000 || f == -20000) {
      while (++moveIdx < nmoves) {
        mtdfThreads[moveIdx].join();
        evalsDone += thEvalsDone[moveIdx];
      }
      return std::make_pair(Move(), f);
    }

    if (f > value) {
      value = f;
      bestIdx = moveIdx;
      numEq = 1;
    }
    // Uniformly select from moves with same evaluation
    else if (f == value && randomUniform01() * ++numEq < 1) {
      value = f;
      bestIdx = moveIdx;
    }
  }
  return std::make_pair(childMoves[bestIdx], value);
}

pair<Move, S16> MyBestMoveAtDepthBatchMTDFParallel(const GameState* state, int depth) {
  S16 value = -20000;
  Move childMoves[MOVE_ARR_LEN];
  int nmoves = GetMoves(state, childMoves);
  int bestIdx = 0;
  int numEq = 0;
  int nbatches = ((nmoves - 1) / 4) + 1;

  if (nmoves <= nbatches) {
    printf("Only %d moves! Resorting to Serial.\n", nmoves);
    return MyBestMoveAtDepthMTDF(state, depth);
  }

  for (int batchIdx = 0; batchIdx < nbatches && batchIdx < nmoves; ++batchIdx) {
    mtdfThreads[batchIdx] = (std::thread(launchBatchMTDF, state, childMoves, depth, batchIdx, nmoves, nbatches));
  }

  for (int batchIdx = 0; batchIdx < nbatches && batchIdx < nmoves; ++batchIdx) {
    mtdfThreads[batchIdx].join();
    for (int moveIdx = batchIdx; moveIdx < nmoves; moveIdx += nbatches) {
      evalsDone += thEvalsDone[moveIdx];
      S16 f = fBuf[moveIdx];

      // ABORT
      if (f == 20000 || f == -20000) {
        while (++batchIdx < nbatches) {
          mtdfThreads[batchIdx].join();
        }
        return std::make_pair(Move(), f);
      }

      if (f > value) {
        value = f;
        bestIdx = moveIdx;
        numEq = 1;
      }
      // Uniformly select from moves with same evaluation
      else if (f == value && randomUniform01() * ++numEq < 1) {
        value = f;
        bestIdx = moveIdx;
      }
    }
  }
  return std::make_pair(childMoves[bestIdx], value);
}

//https://www.chessprogramming.org/MTD(f)#C_Pseudo_Code
S16 MTDF(const GameState* state, S16 f, int depth, int epoch, int thIdx) {
  S16 bound[2] = { -20000, +20000 };
  do {
    //printf("MTDF: { %7d, %7d }\n          ", bound[0], bound[1]);
    S16 beta = (f >= (bound[0] + 1)) ? f : (bound[0] + 1);
    f = negamaxWithMemory(state, depth, beta - 1, beta, epoch, thIdx);
    bound[f < beta] = f;
  } while (f != 20000 && f != -20000 && bound[0] < bound[1]);
  return f;
}

using tt::TTRec;

// https://en.wikipedia.org/wiki/Negamax
// https://en.wikipedia.org/wiki/Negamax#Negamax_with_alpha_beta_pruning
// https://webdocs.cs.ualberta.ca/~tony/OldPapers/icca.Mar1986.pp3-18.pdf
S16 negamax(const GameState* state, int depth, S16 alpha, S16 beta, int epoch, int thIdx) {
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

  const auto p = tt::getValue(state);
  if (p.first == state->hashCode) {
    const auto ttrec = p.second;
    if (ttrec.depth >= depth && ttrec.bound == tt::Bound::EXACT) {
      return ttrec.val;
    }
    // Theodore's highly questionable suggestion.
    //alpha = ttrec.val;
  }

  if (depth <= 0) {
    thEvalsDone[thIdx]++;
    return evaluate(state);
  }

  S16 value = -20000;
  S16 oldAlpha = alpha;
  Move childMoves[MOVE_ARR_LEN];
  int bestMoveIdx = 0;
  int nmoves;

  nmoves = GetMoves(state, childMoves);
  std::sort(childMoves, childMoves + nmoves, hl::keyCmp);

  if (nmoves == 0) {
    std::cerr << "WARN: Umm this shouldn't be possible! Err: 012938";
  }
  for (signed char idx = 0; idx < nmoves; ++idx) {
    Move move = childMoves[idx];
    GameState newState = state->ApplyMove(move).Invert();
    S16 nm = -negamax(&newState, depth-1, -beta, -alpha, epoch + 1, thIdx);
    // ABORT
    if (nm == 20000 || nm == -20000) {
      return nm;
    }
    if (nm > value) {
      value = nm;
      bestMoveIdx = idx;
    }
    alpha = (alpha > value) ? alpha : value;
    if (alpha >= beta) {
      hl::incrementTable(move, 2000);
      break;
    }
  }
  if (20000 > value && value > -20000L) {
    hl::incrementTable(childMoves[bestMoveIdx], 10);
    if (depth > 5) {
      tt::Bound bound = tt::Bound::EXACT;
      if (value <= oldAlpha) {
        bound = tt::Bound::UPPER;
      }
      if (value >= beta) {
        bound = tt::Bound::LOWER;
      }
      tt::setValue(state, TTRec( value, depth, childMoves[bestMoveIdx], bound ));
    }
  }
  return value;
}

// http://people.csail.mit.edu/plaat/mtdf.html#abmem
S16 negamaxWithMemory(const GameState* state, int depth, S16 alpha, S16 beta, int epoch, int thIdx) {
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

  const auto p = tt::getValue(state);
  if (p.first == state->hashCode) {
    const auto ttrec = p.second;
    if (ttrec.lowerBound > beta) {
      return ttrec.lowerBound;
    }
    else if (ttrec.upperBound <= alpha) {
      return ttrec.upperBound;
    }
    alpha = (alpha > ttrec.lowerBound) ? alpha : ttrec.lowerBound;
    beta = (beta < ttrec.upperBound) ? beta : ttrec.upperBound;
  }

  Move childMoves[MOVE_ARR_LEN];
  int bestMoveIdx = 0;
  S16 value;

  if (depth <= 0) {
    thEvalsDone[thIdx]++;
    value = evaluate(state);
  }
  else {
    value = -20000;
    S16 oldAlpha = alpha;

    int nmoves = GetMoves(state, childMoves);
    std::sort(childMoves, childMoves + nmoves, hl::keyCmp);

    if (nmoves == 0) {
      std::cerr << "WARN: Umm this shouldn't be possible! Err: 012938";
    }
    for (signed char idx = 0; idx < nmoves && value < beta; ++idx) {
      Move move = childMoves[idx];
      GameState newState = state->ApplyMove(move).Invert();
      S16 nm = -negamaxWithMemory(&newState, depth-1, -beta, -alpha, epoch + 1, thIdx);
      // ABORT
      if (nm == 20000 || nm == -20000) {
        return nm;
      }
      if (nm > value) {
        value = nm;
        bestMoveIdx = idx;
      }
      alpha = (alpha > value) ? alpha : value;
    }
    alpha = oldAlpha;
  }
  if (20000 > value && value > -20000) {
    hl::incrementTable(childMoves[bestMoveIdx], 100);
    if (depth > 2) {
      // Fail Low
      if (value <= alpha) {
        tt::setValue(state, TTRec(value, -20000, value, depth, childMoves[bestMoveIdx], tt::Bound::UPPER));
      }
      // Fail High
      else if (value >= beta){
        tt::setValue(state, TTRec(value, value, 20000, depth, childMoves[bestMoveIdx], tt::Bound::LOWER));
      }
      // Accurate Value
      else {
        tt::setValue(state, TTRec(value, value, value, depth, childMoves[bestMoveIdx], tt::Bound::EXACT));
      }
    }
  }
  return value;
}


