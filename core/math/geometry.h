/**
 * @file geometry.h
 * @brief 2D geometry primitives and vector mathematics
 *
 * Provides fundamental 2D geometry types including points, vectors,
 * and relative coordinates. Includes functions for distance calculations,
 * vector operations, and random point generation.
 */

#ifndef CORE_MATH_GEOMETRY_H_
#define CORE_MATH_GEOMETRY_H_

#include <stdbool.h>

#include "rng.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
  int x_delta;
  int y_delta;
} rel_point_t, *rel_point_ptr;

typedef struct {
  double x;
  double y;
} point_t, *point_ptr, vector_t, *vector_ptr;

/** A float rectangle; x and y are the top-left corner. */
typedef struct {
  float x, y, w, h;
} frect_t;

static inline float clampf(float value, float low, float high) {
  return value < low ? low : (value > high ? high : value);
}

static inline float lerpf(float a, float b, float t) { return a + (b - a) * t; }

/** Moves `value` toward `target` by at most `step`, without overshooting. */
static inline float approachf(float value, float target, float step) {
  if (value < target) return value + step > target ? target : value + step;
  return value - step < target ? target : value - step;
}

static inline float frect_right(frect_t r) { return r.x + r.w; }
static inline float frect_bottom(frect_t r) { return r.y + r.h; }

static inline bool frect_overlaps(frect_t a, frect_t b) {
  return a.x < b.x + b.w && b.x < a.x + a.w && a.y < b.y + b.h &&
         b.y < a.y + a.h;
}

point_t point(double x, double y);
vector_t vector(double x, double y);
vector_t points_vector(const point_ptr a, const point_ptr b);
point_t relative_point(const point_ptr p, const rel_point_ptr rel_p);
double point_distance(const point_ptr p1, const point_ptr p2);
double random_angle(rng_t* rng);
point_t random_point(rng_t* rng, int width, int height);
point_t random_point_around(rng_t* rng, const point_ptr point,
                            int min_tolerance, int max_tolerance);

#endif  // CORE_MATH_GEOMETRY_H_
