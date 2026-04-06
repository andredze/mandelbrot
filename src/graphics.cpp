#include "graphics.h"
#include "mandelbrot.h"

//——————————————————————————————————————————————————————————————————————————————————————————

GfxErr_t GfxCtor(AppCtx_t* app)
{
    assert(app);

    app->is_running = true;

    //------------------------------------------------------------------//

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        PRINTERR("Failed to initialize SDL: %s", SDL_GetError());
    
        return GFX_SDL_INIT_ERROR;
    }

    //------------------------------------------------------------------//

    app->window = SDL_CreateWindow(APP_WINDOW_NAME,
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   WINDOW_SCREEN_WIDTH,
                                   WINDOW_SCREEN_HEIGHT,
                                   SDL_WINDOW_SHOWN);
    if (app->window == NULL)
    {
        PRINTERR("Failed creating window. SDL_Error: %s", SDL_GetError());

        return GFX_WINDOW_ERROR;
    }

    //------------------------------------------------------------------//

    app->screen_surface = SDL_GetWindowSurface(app->window);

    if (app->screen_surface == NULL)
    {
        PRINTERR("Failed getting window surface. SDL_Error: %s", SDL_GetError());

        return GFX_SURFACE_ERROR;
    }

    //------------------------------------------------------------------//

    if (TTF_Init() == -1)
    {
        PRINTERR("Failed to init fonts. TTF_Error: %s", TTF_GetError());

        return GFX_FONT_INIT_ERROR;
    }

    //------------------------------------------------------------------//

    return GFX_SUCCESS;    
}

//——————————————————————————————————————————————————————————————————————————————————————————

GfxErr_t GfxUpdate(AppCtx_t* app)
{
    assert(app);

    while (SDL_PollEvent(&app->event) != 0)
    {
        if (app->event.type == SDL_QUIT)
        {
            app->is_running = false;
        }
    }

    return GFX_SUCCESS;
}

//——————————————————————————————————————————————————————————————————————————————————————————

GfxErr_t GfxPutPixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
    assert(surface);

    int bpp = surface->format->BytesPerPixel;

    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    
    switch (bpp) 
    {
        case 1:
            *p = pixel;
            break;

        case 2:
            *(Uint16 *)p = pixel;
            break;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) 
            {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } 
            else 
            {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }

            break;

        case 4:
            *(Uint32 *)p = pixel;
            break;
        
        default:
            break;
    }

    return GFX_SUCCESS;
}

//——————————————————————————————————————————————————————————————————————————————————————————

GfxErr_t GfxDraw(AppCtx_t* app)
{
    assert(app);

    GfxErr_t error = GFX_SUCCESS;

    SDL_LockSurface(app->screen_surface);
    
    if ((error = MandelbrotDrawIntrinsics(app)))
    {
        return error;
    }

    SDL_UnlockSurface(app->screen_surface);
    
    SDL_UpdateWindowSurface(app->window);

    return GFX_SUCCESS;
}

//——————————————————————————————————————————————————————————————————————————————————————————

void GfxDtor(AppCtx_t* app)
{
    assert(app);

    SDL_FreeSurface(app->screen_surface);
    app->screen_surface = NULL;

    SDL_DestroyWindow(app->window);
    app->window = NULL;

    TTF_Quit();
    SDL_Quit();
}

//——————————————————————————————————————————————————————————————————————————————————————————
