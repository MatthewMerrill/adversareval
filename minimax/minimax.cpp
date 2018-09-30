#include <limits>
#include <list>

#include <iostream>

#include "game.hpp"
#include "movegen.hpp"
#include "evaluate.hpp"
#include "minimax.hpp"

float negamax(const GameState* state, int depth);
float negamax(const GameState* state, int depth, float alpha, float beta);

Move MyBestMove(const GameState* state, int depth) {
  float value = -1;
  Move best;
  std::vector<Move> childMoves = GetMoves(state);
  for (const Move move : childMoves) {
    GameState newState = state->ApplyMove(move);
    float nm = -negamax(&newState, depth-1, -1, +1);
    //std::cout << move.pos + 1 << "\t" << nm << "\t" << value << std::endl;
    if (nm >= value) {
      value = nm;
      best = move;
    }
  }
  return best;
}

// https://en.wikipedia.org/wiki/Negamax
float negamax(const GameState* state, int depth, float alpha, float beta) {
  if (depth == 0) {
    return evaluate(state);
  }
  int winner = state->GetWinner();
  if (winner != 0) {
    return winner;
  }
  std::vector<Move> childMoves = GetMoves(state);
  float value = -1;
  if (childMoves.size() == 0) {
    return 0;
  }
  for (const Move move : childMoves) {
    GameState newState = state->ApplyMove(move);
    float nm = -negamax(&newState, depth-1, -beta, -alpha);
    value = nm > value ? nm : value;
    if (alpha >= beta) {
      //break;
    }
  }
  return value;
}

