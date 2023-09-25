#include "private.h"

#define PI 3.14156

void test_util() {
	double a = 1235; double b = 50;
	printf("round up %f %f = %f\n", a, b, round_up(a, b));
	printf("round down %f %f = %f\n", a, b, round_down(a, b));
	a = 0.3; b = 0.5;
	printf("round up %f %f = %f\n", a, b, round_up(a, b));
	printf("round down %f %f = %f\n", a, b, round_down(a, b));
}

void test_ticks_lin() {
	ticks_t* ticks = ticks_create(SCALE_LIN);
	ticks->calc_ticks_fun(ticks, { 12, 123 });  // Ticks: [ 20 40 60 80 100 120 ]
	ticks_print(ticks);
	ticks->calc_ticks_fun(ticks, { 20, 140 });  // Ticks: [ 20 40 60 80 100 120 140 ]
	ticks_print(ticks);
}

void test_ticks_log() {
	ticks_t* ticks = ticks_create(SCALE_LOG);
	ticks->calc_ticks_fun(ticks, { 12, 123 });  // Ticks: [ 100 ]
	ticks_print(ticks);
	ticks->calc_ticks_fun(ticks, { 0.3, 123 });  // Ticks: [1, 10, 100]
	ticks_print(ticks);
	ticks->calc_ticks_fun(ticks, { 0, 123 });   // illegal value for limit
	ticks_print(ticks);

}

void test_minor_ticks() {

	minorticks_t* ticks = minorticks_create(SCALE_LOG);
	limit_t lim = { 12, 123 };
	PrintLimit(lim);
	ticks->calc_ticks_fun(ticks, lim);		// MinorTicks: [ 20 30 40 50 60 70 80 90 100 ]
	minorticks_print(ticks);

	lim = { 0.657, 123  };
	PrintLimit(lim);
	ticks->calc_ticks_fun(ticks, lim);  
	minorticks_print(ticks);				// MinorTicks: [0.7 0.8 0.9 1 2 3 4 5 6 7 8 9 10 20 30 40 50 60 70 80 90 100]

	lim = { 23, 5123 };
	PrintLimit(lim);
	ticks->calc_ticks_fun(ticks, lim);
	minorticks_print(ticks);				// MinorTicks: [ 30 40 50 60 70 80 90 100 200 300 400 500 600 700 800 900 1000 2000 3000 4000 5000 ]	

}

double* create_sin(int length, double freq, double ampl) {
	double* values = (double*)malloc(length * sizeof(double));
	for (int i = 0; i < length; i++) {
		values[i] = ampl * sin(i * freq * PI / length);
	}
	return values;
}

double* xvalues(int length) {
	double* values = (double*)malloc(length * sizeof(double));
	for (int i = 0; i < length; i++) {
		values[i] = i;
	}
	return values;
}

void test_graph1() {

	graph_t* g = graph_create();
	int length = 950;
	double* yvalues = create_sin(length, 20, 1);
	row_t* row = graph_add_row(g, xvalues(length), yvalues, length);
	graph_print(g);
	graph_show(g);

}

void test_graph_log() {

	graph_t* g = graph_create();
	graph_set_title(g, L"Logarithmische Y-Achse");
	graph_set_yscale(g, SCALE_LOG);
	double y[] = { 0.01, 30, 500, 0.5 };
	graph_add_row_simple(g, y, 4);
	// graph_print(g);
	graph_show(g);

}

void set_random_data(double* v, int length) {
	for (int i = 0; i < length; i++) {
		v[i] = ((double) rand()) / RAND_MAX * 10.0;
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

	//test_util();
	//test_ticks_lin();
	//test_ticks_log();
	//test_minor_ticks();
	//test_graph1();
	//test_graph_log();
	test_change_data();
	

	
}