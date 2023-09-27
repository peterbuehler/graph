#include "private.h"

// rounds value up to step
double util_round_up(double value, double step) {  // 139 -> 150 (step = 50)
	return ceil(value / step) * step;
}

// rounds value down to step
double util_round_down(double value, double step) { // 139 -> 100 (step = 50)
	return floor(value / step) * step;
}

// find min/mav value
limit_t util_get_limit(double* values, int length) {
	
	if (length == 0) {
		return { 0, 0 };
	}

	double min = DBL_MAX;
	double max = DBL_MIN;

	for (int i = 0; i < length; i++) {
		double v = values[i];
		if (v > max) {
			max = v;
		}
		if (v < min) {
			min = v;
		}
	}

	return { min, max }; 
}

// rounds to ~1.5 digits // 99 -> 100
static limit_t round_limit_lin(limit_t lim) {

	double d = lim.max - lim.min;	// always > 0
	double dim = floor(log10(d));	// d = 527 -> dim = 2
	double r = pow(10, dim) / 5;	// d = 527 -> r = 20  // 99 -> r = 2

	double min = util_round_down(lim.min, r);
	double max = util_round_up(lim.max, r);

	return { min, max };
}

// rounds to nearest power of 10
static limit_t round_limit_log(limit_t lim) {
	double min = pow(10, floor(log10(lim.min)));
	double max = pow(10, ceil(log10(lim.max)));
	return { min, max };
}

limit_t util_round_limit(limit_t lim, AXIS_SCALE scale) {
	if (scale == SCALE_LIN) {
		return round_limit_lin(lim);
	}
	else if (scale == SCALE_LOG) {
		return round_limit_log(lim);
	}
}

