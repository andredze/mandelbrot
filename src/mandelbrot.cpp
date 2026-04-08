#include "mandelbrot.h"
#include "math.h"
#include "graphics.h"
#include <stdint.h>
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
        
        color_r = (Uint8) (255 - 50 * (iters % 40));
        color_b = (Uint8) clr;
        color_g = 205;
    }

    return SDL_MapRGB(format, color_r, color_g, color_b);
}

//——————————————————————————————————————————————————————————————————————————————————————————

GfxErr_t MandelbrotDrawIntrinsics(AppCtx_t* app)
{
    assert(app);

    __m256 mm_number_2f     = _mm256_set1_ps(2.0f);
    __m256 mm_r_squared_max = _mm256_set1_ps(STABLE_POINTS_CIRCLE_RADIUS_SQUARED);
    
    float delta_x = app->x_zoom_scale * 1 / SCREEN_WIDTH;

    __m256 mm_delta_x     = _mm256_set1_ps(delta_x);
    __m256 mm_x_increment = _mm256_set1_ps(MM_SIZE * delta_x);

    __m256 arr01234567         = _mm256_setr_ps(0, 1, 2, 3, 4, 5, 6, 7);
    __m256 mm_delta_x_01234567 = _mm256_mul_ps(arr01234567, mm_delta_x);

    for (int pixel_y = 0; pixel_y < SCREEN_HEIGHT; pixel_y++)
    {
        float x_start_first = app->center_point_x + app->x_zoom_scale * (-0.5f);

        __m256 mm_x_start = _mm256_add_ps(mm_delta_x_01234567, _mm256_set1_ps(x_start_first));
        __m256 mm_y_start = _mm256_set1_ps(app->center_point_y + app->y_zoom_scale * ((float) pixel_y * 1 / SCREEN_HEIGHT - 0.5f));

        // __m256 mm_x_start = _mm256_load_ps(MM_COORD_X_START); 
        // __m256 mm_y_start = _mm256_set1_ps(((float) pixel_y / SCREEN_HEIGHT - 0.5f) * STARTING_ZOOM_SCALE_Y 
        //                                     + STARTING_CENTER_POINT_Y);

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

#ifdef GRAPHICS
    float coord_x_inc = app->x_zoom_scale * 1 / SCREEN_WIDTH;
#else
    float coord_x_inc = STARTING_ZOOM_SCALE_X * 1 / SCREEN_WIDTH;
#endif /* GRAPHICS */

    for (int pixel_y = 0; pixel_y < SCREEN_HEIGHT; pixel_y++)
    {
#ifdef GRAPHICS
        float coord_y_start = app->center_point_y + app->y_zoom_scale * ((float) pixel_y * 1 / SCREEN_HEIGHT - 0.5f);
        float coord_x_start = app->center_point_x + app->x_zoom_scale * (-0.5f);
#else
        float coord_y_start = STARTING_CENTER_POINT_Y + STARTING_ZOOM_SCALE_Y * ((float) pixel_y * 1 / SCREEN_HEIGHT - 0.5f);
        float coord_x_start = STARTING_CENTER_POINT_X + STARTING_ZOOM_SCALE_X * (-0.5f);
#endif /* GRAPHICS */

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

static inline void
MM_Cmp_LE(      int   mm_mask[MM_SIZE],
          const float mm_op1 [MM_SIZE],
          const float mm_op2 [MM_SIZE])
{
    for (int i = 0; i < MM_SIZE; i++) 
    {
        if (mm_op1[i] <= mm_op2[i])
        {
            int32_t all_bits_one = -1;
            memcpy(&mm_mask[i], &all_bits_one, sizeof(uint32_t));
        }
        else
        {
            mm_mask[i] = 0.0f;
        }
    }
}

//------------------------------------------------------------------//

static inline int
MM_MoveMask(const int mm_mask[MM_SIZE])
{
    int mask = 0;

    for (int i = 0; i < MM_SIZE; i++)
    {
        // set bit number i to 1
        mask |= (mm_mask[i] < 0) ? (1 << i) : 0;
    }

    return mask;
}

//------------------------------------------------------------------//

static inline void
MM_SubInt(      int32_t mm_dest[MM_SIZE],
          const int32_t mm_op1 [MM_SIZE],
          const int32_t mm_op2 [MM_SIZE])
{
    for (int i = 0; i < MM_SIZE; i++)
    {
        mm_dest[i] = mm_op1[i] - mm_op2[i];
    }
}

//------------------------------------------------------------------//

GfxErr_t MandelbrotDrawUnrolledWithFunctions(AppCtx_t* app)
{
    assert(app);

    float coord_x_increment[MM_SIZE] = {};
    MM_SetValue(coord_x_increment, COORD_X_DIFF * MM_SIZE);

    float max_radius_squared[MM_SIZE] = {};
    MM_SetValue(max_radius_squared, STABLE_POINTS_CIRCLE_RADIUS_SQUARED);
    
    float number_two[MM_SIZE] = {};
    MM_SetValue(number_two, 2.0f);

    for (int pixel_y = 0; pixel_y < SCREEN_HEIGHT; pixel_y++)
    {
        float coord_y_start[MM_SIZE] = {};
        MM_SetValue(coord_y_start, COORD_Y_SHIFT + (float) pixel_y * COORD_Y_DIFF);
        
        float coord_x_start[MM_SIZE] = {};
        MM_Cpy(coord_x_start, MM_COORD_X_START);
        
        for (int pixel_x = 0; pixel_x < SCREEN_WIDTH; pixel_x += MM_SIZE)
        {
            float coord_y[MM_SIZE] = {};
            MM_Cpy(coord_y, coord_y_start);
            
            float coord_x[MM_SIZE] = {};
            MM_Cpy(coord_x, coord_x_start);
            
            int iters               [MM_SIZE] = {0};
            int is_stable_mask_array[MM_SIZE] = {0};

            for (int n = 0; n < MANDELBROT_MAX_ITERS; n++)
            {
                float coord_x_squared[MM_SIZE] = {};
                MM_Mul(coord_x_squared, coord_x, coord_x);
                
                float coord_y_squared[MM_SIZE] = {};
                MM_Mul(coord_y_squared, coord_y, coord_y);
                
                float radius_vector_squared[MM_SIZE] = {};
                MM_Add(radius_vector_squared, coord_x_squared, coord_y_squared);
                
                MM_Cmp_LE(is_stable_mask_array, radius_vector_squared, max_radius_squared);
                
                int is_stable_mask = MM_MoveMask(is_stable_mask_array);

                if (is_stable_mask == 0)
                {
                    break;
                }

                MM_Mul(coord_y, number_two, coord_y);
                MM_Mul(coord_y, coord_y, coord_x);
                MM_Add(coord_y, coord_y, coord_y_start);
                
                MM_Sub(coord_x, coord_x_squared, coord_y_squared);
                MM_Add(coord_x, coord_x, coord_x_start);

                MM_SubInt(iters, iters, is_stable_mask_array);
            }
            
            DPRINTF("Drawing points: ");
            
            for (int i = 0; i < MM_SIZE; i++)
            {
                Uint32 pixel_color = MandelbrotGetColor(app->screen_surface->format, iters[i]);
                
                GfxPutPixel(app->screen_surface, pixel_x + i, pixel_y, pixel_color);

                DPRINTF("%d %d %d\t", pixel_x + i, pixel_y, iters[i]);
            }

            DPRINTF("\n");

            MM_Add(coord_x_start, coord_x_start, coord_x_increment);
        }
    }

    return GFX_SUCCESS;
}

//——————————————————————————————————————————————————————————————————————————————————————————
