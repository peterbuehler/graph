#include "private.h"

row_t* row_create(double* xvalues, double* yvalues, int length) {

	row_t* row = (row_t*)malloc(sizeof(row_t));
	datarow_t* drow = (datarow_t*)malloc(sizeof(datarow_t));

	*drow = {};

	InitializeCriticalSection(&(drow->lock));
	
	row->priv = drow;
	drow->xvalues = xvalues;
	drow->yvalues = yvalues;
	drow->length = length;

	row_update_points(row);

	return row;
}


void row_update_points(row_t* row) {

	datarow_t* drow = (datarow_t*) row->priv;

	EnterCriticalSection(&(drow->lock));

	SafeFree(drow->points);
	
	drow->points = (point_t*) malloc(drow->length * sizeof(point_t));
	for (int i = 0; i < drow->length; i++) {
		drow->points[i] = { drow->xvalues[i], drow->yvalues[i] };
	}

	LeaveCriticalSection(&(drow->lock));
}

void row_get_points(row_t* row, point_t** points, int* length) {
	datarow_t* drow = (datarow_t*)row->priv;
	EnterCriticalSection(&(drow->lock));
	*points = drow->points;
	*length = drow->length;
	LeaveCriticalSection(&(drow->lock));
}
