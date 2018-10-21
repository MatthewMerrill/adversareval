#include "game.hpp"
#include "zobrist.hpp"

GameState* testRoot = NULL;
unsigned long long testSeed = 1;

U64 HashCode(const GameState* state) {
  return tt::hash(state);
}

using namespace tt;

U64 HashCode(const GameState* state, Move move) {
  U64 fromBit = 1ULL << move.fromIdx;
  U64 toBit = 1ULL << move.toIdx;
  U64 hash = state->hashCode;
  hash ^= (state->teams   & fromBit) ? ZOBRIST_KEYS[1 * 56 + move.fromIdx] : 0;
  hash ^= (state->cars    & fromBit) ? ZOBRIST_KEYS[2 * 56 + move.fromIdx] : 0;
  hash ^= (state->knights & fromBit) ? ZOBRIST_KEYS[3 * 56 + move.fromIdx] : 0;
  hash ^= (state->bishops & fromBit) ? ZOBRIST_KEYS[4 * 56 + move.fromIdx] : 0;
  hash ^= (state->rooks   & fromBit) ? ZOBRIST_KEYS[5 * 56 + move.fromIdx] : 0;
  hash ^= (state->pawns   & fromBit) ? ZOBRIST_KEYS[6 * 56 + move.fromIdx] : 0;
  hash ^= (state->teams   & toBit) ? ZOBRIST_KEYS[1 * 56 + move.toIdx] : 0;
  hash ^= (state->cars    & toBit) ? ZOBRIST_KEYS[2 * 56 + move.toIdx] : 0;
  hash ^= (state->knights & toBit) ? ZOBRIST_KEYS[3 * 56 + move.toIdx] : 0;
  hash ^= (state->bishops & toBit) ? ZOBRIST_KEYS[4 * 56 + move.toIdx] : 0;
  hash ^= (state->rooks   & toBit) ? ZOBRIST_KEYS[5 * 56 + move.toIdx] : 0;
  hash ^= (state->pawns   & toBit) ? ZOBRIST_KEYS[6 * 56 + move.toIdx] : 0;
  return hash;
}

