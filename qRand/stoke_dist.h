#ifndef STOKE_DIST_H
#define STOKE_DIST_H

#include <stdint.h>
#include <random>

extern double normal(size_t index, uint64_t r1, uint64_t r2, uint64_t r3, uint64_t r4, double* table);
extern double exponential(size_t index, uint64_t r1, uint64_t r2, uint64_t r3, uint64_t r4, double* table);
extern double lognormal(size_t index, uint64_t r1, uint64_t r2, uint64_t r3, uint64_t r4, double* table);

namespace stoke {

extern std::default_random_engine engine;
extern size_t counter;

inline void seed(std::default_random_engine::result_type s) {
	engine.seed(s);
}

inline double normal() {
	const auto index = counter++;
	const auto r1 = engine();
	const auto r2 = engine();
	const auto r3 = engine();
	const auto r4 = engine();
	return ::normal(index, r1, r2, r3, r4, 0);
}


inline double exponential() {
	const auto index = counter++;
	const auto r1 = 0;
	const auto r2 = 0;
	const auto r3 = 0;
	const auto r4 = engine();
	return ::exponential(index, r1, r2, r3, r4, 0);
}

inline double lognormal() {
	const auto index = counter++;
	const auto r1 = engine();
	const auto r2 = engine();
	const auto r3 = engine();
	const auto r4 = engine();
	return ::lognormal(index, r1, r2, r3, r4, 0);
}

}

#endif
