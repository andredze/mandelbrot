#ifndef TIME_STAMP_H
#define TIME_STAMP_H

//------------------------------------------------------------------//

#include <stdint.h>
#include <x86intrin.h>

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

#endif /* TIME_STAMP_H */