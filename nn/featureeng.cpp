#include <stdio.h>
#include "game.hpp"

int main() {

  char ser[200];
  GameState state;
  while (scanf("%[^\n]s\n", ser) > 0) {
    state.Serialize(ser);
    printf("%s\n", ser);
  }
  GameState();

}
