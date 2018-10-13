#include <stdio.h>
#include <cstdlib>

#include "montecarlo.hpp"

#ifdef __linux__
// TODO: find source stackoverflow
static unsigned long long rdtsc() {
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

int main() {
  srand(rdtsc());
  char s[128];
  GameState state;
  for (int i = 0; i < 175; ++i) {
    state = MCSelectRoot(GameState());
    state.Serialize(s);
    //state.Print();
    printf("%s %.3f\n", s, MCWinProb(state, 7000, 5));
    fflush(stdout);
  }
  return 0;
}




