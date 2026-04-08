#include "mandelbrot_test.h"
#include "graphics.h"
#include "mandelbrot.h"

//------------------------------------------------------------------//

static inline __attribute__((always_inline)) uint64_t
GetTscStart(void)
{
	_mm_lfence();

	return __rdtsc();
}

//------------------------------------------------------------------//

static inline __attribute__((always_inline)) uint64_t 
GetTscEnd(void)
{
	uint32_t cpuid = 0;
	uint64_t cycles = __rdtscp(&cpuid);
	
	_mm_lfence();

	return cycles;
}

//------------------------------------------------------------------//

static inline __attribute__((always_inline)) void
MakeTestIteration(AppCtx_t* app)
{
#if defined(_SHOW_FPS)
	uint64_t start_ticks_in = GetTscStart();
#endif /* _SHOW_FPS */

#if defined(_TEST_AVX)
	MandelbrotDrawIntrinsics(app);

#elif defined(_TEST_ARRAYS)
	MandelbrotDrawUnrolledWithFunctions(app);

#elif defined(_TEST_UNOPTIMIZED)
	MandelbrotDrawUnoptimized(app);

#endif /* _TEST_AVX */

#if defined(_SHOW_FPS)
// TODO: fps on screen with text?

	uint64_t end_ticks_in = GetTscEnd();

	float fps = 1 / ((float) (end_ticks_in - start_ticks_in) / PROCESSOR_TSC_FREQUENCY);

	fprintf(stderr, "FPS = %.2f\n", fps);
#endif /* _SHOW_FPS */
}

//------------------------------------------------------------------//

void MakeTests(struct AppCtx* app)
{
    for (int test_number = 0; test_number < TESTS_COUNT; test_number++)
	{
		uint64_t start_ticks = GetTscStart();
	
		for (int i = 0; i < TEST_ITERATIONS_COUNT; i++)
		{
			MakeTestIteration(app);
		}
		
		uint64_t end_ticks = GetTscEnd();

		uint64_t cycles = end_ticks - start_ticks;
		
		app->tests_cycles[test_number] = cycles;

		fprintf(stderr, "total ticks = %zu\n", cycles);
	}
}

//------------------------------------------------------------------//