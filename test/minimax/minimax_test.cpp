#include "gtest/gtest.h"

#include "minimax/minimax.hpp"

float evaluate(const GameState* state) {
  return .42;
}

TEST(TestMinimax, Eval) {
  EXPECT_FLOAT_EQ(0.42, MinimaxMin(NULL));
  EXPECT_FLOAT_EQ(0.58, MinimaxMax(NULL));
}
