#include "gtest/gtest.h"

#include "minimax/minimax.hpp"

float evaluate() {
  return .42;
}

TEST(TestMinimax, Eval) {
  EXPECT_FLOAT_EQ(0.42, MinimaxMin());
  EXPECT_FLOAT_EQ(0.58, MinimaxMax());
}
