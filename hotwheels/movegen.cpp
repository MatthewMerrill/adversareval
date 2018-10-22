#include <iostream>
#include <stdio.h>

#include "bitscan.hpp"
#include "movegen.hpp"


/*
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
*/
