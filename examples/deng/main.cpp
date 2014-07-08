#include <cmath>
#include <cstdlib> // strtol
#include <signal.h>
#include <stdio.h>
#include <string>
#include "stdafx.h"
#include <unistd.h>
using namespace std;

#define DATAFILE "/dfs/ilfs2/0/deng/data/glp00ag.asc"
#define MATRIXFILE "output/matrix.out"
// #define DATAFILE "/dfs/ilfs2/0/deng/data/test.asc"
#define HEADERLINES 6
#define NCOLSLINE 0
#define NROWSLINE 1
#define BASE 10
#define MAXVALUES 16
#define NUMFRIENDS 128
#define PI 3.141592653
#define ALPHA 10

// DEBUG
#define SCALE 10
// DEBUG

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

// Extracts first and second from TIntPr
void extractPair(TIntPr &pair, int &first, int &second) {
  TInt tFirst, tSecond;
  pair.GetVal(tFirst, tSecond);
  first = tFirst.Val;
  second = tSecond.Val;
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

// Loads raw data from "infile" into matrix, requires 5 +/- 1s to load
// Saves matrix into "outfile"
// Returns an int64 of total population of the loaded dataset
int64 load(TVVec<TIntPr> &matrix, const TStr &infile, const TStr &outfile,
          int &numCols, int &numRows) {
  int64 census = 0;
  TSsParser parser(infile, ' ');
  extractHeaders(parser, numCols, numRows);
  matrix.Gen(numCols, numRows); // Resize matrix to contain ensuing data
  for (int curRow = 0; curRow < numRows; curRow++) {
    parser.Next();
    for (int curCol = 0; curCol < numCols; curCol++) {
      // ceil(atof(x)) returns the ceiling of a string represented in sci-notation as an int.
      // "init" represents initial count of individuals occupying coordinate (curCol, curRow)
      int init = ceil(atof(parser.GetFld(curCol)) / SCALE);
      census += init;
      TIntPr initPair(init, init);
      matrix.PutXY(curCol, curRow, initPair);
    }
  }
  { TFOut out(outfile);
    matrix.Save(out); }
  return census;
}

// Loads saved data from "infile" into matrix, requires 1 +/- 1s to load
// Returns an int64 of total population of the loaded dataset
int64 load(TVVec<TIntPr> &matrix, const TStr &infile, int &numCols,
          int &numRows) {
  { TFIn in(infile);
    matrix.Load(in); }
  numCols = matrix.GetXDim();
  numRows = matrix.GetYDim();
  int64 census = 0;
  int numInitial, numRemaining;
  for (int curRow = 0; curRow < numRows; curRow++) {
    for (int curCol = 0; curCol < numCols; curCol++) {
      TIntPr pair(matrix.GetXY(curCol, curRow));
      extractPair(pair, numInitial, numRemaining);
      census += numInitial;
    }
  }
  return census;
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
  // return generator.GetUniDev() * 8640;
  return (generator.GetPowerDev(ALPHA) - 1) * 4320;
}

// Returns an angle in radians
double selectAngle() {
  return generator.GetUniDev() * 2 * PI;
}

// Returns number of friends
double selectNumFriends() {
  return generator.GetNrmDev(NUMFRIENDS, 10, 0, 256);
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
}

int generation = 0;
FILE *coordsDump = fopen("output/glp00ag.out", "w");
FILE *sigintDump = fopen("output/10", "w");
int coordinatesAdded = 0; // DEBUG

// For each individual in cur, use power-law distribution to select individuals
// to append to next.
// Currently hardcode the number of friends of each cur individual. Possible
// alternatives include a Gaussian distribution about this value, etc.
void select(TVVec<TIntPr> &matrix, TIntPrI64Vec &startVec,
          TIntPrI64Vec &endVec, int numCols, int numRows) {
  for (int cur = 0; cur < startVec.Len(); cur++) {

    // DEBUG
    if (cur % 1000000 == 0) {
      printf("(%d/%ld)\n", cur, startVec.Len());
      fflush(stdout);
    }
    // DEBUG

    int startCol, startRow;
    extractPair(startVec[cur], startCol, startRow);
    for (int curFriend = 0; curFriend < NUMFRIENDS; curFriend++) {
      int endNumInitial, endNumRemaining, endCol, endRow;
      int counter = 0;
      do {
        selectEnd(matrix, startCol, startRow, numCols, numRows, endCol, endRow);
        TIntPr end(matrix.GetXY(endCol, endRow));
        extractPair(end, endNumInitial, endNumRemaining);
        if (counter > 50) break;
        counter++;
      } while (endNumInitial == 0);
      if (endNumRemaining > 0) {
        // DEBUG
        fprintf(coordsDump, "(%d,%d) (%d,%d)\n", endCol, endRow, startCol, startRow);
        coordinatesAdded++;
        // DEBUG

        decrementRemaining(matrix, endCol, endRow, 1);
        TIntPr end(endCol, endRow);
        endVec.Add(end);
      }
    }
  }
}

void simulate(TVVec<TIntPr> &matrix, int numCols, int numRows, int64 census) {
  // long vector with 32-bit integer values and 64-bit indices:
  TIntPrI64Vec curVec(0);
  TIntPrI64Vec nextVec(0);
  // curVec.Reserve(census);
  // nextVec.Reserve(census);
  seed(matrix, nextVec);

  // each generation, compare length of nextVec to 1, 150, 22500, etc.
  while (nextVec.Len() > 0) {
    generation++;
    curVec = nextVec;
    nextVec.Clr();
    select(matrix, curVec, nextVec, numCols, numRows);
  }
}

void printMatrix(TVVec<TIntPr> &matrix, FILE *outfile) {
  int numCols = matrix.GetXDim(), numRows = matrix.GetYDim();
  for (int curRow = 0; curRow < numRows; curRow++) {
    for (int curCol = 0; curCol < numCols; curCol++) {
      int numInitial, numRemaining;
      TIntPr curPair(matrix.GetXY(curCol, curRow));
      extractPair(curPair, numInitial, numRemaining);
      if (outfile == NULL) {
        printf("(%d,%d) ", numInitial, numRemaining);
      } else {
        fprintf(outfile, "(%d,%d) ", numInitial, numRemaining);
      }
    }
    if (outfile == NULL) {
      printf("\n");
    } else {
      fprintf(outfile, "\n");
    }
  }
}

// DEBUG
TVVec<TIntPr> matrix(0, 0);
// DEBUG

// DEBUG
void my_handler(int param) {
  printf("\nBOOM GOES THE SIGINT!\n");
  printf("Coordinates Added: %d\n", coordinatesAdded);
  printf("Current Generation: %d\n", generation);
  printf("Writing relevant statistics and current state of the matrix to\n");
  printf("file \"output/sigintDump.out\"......\n");
  fprintf(sigintDump, "Coordinates Added: %d\n", coordinatesAdded);
  fprintf(sigintDump, "Current Generation: %d\n", generation);
  printMatrix(matrix, sigintDump);
  exit(1);
}
// DEBUG

int main(int argc, char* argv[]) {
  // DEBUG
  void (*prev_handler) (int);
  prev_handler = signal(SIGINT, my_handler);
  // DEBUG

  int numCols = -1, numRows = -1;
  // TVVec<TIntPr> matrix(0, 0);
  int64 census = load(matrix, DATAFILE, MATRIXFILE, numCols, numRows);
  // int64 census = load(matrix, MATRIXFILE, numCols, numRows);
  // printMatrix(matrix, NULL);
  printf("pop: %I64d\nnumCols: %d\nnumRows: %d\n", census, numCols, numRows);
  // simulate(matrix, numCols, numRows, census);

  // DEBUG
  kill(getpid(), SIGINT);
  // DEBUG

  return 0;
}
