#include <cmath>
#include <cstdlib> // strtol
#include "stdafx.h"
using namespace std;

#define DATAFILE "/dfs/ilfs2/0/deng/data/glp00ag.asc"
#define HEADERLINES 6
#define NCOLSLINE 0
#define NROWSLINE 1
#define BASE 10


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

// Loads data from DATAFILE into matrix, requires 5 +/- 1s to load
void load(TVVec<TPair<TInt, TInt> > &matrix) {
  TSsParser parser(DATAFILE, ' ');
  int numCols = -1, numRows = -1;
  extractHeaders(parser, numCols, numRows);
  // printf("(%d, %d)\n", numRows, numCols);

  // Resize matrix to contain ensuing data
  matrix.Gen(numCols, numRows);

  for (int curRow = 0; curRow < numRows; curRow++) {
    parser.Next();
    for (int curCol = 0; curCol < numCols; curCol++) {
      // printf("%s\n", parser.GetFld(curCol));
      // ceil(atof(x)) returns the ceiling of a string represented in sci-notation as an int.
      // "init" represents initial count of individuals occupying coordinate (curCol, curRow)
      int init = ceil(atof(parser.GetFld(curCol)));
      TPair<TInt, TInt> initPair(init, init);
      matrix.PutXY(curCol, curRow, initPair);
    }
    // if (curRow % 500 == 0) printf("Row %d completed!\n", curRow);
  }
}

void print(TVVec<TPair<TInt, TInt> > &matrix) {
  int numRows = matrix.GetYDim(), numCols = matrix.GetXDim();
  for (int i = 0; i < numRows; i++) {
    for (int j = 0; j < numCols; j++) {
      TInt numRemaining, numInitial;
      (matrix.GetXY(j, i)).GetVal(numRemaining, numInitial);
      // numRemaining() converts TInt to int, quiets compiler
      printf("(%d, %d) ", numRemaining(), numInitial());
    }
    printf("\n");
  }
}

int main(int argc, char* argv[]) {
  TVVec<TPair<TInt, TInt> > matrix(0, 0);
  load(matrix);
  // print(matrix);

  // long vector with 32-bit integer values and 64-bit indices:
  // TVec<TInt, TUInt64> vect();


  // Initalize 2D vector of bucket structs containing two ints: "numRemaining"
  // and "numInitial". Each value should be initialized to the count specified
  // by the corresponding coordinate in the dataset. The value of "numInitial"
  // should remain constant during execution. On removal of an individual from
  // a bucket, decrement the value of "numRemaining."

  // struct person { // we represent an individual by his/her bucket coordinate
  //   int r; // 0-indexed
  //   int c; // 0-indexed
  // }

  // Initialize vector (why not queue?) NodesToVisit (sizes 1, 150, 22500, etc.)

  // While at least one person remains:
  //   Select a Person from this set
  //   For each Friend of Person: // Person has NUM_FRIENDS friends
  //     According to Power Law Distribution (defined in SNAP Library), randomly
  //       select distance; according to Uniform Dist. select an angle [0, 2pi).
  //     Follow this mathematical vector to a Destination.
  //     If Destination in Ocean: repeat
  //     Else:
  //       If Destination.numRemaining > 0:
  //         Destination.numRemaining--
  //         Add Destination to NodesToVisit
  //       Else:
  //         Do nothing (this behavior makes sense since we already accounted
  //           for this individual earlier)

  return 0;
}
