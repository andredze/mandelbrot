#include "mandelbrot.h"
#include "math.h"
#include "graphics.h"
#include <stdint.h>

#ifdef DEBUG
    #include "immintrin_dbg.h"
#else
    #include <x86intrin.h>
#endif /* DEBUG */

//——————————————————————————————————————————————————————————————————————————————————————————

static inline Uint32 MandelbrotGetColor(SDL_PixelFormat* format, int iters)
{
    Uint8 color_r = 0;
    Uint8 color_g = 0;
    Uint8 color_b = 0;

    if (iters < MANDELBROT_MAX_ITERS)
    {
        float clr = 255 * sqrt(sqrt( (float) iters * (1.0f / MANDELBROT_MAX_ITERS) ));
        
        color_r = (Uint8) (255 - 50 * (iters % 40));
        color_b = (Uint8) clr;
        color_g = 205;
    }

    return SDL_MapRGB(format, color_r, color_g, color_b);
}

//——————————————————————————————————————————————————————————————————————————————————————————

void GetColorTable(AppCtx_t* app)
{
    size_t size = sizeof(app->color_table) / sizeof(app->color_table[0]);

    for (int iters = 0; iters < size; iters++)
    {
        app->color_table[iters] = MandelbrotGetColor(app->screen_surface->format, iters);
    }

    return;
}

//——————————————————————————————————————————————————————————————————————————————————————————

void MandelbrotDrawIntrinsics512(AppCtx_t* app)
{
    assert(app);

    __m512 mm_number_2f     = _mm512_set1_ps(2.0f);
    __m512 mm_r_squared_max = _mm512_set1_ps(STABLE_POINTS_CIRCLE_RADIUS_SQUARED);
    
    float delta_x = app->x_zoom_scale * (1.0f / SCREEN_WIDTH);

    __m512 mm_delta_x       = _mm512_set1_ps(delta_x);
    __m512 mm_x_increment   = _mm512_set1_ps(ZMM_SIZE * delta_x);

    float y_coeff  = (1.0f / SCREEN_HEIGHT) * app->y_zoom_scale;
    float y_adding = app->y_zoom_scale * (-0.5f) + app->center_point_y; 

    __m512 arr_0to15        = _mm512_setr_ps(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    __m512 mm_delta_x_0to15 = _mm512_mul_ps (arr_0to15, mm_delta_x);

    for (int pixel_y = 0; pixel_y < SCREEN_HEIGHT; pixel_y++)
    {
        Uint32* surface_row_ptr = (Uint32*)((Uint8*)app->screen_surface->pixels + pixel_y * SURFACE_PITCH);

        float x_start_first = app->center_point_x + app->x_zoom_scale * (-0.5f);

        __m512 mm_x_start = _mm512_add_ps(mm_delta_x_0to15, _mm512_set1_ps(x_start_first));
        __m512 mm_y_start = _mm512_set1_ps(y_adding + y_coeff * (float) pixel_y);

        for (int pixel_x = 0; pixel_x < SCREEN_WIDTH; pixel_x += ZMM_SIZE)
        {
            __m512 mm_x = mm_x_start;
            __m512 mm_y = mm_y_start;

            __m512i mm_iters = _mm512_setzero_si512();

            for (int n = 0; n < MANDELBROT_MAX_ITERS; n++)
            {
                __m512 mm_x_squared = _mm512_mul_ps(mm_x, mm_x);
                __m512 mm_y_squared = _mm512_mul_ps(mm_y, mm_y);
                __m512 mm_r_squared = _mm512_add_ps(mm_x_squared, mm_y_squared);

                // if (r[i]^2 <= max_r[i]^2) { mask[i] = 1 }
                __mmask16 mm_is_stable_mask = _mm512_cmple_ps_mask(mm_r_squared, mm_r_squared_max);

                unsigned int is_stable_mask = _cvtmask16_u32(mm_is_stable_mask);

                // if all points are unstable (outside the circle)
                if (is_stable_mask == 0)
                {
                    break;
                }

                // y = 2 * y * x + y_0
                mm_y = _mm512_fmadd_ps(_mm512_mul_ps(mm_number_2f, mm_y), mm_x, mm_y_start);
                
                // x = (x^2) - (y^2) + x_0
                mm_x = _mm512_add_ps(_mm512_sub_ps(mm_x_squared, mm_y_squared), mm_x_start);
                
                // if   (mask == 1) { iters++    <=> iters + mask }
                // else (mask == 0) { do nothing <=> iters + mask }
                mm_iters = _mm512_add_epi32(mm_iters, _mm512_maskz_set1_epi32(mm_is_stable_mask, 1));
            }

            // colors = gather iters[i] for i = [0, 16]
            __m512i colors = _mm512_i32gather_epi32(mm_iters, app->color_table, 4);
            _mm512_storeu_si512((void*)(surface_row_ptr + pixel_x), colors);

            mm_x_start = _mm512_add_ps(mm_x_start, mm_x_increment);
        }
    }
}

//——————————————————————————————————————————————————————————————————————————————————————————

static inline void
MM_SetValue(float mm_dest[ZMM_SIZE], 
            float value)
{
    for (int i = 0; i < ZMM_SIZE; i++)
    {
        mm_dest[i] = value;
    }
}

//------------------------------------------------------------------//

static inline void
MM_Add(      float mm_dest[ZMM_SIZE], 
       const float mm_src1[ZMM_SIZE], 
       const float mm_src2[ZMM_SIZE])
{
    for (int i = 0; i < ZMM_SIZE; i++)
    {
        mm_dest[i] = mm_src1[i] + mm_src2[i];
    }
}

//------------------------------------------------------------------//

static inline void
MM_Sub(      float mm_dest[ZMM_SIZE], 
       const float mm_src1[ZMM_SIZE], 
       const float mm_src2[ZMM_SIZE])
{
    for (int i = 0; i < ZMM_SIZE; i++)
    {
        mm_dest[i] = mm_src1[i] - mm_src2[i];
    }
}

//------------------------------------------------------------------//

static inline void
MM_Mul(      float mm_dest[ZMM_SIZE],
       const float mm_src1[ZMM_SIZE],
       const float mm_src2[ZMM_SIZE])
{
    for (int i = 0; i < ZMM_SIZE; i++)
    {
        mm_dest[i] = mm_src1[i] * mm_src2[i];
    }
}

//------------------------------------------------------------------//

static inline void
MM_Cpy(      float mm_dest[ZMM_SIZE],
       const float mm_src [ZMM_SIZE])
{
    for (int i = 0; i < ZMM_SIZE; i++)
    {
        mm_dest[i] = mm_src[i];
    }
}

//------------------------------------------------------------------//

static inline void
MM_Cmp_LE(      int   mm_mask[ZMM_SIZE],
          const float mm_op1 [ZMM_SIZE],
          const float mm_op2 [ZMM_SIZE])
{
    for (int i = 0; i < ZMM_SIZE; i++) 
    {
        mm_mask[i] = (mm_op1[i] <= mm_op2[i]) ? 1 : 0;
    }
}

//------------------------------------------------------------------//

static inline int
MM_MoveMask(const int mm_mask[ZMM_SIZE])
{
    int mask = 0;

    for (int i = 0; i < ZMM_SIZE; i++)
    {
        // set bit number i to 1
        mask |= (mm_mask[i] << i);
    }

    return mask;
}

//------------------------------------------------------------------//

static inline void
MM_AddInt(      int32_t mm_dest[ZMM_SIZE],
          const int32_t mm_op1 [ZMM_SIZE],
          const int32_t mm_op2 [ZMM_SIZE])
{
    for (int i = 0; i < ZMM_SIZE; i++)
    {
        mm_dest[i] = mm_op1[i] + mm_op2[i];
    }
}

//------------------------------------------------------------------//

void MandelbrotDrawUnrolledWithFunctions(AppCtx_t* app)
{
    assert(app);

    float number_two[ZMM_SIZE] = {};
    MM_SetValue(number_two, 2.0f);

    float max_radius_squared[ZMM_SIZE] = {};
    MM_SetValue(max_radius_squared, STABLE_POINTS_CIRCLE_RADIUS_SQUARED);
    
    float delta_x = app->x_zoom_scale * (1.0f / SCREEN_WIDTH);
    
    float arr_delta_x[ZMM_SIZE] = {};
    MM_SetValue(arr_delta_x, delta_x);
    
    float coord_x_increment[ZMM_SIZE] = {};
    MM_SetValue(coord_x_increment, ZMM_SIZE * delta_x);

    float arr_0to15[ZMM_SIZE] = {};
    MM_Cpy(arr_0to15, ARR_0TO15);

    float delta_x_0to15[ZMM_SIZE] = {};
    MM_Mul(delta_x_0to15, arr_0to15, arr_delta_x);

    float y_coeff  = (1.0f / SCREEN_HEIGHT) * app->y_zoom_scale;
    float y_adding = app->y_zoom_scale * (-0.5f) + app->center_point_y; 

    for (int pixel_y = 0; pixel_y < SCREEN_HEIGHT; pixel_y++)
    {
        Uint32* surface_row_ptr = (Uint32*)((Uint8*)app->screen_surface->pixels + pixel_y * SURFACE_PITCH);

        float x_start_first = app->center_point_x + app->x_zoom_scale * (-0.5f);

        float coord_x_start[ZMM_SIZE] = {};
        MM_SetValue(coord_x_start, x_start_first);
        MM_Add(coord_x_start, coord_x_start, delta_x_0to15);

        float coord_y_start[ZMM_SIZE] = {};
        MM_SetValue(coord_y_start, y_adding + y_coeff * (float) pixel_y);
        
        for (int pixel_x = 0; pixel_x < SCREEN_WIDTH; pixel_x += ZMM_SIZE)
        {
            float coord_y[ZMM_SIZE] = {};
            MM_Cpy(coord_y, coord_y_start);
            
            float coord_x[ZMM_SIZE] = {};
            MM_Cpy(coord_x, coord_x_start);
            
            int iters               [ZMM_SIZE] = {0};
            int is_stable_mask_array[ZMM_SIZE] = {0};

            for (int n = 0; n < MANDELBROT_MAX_ITERS; n++)
            {
                float coord_x_squared[ZMM_SIZE] = {};
                MM_Mul(coord_x_squared, coord_x, coord_x);
                
                float coord_y_squared[ZMM_SIZE] = {};
                MM_Mul(coord_y_squared, coord_y, coord_y);
                
                float radius_vector_squared[ZMM_SIZE] = {};
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

                MM_AddInt(iters, iters, is_stable_mask_array);
            }
            
            for (int i = 0; i < ZMM_SIZE; i++)
            {
                Uint32 pixel_color = app->color_table[iters[i]];
                
                *(surface_row_ptr + pixel_x + i) = pixel_color;
            }

            MM_Add(coord_x_start, coord_x_start, coord_x_increment);
        }
    }
}

//——————————————————————————————————————————————————————————————————————————————————————————

void MandelbrotDrawUnoptimized(AppCtx_t* app)
{
    assert(app);

    float coord_x_inc = app->x_zoom_scale * (1.0f / SCREEN_WIDTH);

    for (int pixel_y = 0; pixel_y < SCREEN_HEIGHT; pixel_y++)
    {
        Uint32* surface_row_ptr = (Uint32*)((Uint8*)app->screen_surface->pixels + pixel_y * SURFACE_PITCH);

        float coord_y_start = app->center_point_y + app->y_zoom_scale * ((float) pixel_y * (1.0f / SCREEN_HEIGHT) - 0.5f);
        float coord_x_start = app->center_point_x + app->x_zoom_scale * (-0.5f);

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

            *(surface_row_ptr + pixel_x) = app->color_table[iters];
        }
    }
}

//——————————————————————————————————————————————————————————————————————————————————————————
