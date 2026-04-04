#include "mandelbrot.h"
#include "math.h"

//——————————————————————————————————————————————————————————————————————————————————————————

GfxErr_t MandelbrotDraw(AppCtx_t* app)
{
    assert(app);

    for (int pixel_y = 0; pixel_y < SCREEN_HEIGHT; pixel_y++)
    {
        float coord_y_start = 0.15 + COORD_Y_SHIFT + COORD_Y_STEP_COEFF * ((float) pixel_y / SCREEN_HEIGHT);
        
        for (int pixel_x = 0; pixel_x < SCREEN_WIDTH; pixel_x++)
        {
            float coord_x_start = -0.4 + COORD_X_SHIFT + COORD_X_STEP_COEFF * ((float) pixel_x / SCREEN_WIDTH),
                  coord_x       = coord_x_start,
                  coord_y       = coord_y_start;

            int iters = 0;

            for (; iters < MANDELBROT_MAX_ITERS; iters++)
            {
                float coord_x_squared = coord_x * coord_x,
                      coord_y_squared = coord_y * coord_y,
                      radius_vector_squared = coord_x_squared + coord_y_squared;

                if (radius_vector_squared > STABLE_POINTS_CIRCLE_RADIUS_SQUARED)
                {
                    break;
                }

                coord_y = 2 * coord_x * coord_y + coord_y_start;
                coord_x = coord_x_squared - coord_y_squared + coord_x_start;
            }
            
            if (iters == MANDELBROT_MAX_ITERS)
                SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
            else
            {
                float clr = sqrt(sqrt(iters / MANDELBROT_MAX_ITERS));
                SDL_SetRenderDrawColor(app->renderer, 255 * (clr+0.25), 255 - 50 * (iters % 40), 205 - iters * clr, 255);
            }

            SDL_RenderDrawPoint(app->renderer, pixel_x, pixel_y);
        }
    }

    return GFX_SUCCESS;
}

//——————————————————————————————————————————————————————————————————————————————————————————