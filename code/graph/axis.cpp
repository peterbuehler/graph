
#include "private.h"

static limit_t round_limit_lin(limit_t lim);
static limit_t round_limit_log(limit_t lim);

static double transform_lin(axis_t* axis, double d);
static double transform_log(axis_t* axis, double d);

static void axis_update_ticks(axis_t* axis);

axis_t* axis_create(AXIS_SCALE scale) {
	axis_t* axis = (axis_t*) malloc(sizeof(axis_t));
	*axis = { };
	
	axis->ticks = ticks_create(scale);
	axis->ticklabels = ticklabels_create();
	axis->minorticks = minorticks_create(scale);
	axis_set_scale(axis, scale);

	if (axis->scale == SCALE_LIN) {
		axis->limit = { 0, 10 };
	}
	else if (axis->scale = SCALE_LOG) {
		axis->limit = { 0.1, 10 };
	}

	return axis;
}


void axis_set_scale(axis_t* axis, AXIS_SCALE scale) {
	
	axis->scale = scale;
	ticks_set_scale(axis->ticks, scale);
	minorticks_set_scale(axis->minorticks, scale);

	if (scale == SCALE_LIN) {
		axis->transform_fun = transform_lin;
		axis->round_limit_fun = round_limit_lin;
	} else if (scale == SCALE_LOG) {
		axis->transform_fun = transform_log;
		axis->round_limit_fun = round_limit_log;
	}
}


void axis_set_limit(axis_t* axis, limit_t limit) {
	axis->limit = limit;
	axis_update_ticks(axis);
}

// limit gesetzt
static void axis_update_ticks(axis_t* axis) {
	ticks_calculate(axis->ticks, axis->limit);
	ticklabels_update(axis->ticklabels, axis->ticks->values, axis->ticks->length);
	if (axis->scale == SCALE_LOG) {
		minorticks_calculate(axis->minorticks, axis->limit);
	}
}

// liefert 0...1
double axis_transform(axis_t* axis, double d) {
	return axis->transform_fun(axis, d);
}

limit_t axis_round_limit(axis_t* axis, limit_t limit) {
	return axis->round_limit_fun(limit);
}

static limit_t round_limit_lin(limit_t limit) {
	
	double dim = floor(log10(limit.max - limit.min));
	double r = pow(10, dim) / 10;

	double min = round_down(limit.min, r);
	double max = round_up(limit.max, r);

	return { min, max };
}

static limit_t round_limit_log(limit_t limit) {
	double a = pow(10, floor(log10(limit.min)));
	double b = pow(10, ceil(log10(limit.max)));
	return { a, b };
}

static double transform_lin(axis_t* axis, double d) {
	double min = axis->limit.min;
	double max = axis->limit.max;
	return (d - min) / (max - min);
}

static double transform_log(axis_t* axis, double d) {
	double min = log10(axis->limit.min);
	double max = log10(axis->limit.max);
	return (log10(d) - min) / (max - min);
}

void axis_print(axis_t* axis) {
	printf("Axis\n");
	PrintLimit(axis->limit);
	ticks_print(axis->ticks);
	minorticks_print(axis->minorticks);
}





