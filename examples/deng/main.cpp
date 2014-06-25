#include <iostream>
#include "stdafx.h"
using namespace std;


int main(int argc, char* argv[]) {
  // #define NUM_FRIENDS 128

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
