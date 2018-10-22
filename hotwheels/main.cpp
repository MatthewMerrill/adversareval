#include <iostream>
#include <thread>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "ansi.hpp"
#include "bitscan.hpp"
#include "game.hpp"
#include "minimax.hpp"
#include "movegen.hpp"
#include "transpositiontbl.hpp"
#include "zobrist.hpp"

#ifdef NN_EVAL
#include "nn.hpp"
#endif

#include "ui.hpp"

void clearExceptHeader() {
  std::cout << "\x1b[12;1H" << "\x1B[0J";
}

void enableAltScreen() {
  //std::cout << "\x1B[?1049h";
}

void disableAltScreen() {
  //std::cout << "\x3B[?1049lThank you for playing!" << std::endl;
}

int humanVsHuman();
int humanVsComputer();
int computerVsComputer();

int main() {

// https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
#ifdef _WIN32
  // Set output mode to handle virtual terminal sequences
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
#ifdef CHECK_HANDLE
  /*
  if (hOut == INVALID_HANDLE_VALUE) {
    return GetLastError();
  }*/
#endif
  DWORD dwMode = 0;
  if (!GetConsoleMode(hOut, &dwMode)) {
#ifdef CHECK_HANDLE
    /*
    return GetLastError();
    */
#endif
  }
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  if (!SetConsoleMode(hOut, dwMode)) {
#ifdef CHECK_HANDLE
    /*
    return GetLastError();
    */
#endif
  }
#endif

  ui::displayAll();
  int mode;
  std::cout << "What mode are you playing in?" << std::endl
    << " 1 - Human Vs Human" << std::endl
    << " 2 - Human Vs Computer" << std::endl
    << " 3 - Computer Vs Computer" << std::endl;
  std::cin >> mode;
  switch (mode) {
    //case 0: return humanVsHuman();
    case 2: return humanVsComputer();
    case 3: return computerVsComputer();
    default:
      std::cout << "Unknown mode!" << std::endl;
      return 1;
  }
}
 
int humanVsComputer() {
  tt::init();
#ifdef NN_EVAL
  nn::loadGraph();
#endif
  ui::displayAll();

  enableAltScreen();
  atexit(disableAltScreen);

  GameState state = GameState();
  int turn;
  ui::displayAll();

  textattr(28);
  std::cout << "Are you going first (0)? or am I (1)? 0/1: ";
  std::cin >> turn;

  if (turn != 0 && turn != 1) {
    std::cout << "Invalid option... I'll assume you don't want to play." << std::endl;
    return 0;
  }

  char input[4];
  Move moves[MOVE_ARR_LEN];
  Move move;
  int winner;
  std::thread ponderThread;
  epoch = 0;

  for (;;) {
    epoch += 1;
    winner = state.GetWinner();
    if (winner) {
      if (turn) {
        if (move.fromIdx > -1) {
          std::cout << "I'm moving to: ";
          move.Print();
          std::cout << " (";
          move.Invert().Print();
          std::cout << ")";
          std::cin.ignore();
          std::cin.ignore();
        }
      }
      ui::displayAll();
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
      ui::displayAll();
      std::cout << "Computer is thinking... " << std::endl;
      StopPondering();
      if (ponderThread.joinable()) {
        ponderThread.join();
      }
      GameState inv = state.Invert();
      move = MyBestMove(&inv).Invert();
      ui::prevState = state;
      state = state.ApplyMove(move);
      ui::curState = state;
      ui::historyVector.push_back(move);
    } else {
      ponderThread = std::thread(Ponder, state.Invert());
      if (move.fromIdx > -1) {
        std::cout << "I'm moving to: ";
        move.Print();
        std::cout << " (";
        move.Invert().Print();
        std::cout << ")";
        std::cin.ignore();
        std::cin.ignore();
      }
      ui::displayAll();
      std::cout << "Moves available: " << std::endl << "[";
      int nmoves = GetMoves(&state, moves);
      for (int i = 0; i < nmoves; ++i) {
        if (i % 11 == 0) {
          std::cout << std::endl;
        }
        std::cout << "  ";
        moves[i].Print();
        std::cout << ",";
      }
      std::cout << std::endl << "]" << std::endl;
      //std::cout << "Please wait while I clean house... " << std::endl;
      //tt::cleanup(&state);
      //std::cout << "all clean!" << std::endl;
      for (;;) {
        std::cout << "Where would you like to move?" << std::endl;
        std::cin >> input;
        Move inputMove = Move(input[1]-'1', input[0]-'A', input[3]-'1', input[2]-'A');
        //Move inputMove = MyBestMoveAtDepth(&state, 5);
        if (IsValidMove(&state, inputMove)) { 
          move = inputMove;
          ui::prevState = state;
          state = state.ApplyMove(move);
          ui::curState = state;
          ui::historyVector.push_back(move);
          break;
        }
        else {
          std::cout << "Invalid Move!" << std::endl;
        }
      }
    }
    turn ^= 1;
  }
#ifdef NN_EVAL
  nn::unloadGraph();
#endif

  return 0;
}

int computerVsComputer() {
  tt::init();
#ifdef NN_EVAL
  nn::loadGraph();
#endif
  ui::displayAll();

  enableAltScreen();
  atexit(disableAltScreen);

  GameState state;
  int turn;

  textattr(28);
  Move move;
  int winner;
  int wins = 0;

  for (int gameIdx = 0; gameIdx < 20; ++gameIdx) {
    state = GameState();
    turn = 0;
    epoch = 0;
    ui::displayAll();
    ui::historyVector.clear();
    tt::clear();
    for (;;) {
      epoch += 1;
      winner = state.GetWinner();
      if (winner) {
        ui::displayAll();
        std::cout << "Computer 0 has won " << wins << "/" << gameIdx << " games. " << std::endl;
        std::cout << "We have a winner!" << std::endl;
        if (winner == -1) {
          wins++;
        }
        break;
      }
      if (!turn) {
        ui::displayAll();
        std::cout << "Computer 0 has won " << wins << "/" << gameIdx << " games. " << std::endl;
        std::cout << "Computer 0 is thinking... " << std::endl;
        move = MyBestMove(&state);
        ui::prevState = state;
        state = state.ApplyMove(move);
        ui::curState = state;
        ui::historyVector.push_back(move);
        std::cout << "Computer 0 moving to: ";
        move.Print();
        std::cout << " (";
        move.Invert().Print();
        std::cout << ")";
      } else {
        ui::displayAll();
        std::cout << "Computer 0 has won " << wins << "/" << gameIdx << " games. " << std::endl;
        std::cout << "Computer 1 is thinking... " << std::endl;
        GameState inv = state.Invert();
        move = MyBestMove(&inv).Invert();
        ui::prevState = state;
        state = state.ApplyMove(move);
        ui::curState = state;
        ui::historyVector.push_back(move);
        std::cout << "Computer 1 moving to: ";
        move.Print();
        std::cout << " (";
        move.Invert().Print();
        std::cout << ")";
      }
      //if (ui::historyVector.size() % 8 == 7) {
        std::cout << std::endl << "Please wait while I clean house... " << std::endl;
        tt::cleanup(&state);
        std::cout << "all clean!" << std::endl;
      //}
      turn ^= 1;
    }
  }
  return 0;
}

