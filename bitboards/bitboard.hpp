#define U64 unsigned __int64

struct Bitboard {
  const U64 state;

  bool IsEmpty();

  Bitboard Union(Bitboard bb);
  Bitboard Intersect(Bitboard bb);
  Bitboard Complement();

  Bitboard FlipVertical();
  Bitboard FlipHorizontal();

  Bitboard ShiftRows(int rows=1);
};
