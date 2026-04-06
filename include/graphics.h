#ifndef GRAPHICS_H
#define GRAPHICS_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

//------------------------------------------------------------------//

#include <assert.h>
#include "my_printfs.h"
#include <avxintrin.h>

//------------------------------------------------------------------//

alignas (32) const float MM_COORD_X_KEY_STEP = 0.1f;
alignas (32) const float MM_COORD_Y_KEY_STEP = 0.1f;

//------------------------------------------------------------------//

const int SCALE_SCREEN_WIDTH  = 1;
const int SCALE_SCREEN_HEIGHT = 1;

//------------------------------------------------------------------//

const int SCREEN_WIDTH  = 1280 * 4;
const int SCREEN_HEIGHT = 800 * 4;

//------------------------------------------------------------------//

const int WINDOW_SCREEN_WIDTH  = SCREEN_WIDTH  * SCALE_SCREEN_WIDTH;
const int WINDOW_SCREEN_HEIGHT = SCREEN_HEIGHT * SCALE_SCREEN_HEIGHT;

//------------------------------------------------------------------//

const char * const APP_WINDOW_NAME = "mandelbrot";

//------------------------------------------------------------------//

const SDL_Color SDL_COLOR_RESET = {  0,   0,   0, 255};
const SDL_Color SDL_COLOR_WHITE = {255, 255, 255, 255};

//------------------------------------------------------------------//

typedef enum GfxErr
{
    GFX_SUCCESS,
    GFX_SDL_INIT_ERROR,
    GFX_WINDOW_ERROR,
    GFX_SDL_RENDERER_ERROR,
    GFX_FONT_INIT_ERROR,
    GFX_SURFACE_ERROR
}
GfxErr_t;

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

    float           x_zoom_span;
    float           y_zoom_span;

    TTF_Font*       font;
}
AppCtx_t;

//------------------------------------------------------------------//

GfxErr_t GfxPutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);

GfxErr_t GfxCtor    (AppCtx_t* app);
GfxErr_t GfxUpdate  (AppCtx_t* app);
GfxErr_t GfxDraw    (AppCtx_t* app);
void     GfxDtor    (AppCtx_t* app);

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* GRAPHICS_H */