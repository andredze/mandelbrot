#include "graphics.h"
#include "mandelbrot.h"

//——————————————————————————————————————————————————————————————————————————————————————————

static GfxErr_t GfxRendererInit(AppCtx_t* app)
{
    assert(app);

    app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED);

    if (app->renderer == NULL)
    {
        PRINTERR("Failed to create renderer. SDL_Error: %s", SDL_GetError());

        return GFX_SDL_RENDERER_ERROR;
    }
    
    if (SDL_RenderSetScale(app->renderer,
                           SCALE_SCREEN_WIDTH,
                           SCALE_SCREEN_HEIGHT) < 0)
    {
        PRINTERR("Failed to set scale for renderer. SDL_Error: %s", SDL_GetError());
    
        return GFX_SDL_RENDERER_ERROR;
    }

    return GFX_SUCCESS;
}

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

    GfxErr_t error = GFX_SUCCESS;

    if ((error = GfxRendererInit(app)))
    {
        return error;
    }

    //------------------------------------------------------------------//

    // app->screen_surface = SDL_GetWindowSurface(app->window);

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

GfxErr_t GfxDraw(AppCtx_t* app)
{
    assert(app);

    if (SDL_SetRenderDrawColor(app->renderer,
                               SDL_COLOR_RESET.r,
                               SDL_COLOR_RESET.g,
                               SDL_COLOR_RESET.b,
                               SDL_COLOR_RESET.a) < 0)
    {
        PRINTERR("Failed to set color for renderer. SDL_Error: %s", SDL_GetError());
    
        return GFX_SDL_RENDERER_ERROR;
    }

    SDL_RenderClear(app->renderer);

    GfxErr_t error = GFX_SUCCESS;

    if ((error = MandelbrotDraw(app)))
    {
        return error;
    }

    SDL_RenderPresent(app->renderer);

    return GFX_SUCCESS;
}

//——————————————————————————————————————————————————————————————————————————————————————————

void GfxDtor(AppCtx_t* app)
{
    assert(app);

    // SDL_FreeSurface(app->screen_surface);
    // app->screen_surface = NULL;

    SDL_DestroyRenderer(app->renderer);
    app->renderer = NULL;

    SDL_DestroyWindow(app->window);
    app->window = NULL;

    TTF_Quit();
    SDL_Quit();
}

//——————————————————————————————————————————————————————————————————————————————————————————
