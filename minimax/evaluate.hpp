#ifndef EVALUATE_H
#define EVALUATE_H

struct GameState;
struct Move {
  const int from;
  const int to;
};

float evaluate(const GameState* gameState);
GameState* newGameState();
GameState* applyMove();

#endif

