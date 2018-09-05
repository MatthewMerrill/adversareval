#ifndef MINIMAX_H
#define MINIMAX_H

#include "minimax/evaluate.hpp"

float MinimaxMin(const GameState* state, int depth);
float MinimaxMax(const GameState* state, int depth);

#endif

