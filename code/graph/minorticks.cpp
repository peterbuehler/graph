#include "private.h"

static void minorticks_calc_lin(minorticks_t* ticks, limit_t lim);
static void minorticks_calc_log(minorticks_t* ticks, limit_t lim);

minorticks_t* minorticks_create(AXIS_SCALE scale) {

	minorticks_t* ticks = (minorticks_t*)malloc(sizeof(ticks_t));
	*ticks = {};
	minorticks_set_scale(ticks, scale);
	return ticks;

}

void minorticks_set_scale(minorticks_t* ticks, AXIS_SCALE scale) {
	ticks->scale = scale;
	if (scale == SCALE_LIN) {
		ticks->calc_ticks_fun = minorticks_calc_lin;
	}
	else if (scale == SCALE_LOG) {
		ticks->calc_ticks_fun = minorticks_calc_log;
	}
}

void minorticks_calculate(minorticks_t* ticks, limit_t limit) {
	ticks->calc_ticks_fun(ticks, limit);
}

void minorticks_print(minorticks_t* ticks) {
	printf("MinorTicks: ");
	PrintArray(ticks->values, ticks->length);
}

static void minorticks_calc_lin(minorticks_t* ticks, limit_t limit) {
	printf("lin scale has no minor ticks.");
}

static void minorticks_calc_log(minorticks_t* ticks, limit_t limit) {

	if (limit.min <= 0 || limit.max <= 0) {
		printf("ticks: illegal value for limit\n");
		return;
	}

	double a = floor(log10(limit.min));
	double step = pow(10, a);


	// get count of ticks
	double v = step;
	int i = 1;
	int cnt = 0;
	while (v <= limit.max) {
		v = i * step;
		i++;
		if (i == 10) {
			step = step * 10;
			i = 1;
		}
		if ((v > limit.min)  && (v <= limit.max)) {
			cnt++;
		}
		// printf("v: %f, cnt: %d\n", v, cnt);
	}

	SafeFree(ticks->values);
	ticks->values = (double*)malloc(cnt * sizeof(double));
	ticks->length = cnt;

	// save values
	step = pow(10, a);
	v = step;
	i = 1;
	cnt = 0;
	while (v <= limit.max) {
		v = i * step;
		i++;
		if (i == 10) {
			step = step * 10;
			i = 1;
		}
		if ((v > limit.min) && (v <= limit.max)) {
			ticks->values[cnt] = v;
			cnt++;
		}
	}

}

