#ifndef MANDELBROT_TEST_H
#define MANDELBROT_TEST_H

//------------------------------------------------------------------//

#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>

//------------------------------------------------------------------//

struct AppCtx;

//------------------------------------------------------------------//

const int 	 TESTS_COUNT   			 = 20;
const int 	 TEST_ITERATIONS_COUNT   = 2000;
const size_t MHZ				     = 1000000;
const size_t PROCESSOR_TSC_FREQUENCY = 3187 * MHZ; // Hz

//------------------------------------------------------------------//

void MakeTests(struct AppCtx* app);

//------------------------------------------------------------------//

#endif /* MANDELBROT_TEST_H */