#include <stdlib.h>
#include "windows.h"
#include "graph.h"

/*
	
	Add header file:
	C/C++ -> General -> Additional Include Directories
	Add directory containing file graph.h

	Add lib file:
	Linker -> Input -> Additional Dependencies
	Add file graph.lib

*/

void set_random_data(double* v, int length) {
	for (int i = 0; i < length; i++) {
		v[i] = ((double)rand()) / RAND_MAX * 10.0;
	}
}

void test_change_data() {

	double* v = (double*)malloc(500 * sizeof(double));
	set_random_data(v, 500);
	graph_t* g = graph_create();
	row_t* row = graph_add_row_simple(g, v, 500);
	graph_set_timer(g, 20);
	graph_show_non_blocking(g);

	while (true) {
		Sleep(100);
		set_random_data(v, 500);
		graph_update_row(row);
	}
}

int main() {
	test_change_data();
}