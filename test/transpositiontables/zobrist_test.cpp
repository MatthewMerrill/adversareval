#include "gtest/gtest.h"

#include <iostream>
#include <random>

#include "game.hpp"
#include "minimax.hpp"
#include "transpositiontbl.hpp"
#include "transpositiontables/zobrist.hpp"
/*
TEST(TestZobrist, NullSeed) {
  unsigned long long keys[4][4];
  unsigned long long* keysPtrs[] = { keys[0], keys[1], keys[2], keys[3] };
  tt::generateZobristKeysFromSeed((unsigned long long**) keysPtrs, 4, 4, 0);

  unsigned long long expected[4][4] = { 
    {4506900286868959208ULL, 7356933424542005313ULL, 17530672337659192010ULL, 3183792233013426327ULL },
    {10066397631202075120ULL, 11459919534738703185ULL, 2068225597421408603ULL, 153267590404614367ULL },
    {11101330631099477340ULL, 16605519166088731469ULL, 3116564106530927807ULL, 17412714314399620169ULL },
    {4178459793395517324ULL, 13646826735959316505ULL, 5929795176404408539ULL, 1924162175263052706ULL }
  };
  for (int r = 0; r < 4; ++r) {
    //std::cout << "{";
    for (int c = 0; c < 4; ++c) {
      //std::cout << keys[r][c] << ((c == 3) ? "ULL }" : "ULL, ");
      EXPECT_EQ(expected[r][c], keys[r][c]);
    }
    //std::cout << ((r == 3) ? "" : ",") << std::endl;
  }
}*/

TEST(TestRunningHash, InitialHashes) {
  GameState initialState = GameState();
  EXPECT_EQ(4563749063352816831ULL, initialState.hashCode);
  EXPECT_EQ(4563749063352816831ULL, initialState.invHashCode);
}

TEST(TestRunningHash, AfterAMove) {
  GameState initialState = GameState();
  Move carUp = Move(0, 7);
  GameState afterMove = initialState.ApplyMove(carUp);
  GameState afterInvMove = initialState.ApplyMove(carUp.Invert());
  EXPECT_EQ(0, afterMove.hashCode - afterInvMove.invHashCode);
  EXPECT_EQ(0, afterMove.invHashCode - afterInvMove.hashCode);
}

TEST(TestRunningHash, AfterAnotherMove) {
  GameState initialState = GameState();
  Move carUp = Move(0, 7);
  GameState afterMove = initialState.ApplyMove(carUp);
  GameState afterInvMove = initialState.ApplyMove(carUp.Invert()).Invert();
  EXPECT_EQ(0, afterMove.hashCode - afterInvMove.hashCode);
  EXPECT_EQ(0, afterMove.invHashCode - afterInvMove.invHashCode);
}

/*
TEST(TestTable, BasicMap) {
  GameState state;
  tt::setValue(&state, {{MMRet::WIN}, 16});
  MMRet exp = {MMRet::WIN};
  MMRet act = tt::getValue(&state).val;
  EXPECT_EQ(exp, act);
  EXPECT_EQ(16, tt::getValue(&state).depth);
}*/

