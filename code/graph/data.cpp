#include "private.h"

data_t* data_create() {
	data_t* data = (data_t*)malloc(sizeof(data_t));
	*data = {};
	return data;
}

void data_add_row(data_t* data, row_t* row) {

	// first datarow
	if (data->length == 0) {
		data->rows = (row_t**)malloc(sizeof(row_t));
		data->rows[0] = row;
		data->length = 1;
		return;
	}

	// next datarows
	data->length = data->length + 1;
	data->rows = (row_t**)realloc(data->rows, data->length * sizeof(row_t*));
	data->rows[data->length - 1] = row;
}


limit_t data_get_xlimit(data_t* data) {

	double min = DBL_MAX;
	double max = DBL_MIN;

	for (int i = 0; i < data->length; i++) {
		row_t* row = data->rows[i];
		datarow_t* drow = (datarow_t*) row->priv;
		for (int j = 0; j < drow->length; j++) {
			double v = drow->xvalues[j];
			if (v > max) {
				max = v;
			}
			if (v < min) { 
				min = v; 
			}
		}
	}
	return { min, max };
}

limit_t data_get_ylimit(data_t* data) {

	double min = DBL_MAX;
	double max = DBL_MIN;

	for (int i = 0; i < data->length; i++) {
		row_t* row = data->rows[i];
		datarow_t* drow = (datarow_t*)row->priv;
		for (int j = 0; j < drow->length; j++) {
			double v = drow->yvalues[j];
			if (v > max) {
				max = v;
			}
			if (v < min) {
				min = v;
			}
		}
	}
	return { min, max };
}

