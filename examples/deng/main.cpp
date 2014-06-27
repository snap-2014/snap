#include <cmath>
#include <cstdlib> // strtol
#include <string>
#include "stdafx.h"
using namespace std;

#define DATAFILE "/dfs/ilfs2/0/deng/data/glp00ag.asc"
// #define DATAFILE "/dfs/ilfs2/0/deng/data/test.asc"
#define HEADERLINES 6
#define NCOLSLINE 0
#define NROWSLINE 1
#define BASE 10
#define MAXVALUES 16
#define NUMFRIENDS 128
#define PI 3.141592653

TRnd generator(1, 0);

inline int positive_modulo(int i, int n) {
  return (i % n + n) % n;
}

// Vector of TInt pairs, int64 indicates the upper-limit of vector size equals
// 64-bit representation.
typedef TVec<TIntPr, int64> TIntPrI64Vec;

// Returns -1 if TStr does not contain a valid number
// Otherwise returns the number
int extractNum(const char* nptr) {
  char end[16];
  char* endptr = end;
  long int result = strtol(nptr, &endptr, BASE);
  if (strcmp(endptr, "\0") == 0) {
    return result;
  } else {
    return -1;
  }
}

// TODO: Returns true if numCols and numRows populate, otherwise returns false
// Extracts the values of numCols and numRows from the header of the *.asc file
// specified by DATAFILE. Also consumes remaining header lines.
void extractHeaders(TSsParser &parser, int &numCols, int &numRows) {
  for (int line = 0; line < HEADERLINES; line++) {
    parser.Next();
    if (line == NCOLSLINE) {
      for (int i = 0; i < parser.Len(); i++) {
        int result = extractNum(parser.GetFld(i));
        if (result > 0) { numCols = result; }
      }
    }
    else if (line == NROWSLINE) {
      for (int i = 0; i < parser.Len(); i++) {
        int result = extractNum(parser.GetFld(i));
        if (result > 0) { numRows = result; }
      }
    }
  }
}

// Loads data from "infile" into matrix, requires 5 +/- 1s to load
int64 load(TVVec<TIntPr> &matrix, const TStr &infile, int &numCols, int &numRows) {
  int64 census = 0;

  TSsParser parser(infile, ' ');

  extractHeaders(parser, numCols, numRows);
  matrix.Gen(numCols, numRows); // Resize matrix to contain ensuing data
  for (int curRow = 0; curRow < numRows; curRow++) {
    parser.Next();
    for (int curCol = 0; curCol < numCols; curCol++) {
      // ceil(atof(x)) returns the ceiling of a string represented in sci-notation as an int.
      // "init" represents initial count of individuals occupying coordinate (curCol, curRow)
      int init = ceil(atof(parser.GetFld(curCol)));
      census += init;
      TIntPr initPair(init, init);
      matrix.PutXY(curCol, curRow, initPair);
    }
  }

  return census;
}

// Extracts first and second from TIntPr
void extractPair(TIntPr &pair, int &first, int &second) {
  TInt tFirst, tSecond;
  pair.GetVal(tFirst, tSecond);
  first = tFirst.Val;
  second = tSecond.Val;
}

// (For Debugging purposes) Prints matrix
void printMatrix(TVVec<TIntPr> &matrix) {
  int numCols = matrix.GetXDim(), numRows = matrix.GetYDim();
  for (int curRow = 0; curRow < numRows; curRow++) {
    for (int curCol = 0; curCol < numCols; curCol++) {
      int numInitial, numRemaining;
      TIntPr curPair(matrix.GetXY(curCol, curRow));
      extractPair(curPair, numInitial, numRemaining);
      printf("(%d, %d) ", numInitial, numRemaining);
    }
    printf("\n");
  }
}

// (For Debugging purposes) Prints vectors
void printVectors(TIntPrI64Vec &curVec, TIntPrI64Vec &nextVec) {
  printf("curVec: ");
  for (int i = 0; i < curVec.Len(); i++) {
    int curCol, curRow;
    extractPair(curVec[i], curCol, curRow);
    printf("(%d, %d) ", curCol, curRow);
  }
  printf("\nnextVec: ");
  for (int i = 0; i < nextVec.Len(); i++) {
    int nextCol, nextRow;
    extractPair(nextVec[i], nextCol, nextRow);
    printf("(%d, %d) ", nextCol, nextRow);
  }
  printf("\n");
}

// Decrements the count of "numRemaining" by "count" at coordinate
void decrementRemaining(TVVec<TIntPr> &matrix, int col, int row, int count) {
  TIntPr pair(matrix.GetXY(col, row));
  int numInitial, numRemaining;
  extractPair(pair, numInitial, numRemaining);

  // create a new pair
  TIntPr newPair(numInitial, numRemaining - count);
  matrix.PutXY(col, row, newPair);
}

// Hardcoded initial pair.
// TODO: Implement two more seed functions: 1) random and 2) specify
void seed(TVVec<TIntPr> &matrix, TIntPrI64Vec &next) {
  // int numInitial, numRemaining;
  int col = 6902;
  int row = 1370;
  TIntPr initPair(col, row);

  // decrement numRemaining at (col, row)
  decrementRemaining(matrix, col, row, 1);

  next.Add(initPair);
}

// Returns a distance in boxes
double selectDistance() {
  return generator.GetUniDev() * 8640;
}


// Returns an angle in radians
double selectAngle() {
  return generator.GetUniDev() * 2 * PI;
}


// Populate endCol and endRow and returns the associated TIntPair of numInitial
// and numRemaining occupying that coordinate
// Randomly select distance, randomly select angle
void selectEnd(TVVec<TIntPr> &matrix, int startCol, int startRow,
          int numCols, int numRows, int &endCol, int &endRow) {
  double distance = selectDistance();
  double angle = selectAngle();
  endCol = positive_modulo(static_cast<int>(startCol + (distance * cos(angle))), numCols);
  endRow = positive_modulo(static_cast<int>(startRow + (distance * sin(angle))), numRows);

  // printf("distance: %.2f, angle %.2f, endCol: %d, endRow: %d\n", distance, angle, endCol, endRow);
}

// For each individual in cur, use power-law distribution to select individuals
// to append to next.
// Currently hardcode the number of friends of each cur individual. Possible
// alternatives include a Gaussian distribution about this value, etc.
void select(TVVec<TIntPr> &matrix, TIntPrI64Vec &startVec,
          TIntPrI64Vec &endVec, int numCols, int numRows) {

  for (int cur = 0; cur < startVec.Len(); cur++) {

    int startCol, startRow;
    extractPair(startVec[cur], startCol, startRow);

    for (int curFriend = 0; curFriend < NUMFRIENDS; curFriend++) {
      int endNumInitial, endNumRemaining, endCol, endRow;
      int counter = 0;
      do {
        // printf("here\n");
        selectEnd(matrix, startCol, startRow, numCols, numRows, endCol, endRow);
        // printf("there\n");

        TIntPr end(matrix.GetXY(endCol, endRow));
        // printf("everywhere\n");
        extractPair(end, endNumInitial, endNumRemaining);
        // printf("nowhere\n");

        // printf("startCol: %d, startRow: %d, numCols %d, numRows %d, endCol: %d, endRow: %d\n", startCol, startRow, numCols, numRows, endCol, endRow);

        if (counter > 50) break;
        counter++;
      } while (endNumInitial == 0);

      // printf("endNumInitial: %d, endNumRemaining: %d\n", endNumInitial, endNumRemaining);

      if (endNumRemaining > 0) {
        decrementRemaining(matrix, endCol, endRow, 1);
        TIntPr end(endCol, endRow);
        endVec.Add(end);
      }
    }
    // printf("(%d/%li)\n", cur, startVec.Len());
  }
}

int main(int argc, char* argv[]) {
  int numCols = -1, numRows = -1;
  TVVec<TIntPr> matrix(0, 0);
  int64 census = load(matrix, DATAFILE, numCols, numRows);
  // printMatrix(matrix);

  printf("pop: %I64d\n", census);

  // long vector with 32-bit integer values and 64-bit indices:
  TIntPrI64Vec curVec(0);
  TIntPrI64Vec nextVec(0);
  seed(matrix, nextVec);

  // each generation, compare length of nextVec to 1, 150, 22500, etc.
  while (nextVec.Len() > 0) {
    printf("%li\n", nextVec.Len());
    curVec = nextVec;
    nextVec.Clr();
    select(matrix, curVec, nextVec, numCols, numRows);
  }
  return 0;
}
