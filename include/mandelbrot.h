#ifndef MANDELBROT_H
#define MANDELBROT_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "graphics.h"

//------------------------------------------------------------------//

// TODO: remove not used constants

const int   MM_SIZE              = 8;
const int   ZMM_SIZE             = 16;

const int   MANDELBROT_MAX_ITERS = 100;

const float STABLE_POINTS_CIRCLE_RADIUS         = 2.0f;
const float STABLE_POINTS_CIRCLE_RADIUS_SQUARED = STABLE_POINTS_CIRCLE_RADIUS * STABLE_POINTS_CIRCLE_RADIUS;

const float COORD_Y_SHIFT      = -1.275f;
const float COORD_Y_STEP_COEFF = 2.5f;

const float COORD_X_SHIFT      = -2.4f;
const float COORD_X_STEP_COEFF = 4.0f;

//------------------------------------------------------------------//

const float COORD_X_DIFF = COORD_X_STEP_COEFF / SCREEN_WIDTH;
const float COORD_Y_DIFF = COORD_Y_STEP_COEFF / SCREEN_HEIGHT;

//------------------------------------------------------------------//

const float ZOOM_SCALE_COEFF = 1.05f;

const float COORD_X_KEY_STEP = 0.02f;
const float COORD_Y_KEY_STEP = 0.02f;

//------------------------------------------------------------------//

const float STARTING_ZOOM_SCALE_X = 3.5f;
const float STARTING_ZOOM_SCALE_Y = STARTING_ZOOM_SCALE_X * ((float) SCREEN_HEIGHT / SCREEN_WIDTH);

const float STARTING_CENTER_POINT_X = -0.75f;
const float STARTING_CENTER_POINT_Y = 0.0f;

const float ARR_0TO15[ZMM_SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

//------------------------------------------------------------------//

void MandelbrotDrawUnoptimized           (AppCtx_t* app);
void MandelbrotDrawUnrolledWithFunctions (AppCtx_t* app);
void MandelbrotDrawIntrinsics256         (AppCtx_t* app);
void MandelbrotDrawIntrinsics512         (AppCtx_t* app);

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* MANDELBROT_H */