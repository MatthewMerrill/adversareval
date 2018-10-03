#include <limits>
#include <list>
#include <chrono>

#include <iostream>
#include <stdio.h>

#include "game.hpp"
#include "movegen.hpp"
#include "evaluate.hpp"
#include "minimax.hpp"

float negamax(const GameState* state, int depth);
float negamax(const GameState* state, int depth, float alpha, float beta);

U64 evalsDone = 0;
time_t start;

static float randomUniform01() {
  // https://stackoverflow.com/a/9879024/3188059
  return ((float) rand() / (RAND_MAX));
}

Move MyBestMove(const GameState* state) {
  // Seed with current time https://stackoverflow.com/a/14914657/3188059
  srand(time(NULL));
  int depth = 2;
  start = time(NULL); 
  Move m;
  while (time(NULL) - start < 5) {
    evalsDone = 0;
    m = MyBestMoveAtDepth(state, depth);
    std::cout << "Performed " << evalsDone << " evals at depth "
      << depth << "." << std::endl;
    ++depth;
    if (evalsDone == 0) {
      break;
    }
  }
  return m;
}

Move MyBestMoveAtDepth(const GameState* state, int depth) {
  float value = -1;
  Move best;
  std::vector<Move> childMoves = GetMoves(state);
  if (childMoves.size() == 1) {
    return childMoves.at(0);
  }
  int numEq = 0;
  for (const Move move : childMoves) {
    GameState newState = state->ApplyMove(move).Invert();
    float nm = -negamax(&newState, depth-1, -1, +1);
    //move.Invert().Print();
    //printf("\t%.3f\t%.3f\n", nm, value);
    if (nm > value) {
      value = nm;
      best = move;
      numEq = 1;
    }
    // Uniformly select from moves with same evaluation
    else if (nm == value && randomUniform01() * numEq < 1) {
      value = nm;
      best = move;
      numEq += 1;
    }
  }
  return best;
}

// https://en.wikipedia.org/wiki/Negamax
// https://en.wikipedia.org/wiki/Negamax#Negamax_with_alpha_beta_pruning
float negamax(const GameState* state, int depth, float alpha, float beta) {
  if (depth == 0 || time(NULL) - start > 5) {
    evalsDone++;
    return evaluate(state);
  }
  int winner = state->GetWinner();
  if (winner != 0) {
    return winner;
  }
  std::vector<Move> childMoves = GetMoves(state);
  float value = -1;
  if (childMoves.size() == 0) {
    std::cerr << "WARN: Umm this shouldn't be possible! Err: 012938";
  }
  for (const Move move : childMoves) {
    GameState newState = state->ApplyMove(move).Invert();
    float nm = -negamax(&newState, depth-1, -beta, -alpha);
    value = (nm > value) ? nm : value;
    alpha = (alpha > value) ? alpha : value;
    if (alpha >= beta) {
      break;
    }
  }
  return value;
}

