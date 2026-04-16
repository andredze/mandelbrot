#include <stdlib.h>
#include <stdio.h>
#include <immintrin.h>
#include "graphics.h"
#include "mandelbrot.h"
#include "mandelbrot_test.h"

//------------------------------------------------------------------//

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

	const char* test_filename = DEFAULT_TEST_FILENAME;

	if (argc == 2)
	{
		test_filename = argv[1];
	}

	app.screen_surface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
	
	GetColorTable(&app);

	MakeTests(&app);
	
	SDL_FreeSurface(app.screen_surface);
	app.screen_surface = NULL;

	if (WriteTestData(&app, test_filename) == 1)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
#endif
}

//------------------------------------------------------------------//