#include "private.h"

graph_t* graph_create() {
	graph_t* g = (graph_t*)malloc(sizeof(graph_t));
	g->priv = graphics_create();
	return g;
}

void graph_set_title(graph_t* graph, const wchar_t* title) {
	graphics_t* g = (graphics_t*)graph->priv;


}

void graph_set_xscale(graph_t* graph, AXIS_SCALE scale) {
	graphics_t* g = (graphics_t*)graph->priv;
	graphics_set_xscale(g, scale);
}

void graph_set_yscale(graph_t* graph, AXIS_SCALE scale) {
	graphics_t* g = (graphics_t*)graph->priv;
	graphics_set_yscale(g, scale);
}

void graph_set_size(graph_t* graph, dim_t size) {
	graphics_t* g = (graphics_t*) graph->priv;
	graphics_set_size(g, size);
}

void graph_set_border(graph_t* graph, border_t border) {
	graphics_t* g = (graphics_t*) graph->priv;
	graphics_set_border(g, border);
}

void graph_set_xlimit(graph_t* graph, limit_t lim) {

	// check limit
	if (lim.max <= lim.min) {
		printf("[WARN] limit.max must be > limit.min");
		return;
	}
	graphics_t* g = (graphics_t*)graph->priv;
	graphics_set_xlimit(g, lim);
}

void graph_set_ylimit(graph_t* graph, limit_t lim) {

	// check_limit
	if (lim.max <= lim.min) {
		printf("[WARN] limit.max must be > limit.min");
		return;
	}
	graphics_t* g = (graphics_t*)graph->priv;
	graphics_set_ylimit(g, lim);
}

row_t* graph_add_row(graph_t* graph, double* xvalues, double* yvalues, int length) {
	graphics_t* g = (graphics_t*)graph->priv;
	return graphics_add_row(g, xvalues, yvalues, length);
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

