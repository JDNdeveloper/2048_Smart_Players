#include <sstream>
#include "ExpectiMaxPlayer.h"

/* ExpectiMaxPlayer interface */

extern "C" {
   ExpectiMaxPlayer* ExpectiMaxPlayer_new(bool debug, int depth) {
      return new ExpectiMaxPlayer(debug, depth);
   }

   void ExpectiMaxPlayer_delete(ExpectiMaxPlayer* player) {
      delete player;
   }

   int ExpectiMaxPlayer_getMove(ExpectiMaxPlayer* player, Board* board) {
      return player->getMove(board);
   }
}

/* Board interface */

extern "C" {
   Board* Board_new(int size) {
      return new Board(size);
   }

   void Board_delete(Board* board) {
      delete board;
   }

   void Board_setPos(Board* board, int row, int col, int val) {
      board->setPos(row, col, val);
   }

   void Board_setScore(Board* board, int score) {
      board->setScore(score);
   }
}

/* Board */

Board::Board(int sizeArg) {
   // set the board size
   size = sizeArg;

   // setup the board
   boardVec = new BoardVec;
   for (int i = 0; i < size; i++) {
      RowVec* rowVec = new RowVec;
      for (int j = 0; j < size; j++) {
         rowVec->push_back(0);
      }
      boardVec->push_back(rowVec);
   }
}

Board::Board(const Board& oldBoard) {
   // set the board size and score
   size = oldBoard.size;
   score = oldBoard.score;

   // copy the board contents
   boardVec = new BoardVec;
   BoardVec* oldBoardVec = oldBoard.boardVec;
   for (BoardVec::iterator rowVecIt = oldBoardVec->begin();
        rowVecIt != oldBoardVec->end(); rowVecIt++) {
      RowVec* oldRowVec = *rowVecIt;
      RowVec* rowVec = new RowVec;
      for (RowVec::iterator valIt = oldRowVec->begin(); valIt != oldRowVec->end();
           valIt++) {
         rowVec->push_back(*valIt);
      }
      boardVec->push_back(rowVec);
   }
}

Board::~Board() {
   while(!boardVec->empty()) {
      delete boardVec->back();
      boardVec->pop_back();
   }
   delete boardVec;
}

std::string Board::getString() {
   std::ostringstream os;
   for (BoardVec::iterator rowVecIt = boardVec->begin();
        rowVecIt != boardVec->end(); rowVecIt++) {
      RowVec* rowVec = *rowVecIt;
      for (RowVec::iterator valIt = rowVec->begin(); valIt != rowVec->end();
           valIt++) {
         int val = *valIt;
         os << val << " ";
      }
   }
   return os.str();
}

void Board::print() {
   for (BoardVec::iterator rowVecIt = boardVec->begin();
        rowVecIt != boardVec->end(); rowVecIt++) {
      RowVec* rowVec = *rowVecIt;
      for (RowVec::iterator valIt = rowVec->begin(); valIt != rowVec->end();
           valIt++) {
         int val = *valIt;
         std::cout << val << " ";
      }
      std::cout << std::endl;
   }
   std::cout << std::endl;
}

void Board::setPos(int row, int col, int val) {
   if (row < 0 || row >= size || col < 0 || col >= size)
      return;

   boardVec->at(row)->at(col) = val;
}

int Board::getTileSum() {
   int score = 0;
   for (BoardVec::iterator rowVecIt = boardVec->begin();
        rowVecIt != boardVec->end(); rowVecIt++) {
      RowVec* rowVec = *rowVecIt;
      for (RowVec::iterator valIt = rowVec->begin(); valIt != rowVec->end();
           valIt++) {
         int val = *valIt;
         score += val;
      }
   }
   return score;
}

int Board::getMaxTile() {
   int maxTile = 0;
   for (BoardVec::iterator rowVecIt = boardVec->begin();
        rowVecIt != boardVec->end(); rowVecIt++) {
      RowVec* rowVec = *rowVecIt;
      for (RowVec::iterator valIt = rowVec->begin(); valIt != rowVec->end();
           valIt++) {
         int val = *valIt;
         maxTile = val > maxTile ? val : maxTile;
      }
   }
   return maxTile;
}

int Board::makeMove(Move move) {
   bool boardChanged = false;
   int moveScore = 0;

   int rowIdx = 0;
   int colIdx = 0;
   switch (move) {
   case UP:
      rowIdx = 0;
      colIdx = 0;
      break;
   case DOWN:
      rowIdx = size - 1;
      colIdx = 0;
      break;
   case LEFT:
      rowIdx = 0;
      colIdx = 0;
      break;
   case RIGHT:
      rowIdx = 0;
      colIdx = size - 1;
      break;
   }
   for (int i = 0; i < size; i++) {
      int rowBackIdx = rowIdx;
      int colBackIdx = colIdx;
      int setCount = 0;
      int prevVal = -1;
      int rowPrevIdx = -1;
      int colPrevIdx = -1;

      for (int j = 0; j < size; j++) {
         int val = boardVec->at(rowIdx)->at(colIdx);
         if (val > 0) {
            if (val == prevVal) {
               int newVal = 2 * val;
               boardVec->at(rowPrevIdx)->at(colPrevIdx) = newVal;
               moveScore += newVal;
               prevVal = -1;
               boardChanged = true;
            } else {
               boardVec->at(rowBackIdx)->at(colBackIdx) = val;
               setCount++;
               if (rowBackIdx != rowIdx || colBackIdx != colIdx)
                  boardChanged = true;
               prevVal = val;

               switch (move) {
               case UP:
                  rowBackIdx++;
                  break;
               case DOWN:
                  rowBackIdx--;
                  break;
               case LEFT:
                  colBackIdx++;
                  break;
               case RIGHT:
                  colBackIdx--;
                  break;
               }
            }
         }

         rowPrevIdx = rowIdx;
         colPrevIdx = colIdx;

         switch (move) {
         case UP:
            rowIdx++;
            break;
         case DOWN:
            rowIdx--;
            break;
         case LEFT:
            colIdx++;
            break;
         case RIGHT:
            colIdx--;
            break;
         }
      }

      for (; setCount < size; setCount++) {
         boardVec->at(rowBackIdx)->at(colBackIdx) = 0;
         switch (move) {
         case UP:
            rowBackIdx++;
            break;
         case DOWN:
            rowBackIdx--;
            break;
         case LEFT:
            colBackIdx++;
            break;
         case RIGHT:
            colBackIdx--;
            break;
         }
      }

      switch (move) {
      case UP:
         rowIdx = 0;
         colIdx++;
         break;
      case DOWN:
         rowIdx = size - 1;
         colIdx++;
         break;
      case LEFT:
         rowIdx++;
         colIdx = 0;
         break;
      case RIGHT:
         rowIdx++;
         colIdx = size - 1;
         break;
      }
   }

   score += moveScore;

   /* return value:
    * -1 = board did not change
    *  0 = no tiles merged
    * >0 = tiles merged
    */
   return boardChanged ? moveScore : -1;
}

/* ExpectiMaxPlayer */

ExpectiMaxPlayer::ExpectiMaxPlayer(bool debugArg, int depthArg) {
   debug = debugArg;
   depth = depthArg;
}

int tryMove(Board* board, Move move) {
   Board* newBoard = new Board(*board);
   int moveScore = newBoard->makeMove(move);
   delete newBoard;
   return moveScore;
}

float getHeuristicScore(Board* board) {
   return board->getScore();
}

Result getMoveRecursive(StateCache* stateCache, Board* board, Player player,
                        int depth, double prob) {
   State state(board->getString(), player, depth);
   StateCache::iterator stateIt = stateCache->find(state);
   if (stateIt != stateCache->end()) {
      return Result(stateIt->second, NO_MOVE);
   }

   if (depth == 0 || prob < PROB_CUTOFF) {
      int heuristicScore = getHeuristicScore(board);
      stateCache->insert({state, heuristicScore});
      return Result(heuristicScore, NO_MOVE);
   }

   switch (player) {
   case USER:
      {
         Board* newBoard;
         Move move;
         int moveScore;

         Move maxMove = NO_MOVE;
         float maxScore = -1;

         // move up
         move = UP;
         newBoard = new Board(*board);
         moveScore = newBoard->makeMove(move);
         if (moveScore != -1) {
            Result result = getMoveRecursive(stateCache, newBoard, TILE_SPAWN,
                                             depth, prob / 4.0);
            float score = result.score;
            if (score >= maxScore) {
               maxScore = score;
               maxMove = move;
            }
         }
         delete newBoard;

         // move down
         move = DOWN;
         newBoard = new Board(*board);
         moveScore = newBoard->makeMove(move);
         if (moveScore != -1) {
            Result result = getMoveRecursive(stateCache, newBoard, TILE_SPAWN,
                                             depth, prob / 4.0);
            float score = result.score;
            if (score >= maxScore) {
               maxScore = score;
               maxMove = move;
            }
         }
         delete newBoard;

         // move left
         move = LEFT;
         newBoard = new Board(*board);
         moveScore = newBoard->makeMove(move);
         if (moveScore != -1) {
            Result result = getMoveRecursive(stateCache, newBoard, TILE_SPAWN,
                                             depth, prob / 4.0);
            float score = result.score;
            if (score >= maxScore) {
               maxScore = score;
               maxMove = move;
            }
         }
         delete newBoard;

         // move right
         move = RIGHT;
         newBoard = new Board(*board);
         moveScore = newBoard->makeMove(move);
         if (moveScore != -1) {
            Result result = getMoveRecursive(stateCache, newBoard, TILE_SPAWN,
                                             depth, prob / 4.0);
            float score = result.score;
            if (score >= maxScore) {
               maxScore = score;
               maxMove = move;
            }
         }
         delete newBoard;

         stateCache->insert({state, maxScore});
         return Result(maxScore, maxMove);
      }
      break;
   case TILE_SPAWN:
      {
         float expectedScore = 0;
         float twoTileScores = 0;
         float fourTileScores = 0;

         int size = board->getSize();
         int emptySlots = 0;
         for (int row = 0; row < size; row++) {
            for (int col = 0; col < size; col++) {
               if (board->getPos(row, col) == 0) {
                  emptySlots++;
               }
            }
         }
         for (int row = 0; row < size; row++) {
            for (int col = 0; col < size; col++) {
               if (board->getPos(row, col) == 0) {
                  Board* newBoard;
                  int tile;
                  double newProb;

                  // spawn a 2 tile in the empty position
                  tile = 2;
                  newBoard = new Board(*board);
                  newBoard->setPos(row, col, tile);
                  newProb = prob * (1.0 / emptySlots) * 0.9;
                  {
                     Result result = getMoveRecursive(stateCache, newBoard, USER,
                                                      depth - 1, newProb);
                     twoTileScores += result.score;
                  }
                  delete newBoard;

                  // spawn a 4 tile in the empty position
                  tile = 4;
                  newBoard = new Board(*board);
                  newBoard->setPos(row, col, tile);
                  newProb = prob * (1.0 / emptySlots) * 0.1;
                  {
                     Result result = getMoveRecursive(stateCache, newBoard, USER,
                                                      depth - 1, newProb);
                     fourTileScores += result.score;
                  }
                  delete newBoard;
               }
            }
         }
         if (emptySlots == 0) {
            expectedScore = 0;
         } else {
            expectedScore = (0.9 * (twoTileScores / emptySlots) +
                             0.1 * (fourTileScores / emptySlots));
         }
         stateCache->insert({state, expectedScore});
         return Result(expectedScore, NO_MOVE);
      }
      break;
   }
}

int ExpectiMaxPlayer::getMove(Board* board) {
   StateCache* stateCache = new StateCache();
   Result result = getMoveRecursive(stateCache, board, USER, depth, 1.0);
   delete stateCache;
   return result.move;
}
