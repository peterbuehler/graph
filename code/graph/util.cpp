#include "private.h"

double round_up(double value, double step) {  // 139 -> 150 (step = 50)
	return ceil(value / step) * step;
}

double round_down(double value, double step) { // 139 -> 100 (step = 50)
	return floor(value / step) * step;
}

