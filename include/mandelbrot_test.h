#ifndef MANDELBROT_TEST_H
#define MANDELBROT_TEST_H

//------------------------------------------------------------------//

#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>

//------------------------------------------------------------------//

struct AppCtx;

//------------------------------------------------------------------//

const int 	 WARMUP_TESTS_COUNT      = 2;
const int 	 TESTS_COUNT   			 = 7;
const int 	 TEST_ITERATIONS_COUNT   = 250;
const size_t MHZ				     = 1000000;
const size_t PROCESSOR_TSC_FREQUENCY = 3187 * MHZ; // Hz

const char * const DEFAULT_TEST_FILENAME = "data/test.csv";

//------------------------------------------------------------------//

void MakeTests    (struct AppCtx* app);
int  WriteTestData(struct AppCtx* app, const char* test_filename);

//------------------------------------------------------------------//

#endif /* MANDELBROT_TEST_H */