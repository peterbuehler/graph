#pragma once

// lib files needed
#pragma comment(lib, "d2d1.lib")  
#pragma comment(lib, "Dwrite.lib")

// public interface

typedef struct dim_t {
	double width, height;
} dim_t;

typedef struct limit_t {
	double min, max;
} limit_t;

typedef struct border_t {
	double top, right, bottom, left;
} border_t;

typedef struct row_t {
	void* priv;
} row_t;

typedef struct graph_t {
	void* priv;
} graph_t;

enum AXIS_SCALE { SCALE_LIN, SCALE_LOG };

// create graph
graph_t* graph_create();

// set title of graph
void graph_set_title(graph_t* graph, const wchar_t* title);

// set window size in pixel
void graph_set_size(graph_t* graph, dim_t size);

// set border size in pixel
void graph_set_border(graph_t* graph, border_t border);

// set scale (lin/log) of xaxis
void graph_set_xscale(graph_t* graph, AXIS_SCALE scale);

// set scale (lin/log) of yaxis
void graph_set_yscale(graph_t* graph, AXIS_SCALE scale);

// set limit of xaxis
void graph_set_xlimit(graph_t* graph, limit_t xlimit);

// set limit of yaxis
void graph_set_ylimit(graph_t* graph, limit_t ylimit);

// add datarow to graph
row_t* graph_add_row(graph_t* graph, double* xvalues, double* yvalues, int length);

// add datarow to graph, xvalues will be 0..(length-1)
row_t* graph_add_row_simple(graph_t* graph, double* yvalues, int length);

// set repaint cycle of window
void graph_set_timer(graph_t* graph, int timeMillis);

// show window with thread blocking
void graph_show(graph_t* graph);

// show window with thread non blocking
void graph_show_non_blocking(graph_t* graph);

// update datarow
void graph_update_row(row_t* row);





