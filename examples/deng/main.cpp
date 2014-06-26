#include <algorithm>
#include <iterator>
#include <cstdlib> // strtol
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "stdafx.h"
using namespace std;

#define DATAFILE "/dfs/ilfs2/0/deng/data/glp00ag.asc"
#define NUM_FRIENDS 128

// Requires 15 +/- 1s to load

void load(TVVec<TInt> &matrix) {
  ifstream infile(DATAFILE);
  string strNRows, strNCols, strXLLCorner, strYLLCorner, strCellSize, strValue;
  getline(infile, strNCols);
  getline(infile, strNRows);
  getline(infile, strXLLCorner); // unused
  getline(infile, strYLLCorner); // unused
  getline(infile, strCellSize);  // unused
  getline(infile, strValue);     // unused

  int numCols = strtol((strNCols.substr(strlen("ncols"))).c_str(), NULL, 10);
  int numRows = strtol((strNRows.substr(strlen("ntows"))).c_str(), NULL, 10);

  matrix.Gen(numCols, numRows);

  string strCurRow;
  for (int r = 0; r < numRows; r++) {
    getline(infile, strCurRow);
    istringstream iss(strCurRow);
    vector<string> tokens;
    copy(istream_iterator<string>(iss), istream_iterator<string>(),
      back_inserter<vector<string> >(tokens));
    for (int c = 0; c < numCols; c++) {
      matrix.PutXY(c, r, strtol(tokens[c].c_str(), NULL, 10));
    }
    // if (r % 500 == 0) cout << "Row " << r << " completed!" << endl;
  }
}

void print(TVVec<TInt> &matrix) {
  int numRows = matrix.GetYDim(), numCols = matrix.GetXDim();

  for (int i = 0; i < numRows; i++) {
    for (int j = 0; j < numCols; j++) {
      cout << matrix.GetXY(j, i).Val << " ";
    }
    cout << endl;
  }
}

int main(int argc, char* argv[]) {
  TVVec<TInt> matrix(0, 0);
  load(matrix);
  // print(matrix);


  // long vector with 32-bit integer values and 64-bit indices:
  // TVec<TInt, TUInt64> vect();

  // Import data

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
