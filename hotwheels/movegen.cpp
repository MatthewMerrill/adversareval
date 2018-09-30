#include <iostream>

#include "movegen.hpp"

U64 MovesForKnight(const GameState* state, U64 pos) {
  U64 positions = 0;
  // See https://go.mattmerr.com/bitboardhex
  // Backwards Attacks
  positions |= (pos & 0x3e7cf9f3e7cf80) >> -(-1*7 + 2);
  positions |= (pos & 0xf9f3e7cf9f3e00) >> -(-1*7 - 2);
  positions |= (pos & 0x7efdfbf7efc000) >> -(-2*7 + 1);
  positions |= (pos & 0xfdfbf7efdf8000) >> -(-2*7 - 1);
  positions &= (state->pieces & state->teams);

  positions |= (pos & 0x007cf9f3e7cf9f) << (+1*7 + 2);
  positions |= (pos & 0x01f3e7cf9f3e7c) << (+1*7 - 2);
  positions |= (pos & 0x0001fbf7efdfbf) << (+2*7 + 1);
  positions |= (pos & 0x0003f7efdfbf7e) << (+2*7 - 1);
  positions &= ~(state->pieces & ~state->teams);
  positions &= ~state->cars;
  return positions;
}

U64 MovesForPawn(const GameState* state, U64 pos) {
  U64 positions = 0;
  // See https://go.mattmerr.com/bitboardhex
  // Pawn Attacks
  positions |= (pos & 0x00fdfbf7efdfbf) << (1*7 + 1);
  positions |= (pos & 0x01fbf7efdfbf7e) << (1*7 - 1);
  positions &= (state->pieces & state->teams);

  positions |= ((pos & 0x01ffffffffffff) << (1*7)) & ~(state->pieces);
  positions &= ~(state->pieces & ~state->teams);
  positions &= ~state->cars;
  return positions;
}

#define SHIFT(x, d) ((d) >= 0) ? ((x) << (d)) : ((x) >> -(d))

/**
 * @param delta amount to shift by
 */
U64 ProjectFwd(const GameState* state, U64 pos, U64 mask, U64 delta) {
  U64 positions = 0;
  // See https://go.mattmerr.com/bitboardhex
  positions |= (((positions & ~(state->pieces)) | pos) & mask) << delta;
  positions |= (((positions & ~(state->pieces)) | pos) & mask) << delta;
  positions |= (((positions & ~(state->pieces)) | pos) & mask) << delta;
  positions |= (((positions & ~(state->pieces)) | pos) & mask) << delta;
  positions |= (((positions & ~(state->pieces)) | pos) & mask) << delta;
  positions |= (((positions & ~(state->pieces)) | pos) & mask) << delta;
  positions |= (((positions & ~(state->pieces)) | pos) & mask) << delta;
  positions |= (((positions & ~(state->pieces)) | pos) & mask) << delta;
  positions &= ~(state->pieces & ~state->teams);
  positions &= ~state->cars;
  return positions;
}

U64 ProjectBwd(const GameState* state, U64 pos, U64 mask, U64 delta) {
  U64 positions = 0;
  // See https://go.mattmerr.com/bitboardhex
  positions |= (((positions & ~(state->pieces)) | pos) & mask) >> delta;
  positions |= (((positions & ~(state->pieces)) | pos) & mask) >> delta;
  positions |= (((positions & ~(state->pieces)) | pos) & mask) >> delta;
  positions |= (((positions & ~(state->pieces)) | pos) & mask) >> delta;
  positions |= (((positions & ~(state->pieces)) | pos) & mask) >> delta;
  positions |= (((positions & ~(state->pieces)) | pos) & mask) >> delta;
  positions |= (((positions & ~(state->pieces)) | pos) & mask) >> delta;
  positions |= (((positions & ~(state->pieces)) | pos) & mask) >> delta;
  positions &= ~(state->pieces & ~state->teams);
  positions &= ~state->cars;
  return positions;
}

U64 MovesForRook(const GameState* state, U64 pos) {
  // See https://go.mattmerr.com/bitboardhex
  U64 positions = 0;
  // Attacks
  positions |= ProjectFwd(state, pos, 0x7efdfbf7efdfbf, 1);
  positions |= ProjectBwd(state, pos, 0xfdfbf7efdfbf7e, 1);
  positions |= ProjectBwd(state, pos, 0xffffffffffff80, 7);
  positions &= (state->pieces & state->teams);

  positions |= ProjectFwd(state, pos, 0x01ffffffffffff, 7);
  positions &= ~state->cars;
  return positions;
}

U64 MovesForBishop(const GameState* state, U64 pos) {
  // See https://go.mattmerr.com/bitboardhex
  U64 positions = 0;
  // Attacks
  positions |= ProjectBwd(state, pos, 0xfdfbf7efdfbf00, 8);
  positions |= ProjectBwd(state, pos, 0x7efdfbf7efdf80, 6);
  positions &= (state->pieces & state->teams);

  positions |= ProjectFwd(state, pos, 0x00fdfbf7efdfbf, 8);
  positions |= ProjectFwd(state, pos, 0x01fbf7efdfbf7e, 6);
  positions &= ~state->cars;
  return positions;
}

void AppendMoves(int fr, int fc, U64 moves, std::vector<Move>* ls) {
  U64 bit = 1l;
  for (int r = 0; r < 8; ++r) {
    for (int c = 0; c < 7; ++c) {
      if (moves & bit) {
        ls->push_back(Move(fr, fc, r, c));
      }
      bit <<= 1;
    }
  }
}

const int CAR_POSITIONS[] = {
  0, 1, 2, 3, 3, 3, 3,
};

void MaybeAppendCarMove(
    std::vector<Move>* moves,
    const GameState* state, int c) {
  if (c < 6 && !(state->pieces & (1 << (CAR_POSITIONS[c+1]*7 + c+1)))) {
    moves->push_back(Move(CAR_POSITIONS[c], c, CAR_POSITIONS[c+1], c+1));
  }
}

std::vector<Move> GetMoves(const GameState* state) {
  std::vector<Move> moves;
  U64 bit = 1l;
  U64 teamPieces = state->pieces & ~state->teams;
  for (int r = 0; r < 8; ++r) {
    for (int c = 0; c < 7; ++c) {
      if (teamPieces & bit) {
        if (bit & state->knights) {
          AppendMoves(r, c, MovesForKnight(state, bit), &moves);
        }
        else if (bit & state->pawns) {
          AppendMoves(r, c, MovesForPawn(state, bit), &moves);
        }
        else if (bit & state->rooks) {
          AppendMoves(r, c, MovesForRook(state, bit), &moves);
        }
        else if (bit & state->bishops) {
          AppendMoves(r, c, MovesForBishop(state, bit), &moves);
        }
        else if (bit & state->cars) {
          MaybeAppendCarMove(&moves, state, c);
        }
        else {
          std::cerr << "Uh idk" << std::endl;
        }
      }
      bit <<= 1;
    }
  }
  return moves;
}


int maina() {
  GameState* state = new GameState();
  *state = state->ApplyMove(Move(2, 4, 6, 5));
  std::vector<Move> moves = GetMoves(state);
  std::cout << "[";
  for (const Move move : moves) {
    move.Print();
    std::cout << ",";
  }
  std::cout << "]" << std::endl;
  state->Print();
  *state = state->Invert();
  moves = GetMoves(state);
  std::cout << "[";
  for (const Move move : moves) {
    move.Invert().Print();
    std::cout << ",";
  }
  std::cout << "]" << std::endl;
  return 0;
}

