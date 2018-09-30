#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <list>
#include <stdio.h>

#include "ansi.hpp"

#define U64 unsigned long long

struct Move {
  int fromRow;
  int fromCol;
  int toRow;
  int toCol;

  Move() {}

  Move(int fr, int fc, int tr, int tc) :
    fromRow(fr),
    fromCol(fc),
    toRow(tr),
    toCol(tc) {}

  void Print() const {
    printf("%c%c%c%c", fromCol + 'A', fromRow + '1', toCol + 'A', toRow + '1');
  }

  Move Invert() const {
    return Move(7 - fromRow, fromCol, 7 - toRow, toCol);
  }
};

static U64 FlipVert(U64 state) {
  const U64 k1 = 0x01fc07f01fc07f;
  const U64 k2 = 0x0003fff0003fff;
  U64 x = state;
  x = ((x >>  7) & k1) | ((x & k1) <<  7);
  x = ((x >> 14) & k2) | ((x & k2) << 14);
  x = ( x >> 28)       | ( x       << 28);
  return x;
}

struct GameState {
  
  U64 pieces;
  U64 teams;
  U64 cars;
  U64 knights;
  U64 bishops;
  U64 rooks;
  U64 pawns;

  GameState():
    pieces(0b00000010000010001111111110111111011001111100000100000001),
    teams(0b00000010000010001111111110110000000000000000000000000000),
    cars(0b00000010000000000000000000000000000000000000000000000001),
    knights(0b00000000000000000000000000110000011000000000000000000000),
    bishops(0b00000000000000001100000000000000000001100000000000000000),
    rooks(0b00000000000000000001100000000000000000001100000000000000),
    pawns(0b00000000000010000010011110001111000000010000000100000000) {
      if (pieces != (cars | knights | bishops | rooks | pawns)) {
        std::cerr << "WARN! Pieces Board != BitwiseOR of piece boards!" << std::endl;
        PrintComponents();
      }
    }

  GameState(U64 pc, U64 t, U64 c, U64 k, U64 b, U64 r, U64 p)
    : pieces(pc), teams(t), cars(c), knights(k), bishops(b), rooks(r), pawns(p) {
      if (pieces != (cars | knights | bishops | rooks | pawns)) {
        std::cerr << "WARN! Pieces Board != BitwiseOR of piece boards!" << std::endl;
        PrintComponents();
      }
    }

  void Print() const {
    PrintHighlighting(NULL);
  }

  void PrintHighlighting(Move* move) const {
    std::cout << "   --------------------- ";
    textfg(RED);
    std::cout << "Computer";
    textattr(RESET);
    std::cout << "\n";
    for (int r = 7; r >= 0; --r) {
      std::cout << " " << (r + 1) << " ";
      for (int c = 0; c < 7; ++c) {
        if (1&(r^c)) {
          textcolor(
              BRIGHT,
              (teams & (1l<<(r*7+c))) ? RED : BLUE,
              BLACK
              );
        }
        else {
          textcolor(
              RESET,
              (teams & (1l<<(r*7+c))) ? RED : BLUE,
              WHITE
              );
        }
        PrintChar(r, c);
      }
      resettext();
      std::cout << " |" << std::endl;
    }
    std::cout << "   --------------------- ";
    textfg(BLUE);
    std::cout << "Human";
    textattr(RESET);
    std::cout << std::endl << "    A  B  C  D  E  F  G" << std::endl << std::endl;
  }

  void PrintChar(int r, int c) const {
    U64 bit = 1l << (r * 7 + c);
    if (cars & bit) {
      std::cout << ((teams & bit) ? "_C_" : ".C.");
    }
    else if (knights & bit) {
      std::cout << ((teams & bit) ? "_N_" : ".N.");
    }
    else if (bishops & bit) {
      std::cout << ((teams & bit) ? "_B_" : ".B.");
    }
    else if (rooks & bit) {
      std::cout << ((teams & bit) ? "_R_" : ".R.");
    }
    else if (pawns & bit) {
      std::cout << ((teams & bit) ? "_P_" : ".P.");
    }
    else {
      std::cout << "   ";
    }
  }

  bool IsValidMove(Move move) const {
    return true;
  }

  #define ApplyMoveToPieceBoard(board) \
    (((board & ~toBit & ~fromBit) | ((board & fromBit) ? toBit : 0)))

  GameState ApplyMove(Move move) const {
    U64 fromBit = 1l << (move.fromRow*7 + move.fromCol);
    U64 toBit = 1l << (move.toRow*7 + move.toCol);
    return GameState(
      ((this->pieces & ~fromBit) | toBit),
      ApplyMoveToPieceBoard(this->teams),
      ApplyMoveToPieceBoard(this->cars),
      ApplyMoveToPieceBoard(this->knights),
      ApplyMoveToPieceBoard(this->bishops),
      ApplyMoveToPieceBoard(this->rooks),
      ApplyMoveToPieceBoard(this->pawns)
    );
  }

  void PrintComponents() const {
    std::cout << "PIECES:" << std::endl;
    PrintBoard(pieces);
    std::cout << "TEAMS:" << std::endl;
    PrintBoard(teams);
    std::cout << "CARS:" << std::endl;
    PrintBoard(cars);
    std::cout << "KNIGHTS:" << std::endl;
    PrintBoard(knights);
    std::cout << "BISHOPS:" << std::endl;
    PrintBoard(bishops);
    std::cout << "ROOKS:" << std::endl;
    PrintBoard(rooks);
    std::cout << "PAWNS:" << std::endl;
    PrintBoard(pawns);
  } 
  void PrintBoard(U64 board) const {
    for (int r = 0; r < 8; ++r) {
      for (int c = 0; c < 7; ++c) {
        std::cout << ((board & (1l << (r*7 + c))) ? 1 : 0);
      }
      std::cout << std::endl;
    }
  }

  int GetWinner() const {
    if (cars & (1l << 34)) {
      return 1;
    }
    else if (cars & (1l << 41)) {
      return -1;
    }
    return 0;
  }

  GameState Invert() const {
    return GameState(
      FlipVert(this->pieces),
      (~FlipVert(this->teams)) & FlipVert(this->pieces),
      FlipVert(this->cars),
      FlipVert(this->knights),
      FlipVert(this->bishops),
      FlipVert(this->rooks),
      FlipVert(this->pawns)
    );
  }
};

#endif

