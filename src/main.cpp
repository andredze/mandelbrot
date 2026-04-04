#include <stdlib.h>
#include <stdio.h>
#include "graphics.h"

//------------------------------------------------------------------//

int main()
{
	AppCtx_t app = {};
	
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
}

//------------------------------------------------------------------//