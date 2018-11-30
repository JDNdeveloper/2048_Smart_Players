#include <functional>
#include <iostream>
#include <unordered_map>
#include <vector>

typedef std::vector<int> RowVec;
typedef std::vector<RowVec*> BoardVec;

enum Move {
   NO_MOVE = -1,
   UP = 1,
   DOWN = 2,
   LEFT = 3,
   RIGHT = 4
};

enum Player {
   USER,
   TILE_SPAWN
};

class Result {
 public:
   float score;
   Move move;

   Result(float scoreArg, Move moveArg) {
      score = scoreArg;
      move = moveArg;
   };
};

class State {
 public:
   std::string board;
   Player player;
   int depth;

   State(std::string boardArg, Player playerArg, int depthArg) {
      board = boardArg;
      player = playerArg;
      depth = depthArg;
   };
   bool operator==(const State& rhs) const {
      return (rhs.board == board &&
              rhs.player == player &&
              rhs.depth == depth);
   };
};

typedef std::unordered_map<State, int> StateCache;

namespace std {
   template <> struct hash<State> {
      size_t operator()(const State& state) const {
         return hash<int>()(hash<string>()(state.board) +
                            hash<int>()(state.player) +
                            hash<int>()(state.depth));
      }
   };
}

class Board {
 public:
   Board(int);
   Board(const Board&);
   ~Board();
   void print();
   std::string getString();
   void setPos(int, int, int);
   int getPos(int row, int col) { return boardVec->at(row)->at(col); };
   int getSize() { return size; };
   int getTileSum();
   int getMaxTile();
   void setScore(int scoreArg) { score = scoreArg; };
   int getScore() { return score; };
   int makeMove(Move);
 private:
   int size;
   int score;
   BoardVec* boardVec;
};

class ExpectiMaxPlayer {
 public:
   ExpectiMaxPlayer(bool, int);
   int getMove(Board*);
 private:
   bool debug;
   int depth;
};
