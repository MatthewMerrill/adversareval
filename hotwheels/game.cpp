#include "game.hpp"
#include "zobrist.hpp"

GameState* testRoot = NULL;
unsigned long long testSeed = 1;

U64 HashCode(const GameState* state) {
  return tt::hash(state);
}

signed char pieceTypeAt(const GameState* state, int idx) {
  U64 bit = 1ULL << idx;
  int type = 0; // no piece
  type += 1 * ((state->cars    & bit) >> idx);
  type += 2 * ((state->knights & bit) >> idx);
  type += 3 * ((state->bishops & bit) >> idx);
  type += 4 * ((state->rooks   & bit) >> idx);
  type += 5 * ((state->pawns   & bit) >> idx);
  return type;
}

using namespace tt;

U64 HashCode(const Move move, signed char fromType, signed char capType) {
  U64 hash = 0;
  hash ^= ZOBRIST_KEYS[fromType * 56 + move.fromIdx];
  hash ^= ZOBRIST_KEYS[fromType * 56 + move.toIdx];
  hash ^= ZOBRIST_KEYS[capType * 56 + move.toIdx];
  return hash;
}

