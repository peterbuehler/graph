#include "private.h"

static dim_t graphics_get_size_content(graphics_t* g);

graphics_t* graphics_create() {
	graphics_t* g = (graphics_t*)malloc(sizeof(graphics_t));

	g->size = { .width = 800, .height = 400 };
	g->border = { .top = 30, .right = 60, .bottom = 50, .left = 60 };  // reihenfolge: must
	g->data = data_create();
	g->xaxis = axis_create(SCALE_LIN);
	g->yaxis = axis_create(SCALE_LIN);

	g->title[0] = L'\0';  // set term character
	g->timerTimeMillis = 0;  // no timer

	return g;
}

void graphics_set_size(graphics_t* g, dim_t size) {
	g->size.width = size.width;
	g->size.height = size.height;
}

void graphics_set_xlimit(graphics_t* g, limit_t lim) {

	// guard
	if (g->xaxis->scale == SCALE_LOG && lim.min <= 0) {
		printf("[WARN] xaxis has log scale: limit.min must be >= 0");
		return;
	}
	axis_set_limit(g->xaxis, lim);
	axis_update_ticks(g->xaxis);
}

void graphics_set_ylimit(graphics_t* g, limit_t lim) {

	// guard
	if (g->yaxis->scale == SCALE_LOG && lim.min <= 0) {
		printf("[WARN] yaxis has log scale: limit.min must be >= 0");
		return;
	}
	axis_set_limit(g->yaxis, lim);
	axis_update_ticks(g->yaxis);
}

void graphics_set_xscale(graphics_t* g, AXIS_SCALE scale) {

	// check data if scale log
	if (scale == SCALE_LOG && g->data->length > 0) {
		limit_t lim = data_get_xlimit(g->data);
		if (lim.min <= 0) {
			printf("[WARN] xdata is <= 0, cannot set log scale\n");
			return;
		}
	}

	// set limits again
	if (g->data->length > 0) {
		limit_t lim = data_get_xlimit(g->data);
		lim = util_round_limit(lim, scale);
		axis_set_limit(g->xaxis, lim);
	}

	axis_set_scale(g->xaxis, scale);
	axis_update_ticks(g->xaxis);
}

void graphics_set_yscale(graphics_t* g, AXIS_SCALE scale) {

	// check data if scale log
	if (scale == SCALE_LOG && g->data->length > 0) {
		limit_t lim = data_get_ylimit(g->data);
		if (lim.min <= 0) {
			printf("[WARN] ydata is <= 0, cannot set log scale\n");
			return;
		}
	}

	// set limits again
	if (g->data->length > 0) {
		limit_t lim = data_get_ylimit(g->data);
		lim = util_round_limit(lim, scale);
		axis_set_limit(g->yaxis, lim);
	}
	
	axis_set_scale(g->yaxis, scale);
	axis_update_ticks(g->yaxis);
}

static dim_t graphics_get_size_content(graphics_t* g) {
	double w = g->size.width - g->border.left - g->border.right;
	double h = g->size.height - g->border.top - g->border.bottom;
	return { w, h };
}


// transforms point from model to pixel
point_t graphics_to_view(graphics_t* g, point_t p) {

	double xt = axis_transform(g->xaxis, p.x);
	double yt = axis_transform(g->yaxis, p.y);

	dim_t content = graphics_get_size_content(g);
	double x = g->border.left + xt * content.width;
	double y = g->border.top + content.height * (1 - yt);

	return { x, y };
}

void graphics_set_border(graphics_t* g, border_t border) {
	g->border.top = border.top;
	g->border.bottom = border.bottom;
	g->border.left = border.left;
	g->border.right = border.right;
}

void graphics_set_title(graphics_t* g, const wchar_t* title) {
	wcscpy_s(g->title, MAX_TITLE_LENGTH, title);
	border_t border = g->border;
	border.top = border.top + FONT_SIZE_TITLE * 2;  // guess
	graphics_set_border(g, border);
}

row_t* graphics_add_row(graphics_t* g, double* xvalues, double* yvalues, int length) {

	// check log scale
	if (g->xaxis->scale == SCALE_LOG) {
		limit_t lim = util_get_limit(xvalues, length);
		if (lim.min <= 0) {
			printf("[warn] illegal xvalues for scale log");
			return NULL;
		}
	}
	if (g->yaxis->scale == SCALE_LOG) {
		limit_t lim = util_get_limit(yvalues, length);
		if (lim.min <= 0) {
			printf("[warn] illegal yvalues for scale log");
			return NULL;
		}
	}

	// create row
	row_t* row = row_create(xvalues, yvalues, length);
	data_add_row(g->data, row);

	// update limit xaxis
	limit_t xlim = data_get_xlimit(g->data);
	xlim = util_round_limit(xlim, g->xaxis->scale);
	axis_set_limit(g->xaxis, xlim);

	// update limit yaxis
	limit_t ylim = data_get_ylimit(g->data);
	ylim = util_round_limit(ylim, g->yaxis->scale);
	axis_set_limit(g->yaxis, ylim);

	axis_update_ticks(g->xaxis);
	axis_update_ticks(g->yaxis);

	return row;

}