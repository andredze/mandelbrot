#include <stdlib.h>
#include <stdio.h>
#include <immintrin.h>
#include "graphics.h"
#include "mandelbrot.h"
#include "mandelbrot_test.h"

//------------------------------------------------------------------//

// TODO: avx 512 instructions?
// TODO: choose version from argv instead of ifdef

int main(int argc, char* argv[])
{
	AppCtx_t app = {};

#ifdef GRAPHICS
	do
	{
		if (GfxCtor(&app) != GFX_SUCCESS)
			break;

		while (app.is_running)
		{
			if (GfxUpdate(&app) != GFX_SUCCESS)
				break;

			if (GfxDraw(&app) != GFX_SUCCESS)
				break;
		}
	}
	while (0);

	GfxDtor(&app);
	
	return EXIT_SUCCESS;

#else
	
	fprintf(stderr, "Test mode\n");

	app.screen_surface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
	
	MakeTests(&app);
	
	SDL_FreeSurface(app.screen_surface);
	app.screen_surface = NULL;

	const char* test_filename = "data/test.csv";

	if (argc == 2)
	{
		test_filename = argv[1];
	}

	FILE* test_data_file = fopen(test_filename, "w");

	if (test_data_file == NULL)
	{
		PRINTERR("Failed to open file %s", test_filename);

		return EXIT_FAILURE;
	}

	for (int i = 0; i < TESTS_COUNT; i++)
	{
		fprintf(test_data_file, "%zu\n", app.tests_cycles[i]);
	}

	fclose(test_data_file);

	return EXIT_SUCCESS;
#endif
}

//------------------------------------------------------------------//