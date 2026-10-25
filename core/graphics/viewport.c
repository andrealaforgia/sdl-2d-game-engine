#include "viewport.h"

void viewport_fit(int output_width, int output_height, float view_height,
                  float* scale, float* view_width) {
  if (output_width < 1) output_width = 1;
  if (output_height < 1) output_height = 1;
  *scale = view_height > 0.0f ? (float)output_height / view_height : 1.0f;
  *view_width = (float)output_width / *scale;
}
