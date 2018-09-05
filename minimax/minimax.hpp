#ifndef MINIMAX_H
#define MINIMAX_H

#include "evaluate.hpp"

float MinimaxMin(const GameState* state, int depth);
float MinimaxMax(const GameState* state, int depth);

#endif

