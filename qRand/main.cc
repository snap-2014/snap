#include <cstdlib>
#include <iostream>
#include "stoke_dist.h"

using namespace std;
using namespace stoke;

int main(int argc, char** argv) {
	if (argc > 1) {
		seed(atoi(argv[1]));
	}

	for (auto i = 0; i < 1024; ++i) {
		cout << normal() << "\t" << exponential() << "\t" << lognormal() << endl;
	}

	return 0;
}
