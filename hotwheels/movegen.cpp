#include <iostream>
#include <stdio.h>

#include "bitscan.hpp"
#include "movegen.hpp"

U64 MovesForKnight(const GameState* state, U64 pos) {
  U64 positions = 0;
  // See https://go.mattmerr.com/bitboardhex
  // Backwards Attacks
  positions |= (pos & 0x3e7cf9f3e7cf80ULL) >> -(-1*7 + 2);
  positions |= (pos & 0xf9f3e7cf9f3e00ULL) >> -(-1*7 - 2);
  positions |= (pos & 0x7efdfbf7efc000ULL) >> -(-2*7 + 1);
  positions |= (pos & 0xfdfbf7efdf8000ULL) >> -(-2*7 - 1);
  positions &= (state->pieces & state->teams);

  positions |= (pos & 0x007cf9f3e7cf9fULL) << (+1*7 + 2);
  positions |= (pos & 0x01f3e7cf9f3e7cULL) << (+1*7 - 2);
  positions |= (pos & 0x0001fbf7efdfbfULL) << (+2*7 + 1);
  positions |= (pos & 0x0003f7efdfbf7eULL) << (+2*7 - 1);
  positions &= ~(state->pieces & ~state->teams);
  positions &= ~state->cars;
  return positions;
}

U64 MovesForPawn(const GameState* state, U64 pos) {
  U64 positions = 0;
  // See https://go.mattmerr.com/bitboardhex
  // Pawn Attacks
  positions |= (pos & 0x00fdfbf7efdfbfULL) << (1*7 + 1);
  positions |= (pos & 0x01fbf7efdfbf7eULL) << (1*7 - 1);
  positions &= (state->pieces & state->teams);

  positions |= ((pos & 0x01ffffffffffffULL) << (1*7)) & ~(state->pieces);
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
  positions |= ProjectFwd(state, pos, 0x7efdfbf7efdfbfULL, 1);
  positions |= ProjectBwd(state, pos, 0xfdfbf7efdfbf7eULL, 1);
  positions |= ProjectBwd(state, pos, 0xffffffffffff80ULL, 7);
  positions &= (state->pieces & state->teams);

  positions |= ProjectFwd(state, pos, 0x01ffffffffffffULL, 7);
  positions &= ~state->cars;
  return positions;
}

U64 MovesForBishop(const GameState* state, U64 pos) {
  // See https://go.mattmerr.com/bitboardhex
  U64 positions = 0;
  // Attacks
  positions |= ProjectBwd(state, pos, 0xfdfbf7efdfbf00ULL, 8);
  positions |= ProjectBwd(state, pos, 0x7efdfbf7efdf80ULL, 6);
  positions &= (state->pieces & state->teams);

  positions |= ProjectFwd(state, pos, 0x00fdfbf7efdfbfULL, 8);
  positions |= ProjectFwd(state, pos, 0x01fbf7efdfbf7eULL, 6);
  positions &= ~state->cars;
  return positions;
}

void AppendMoves(U64 bit, U64 moves, std::vector<Move>* ls) {
  /*
  U64 bit = 1l;
  for (int r = 0; r < 8; ++r) {
    for (int c = 0; c < 7; ++c) {
      if (moves & bit) {
        ls->push_back(Move(fr, fc, r, c));
      }
      bit <<= 1;
    }
  }//*/
  int idx; 
  while ((idx = bitscanll(moves))) {
    moves ^= 1ULL << (idx - 1);
    ls->push_back(Move(bit, 1ULL << (idx-1)));
  }
}

const int CAR_POSITIONS[] = {
  0, 1, 2, 3, 3, 3, 3,
};

void MaybeAppendCarMove(
    std::vector<Move>* moves,
    const GameState* state, int c) {
  if (c < 6 && !(state->pieces & (1ULL << (CAR_POSITIONS[c+1]*7 + c+1)))) {
    //moves->push_back(Move(CAR_POSITIONS[c], c, CAR_POSITIONS[c+1], c+1));
    moves->emplace(moves->begin(), Move(CAR_POSITIONS[c], c, CAR_POSITIONS[c+1], c+1));
  }
}

std::vector<Move> GetMoves(const GameState* state) {
  //std::cout << "FINDING MOVES FOR \n";
  //state->Print();
  std::vector<Move> moves;
  U64 bit = 0;
  U64 teamPieces = state->pieces & ~state->teams;
  int carcol = -1;
  int idx; 
  while ((idx = bitscanll(teamPieces))) {
    //printf("%llx", teamPieces);
    //std::cout << "PIECEAT:" << (idx-1) << std::endl;
    bit = 1ULL << (idx - 1);
    teamPieces ^= bit;
    if (bit & state->knights) {
      AppendMoves(bit, MovesForKnight(state, bit), &moves);
    }
    else if (bit & state->pawns) {
      AppendMoves(bit, MovesForPawn(state, bit), &moves);
    }
    else if (bit & state->rooks) {
      AppendMoves(bit, MovesForRook(state, bit), &moves);
    }
    else if (bit & state->bishops) {
      AppendMoves(bit, MovesForBishop(state, bit), &moves);
    }
    else if (bit & state->cars) {
      int c = (idx - 1) % 7;
      MaybeAppendCarMove(&moves, state, c);
      carcol = c;
      //std::cout << "ITSA CAR:" << std::endl;
    }
    else {
      std::cerr << "Uh idk Err: 23434580" << std::endl;
    }
  }
  //std::cout << "CARCOL:" << carcol << std::endl;
  if (moves.size() == 0) {
    moves.push_back(
      Move(
        CAR_POSITIONS[carcol], carcol,
        CAR_POSITIONS[carcol+1], carcol+1));
  }
  return moves;
}


bool IsValidMove(const GameState* state, Move move) {
  std::vector<Move> moves = GetMoves(state);
  for (const Move m : moves) {
    if (m.from == move.from && m.to == move.to) {
      return true;
    }
  }
  return false;
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

