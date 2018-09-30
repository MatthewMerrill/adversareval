#include <iostream>

#include "minimax.hpp"
#include "evaluate.hpp"

using namespace std;

int main() {
  cout << "TicTacToe!" << endl;
  GameState state;
  state.Print();

  Move move = Move(1, 0);
  int computerTeam = 1;
  while (true) {
    if (state.pieces == 511) {
      cout << "Cat's Game!" << endl;
      break;
    }
    if (move.team == -computerTeam) {
      cin >> move.pos;
      move.pos -= 1;
    } else {
      move = MyBestMove(&state, 12, computerTeam);
      cout << "Computer moved: " << (move.pos + 1) << endl;
    }
    if (!state.IsValidMove(move)) {
      cout << "Invalid: " << move.pos << ". You lose!" << endl;
      break;
    }
    state = state.ApplyMove(move);
    cout << endl;
    state.Print();
    if (state.XWins()) {
      cout << "X wins!" << endl;
      break;
    }
    if (state.OWins()) {
      cout << "O wins!" << endl;
      break;
    }
    move.team *= -1;
  }
  return 0;
}

