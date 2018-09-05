// Author: Matthew Merrill <mattmerr.com>

#include "bitboard.hpp"

bool Bitboard::IsEmpty() {
  return state == 0;
}

Bitboard Bitboard::Union(Bitboard bb) {
  return { state | bb.state };
}

Bitboard Bitboard::Intersect(Bitboard bb) {
  return { state & bb.state };
}

Bitboard Bitboard::Complement() {
  return { ~state };
}

// https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating#FlipVertically
Bitboard Bitboard::FlipVertical() {
  const U64 k1 = 0x00FF00FF00FF00FF;
  const U64 k2 = 0x0000FFFF0000FFFF;
  U64 x = state;
  x = ((x >>  8) & k1) | ((x & k1) <<  8);
  x = ((x >> 16) & k2) | ((x & k2) << 16);
  x = ( x >> 32)       | ( x       << 32);
  return {x};
}

// https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating#MirrorHorizontally
Bitboard Bitboard::FlipHorizontal() {
  const U64 k1 = 0x5555555555555555;
  const U64 k2 = 0x3333333333333333;
  const U64 k4 = 0x0f0f0f0f0f0f0f0f;
  U64 x = state;
  x = ((x >> 1) & k1) +  2*(x & k1);
  x = ((x >> 2) & k2) +  4*(x & k2);
  x = ((x >> 4) & k4) + 16*(x & k4);
  return { x };
}

// https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating#Diagonal
Bitboard Bitboard::FlipDiagonal() {
  U64 t;
  U64 x = state;
  const U64 k1 = 0x5500550055005500;
  const U64 k2 = 0x3333000033330000;
  const U64 k4 = 0x0f0f0f0f00000000;
  t  = k4 & (x ^ (x << 28));
  x ^=       t ^ (t >> 28) ;
  t  = k2 & (x ^ (x << 14));
  x ^=       t ^ (t >> 14) ;
  t  = k1 & (x ^ (x <<  7));
  x ^=       t ^ (t >>  7) ;
  return { x };
}

Bitboard Bitboard::ShiftRows(int rows /* = 1 */) {
  return { state >> (rows << 3) };
}
