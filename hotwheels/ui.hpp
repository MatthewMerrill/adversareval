#include <stdio.h>
#include <vector>

#include "game.hpp"

#ifdef __WIN32
#define adversareval_googly_txt_len 852
#else
#define adversareval_googly_txt_len 1064
#endif
extern unsigned char adversareval_googly_txt[adversareval_googly_txt_len];

namespace ui {

  extern char boardView[13][47];
  extern std::vector<Move> historyVector;

  extern GameState prevState;
  extern GameState curState;

  void displayAll();
  void updateStateView();
}

