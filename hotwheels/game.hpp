#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <list>
#include <stdio.h>

#include "ansi.hpp"
#include "bitscan.hpp"

#define U64 unsigned long long
#define MOVE_ARR_LEN 48

static U64 FULL_BOARD = (1ULL << 56) - 1;
static U64 FlipVert(U64 state) {
  const U64 k1 = 0x01fc07f01fc07fULL;
  const U64 k2 = 0x0003fff0003fffULL;
  U64 x = state;
  x = ((x >>  7) & k1) | ((x & k1) <<  7);
  x &= FULL_BOARD;
  x = ((x >> 14) & k2) | ((x & k2) << 14);
  x &= FULL_BOARD;
  x = ( x >> 28)       | ( x       << 28);
  x &= FULL_BOARD;
  return x;
}

struct Move {

  signed char fromIdx = -1;
  signed char toIdx = -1;

  Move(): fromIdx(-1), toIdx(-1) {}

  Move(int fr, int fc, int tr, int tc) {
    fromIdx = (fr*7 + fc);
    toIdx = (tr*7 + tc);
  }

  Move(signed char f, signed char t): fromIdx(f), toIdx(t) {
#ifndef UNSAFE_MODE
    if (f > 55 || t > 55) {
      printf("INVALID MOVE! f:%d t:%d\n", f, t);
    }
#endif
  }

  inline int FromRow() const {
    return fromIdx / 7;
  }
  inline int FromCol() const {
    return fromIdx % 7;
  }
  inline U64 FromBit() const {
    return 1ULL << fromIdx;
  }

  inline int ToRow() const {
    return toIdx / 7;
  }
  inline int ToCol() const {
    return toIdx % 7;
  }
  inline U64 ToBit() const {
    return 1ULL << toIdx;
  }

  inline void Print() const {
    printf("%c%c%c%c",
        FromCol() + 'A', FromRow() + '1',
        ToCol() + 'A', ToRow() + '1');
  }

  inline Move Invert() const {
    return Move(7 - FromRow(), FromCol(), 7 - ToRow(), ToCol());
  }

  inline bool operator==(Move move) {
    return fromIdx == move.fromIdx && toIdx == move.toIdx;
  }
};

struct GameState;

extern GameState* testRoot;
extern unsigned long long testSeed;

U64 HashCode(const GameState* state);
U64 HashCode(const GameState* old, Move move);

struct GameState {
  
  GameState const* prev = NULL;
  Move moveToHere;
  union {
    U64 bitBoards[7];
    struct {
      U64 pieces;
      U64 teams;
      union {
        U64 typeBoards[5];
        struct {
          U64 cars;
          U64 knights;
          U64 bishops;
          U64 rooks;
          U64 pawns;
        };
      };
    };
  };
  U64 hashCode;
#ifndef UNSAFE_MODE
  bool valid = true;
#endif

#define ApplyLMoveToBoard(board, fromBit, diff) \
  ((board & ~fromBit & ~toBit) | ((board & fromBit) ? toBit : 0))
#define ApplyRMoveToBoard(board, fromBit, diff) \
  ((board & ~fromBit & ~toBit) | ((board & fromBit) ? toBit : 0))

  GameState(const GameState* state, Move move) {
    prev = state;
    moveToHere = move;
    U64 fromBit = 1ULL << move.fromIdx;
    U64 toBit = 1ULL << move.toIdx;
    pieces = (state->pieces & ~fromBit) | toBit;
    if (move.toIdx >= move.fromIdx) {
      //int diff = move.toIdx - move.fromIdx;
      teams = ApplyLMoveToBoard(state->teams, fromBit, diff);
      cars = ApplyLMoveToBoard(state->cars, fromBit, diff);
      knights = ApplyLMoveToBoard(state->knights, fromBit, diff);
      bishops = ApplyLMoveToBoard(state->bishops, fromBit, diff);
      rooks = ApplyLMoveToBoard(state->rooks, fromBit, diff);
      pawns = ApplyLMoveToBoard(state->pawns, fromBit, diff);
    }
    else {
      //int diff = move.fromIdx - move.toIdx;
      teams = ApplyRMoveToBoard(state->teams, fromBit, diff);
      cars = ApplyRMoveToBoard(state->cars, fromBit, diff);
      knights = ApplyRMoveToBoard(state->knights, fromBit, diff);
      bishops = ApplyRMoveToBoard(state->bishops, fromBit, diff);
      rooks = ApplyRMoveToBoard(state->rooks, fromBit, diff);
      pawns = ApplyRMoveToBoard(state->pawns, fromBit, diff);
    }
#ifndef UNSAFE_MODE
    if (pieces != (cars | knights | bishops | rooks | pawns)) {
      std::cerr << "WARN! Pieces Board != BitwiseOR of piece boards!" << std::endl;
      PrintComponents();
      valid = false;
    }
    if (pieces >= (1ULL << 56)) {
      std::cerr << "WARN! Pieces Off Board!" << std::endl;
      valid = false;
    }
#endif
    hashCode = HashCode(this, move);
  }

  GameState():
    pieces(0x208ffbf67c101ULL),
    teams(0x208ffb0000000ULL),
    cars(0x2000000000001ULL),
    knights(0x30600000ULL),
    bishops(0xc000060000ULL),
    rooks(0x180000c000ULL),
    pawns(0x8278f010100ULL) {
#ifndef UNSAFE_MODE
      if (pieces != (cars | knights | bishops | rooks | pawns)) {
        std::cerr << "WARN! Pieces Board != BitwiseOR of piece boards!" << std::endl;
        PrintComponents();
        valid = false;
      }
      if (pieces >= (1ULL << 56)) {
        std::cerr << "WARN! Pieces Off Board!" << std::endl;
        valid = false;
      }
#endif
      hashCode = HashCode(this);
    }

  GameState(U64 pc, U64 t, U64 c, U64 k, U64 b, U64 r, U64 p)
    : pieces(pc), teams(t), cars(c), knights(k), bishops(b), rooks(r), pawns(p) {
#ifndef UNSAFE_MODE
      if (pieces != (cars | knights | bishops | rooks | pawns)) {
        std::cerr << "WARN! Pieces Board != BitwiseOR of piece boards!" << std::endl;
        PrintComponents();
        valid = false;
      }
      if (pieces >= (1ULL << 56)) {
        std::cerr << "WARN! Pieces Off Board!" << std::endl;
        valid = false;
      }
#endif
      hashCode = HashCode(this);
  }

  GameState(U64 pc, U64 t, U64 c, U64 k, U64 b, U64 r, U64 p, U64 hc)
    : pieces(pc), teams(t), cars(c), knights(k), bishops(b), rooks(r), pawns(p), hashCode(hc) {
#ifndef UNSAFE_MODE
      if (pieces != (cars | knights | bishops | rooks | pawns)) {
        std::cerr << "WARN! Pieces Board != BitwiseOR of piece boards!" << std::endl;
        PrintComponents();
        valid = false;
      }
      if (pieces >= (1ULL << 56)) {
        std::cerr << "WARN! Pieces Off Board!" << std::endl;
        valid = false;
      }
#endif
  }

  GameState(char* serialized) {
    sscanf(serialized, "%llx %llx %llx %llx %llx %llx %llx",
        &pieces, &teams, &cars, &knights, &bishops, &rooks, &pawns);
#ifndef UNSAFE_MODE
    if (pieces != (cars | knights | bishops | rooks | pawns)) {
      std::cerr << "WARN! Pieces Board != BitwiseOR of piece boards!" << std::endl;
      PrintComponents();
      valid = false;
    }
    if (pieces >= (1ULL << 56)) {
      std::cerr << "WARN! Pieces Off Board!" << std::endl;
      valid = false;
    }
#endif
    hashCode = HashCode(this);
  }

  inline void Serialize(char* s) const {
    sprintf(s, "%llx %llx %llx %llx %llx %llx %llx",
        pieces, teams, cars, knights, bishops, rooks, pawns);
  }

  inline void Print() const {
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

  inline void PrintChar(int r, int c) const {
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

  inline GameState ApplyMove(Move move) const {
    GameState next = GameState(this, move);
#ifndef UNSAFE_MODE
    if (!next.valid) {
      char dbg[128];
      printf("There's a bug 'round these parts! Last move: ");
      move.Print();
      printf("{%d, %d}\n", move.fromIdx, move.toIdx);
      
      GameState const* cur = this;
      while (cur != NULL) {
        printf("From hash: %016llx and move: ", cur->hashCode);
        if (cur->moveToHere.fromIdx == -1) {
          printf("INVERT\n");
        }
        else {
          cur->moveToHere.Print();
          printf("{%d, %d}\n", cur->moveToHere.fromIdx, cur->moveToHere.toIdx);
        }
        cur->Print();
        cur = cur->prev;
      }
      if (testRoot != NULL) {
        printf("\nfrom seed: %llu\nfrom initial state:\n", testSeed);
        testRoot->Print();
        testRoot->Serialize(dbg);
        printf("%s\n", dbg);
      }
      exit(1);
    }
#endif
    return next;
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
    for (int r = 7; r >= 0; --r) {
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
    else if (cars & (1ULL << 34)) {
      return -1;
    }
    return 0;
  }

  GameState Invert() const {
    GameState ret = GameState(
      FlipVert(this->pieces),
      (~FlipVert(this->teams)) & FlipVert(this->pieces),
      FlipVert(this->cars),
      FlipVert(this->knights),
      FlipVert(this->bishops),
      FlipVert(this->rooks),
      FlipVert(this->pawns)//,
      //~((((hashCode & 0xFFFFFFFFULL)) << 32) | ((hashCode & 0xFFFFFFFF00000000ULL) >> 32))
    );
    ret.prev = this;
    return ret;
  }

  bool operator==(const GameState &other) const {
    return pieces == other.pieces
      && teams == other.teams
      && cars == other.cars
      && knights == other.knights
      && bishops == other.bishops
      && rooks == other.rooks
      && pawns == other.pawns;
  } 
};
#endif

