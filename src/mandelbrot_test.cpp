#include "mandelbrot_test.h"
#include "graphics.h"
#include "mandelbrot.h"
#include "time_stamp.h"

//------------------------------------------------------------------//

static inline __attribute__((always_inline)) void
MakeTestIteration(AppCtx_t* app)
{
#if defined(_AVX)
	MandelbrotDrawIntrinsics512(app);

#elif defined(_ARRAYS)
	MandelbrotDrawUnrolledWithFunctions(app);

#elif defined(_NAIVE)
	MandelbrotDrawUnoptimized(app);
#endif /* _AVX */
}

//------------------------------------------------------------------//

void MakeTests(struct AppCtx* app)
{
	// warmup cache
	for (int test_number = 0; test_number < WARMUP_TESTS_COUNT; test_number++)
	{
		for (int i = 0; i < TEST_ITERATIONS_COUNT; i++)
		{
			MakeTestIteration(app);
		}
	}

    for (int test_number = 0; test_number < TESTS_COUNT; test_number++)
	{
		uint64_t start_cycles = GetTscStart();
	
		for (int i = 0; i < TEST_ITERATIONS_COUNT; i++)
		{
			MakeTestIteration(app);
		}
		
		uint64_t end_cycles = GetTscEnd();

		uint64_t cycles = end_cycles - start_cycles;
		
		app->tests_cycles[test_number] = cycles;

		fprintf(stderr, "total cycles = %zu\n", cycles);
	}
}

//------------------------------------------------------------------//

int WriteTestData(AppCtx_t* app, const char* test_filename)
{
	FILE* test_data_file = fopen(test_filename, "w");

	if (test_data_file == NULL)
	{
		PRINTERR("Failed to open file %s", test_filename);

		return 1;
	}

	for (int i = 0; i < TESTS_COUNT; i++)
	{
		fprintf(test_data_file, "%zu\n", app->tests_cycles[i]);
	}

	fclose(test_data_file);

	return 0;
}

//------------------------------------------------------------------//