#include <iostream>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "ansi.hpp"

int main() {
#ifdef _WIN32
  // https://stackoverflow.com/q/38772468/3188059
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD dwMode = 0;
  GetConsoleMode(hOut, &dwMode);
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(hOut, dwMode);
#endif
  std::cout << "\x1B[1;1H" << std::flush;
  std::cout << "memes" << std::endl;
  printf("%c[%dmmemes\n", 27, 30 + RED);
}

