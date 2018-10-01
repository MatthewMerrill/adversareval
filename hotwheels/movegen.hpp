#include "game.hpp"

#include <vector>

std::vector<Move> GetMoves(const GameState* state);
bool IsValidMove(const GameState* state, Move move);

