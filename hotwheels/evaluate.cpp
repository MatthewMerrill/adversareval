#include <iostream>

#include "bitscan.hpp"
#include "evaluate.hpp"

U64 myTrack = 0x0000000f010101ULL;
U64 theirTrack = 0x02082780000000ULL;

//*
signed short evaluate(const GameState* state) {
  U64 myCar = state->cars & ((state->cars - 1) << 1);
  U64 theirCar = state->cars - myCar;
  U64 blockingMine = state->pieces & myTrack & ~(((myCar - 1) << 1) | 1);
  U64 blockingTheirs = state->pieces & theirTrack & ~(theirCar - 1);
  unsigned long myCarIdx = bitscanll(myCar);
  unsigned long theirCarIdx = bitscanll(theirCar);

  U64 bit = 1;
  signed short eval = 0;
  int idx; 
  U64 pieces = state->pieces;
  while ((idx = bitscanll(pieces))) {
    bit = 1ULL << (idx-1);
    pieces ^= bit;
    if (state->teams & bit) {
      if (blockingMine & bit) {
        eval -= 5;
      }
      if (blockingTheirs & bit) {
        eval += 3;
      }
      if (state->cars & bit) {
        eval -= 6 * (idx % 7);
      }
      eval -= 1;
    }
    else {
      if (blockingTheirs & bit) {
        eval += 5;
      }
      if (blockingMine &= bit) {
        eval -= 3;
      }
      if (state->cars & bit) {
        eval += 6 * (idx % 7);
      }
      eval += 1;
    }
  }
  return eval;
}
/*/
//static Move idkWhichMoves[MOVE_ARR_LEN];
signed short evaluate(const GameState* state) {
  U64 myCar = state->cars & ((state->cars - 1) << 1);
  U64 theirCar = state->cars - myCar;
  U64 blockingMine = state->pieces & myTrack & ~(((myCar - 1) << 1) | 1);
  U64 blockingTheirs = state->pieces & theirTrack & ~(theirCar - 1);
  unsigned long myCarIdx = bitscanll(myCar);
  unsigned long theirCarIdx = bitscanll(theirCar);
  //
  //int nMyMoves = GetMoves(state, idkWhichMoves); // TODO: better to give them 1 move that's not their 1 car move
  //int nTheirMoves = GetMoves(state, idkWhichMoves);
  return 20*(myCarIdx%7)
    + 20 * popcount(blockingTheirs)
    + 10 * popcount(blockingTheirs & ~state->teams)
    + popcount(state->pieces & ~state->teams)

    - 21*(theirCarIdx%7)
    - 20 * popcount(blockingMine)
    - 10 * popcount(blockingMine & state->teams)
    - popcount(state->pieces & state->teams);
}
//*/

