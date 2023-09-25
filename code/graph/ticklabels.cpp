#include "private.h"

ticklabels_t* ticklabels_create() {
	ticklabels_t* labels = (ticklabels_t*)malloc(sizeof(ticklabels_t));
	*labels = {};
	labels->format = (string_t) malloc(10 * sizeof(wchar_t));
	wcscpy_s(labels->format, 10, L"%g");
	return labels;
}

void ticklabels_update(ticklabels_t* labels, double* values, int length) {

	for (int i = 0; i < labels->length; i++) {
		SafeFree(labels->texts[i]);
	}
	SafeFree(labels->values);

	// copy values (von ticks entkoppeln)
	labels->values = (double*)malloc(length * sizeof(double));
	memcpy(labels->values, values, length * sizeof(double));

	labels->texts = (string_t*)malloc(length * sizeof(string_t));
	for (int i = 0; i < length; i++) {
		double value = values[i];
		string_t buffer = (string_t)malloc(20 * sizeof(wchar_t));
		swprintf(buffer, 20, labels->format, value);				// wsprintf : no float
		labels->texts[i] = buffer;
	}
	labels->length = length;

}