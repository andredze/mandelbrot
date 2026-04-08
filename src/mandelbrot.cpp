#include "mandelbrot.h"
#include "math.h"
#include "graphics.h"
#include <avxintrin.h>

//——————————————————————————————————————————————————————————————————————————————————————————

static inline Uint32 MandelbrotGetColor(SDL_PixelFormat* format, int iters)
{
    Uint8 color_r = 0;
    Uint8 color_g = 0;
    Uint8 color_b = 0;

    if (iters < MANDELBROT_MAX_ITERS)
    {
        float clr = 255 * sqrt(sqrt( (float) iters / MANDELBROT_MAX_ITERS ));
        
        color_r = (Uint8) clr;
        color_g = (Uint8) (255 - 50 * (iters % 40));
        color_b = 205;
    }

    return SDL_MapRGB(format, color_r, color_g, color_b);
}

//——————————————————————————————————————————————————————————————————————————————————————————

GfxErr_t MandelbrotDrawIntrinsics(AppCtx_t* app)
{
    assert(app);

    __m256 mm_number_2f     = _mm256_set1_ps(2.0f);
    __m256 mm_x_increment   = _mm256_set1_ps(COORD_X_DIFF * MM_SIZE * app->x_zoom_span);
    __m256 mm_r_squared_max = _mm256_set1_ps(STABLE_POINTS_CIRCLE_RADIUS_SQUARED);

    for (int pixel_y = 0; pixel_y < SCREEN_HEIGHT; pixel_y++)
    {
        __m256 mm_x_start = _mm256_add_ps(_mm256_load_ps(MM_COORD_X_START), app->mm_x_key_shift); 
        __m256 mm_y_start = _mm256_add_ps(_mm256_set1_ps(COORD_Y_SHIFT + (float) pixel_y * COORD_Y_DIFF * app->x_zoom_span), app->mm_y_key_shift);
        
        for (int pixel_x = 0; pixel_x < SCREEN_WIDTH; pixel_x += MM_SIZE)
        {
            __m256 mm_x = mm_x_start;
            __m256 mm_y = mm_y_start;

            __m256i mm_iters = _mm256_setzero_si256();

            for (int n = 0; n < MANDELBROT_MAX_ITERS; n++)
            {
                __m256 mm_x_squared = _mm256_mul_ps(mm_x, mm_x);
                __m256 mm_y_squared = _mm256_mul_ps(mm_y, mm_y);
                __m256 mm_r_squared = _mm256_add_ps(mm_x_squared, mm_y_squared);

                // if (r[i]^2 <= max_r[i]^2) { mask[i] = -1 }
                __m256 mm_is_stable_mask = _mm256_cmp_ps(mm_r_squared, mm_r_squared_max, _CMP_LE_OQ);

                int is_stable_mask = _mm256_movemask_ps(mm_is_stable_mask);

                // if all points are unstable (outside the circle)
                if (is_stable_mask == 0)
                {
                    break;
                }

                // y = 2 * y * x + y_0
                mm_y = _mm256_fmadd_ps(_mm256_mul_ps(mm_number_2f, mm_y), mm_x, mm_y_start);
                
                // x = (x^2) - (y^2) + x_0
                mm_x = _mm256_add_ps(_mm256_sub_ps(mm_x_squared, mm_y_squared), mm_x_start);
                
                // // if   (mask == -1) { iters++    <=> iters - mask }
                // // else (mask ==  0) { do nothing <=> iters - mask }
                mm_iters = _mm256_sub_epi32(mm_iters, _mm256_castps_si256(mm_is_stable_mask));
            }
            
            // convert __m256i to integer array through memory
            // mem_addr must be aligned on a 32-byte boundary
            alignas(32) int iters[MM_SIZE] = {};

            _mm256_store_si256( (__m256i*) iters, mm_iters);

            for (int i = 0; i < MM_SIZE; i++)
            {
                Uint32 pixel_color = MandelbrotGetColor(app->screen_surface->format, iters[i]);
                
                GfxPutPixel(app->screen_surface, pixel_x + i, pixel_y, pixel_color);

                DPRINTF("%d %d %d\t", pixel_x + i, pixel_y, iters[i]);
            }

            DPRINTF("\n");

            mm_x_start = _mm256_add_ps(mm_x_start, mm_x_increment);
        }
    }

    return GFX_SUCCESS;
}

//——————————————————————————————————————————————————————————————————————————————————————————

GfxErr_t MandelbrotDrawUnoptimized(AppCtx_t* app)
{
    assert(app);

    for (int pixel_y = 0; pixel_y < SCREEN_HEIGHT; pixel_y++)
    {
        float coord_y_start = COORD_Y_SHIFT + COORD_Y_STEP_COEFF * ((float) pixel_y / SCREEN_HEIGHT),
              coord_x_start = COORD_X_SHIFT,
              coord_x_inc   = (float) COORD_X_STEP_COEFF / SCREEN_WIDTH;
        
        for (int pixel_x = 0; pixel_x < SCREEN_WIDTH; pixel_x++, coord_x_start += coord_x_inc)
        {
            float coord_x = coord_x_start,
                  coord_y = coord_y_start;

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

            Uint32 pixel_color = MandelbrotGetColor(app->screen_surface->format, iters);
            
            GfxPutPixel(app->screen_surface, pixel_x, pixel_y, pixel_color);

            DPRINTF("%d %d %d\n", pixel_x, pixel_y, iters);
        }
    }

    return GFX_SUCCESS;
}

//——————————————————————————————————————————————————————————————————————————————————————————

GfxErr_t MandelbrotDrawUnrolled(AppCtx_t* app)
{
    assert(app);

    for (int pixel_y = 0; pixel_y < SCREEN_HEIGHT; pixel_y++)
    {
        float coord_y_start[MM_SIZE] = {};
        float coord_x_start[MM_SIZE] = {};
        
        for (int i = 0; i < MM_SIZE; i++) { coord_y_start[i] = COORD_Y_SHIFT + COORD_Y_STEP_COEFF * ((float) pixel_y / SCREEN_HEIGHT); }
        for (int i = 0; i < MM_SIZE; i++) { coord_x_start[i] = COORD_X_SHIFT + i * (float) COORD_X_STEP_COEFF / SCREEN_WIDTH; }        
        
        for (int pixel_x = 0; pixel_x < SCREEN_WIDTH; pixel_x += MM_SIZE)
        {
            float coord_y[MM_SIZE] = {};
            float coord_x[MM_SIZE] = {};
            
            for (int i = 0; i < MM_SIZE; i++) { coord_y[i] = coord_y_start[i]; }
            for (int i = 0; i < MM_SIZE; i++) { coord_x[i] = coord_x_start[i]; }
            
            int iters      [MM_SIZE] = {0};
            int is_unstable[MM_SIZE] = {0};

            for (;;)
            {
                int is_all_unstable = 1;

            for (int i = 0; i < MM_SIZE; i++) { is_all_unstable *= is_unstable[i]; }

                if (is_all_unstable) { break; }

                float coord_x_squared      [MM_SIZE] = {};
                float coord_y_squared      [MM_SIZE] = {};
                float radius_vector_squared[MM_SIZE] = {};
                
                for (int i = 0; i < MM_SIZE; i++) { coord_x_squared[i] = coord_x[i] * coord_x[i]; }
                for (int i = 0; i < MM_SIZE; i++) { coord_y_squared[i] = coord_y[i] * coord_y[i]; }
                for (int i = 0; i < MM_SIZE; i++) { radius_vector_squared[i] = coord_x_squared[i] + coord_y_squared[i]; }
                
            for (int i = 0; i < MM_SIZE; i++) { if (radius_vector_squared[i] > STABLE_POINTS_CIRCLE_RADIUS_SQUARED) { is_unstable[i] = 1; } }
                
                for (int i = 0; i < MM_SIZE; i++) { coord_y[i] = 2 * coord_x[i] * coord_y[i] + coord_y_start[i]; }
                for (int i = 0; i < MM_SIZE; i++) { coord_x[i] = coord_x_squared[i] - coord_y_squared[i] + coord_x_start[i]; }
                
            for (int i = 0; i < MM_SIZE; i++) { if (!is_unstable[i]) { iters[i] += 1; } if (iters[i] >= MANDELBROT_MAX_ITERS) { is_unstable[i] = 1; }}
            }
            
            DPRINTF("Drawing points: ");

            for (int i = 0; i < MM_SIZE; i++)
            {
                Uint32 pixel_color = MandelbrotGetColor(app->screen_surface->format, iters[i]);
                
                GfxPutPixel(app->screen_surface, pixel_x + i, pixel_y, pixel_color);

                DPRINTF("%d %d %d\t", pixel_x + i, pixel_y, iters[i]);
            }

            DPRINTF("\n");

            for (int i = 0; i < MM_SIZE; i++) { coord_x_start[i] += MM_SIZE * (float) COORD_X_STEP_COEFF / SCREEN_WIDTH; }
        }
    }

    return GFX_SUCCESS;
}

//——————————————————————————————————————————————————————————————————————————————————————————

static inline void
MM_SetValue(float mm_dest[MM_SIZE], 
            float value)
{
    for (int i = 0; i < MM_SIZE; i++)
    {
        mm_dest[i] = value;
    }
}

//------------------------------------------------------------------//

static inline void
MM_Add(      float mm_dest[MM_SIZE], 
       const float mm_src1[MM_SIZE], 
       const float mm_src2[MM_SIZE])
{
    for (int i = 0; i < MM_SIZE; i++)
    {
        mm_dest[i] = mm_src1[i] + mm_src2[i];
    }
}

//------------------------------------------------------------------//

static inline void
MM_Sub(      float mm_dest[MM_SIZE], 
       const float mm_src1[MM_SIZE], 
       const float mm_src2[MM_SIZE])
{
    for (int i = 0; i < MM_SIZE; i++)
    {
        mm_dest[i] = mm_src1[i] - mm_src2[i];
    }
}

//------------------------------------------------------------------//

static inline void
MM_Mul(      float mm_dest[MM_SIZE],
       const float mm_src1[MM_SIZE],
       const float mm_src2[MM_SIZE])
{
    for (int i = 0; i < MM_SIZE; i++)
    {
        mm_dest[i] = mm_src1[i] * mm_src2[i];
    }
}

//------------------------------------------------------------------//

static inline void
MM_Cpy(      float mm_dest[MM_SIZE],
       const float mm_src [MM_SIZE])
{
    for (int i = 0; i < MM_SIZE; i++)
    {
        mm_dest[i] = mm_src[i];
    }
}

//------------------------------------------------------------------//

GfxErr_t MandelbrotDrawUnrolledWithFunctions(AppCtx_t* app)
{
    assert(app);

    for (int pixel_y = 0; pixel_y < SCREEN_HEIGHT; pixel_y++)
    {
        float coord_y_start    [MM_SIZE] = {};
        float coord_x_start    [MM_SIZE] = {};
        float coord_x_increment[MM_SIZE] = {};
        
        MM_SetValue(coord_y_start, COORD_Y_SHIFT + COORD_Y_STEP_COEFF * ((float) pixel_y / SCREEN_HEIGHT));
        MM_Cpy     (coord_x_start, MM_COORD_X_START);
        MM_SetValue(coord_x_increment, COORD_X_DIFF * MM_SIZE);
        
        for (int pixel_x = 0; pixel_x < SCREEN_WIDTH; pixel_x += MM_SIZE)
        {
            float coord_y[MM_SIZE] = {};
            float coord_x[MM_SIZE] = {};
            
            MM_Cpy(coord_y, coord_y_start);
            MM_Cpy(coord_x, coord_x_start);
            
            int iters      [MM_SIZE] = {0};
            int is_unstable[MM_SIZE] = {0};

            for (;;)
            {
                int is_all_unstable = 1;

                for (int i = 0; i < MM_SIZE; i++) { is_all_unstable *= is_unstable[i]; }

                if (is_all_unstable) { break; }

                float coord_x_squared      [MM_SIZE] = {};
                float coord_y_squared      [MM_SIZE] = {};
                float radius_vector_squared[MM_SIZE] = {};
                
                MM_Mul(coord_x_squared, coord_x, coord_x);
                MM_Mul(coord_y_squared, coord_y, coord_y);
                MM_Add(radius_vector_squared, coord_x_squared, coord_y_squared);
                
                for (int i = 0; i < MM_SIZE; i++) { if (radius_vector_squared[i] > STABLE_POINTS_CIRCLE_RADIUS_SQUARED) { is_unstable[i] = 1; } }
                
                MM_Mul(coord_y, coord_y, MM_NUMBER_2);
                MM_Mul(coord_y, coord_y, coord_x);
                MM_Add(coord_y, coord_y, coord_y_start);
                
                MM_Sub(coord_x, coord_x_squared, coord_y_squared);
                MM_Add(coord_x, coord_x, coord_x_start);
                
                for (int i = 0; i < MM_SIZE; i++) { if (!is_unstable[i]) { iters[i] += 1; } if (iters[i] >= MANDELBROT_MAX_ITERS) { is_unstable[i] = 1; }}
            }
            
            DPRINTF("Drawing points: ");
            
            for (int i = 0; i < MM_SIZE; i++)
            {
                if (iters[i] == MANDELBROT_MAX_ITERS)
                {
                    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
                }
                else
                {
                    float clr = sqrt(sqrt(iters[i] / MANDELBROT_MAX_ITERS));
                    SDL_SetRenderDrawColor(app->renderer, 255 * (clr+0.25), 255 - 50 * (iters[i] % 40), 205 - iters[i] * clr, 255);
                }

                SDL_RenderDrawPoint(app->renderer, pixel_x + i, pixel_y);

                DPRINTF("%d %d %d\t", pixel_x + i, pixel_y, iters[i]);
            }

            DPRINTF("\n");

            MM_Add(coord_x_start, coord_x_start, coord_x_increment);
        }
    }

    return GFX_SUCCESS;
}

//——————————————————————————————————————————————————————————————————————————————————————————
