#include "mandelbrot.h"
#include "math.h"
#include <avxintrin.h>

//——————————————————————————————————————————————————————————————————————————————————————————

GfxErr_t MandelbrotDraw(AppCtx_t* app)
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
            
            if (iters == MANDELBROT_MAX_ITERS)
            {
                SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
            }
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

const float COORD_X_DIFF = (float) COORD_X_STEP_COEFF / SCREEN_WIDTH;

const float MM_COORD_X_START[MM_SIZE] = {COORD_X_SHIFT + 0 * COORD_X_DIFF,
                                         COORD_X_SHIFT + 1 * COORD_X_DIFF,
                                         COORD_X_SHIFT + 2 * COORD_X_DIFF,
                                         COORD_X_SHIFT + 3 * COORD_X_DIFF,
                                         COORD_X_SHIFT + 4 * COORD_X_DIFF,
                                         COORD_X_SHIFT + 5 * COORD_X_DIFF,
                                         COORD_X_SHIFT + 6 * COORD_X_DIFF,
                                         COORD_X_SHIFT + 7 * COORD_X_DIFF};

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

GfxErr_t MandelbrotDrawIntrinsics(AppCtx_t* app)
{
    assert(app);

    for (int pixel_y = 0; pixel_y < SCREEN_HEIGHT; pixel_y++)
    {
        __m256 coord_y_start     = _mm256_set1_ps(COORD_Y_SHIFT + COORD_Y_STEP_COEFF * ((float) pixel_y / SCREEN_HEIGHT));
        __m256 coord_x_start     = _mm256_load_ps(MM_COORD_X_START);
        __m256 coord_x_increment = _mm256_set1_ps(COORD_X_DIFF * MM_SIZE);
        
        for (int pixel_x = 0; pixel_x < SCREEN_WIDTH; pixel_x += MM_SIZE)
        {
            __m256 coord_x = _mm256_setzero_ps();
            __m256 coord_y = _mm256_setzero_ps();

            coord_x = _mm256_add_ps(coord_x, coord_x_start);
            coord_y = _mm256_add_ps(coord_y, coord_y_start);
            
            int iters      [MM_SIZE] = {0};
            int is_unstable[MM_SIZE] = {0};

            for (;;)
            {
                int is_all_unstable = 1;

                for (int i = 0; i < MM_SIZE; i++) { is_all_unstable *= is_unstable[i]; }

                if (is_all_unstable) { break; }
            
                __m256 coord_x_squared = _mm256_mul_ps(coord_x, coord_x);
                __m256 coord_y_squared = _mm256_mul_ps(coord_y, coord_y);
                __m256 radius_vector_squared = _mm256_add_ps(coord_x_squared, coord_y_squared);
                
                for (int i = 0; i < MM_SIZE; i++) { if (radius_vector_squared[i] > STABLE_POINTS_CIRCLE_RADIUS_SQUARED) { is_unstable[i] = 1; } }
                
                __m256 mm_number_2 = _mm256_set1_ps(2.0f);

                coord_y = _mm256_mul_ps(coord_y, mm_number_2);
                coord_y = _mm256_mul_ps(coord_y, coord_x);
                coord_y = _mm256_add_ps(coord_y, coord_y_start);
                
                coord_x = _mm256_sub_ps(coord_x_squared, coord_y_squared);
                coord_x = _mm256_add_ps(coord_x, coord_x_start);
                
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

            coord_x_start = _mm256_add_ps(coord_x_start, coord_x_increment);
        }
    }

    return GFX_SUCCESS;
}

//——————————————————————————————————————————————————————————————————————————————————————————