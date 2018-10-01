#include <iostream>
#include <stdlib.h>

#include "ansi.hpp"
#include "prog.hpp"
#include "game.hpp"
#include "minimax.hpp"
#include "movegen.hpp"

const char* hotwheelschess[] {
  "* * * **** ** ********  ***    *** **********",
  "**** * * * ****** ** *  **    *  ***** ** ** ",
  "* ** * * **** **  *  *   **   *  * **   ** **",
  "* * *  * **** *************    *** **********"
};


void clearExceptHeader() {
  std::cout << "\x1b[12;1H" << "\x1B[0J";
}

void enableAltScreen() {
  std::cout << "\x1B[?1049h";
}

void disableAltScreen() {
  std::cout << "\x1B[?1049lThank you for playing!" << std::endl;
}

int main() {
  enableAltScreen();
  atexit(disableAltScreen);
  GameState state = GameState();
  int turn;

  std::cout << "\x1B[2J";
  //std::cout << adversareval3d << std::endl;
  printGooglyHeader(adversarevalBlocks, 3 * 12, "\u2593", 80);
  std::cout << std::endl;
  printHeader(hotwheelschess, 3 * 15, RED + 30, YELLOW + 30, "\u2593", 80);
  textattr(28);
  std::cout << std::endl << std::endl << "Are you going first (0)? or am I (1)? 0/1: ";
  std::cin >> turn;
  std::cout << std::endl;

  char input[4];
  Move* move = NULL;
  int winner;

  for (;;) {
    winner = state.GetWinner();
    if (winner) {
      clearExceptHeader();
      if (move != NULL) {
        state.PrintHighlighting(move);
        std::cout << "Computer moved: ";
        move->Print();
        std::cout << " (";
        move->Invert().Print();
        std::cout << ")"<< std::endl;
      }
      else {
        state.Print();
      }
      std::cout << "We have a winner!" << std::endl;
      if (winner == 1) {
        std::cout << "You won!" << std::endl;
      }
      else if (winner == -1) {
        std::cout << "I won!" << std::endl;
      }
      else {
        std::cout << "Somebody wins!" << std::endl;
      }
      std::cin.ignore();
      std::cin.ignore();
      break;
    }
    if (turn) {
      clearExceptHeader();
      if (move == NULL) {
        state.Print();
        move = new Move();
      }
      else {
        state.PrintHighlighting(move);
        std::cout << "You moved: ";
        move->Print();
        std::cout << std::endl;
      }
      std::cout << "Computer is thinking... " << std::endl;
      std::cout << "\x1B[25;1H" << std::flush;
      //ProgBar prog; // TODO: fix
      GameState inv = state.Invert();
      *move = MyBestMove(&inv).Invert();
      state = state.ApplyMove(*move);
      //prog.ForceStop();
    } else {
      clearExceptHeader();
      if (move == NULL) {
        state.Print();
        move = new Move();
      }
      else {
        state.PrintHighlighting(move);
        std::cout << "Computer moved: ";
        move->Print();
        std::cout << " (";
        move->Invert().Print();
        std::cout << ")"<< std::endl;
      }
      std::cout << "Moves available: " << std::endl << "[";
      std::vector<Move> moves = GetMoves(&state);
      for (size_t i = 0; i < moves.size(); ++i) {
        if (i % 8 == 0) {
          std::cout << std::endl;
        }
        std::cout << "  ";
        moves.at(i).Print();
        std::cout << ",";
      }
      std::cout << std::endl << "]" << std::endl;

      for (;;) {
        std::cout << "Where would you like to move?" << std::endl;
        std::cin >> input;
        Move inputMove = Move(input[1]-'1', input[0]-'A', input[3]-'1', input[2]-'A');
        if (IsValidMove(&state, inputMove)) { 
          *move = inputMove;
          state = state.ApplyMove(*move);
          break;
        }
        else {
          std::cout << "Invalid Move!" << std::endl;
        }
      }
    }
    turn ^= 1;
  }

  return 0;
}
