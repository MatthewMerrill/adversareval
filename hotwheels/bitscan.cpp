#ifdef __linux__
unsigned long bitscanll(unsigned long long l) {
  return __builtin_ffsll(l);
}

#elif _WIN32
#include <intrin.h>
unsigned long bitscanll(unsigned __int64 l) {
  unsigned long idx;
  if (_BitScanForward64(&idx, l)) {
    return idx + 1;
  }
  else {
    return 0;
  }
}

#else
#include <iostream>
unsigned long bitscanll(unsigned long long l) {
  std::cout << "AGH IDK THIS PLATFORM" << std::endl;
  return 0;
}
#endif
