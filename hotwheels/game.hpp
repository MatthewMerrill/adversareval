#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <list>
#include <stdio.h>

#include "ansi.hpp"
#include "bitscan.hpp"

#define U64 unsigned long long

static U64 FlipVert(U64 state) {
  const U64 k1 = 0x01fc07f01fc07fULL;
  const U64 k2 = 0x0003fff0003fffULL;
  U64 x = state;
  x = ((x >>  7) & k1) | ((x & k1) <<  7);
  x = ((x >> 14) & k2) | ((x & k2) << 14);
  x = ( x >> 28)       | ( x       << 28);
  return x;
}

struct Move {

  U64 from;
  U64 to;

  Move() {}

  Move(int fr, int fc, int tr, int tc) {
    from = 1ULL << (fr*7 + fc);
    to = 1ULL << (tr*7 + tc);
  }

  Move(U64 f, U64 t): from(f), to(t) {}

  int FromRow() const {
    return (bitscanll(from)-1) / 7;
  }
  int FromCol() const {
    return (bitscanll(from)-1) % 7;
  }

  int ToRow() const {
    return (bitscanll(to)-1) / 7;
  }
  int ToCol() const {
    return (bitscanll(to)-1) % 7;
  }

  void Print() const {
    printf("%c%c%c%c",
        FromCol() + 'A', FromRow() + '1',
        ToCol() + 'A', ToRow() + '1');
  }

  Move Invert() const {
    return Move(7 - FromRow(), FromCol(), 7 - ToRow(), ToCol());
  }
};

struct GameState {
  
  U64 pieces;
  U64 teams;
  U64 cars;
  U64 knights;
  U64 bishops;
  U64 rooks;
  U64 pawns;

  GameState():
    pieces(0x208ffbf67c101ULL),
    teams(0x208ffb0000000ULL),
    cars(0x2000000000001ULL),
    knights(0x30600000ULL),
    bishops(0xc000060000ULL),
    rooks(0x180000c000ULL),
    pawns(0x8278f010100ULL) {
      std::cout << std::endl << std::endl << bitscanll(12) << std::endl;
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
              (teams & (1ULL<<(r*7+c))) ? RED : BLUE,
              BLACK
              );
        }
        else {
          textcolor(
              RESET,
              (teams & (1ULL<<(r*7+c))) ? RED : BLUE,
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
    U64 bit = 1ULL << (r * 7 + c);
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

  #define ApplyMoveToPieceBoard(board) \
    (((board & ~toBit & ~fromBit) | ((board & fromBit) ? toBit : 0)))

  GameState ApplyMove(Move move) const {
    U64 fromBit = move.from;
    U64 toBit = move.to;
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
        std::cout << ((board & (1ULL << (r*7 + c))) ? 1 : 0);
      }
      std::cout << std::endl;
    }
  }

  int GetWinner() const {
    if (cars & (1ULL << 27)) {
      return 1;
    }
    else if (cars & (17179869184l)) {
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

