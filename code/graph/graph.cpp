#include "private.h"

static graphics_t* graphics_create() {
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

graph_t* graph_create() {
	graph_t* g = (graph_t*)malloc(sizeof(graph_t));
	g->priv = graphics_create();
	return g;
}

void graph_set_title(graph_t* graph, const wchar_t* title) {
	graphics_t* g = (graphics_t*)graph->priv;
	wcscpy_s(g->title, MAX_TITLE_LENGTH, title);

	border_t border = g->border;
	border.top = border.top + FONT_SIZE_TITLE * 2;  // guess
	graph_set_border(graph, border);

}

void graph_set_xscale(graph_t* graph, AXIS_SCALE scale) {
	graphics_t* g = (graphics_t*)graph->priv;
	axis_set_scale(g->xaxis, scale);
}

void graph_set_yscale(graph_t* graph, AXIS_SCALE scale) {
	graphics_t* g = (graphics_t*)graph->priv;
	axis_set_scale(g->yaxis, scale);
}


static dim_t graph_get_size_content(graphics_t* g) {
	double w = g->size.width - g->border.left - g->border.right;
	double h = g->size.height - g->border.top - g->border.bottom;
	return { w, h };
}

point_t graph_to_view(graphics_t* g, point_t p) {

	//graphics_t* g = (graphics_t*)graph->priv;

	double xt = axis_transform(g->xaxis, p.x);
	double yt = axis_transform(g->yaxis, p.y);

	dim_t content = graph_get_size_content(g);
	double x = g->border.left + xt * content.width;
	double y = g->border.top + content.height * (1 - yt);

	return { x, y };

}

void graphics_set_size(graphics_t* g, dim_t size) {
	g->size.width = size.width;
	g->size.height = size.height;
}

void graph_set_size(graph_t* graph, dim_t size) {
	graphics_t* g = (graphics_t*) graph->priv;
	g->size.width = size.width;
	g->size.height = size.height;
}

void graph_set_border(graph_t* graph, border_t border) {
	graphics_t* g = (graphics_t*) graph->priv;
	g->border.top = border.top;
	g->border.bottom = border.bottom;
	g->border.left = border.left;
	g->border.right = border.right;
}

void graph_set_xlimit(graph_t* graph, limit_t xlimit) {
	graphics_t* g = (graphics_t*)graph->priv;
	axis_set_limit(g->xaxis, xlimit);
}

void graph_set_ylimit(graph_t* graph, limit_t ylimit) {
	graphics_t* g = (graphics_t*)graph->priv;
	axis_set_limit(g->yaxis, ylimit);
}

row_t* graph_add_row(graph_t* graph, double* xValues, double* yValues, int length) {

	graphics_t* g = (graphics_t*)graph->priv;
	row_t* row = row_create(xValues, yValues, length);
	data_add_row(g->data, row);
	
	limit_t xlim = data_get_xlimit(g->data);
	xlim = axis_round_limit(g->xaxis, xlim);
	axis_set_limit(g->xaxis, xlim);

	limit_t ylim = data_get_ylimit(g->data);
	ylim = axis_round_limit(g->yaxis, ylim);
	axis_set_limit(g->yaxis, ylim);

	return row;
}

row_t* graph_add_row_simple(graph_t* graph, double* yvalues, int length) {
	double* xvalues = (double*)malloc(length * sizeof(double));
	for (int i = 0; i < length; i++) {
		xvalues[i] = i;
	}
	return graph_add_row(graph, xvalues, yvalues, length);
}

void graph_print(graph_t* graph) {
	graphics_t* g = (graphics_t*)graph->priv;
	printf("Graph\n");
	axis_print(g->xaxis);
	axis_print(g->yaxis);
}

DWORD WINAPI win_thread(LPVOID lpParameter) {
	graph_t* g = (graph_t*)lpParameter;
	graphics_t* graphics = (graphics_t*)g->priv;
	win_t* win = win_create(graphics);
	win_message_loop();
	return 0;
}

void graph_show(graph_t* g) {

	HANDLE hThread = CreateThread(NULL, 0, win_thread, g, 0, NULL);

	WaitForSingleObject(hThread, INFINITE);
}

void graph_show_non_blocking(graph_t* g) {

	HANDLE hThread = CreateThread( NULL, 0, win_thread, g, 0, NULL	);

}

void graph_set_timer(graph_t* graph, int timeMillis) {
	graphics_t* g = (graphics_t*)graph->priv;
	g->timerTimeMillis = timeMillis;
}

void graph_update_row(row_t* row) {
	row_update_points(row);
}

