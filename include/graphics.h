#ifndef GRAPHICS_H
#define GRAPHICS_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

//------------------------------------------------------------------//

#include <assert.h>
#include "my_printfs.h"
#include <avxintrin.h>
#include "mandelbrot_test.h"

//------------------------------------------------------------------//

const int BYTES_PER_PIXEL = 4;

//------------------------------------------------------------------//

const int SCALE_SCREEN_WIDTH  = 1;
const int SCALE_SCREEN_HEIGHT = 1;

//------------------------------------------------------------------//

#ifdef GRAPHICS
const int SCREEN_WIDTH  = 1024 * 2;
const int SCREEN_HEIGHT = 576 * 2;
#else
const int SCREEN_WIDTH  = 1024;
const int SCREEN_HEIGHT = 576;
#endif

//------------------------------------------------------------------//

const int SURFACE_PITCH = SCREEN_WIDTH * BYTES_PER_PIXEL;

//------------------------------------------------------------------//

const int WINDOW_SCREEN_WIDTH  = SCREEN_WIDTH  * SCALE_SCREEN_WIDTH;
const int WINDOW_SCREEN_HEIGHT = SCREEN_HEIGHT * SCALE_SCREEN_HEIGHT;

//------------------------------------------------------------------//

const char * const APP_WINDOW_NAME = "mandelbrot";
const char * const FONT_FILE_PATH  = "assets/font.ttf";
const int          FONT_SIZE       = 28;

//------------------------------------------------------------------//

const SDL_Color SDL_COLOR_RESET = {  0,   0,   0, 255};
const SDL_Color SDL_COLOR_WHITE = {255, 255, 255, 255};

//------------------------------------------------------------------//

const int       FPS_TEXT_SIZE = 32;
const SDL_Color FPS_FG_COLOR  = SDL_COLOR_WHITE;

//------------------------------------------------------------------//

typedef enum GfxErr
{
    GFX_SUCCESS,
    GFX_SDL_INIT_ERROR,
    GFX_WINDOW_ERROR,
    GFX_SDL_RENDERER_ERROR,
    GFX_FONT_INIT_ERROR,
    GFX_LOAD_FONT_ERROR,
    GFX_SURFACE_ERROR,
    GFX_BLIT_ERROR
}
GfxErr_t;

//------------------------------------------------------------------//

const int MANDELBROT_MAX_ITERS = 100;

//------------------------------------------------------------------//

typedef struct AppCtx
{
    bool            is_running;
    SDL_Event       event;

    SDL_Renderer*   renderer;
    SDL_Window*     window;
    SDL_Surface*    screen_surface;

    __m256          mm_x_key_shift;
    __m256          mm_y_key_shift;

    float           x_zoom_scale;
    float           y_zoom_scale;

    float           center_point_x;
    float           center_point_y;

    TTF_Font*       font;

    int             fps_counter;

    uint64_t        tests_cycles[TESTS_COUNT];

    Uint32          color_table[MANDELBROT_MAX_ITERS + 1];
}
AppCtx_t;

//------------------------------------------------------------------//

inline void __attribute__((always_inline)) 
GfxPutPixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
    *(Uint32*)(((Uint8 *)surface->pixels) + y * SURFACE_PITCH + x * BYTES_PER_PIXEL) = pixel;
}

//------------------------------------------------------------------//

GfxErr_t GfxCtor    (AppCtx_t* app);
GfxErr_t GfxUpdate  (AppCtx_t* app);
GfxErr_t GfxDraw    (AppCtx_t* app);
void     GfxDtor    (AppCtx_t* app);

GfxErr_t GfxDrawText(AppCtx_t*        app,
                     const char*      string,
                     int              x, 
                     int              y,
                     const SDL_Color* fg_color);

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* GRAPHICS_H */