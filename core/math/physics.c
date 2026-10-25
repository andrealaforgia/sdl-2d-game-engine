#include "physics.h"

#include "geometry.h"
#include "inline.h"
#include "math.h"

ALWAYS_INLINE velocity_t velocity(const double speed,
                                  const vector_t direction) {
  velocity_t velocity;
  velocity.speed = speed;
  velocity.direction = direction;
  return velocity;
}

ALWAYS_INLINE velocity_t random_velocity(rng_t* rng) {
  double angle = random_angle(rng);
  double rnd = rng_range(rng, 1, 10);
  double speed = 2 * rnd / (rnd + 1);
  return velocity(speed, vector(cos(angle), sin(angle)));
}
