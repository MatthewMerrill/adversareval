#ifndef EVALUATE_H
#define EVALUATE_H

#include "game.hpp"

float evaluate(const GameState* gameState);
GameState* newGameState();
GameState applyMove(GameState gameState, Move m);
GameState negate(const GameState gameState);

#endif


