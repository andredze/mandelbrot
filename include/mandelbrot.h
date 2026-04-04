#ifndef MANDELBROT_H
#define MANDELBROT_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include "graphics.h"

//------------------------------------------------------------------//

const int   MANDELBROT_MAX_ITERS = 256;

const float STABLE_POINTS_CIRCLE_RADIUS         = 10;
const float STABLE_POINTS_CIRCLE_RADIUS_SQUARED = STABLE_POINTS_CIRCLE_RADIUS * STABLE_POINTS_CIRCLE_RADIUS;

const float COORD_Y_SHIFT      = -1.5f;
const float COORD_Y_STEP_COEFF = 2.5f;

const float COORD_X_SHIFT      = -2.0f;
const float COORD_X_STEP_COEFF = 4.0f;

//------------------------------------------------------------------//

GfxErr_t MandelbrotDraw(AppCtx_t* app);

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* MANDELBROT_H */