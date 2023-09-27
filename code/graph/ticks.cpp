#include "private.h"

static void ticks_calc_lin(ticks_t* ticks, limit_t lim);
static void ticks_calc_log(ticks_t* ticks, limit_t lim);

ticks_t* ticks_create(AXIS_SCALE scale) {
	ticks_t* ticks = (ticks_t*)malloc(sizeof(ticks_t));
	*ticks = {};
	ticks_set_scale(ticks, scale);
	return ticks;
}

void ticks_set_scale(ticks_t* ticks, AXIS_SCALE scale) {
	ticks->scale = scale;
	if (scale == SCALE_LIN) {
		ticks->calc_ticks_fun = ticks_calc_lin;
	}
	else if (scale == SCALE_LOG) {
		ticks->calc_ticks_fun = ticks_calc_log;
	}
}

void ticks_calculate(ticks_t* ticks, limit_t limit) {
	ticks->calc_ticks_fun(ticks, limit);
}

void ticks_print(ticks_t* ticks) {
	printf("Ticks: ");
	PrintArray(ticks->values, ticks->length);
}

static void ticks_calc_lin(ticks_t* ticks, limit_t limit) {

	// erster Tick: >= limit.min
	const int MIN_GAPS = 3;  // anzahl gaps

	// Schrittweite bestimmen
	double delta = (limit.max - limit.min) / MIN_GAPS;  // 250
	double dim = pow(10, floor(log10(delta))); // 100
	double x = delta / dim;  // 2.5  (1.0 ... 9.9)

	double gap = 0.0;
	if (x > 5.0) {
		gap = 5.0 * dim;
	} 
	else if (x > 2.0) {
		gap = 2.0 * dim;
	}
	else {
		gap = 1.0 * dim;
	}

	// printf("min=%f, max=%f, delta=%f, dim=%f, x=%f, gap=%f\n", limit.min, limit.max, delta, dim, x, gap);

	// Anzahl Werte bestimmen
	double firstValue = util_round_up(limit.min, gap);
	double tmp = firstValue;
	int cnt = 1;
	while (tmp <= limit.max) {
		tmp = firstValue + cnt * gap;
		cnt++;
	}
	cnt--;  // korr

	// Werte ablegen
	SafeFree(ticks->values);
	ticks->values = (double*)malloc(cnt * sizeof(double));
	for (int i = 0; i < cnt; i++) {
		ticks->values[i] = firstValue + i * gap;
	}
	ticks->length = cnt;
}

static void ticks_calc_log(ticks_t* ticks, limit_t limit) {

	if (limit.min <= 0 || limit.max <= 0) {
		printf("ticks: illegal value for limit\n");
		return;
	}

	double a = ceil(log10(limit.min));
	double b = floor(log10(limit.max));

	SafeFree(ticks->values);

	int length = (int) (b - a) + 1;
	ticks->values = (double*)malloc(length * sizeof(double));
	for (int i = 0; i < length; i++) {
		double tmp = a + i;
		double v = pow(10, tmp);
		ticks->values[i] = v;
	}
	ticks->length = length;

}


