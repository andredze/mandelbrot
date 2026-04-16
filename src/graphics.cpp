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

    app->font = TTF_OpenFont(FONT_FILE_PATH, FONT_SIZE);

    if (app->font == NULL)
    {
        PRINTERR("Failed to open font %s. Error: %s", FONT_FILE_PATH, TTF_GetError());
        
        return GFX_LOAD_FONT_ERROR;
    }

    //------------------------------------------------------------------//

    app->center_point_x = STARTING_CENTER_POINT_X;
    app->center_point_y = STARTING_CENTER_POINT_Y;

    //------------------------------------------------------------------//

    app->x_zoom_scale = STARTING_ZOOM_SCALE_X;
    app->y_zoom_scale = STARTING_ZOOM_SCALE_Y;

    //------------------------------------------------------------------//

    GetColorTable(app);

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
        if (app->event.type != SDL_KEYDOWN)
        {
            continue;
        }

        switch (app->event.key.keysym.sym)
        {
            case SDLK_UP:
                app->center_point_y -= COORD_Y_KEY_STEP * app->y_zoom_scale;
                break;

            case SDLK_DOWN:
                app->center_point_y += COORD_Y_KEY_STEP * app->y_zoom_scale;
                break;

            case SDLK_LEFT:
                app->center_point_x -= COORD_X_KEY_STEP * app->x_zoom_scale;
                break;

            case SDLK_RIGHT:
                app->center_point_x += COORD_X_KEY_STEP * app->x_zoom_scale;
                break;

            default:
                break;
        }
    }

    // using key states instead of key presses for smooth changes when holding button
    const Uint8* current_key_states = SDL_GetKeyboardState(NULL);

    if (current_key_states[SDL_SCANCODE_SPACE])
    {
        app->x_zoom_scale *= 1.0f / ZOOM_SCALE_COEFF;
        app->y_zoom_scale *= 1.0f / ZOOM_SCALE_COEFF;
    }
    else if (current_key_states[SDL_SCANCODE_RSHIFT])
    {
        app->x_zoom_scale *= ZOOM_SCALE_COEFF;
        app->y_zoom_scale *= ZOOM_SCALE_COEFF;
    }

    return GFX_SUCCESS;
}

//——————————————————————————————————————————————————————————————————————————————————————————

GfxErr_t GfxDraw(AppCtx_t* app)
{
    assert(app);

    uint64_t start_cycles_in = __rdtsc();

    Uint32 black_color = SDL_MapRGB(app->screen_surface->format, 0, 0, 0);

    SDL_Rect screen_rect = {.x = 0,
                            .y = 0,
                            .w = SCREEN_WIDTH,
                            .h = SCREEN_HEIGHT};

    SDL_FillRect(app->screen_surface, &screen_rect, black_color);

    GfxErr_t error = GFX_SUCCESS;

    SDL_LockSurface(app->screen_surface);

#if defined(_AVX)
	MandelbrotDrawIntrinsics512(app);

#elif defined(_ARRAYS)
	MandelbrotDrawUnrolledWithFunctions(app);

#elif defined(_NAIVE)
	MandelbrotDrawUnoptimized(app);
#endif /* _AVX */

	uint64_t end_cycles_in = __rdtsc();

	app->fps_counter = (int) ((float) PROCESSOR_TSC_FREQUENCY / ((float) (end_cycles_in - start_cycles_in)));

    char fps_text[FPS_TEXT_SIZE] = {};

    snprintf(fps_text, sizeof(fps_text), "FPS = %d", app->fps_counter);

    SDL_UnlockSurface(app->screen_surface);

    GfxDrawText(app, fps_text, 20, 20, &FPS_FG_COLOR);

    SDL_UpdateWindowSurface(app->window);

    return GFX_SUCCESS;
}

//——————————————————————————————————————————————————————————————————————————————————————————

GfxErr_t GfxDrawText(AppCtx_t*        app,
                     const char*      string,
                     int              x, 
                     int              y,
                     const SDL_Color* fg_color)
{
    SDL_Surface* text_surface = TTF_RenderText_Blended(app->font, string, *fg_color);

    SDL_Rect text_location_rect = {x, y, 0, 0};

    if (SDL_BlitSurface(text_surface, NULL, app->screen_surface, &text_location_rect) < 0)
    {
        PRINTERR("Failed blit surface. Error %s", SDL_GetError());

        return GFX_BLIT_ERROR;
    }

    SDL_FreeSurface(text_surface);

    return GFX_SUCCESS;
}

//——————————————————————————————————————————————————————————————————————————————————————————

void GfxDtor(AppCtx_t* app)
{
    assert(app);

    SDL_DestroyWindow(app->window);
    app->window = NULL;

    TTF_CloseFont(app->font);
    app->font = NULL;

    TTF_Quit();
    SDL_Quit();
}

//——————————————————————————————————————————————————————————————————————————————————————————
