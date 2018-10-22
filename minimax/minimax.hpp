#ifndef MINIMAX_H
#define MINIMAX_H

#include <cassert>
#include <utility>

#include "game.hpp"
#include "evaluate.hpp"

extern int epoch;

void Ponder(const GameState state);
void StopPondering();
Move MyBestMove(const GameState* state);
std::pair<Move, S16> MyBestMoveAtDepth(const GameState* state, int depth,
                                       S16 alpha, S16 beta);

std::pair<Move, S16> MyBestMoveAtDepthMTDF(const GameState* state, int depth);
S16 MTDF(const GameState* state, S16 f, int depth, int epoch);

#endif

