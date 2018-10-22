#ifndef BITSCAN_HPP
#define BITSCAN_HPP

#ifdef __linux__
static inline unsigned long bitscanll(unsigned long long l) {
  return __builtin_ffsll(l);
}
static inline unsigned int popcount(unsigned long long l) {
  return __builtin_popcountll(l);
}

#elif _WIN32
#include <intrin.h>
static inline unsigned long bitscanll(unsigned __int64 l) {
  unsigned long idx;
  if (_BitScanForward64(&idx, l)) {
    return (unsigned long) (idx + 1);
  }
  else {
    return 0;
  }
}

static inline unsigned int popcount(unsigned __int64 l) {
  return (unsigned int) __popcnt64(l);
}

#else
#include <iostream>
static unsigned long bitscanll(unsigned long long l) {
  std::cout << "AGH IDK THIS PLATFORM" << std::endl;
  return 0;
}
static unsigned long popcount(unsigned long long l) {
  std::cout << "AGH IDK THIS PLATFORM" << std::endl;
  return 0;
}
#endif

#endif
