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

#endif

