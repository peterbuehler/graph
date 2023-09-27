#pragma once
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <float.h>
#include <wchar.h>

#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>

#include "graph.h"

// makros -----------------------------------------------------------

#define SafeFree(p) { \
	if ((p) != NULL) { \
		free(p); \
	} \
}

#define PrintArray(a, l) {\
	printf("[ "); \
	for (int i=0; i<(l); i++) { \
		printf("%g ", a[i]); \
	}\
	printf("]\n"); \
}

#define PrintLimit(a) { \
	printf("Limit min=%g max=%g\n", a.min, a.max); \
}


// misc -------------------------------------------------------------

typedef wchar_t* string_t;

typedef struct point_t {
	double x, y;
} point_t;


// util -------------------------------------------------------------

double util_round_up(double value, double step);
double util_round_down(double value, double step);
limit_t util_get_limit(double* values, int length);
limit_t util_round_limit(limit_t lim, AXIS_SCALE scale);


// ticks ------------------------------------------------------------

typedef void (*calc_ticks_fun_t) (struct ticks_t*, limit_t limit);

typedef struct ticks_t {
	AXIS_SCALE scale;
	calc_ticks_fun_t calc_ticks_fun;
	double* values;
	int length;
} ticks_t;


ticks_t* ticks_create(AXIS_SCALE scale);
void ticks_set_scale(ticks_t* ticks, AXIS_SCALE scale);
void ticks_calculate(ticks_t *ticks , limit_t limit);
void ticks_print(ticks_t* ticks);


// minorticks -------------------------------------------------------

typedef void (*calc_minorticks_fun_t) (struct minorticks_t*, limit_t limit);

typedef struct minorticks_t {
	AXIS_SCALE scale;
	calc_minorticks_fun_t calc_ticks_fun;
	double* values;
	int length;
} minorticks_t;

minorticks_t* minorticks_create(AXIS_SCALE scale);
void minorticks_set_scale(minorticks_t* ticks, AXIS_SCALE scale);
void minorticks_calculate(minorticks_t* ticks, limit_t limit);
void minorticks_print(minorticks_t* ticks);


// ticklabel -------------------------------------------------------.

typedef struct ticklabels_t {
	string_t format;
	string_t* texts;
	double* values;
	int length;
} ticklabels_t;

ticklabels_t* ticklabels_create();
void ticklabels_update(ticklabels_t* ticklabel, double* values, int length);


// axis -------------------------------------------------------------

typedef double (*transform_fun_t) (struct axis_t*, double);

typedef struct axis_t {
	AXIS_SCALE scale;
	transform_fun_t transform_fun;
	limit_t limit;
	ticks_t* ticks;
	minorticks_t* minorticks;
	ticklabels_t* ticklabels;
} axis_t;

axis_t* axis_create(AXIS_SCALE scale);
void axis_set_scale(axis_t* axis, AXIS_SCALE scale);
void axis_set_limit(axis_t* axis, limit_t limit);  
void axis_update_ticks(axis_t* axis);
double axis_transform(axis_t* axis, double d);

void axis_print(axis_t* axis);


// datarow ------------------------------------------------------------

typedef struct datarow_t {  // private struct
	double* xvalues;
	double* yvalues;
	point_t* points;
	int length;
	CRITICAL_SECTION lock;
} datarow_t;

row_t* row_create(double* xvalues, double* yvalues, int length);
void row_update_points(row_t* row);
void row_get_points(row_t* row, point_t** points, int* length);

// data ---------------------------------------------------------------

typedef struct data_t {
	row_t** rows;
	int length;
} data_t;

data_t* data_create();
void data_add_row(data_t* data, row_t* row);
limit_t data_get_xlimit(data_t* data);
limit_t data_get_ylimit(data_t* data);


// graph --------------------------------------------------------------

#define MAX_TITLE_LENGTH 50

typedef struct graphics_t {  // priv member graph
	dim_t size;
	border_t border;
	axis_t* xaxis;
	axis_t* yaxis;
	data_t* data;
	wchar_t title[MAX_TITLE_LENGTH];
	int timerTimeMillis;
} graphics_t;

graphics_t* graphics_create();
void graphics_set_size(graphics_t* graph, dim_t size); 
void graphics_set_border(graphics_t* g, border_t size);
void graphics_set_title(graphics_t* graph, wchar_t* title);
void graphics_set_xlimit(graphics_t* g, limit_t lim);
void graphics_set_ylimit(graphics_t* g, limit_t lim);
void graphics_set_xscale(graphics_t* g, AXIS_SCALE scale);
void graphics_set_yscale(graphics_t* g, AXIS_SCALE scale);
point_t graphics_to_view(graphics_t* g, point_t p);
row_t* graphics_add_row(graphics_t* g, double* xvalues, double* yvalues, int length);


void graph_print(graph_t* graph);


// engine ------------------------------------------------------------------------

#define FONT_SIZE_TITLE 14
#define FONT_SIZE_TICKLABELS 12
#define BRUSH_COUNT_ROWS 6

typedef struct engine_t {
	graphics_t* graphics;
	ID2D1Factory* factory;
	ID2D1HwndRenderTarget* target;
	ID2D1SolidColorBrush* blackBrush;
	ID2D1SolidColorBrush* redBrush;
	ID2D1SolidColorBrush* grayBrush;
	ID2D1SolidColorBrush* whiteBrush;
	ID2D1SolidColorBrush* brushes[BRUSH_COUNT_ROWS];
	IDWriteFactory* writeFactory;
	IDWriteTextFormat* textFormatTitle;
	IDWriteTextFormat* textFormatXTickLabels;
	IDWriteTextFormat* textFormatYTickLabels;
} engine_t;

engine_t* engine_create(graphics_t* g);
void engine_create_target(engine_t* engine, HWND hwnd);
void engine_create_brush(engine_t* engine);
void engine_create_text(engine_t* engine);
void engine_paint(engine_t* engine);
void engine_window_resized(engine_t* engine, int width, int height);
void engine_release(engine_t* engine);

// window ---------------------------------------------------------------------

typedef struct win_t {
	HWND hWnd;
	HANDLE hThread;
	engine_t* engine;
} win_t;

win_t* win_create(graphics_t* graphics);
void win_message_loop();








