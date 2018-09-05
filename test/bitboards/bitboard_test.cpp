#include "gtest/gtest.h"

#include "bitboards/bitboard.hpp"

TEST(TestBitboard, Union) {
  Bitboard a = { 1 }, b = { 2 };
  EXPECT_EQ(a.Union(a).state, 1u);
  EXPECT_EQ(a.Union(b).state, 3u);
  EXPECT_EQ(b.Union(a).state, 3u);
  EXPECT_EQ(b.Union(b).state, 2u);

  Bitboard c = { 0b1010 }, d = { 0b0101 };
  EXPECT_EQ(c.Union(d).state, 15u);
  EXPECT_EQ(d.Union(c).state, 15u);

  Bitboard e = { 0b1110 }, f = { 0b0111 };
  EXPECT_EQ(e.Union(f).state, 15u);
  EXPECT_EQ(f.Union(e).state, 15u);
}

TEST(TestBitboard, Intersect) {
  Bitboard a = { 1 }, b = { 2 };
  EXPECT_EQ(a.Intersect(a).state, 1u);
  EXPECT_EQ(a.Intersect(b).state, 0u);
  EXPECT_EQ(b.Intersect(a).state, 0u);
  EXPECT_EQ(b.Intersect(b).state, 2u);

  Bitboard c = { 0b1010 }, d = { 0b0101 };
  EXPECT_EQ(c.Intersect(d).state, 0u);
  EXPECT_EQ(d.Intersect(c).state, 0u);

  Bitboard e = { 0b1110 }, f = { 0b0111 };
  EXPECT_EQ(e.Intersect(f).state, 6u);
  EXPECT_EQ(f.Intersect(e).state, 6u);
}

TEST(TestBitboard, Complement) {
  Bitboard EMPTY = { 0 }, FULL = { 0xFFFFFFFFFFFFFFFF };
  EXPECT_EQ(EMPTY.Complement().state, 0xFFFFFFFFFFFFFFFFu);
  EXPECT_EQ(FULL.Complement().state, 0u);

  Bitboard a = { 0xF0F0F0F0F0F0F0F0 }, b = { 0x0F0F0F0F0F0F0F0F };
  EXPECT_EQ(a.Complement().state, b.state);
}

TEST(TestBitboard, FlipVertical) {
  Bitboard EMPTY = { 0 }, FULL = { 0xFFFFFFFFFFFFFFFF };
  EXPECT_EQ(EMPTY.FlipVertical().state, 0u);
  EXPECT_EQ(FULL.FlipVertical().state, 0xFFFFFFFFFFFFFFFFu);

  Bitboard a = { 0x1122334455667788 }, b = { 0x0123456789ABCDEF };
  EXPECT_EQ(a.FlipVertical().state, 0x8877665544332211u);
  EXPECT_EQ(b.FlipVertical().state, 0xEFCDAB8967452301u);
}

TEST(TestBitboard, FlipHorizontal) {
  Bitboard EMPTY = { 0 }, FULL = { 0xFFFFFFFFFFFFFFFF };
  EXPECT_EQ(EMPTY.FlipHorizontal().state, 0u);
  EXPECT_EQ(FULL.FlipHorizontal().state, 0xFFFFFFFFFFFFFFFFu);

  Bitboard a = { 0x1122334455667788 }, b = { 0x0123456789ABCDEF };
  EXPECT_EQ(a.FlipHorizontal().state, 0x8844CC22AA66EE11u);
  EXPECT_EQ(b.FlipHorizontal().state, 0x80C4A2E691D5B3F7u);
}

TEST(TestBitboard, FlipDiagonal) {
  Bitboard EMPTY = { 0 }, FULL = { 0xFFFFFFFFFFFFFFFF };
  EXPECT_EQ(EMPTY.FlipHorizontal().state, 0u);
  EXPECT_EQ(FULL.FlipHorizontal().state, 0xFFFFFFFFFFFFFFFFu);
  // TODO: More testing
}

TEST(TestBitboard, ShiftRows) {
  Bitboard a = { 0xAAFF00 };
  EXPECT_EQ(a.ShiftRows( ).state, 0xAAFFu);
  EXPECT_EQ(a.ShiftRows(1).state, 0xAAFFu);
  EXPECT_EQ(a.ShiftRows(2).state, 0xAAu);
}
