#ifndef MANDELBROT_H
#define MANDELBROT_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "graphics.h"

//------------------------------------------------------------------//

const int   MM_SIZE              = 8;

const int   MANDELBROT_MAX_ITERS = 256;

const float STABLE_POINTS_CIRCLE_RADIUS         = 10;
const float STABLE_POINTS_CIRCLE_RADIUS_SQUARED = STABLE_POINTS_CIRCLE_RADIUS * STABLE_POINTS_CIRCLE_RADIUS;

const float COORD_Y_SHIFT      = -1.275f;
const float COORD_Y_STEP_COEFF = 2.5f;

const float COORD_X_SHIFT      = -2.4f;
const float COORD_X_STEP_COEFF = 4.0f;

//------------------------------------------------------------------//

const float MM_76543210[MM_SIZE] = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f};
const float MM_NUMBER_2[MM_SIZE] = {2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f};

//------------------------------------------------------------------//

GfxErr_t MandelbrotDraw                         (AppCtx_t* app);
GfxErr_t MandelbrotDrawUnrolled                 (AppCtx_t* app);
GfxErr_t MandelbrotDrawUnrolledWithFunctions    (AppCtx_t* app);
GfxErr_t MandelbrotDrawIntrinsics               (AppCtx_t* app);

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* MANDELBROT_H */