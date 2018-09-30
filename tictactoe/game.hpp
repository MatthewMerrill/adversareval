#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <list>

struct Move {
  int team;
  int pos;

  Move(int t, int p) : team(t), pos(p){
  }
};

struct GameState {
  
  int pieces;
  int team;

  GameState() : pieces(0), team(0){
  } 

  GameState(int p, int t) : pieces(p), team(t){
  }

  void Print() const {
    std::cout
      << " " << SquareChar(0)
      << " ┃ " << SquareChar(1)
      << " ┃ " << SquareChar(2) << " " << std::endl
      << "━━━╋━━━╋━━━" << std::endl
      << " " << SquareChar(3)
      << " ┃ " << SquareChar(4)
      << " ┃ " << SquareChar(5) << " " << std::endl
      << "━━━╋━━━╋━━━" << std::endl
      << " " << SquareChar(6)
      << " ┃ " << SquareChar(7)
      << " ┃ " << SquareChar(8) << " " << std::endl;
  }
  char SquareChar(int pos) const {
    if (!(pieces & (1 << pos))) {
      return ' ';
    }
    return ((team & (1 << pos)) == 0) ? 'X' : 'O';
  }

  GameState Negate() const {
    GameState inv;
    inv.pieces = pieces;
    inv.team = ~team;
    return inv;
  }

  bool IsValidMove(Move move) const {
    return move.pos >= 0 && move.pos < 9 && (pieces & (1 << move.pos)) == 0;
  }

  GameState ApplyMove(Move move) const {
    return GameState(
        pieces | (1 << move.pos),
        team | ((move.team == 1 ? 1 : 0) << move.pos));
  }

  bool XWins() const {
    int pieces = this->pieces & ~team;
    return (
        (pieces &   7) ==   7 || // hgh row
        (pieces &  56) ==  56 || // mid row
        (pieces & 448) == 448 || // low row
        (pieces &  73) ==  73 || // lft col
        (pieces & 146) == 146 || // mid col
        (pieces & 292) == 292 || // rgt col
        (pieces & 273) == 273 || // diag
        (pieces &  84) ==  84 // anti-diag
    );
  }

  bool OWins() const {
    int pieces = this->pieces & team;
    return (
        (pieces &   7) ==   7 || // hgh row
        (pieces &  56) ==  56 || // mid row
        (pieces & 448) == 448 || // low row
        (pieces &  73) ==  73 || // lft col
        (pieces & 146) == 146 || // mid col
        (pieces & 292) == 292 || // rgt col
        (pieces & 273) == 273 || // diag
        (pieces &  84) ==  84 // anti-diag
    );
  } 

  int GetWinner() const {
    if (XWins()) {
      return -1;
    }
    else if (OWins()) {
      return 1;  
    }
    else {
      return 0;
    }
  }

  std::list<Move> GetMoves(int team) const {
    std::list<Move> moves;
    for (int i = 0; i < 9; ++i) {
      Move m = Move(team, i);
      if (IsValidMove(m)) {
        moves.push_back(Move(team, i));
      }
    }
    return moves;
  }
};

#endif

