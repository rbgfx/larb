#ifndef LARB_H
#define LARB_H

#include <ruby.h>
#include <math.h>

extern VALUE mLarb;

static inline void larb_normalize(double *values, size_t count) {
  double scale = 0.0;
  for (size_t i = 0; i < count; i++) {
    if (!isfinite(values[i])) {
      rb_raise(rb_eArgError, "Cannot normalize non-finite components");
    }
    scale = fmax(scale, fabs(values[i]));
  }
  if (scale == 0.0) {
    rb_raise(rb_eArgError, "Cannot normalize a zero-length value");
  }
  double sum = 0.0;
  for (size_t i = 0; i < count; i++) {
    values[i] /= scale;
    sum += values[i] * values[i];
  }
  double length = sqrt(sum);
  for (size_t i = 0; i < count; i++) {
    values[i] /= length;
  }
}

#endif
