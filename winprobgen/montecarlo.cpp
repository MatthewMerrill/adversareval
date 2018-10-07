#include <iostream>
#include <limits>
#include <stdlib.h>
#include <vector>

#include "game.hpp"
#include "minimax.hpp"
#include "movegen.hpp"

#ifdef __linux__
unsigned long long rdtsc() {
  unsigned int lo,hi;
  __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
  return ((unsigned long long)hi << 32) | lo;
}
#elif _WIN32
#include <windows.h>
static unsigned long long rdtsc() {
  return __rdtsc();
}
#else
static unsigned long long rdtsc() {
  printf("I don't know this platform! Err: 298374");
  return 0;
}
#endif

static float randomUniform01() {
  // https://stackoverflow.com/a/9879024/3188059
  return ((float) rand() / (RAND_MAX));
}

GameState MCSelectRoot(GameState state) {
  int depth = 1;
  GameState ret = state;
  while (state.GetWinner() == 0) {
    std::vector<Move> moves = GetMoves(&state);
    Move chosenMove = moves.at(rand() % moves.size());
    state = state.ApplyMove(chosenMove);
    if (randomUniform01() * depth < 1) {
      ret = state;
    }
    state = state.Invert();
    depth += 1;
  }
  return ret;
}

int MCWinProbTrial(GameState state, int searchDepth) {
  testRoot = &state;
  while (state.GetWinner() == 0) {
    //state.PrintComponents();
    Move chosenCpuMove = MyBestMoveAtDepth(&state, searchDepth);
    //printf("\nCPU: ");
    //chosenCpuMove.Print();
    //printf("\nAfter CPU Move:");
    state = state.ApplyMove(chosenCpuMove);
    //state.Print();
    if (state.GetWinner()) {
      break;
    }
    state = state.Invert();
    Move chosenHumMove = MyBestMoveAtDepth(&state, searchDepth);
    //printf("\nHUM: ");
    //chosenHumMove.Invert().Print();
    state = state.ApplyMove(chosenHumMove);
    state = state.Invert();
    //printf("\nAfter CPU Move:");
    //state.Print();
  }
  return (state.GetWinner() == 1) ? 1 : 0;
}

int mainc() {
  char c[4];
  GameState state = GameState("a1d00800028181a0 a0100800028080a0 100080000000100 2040000000000020 8010000000000080 8000 80000002810000");
  unsigned long long seed = 301695153011306ULL;
  srand(seed);
  MCWinProbTrial(state, 3);
  return 0;
}

float MCWinProb(GameState root, int numTrials, int searchDepth) {
  int wonTrials = 0;
  testRoot = &root;
  for (int trial = 0; trial < numTrials; ++trial) {
    testSeed = rdtsc();
    srand(testSeed);
    wonTrials += MCWinProbTrial(root, searchDepth);
  }
  return wonTrials * 1.0f / numTrials;
}
