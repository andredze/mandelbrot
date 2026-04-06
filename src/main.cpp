#include <stdlib.h>
#include <stdio.h>
#include "graphics.h"
#include "mandelbrot.h"

//------------------------------------------------------------------//

int main()
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
	
	AppCtx_t app = {};

	if (GfxCtor(&app) != GFX_SUCCESS)
		return EXIT_FAILURE;

	MandelbrotDrawIntrinsics(&app);

	GfxDtor(&app);

	return EXIT_SUCCESS;
#endif
}

//------------------------------------------------------------------//