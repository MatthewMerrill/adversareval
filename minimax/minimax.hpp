#ifndef MINIMAX_H
#define MINIMAX_H

#include <cassert>
#include <utility>

#include "game.hpp"
#include "evaluate.hpp"

void Ponder(const GameState state);
void StopPondering();
Move MyBestMove(const GameState* state);
std::pair<Move, S16> MyBestMoveAtDepth(const GameState* state, int depth);

#endif

