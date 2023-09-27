
#include "private.h"

static double transform_lin(axis_t* axis, double d);
static double transform_log(axis_t* axis, double d);

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

// set_scale of axis
void axis_set_scale(axis_t* axis, AXIS_SCALE scale) {
	
	axis->scale = scale;
	ticks_set_scale(axis->ticks, scale);
	minorticks_set_scale(axis->minorticks, scale);

	/// set function pointer
	if (scale == SCALE_LIN) {
		axis->transform_fun = transform_lin;
	} else if (scale == SCALE_LOG) {
		axis->transform_fun = transform_log;
	}
}

// set limit of axis
void axis_set_limit(axis_t* axis, limit_t limit) {
	axis->limit = limit;
}

// limit and/or scale changed
void axis_update_ticks(axis_t* axis) {
	ticks_calculate(axis->ticks, axis->limit);
	ticklabels_update(axis->ticklabels, axis->ticks->values, axis->ticks->length);
	if (axis->scale == SCALE_LOG) {
		minorticks_calculate(axis->minorticks, axis->limit);
	}
}

// return 0...1 if within limit
double axis_transform(axis_t* axis, double d) {
	return axis->transform_fun(axis, d);
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





