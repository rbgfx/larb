#ifndef LARB_MATRIX_UTILS_H
#define LARB_MATRIX_UTILS_H

#include "larb.h"
#include <float.h>
#include <limits.h>
#include <math.h>

/* Column-major matrices of size 2 through 4. Combine row and column exponents
 * before scaling, so neither intermediate division loses small components. */
static inline void larb_matrix_inverse(const double *values, double *inverse,
                                       int size) {
  double rows[4][8] = {{0.0}};
  int row_exponents[4], column_exponents[4];
  double row_scales[4] = {0.0};
  for (int col = 0; col < size; col++) {
    for (int row = 0; row < size; row++) {
      double value = values[col * size + row];
      if (!isfinite(value)) {
        rb_raise(rb_eRuntimeError, "Matrix is not invertible");
      }
      row_scales[row] = fmax(row_scales[row], fabs(value));
    }
  }
  for (int row = 0; row < size; row++) {
    if (row_scales[row] == 0.0) {
      rb_raise(rb_eRuntimeError, "Matrix is not invertible");
    }
    row_exponents[row] = ilogb(row_scales[row]);
    row_scales[row] = 0.0;
  }
  for (int col = 0; col < size; col++) {
    int exponent = INT_MIN;
    for (int row = 0; row < size; row++) {
      double value = values[col * size + row];
      if (value != 0.0) {
        int adjusted = ilogb(fabs(value)) - row_exponents[row];
        if (adjusted > exponent) exponent = adjusted;
      }
    }
    if (exponent == INT_MIN) {
      rb_raise(rb_eRuntimeError, "Matrix is not invertible");
    }
    column_exponents[col] = exponent;
    for (int row = 0; row < size; row++) {
      rows[row][col] = scalbn(values[col * size + row],
                               -(row_exponents[row] + exponent));
      row_scales[row] = fmax(row_scales[row], fabs(rows[row][col]));
    }
    rows[col][size + col] = 1.0;
  }

  for (int col = 0; col < size; col++) {
    int pivot = col;
    for (int row = col + 1; row < size; row++) {
      if (fabs(rows[row][col]) / row_scales[row] >
          fabs(rows[pivot][col]) / row_scales[pivot]) {
        pivot = row;
      }
    }
    if (fabs(rows[pivot][col]) / row_scales[pivot] <= size * DBL_EPSILON) {
      rb_raise(rb_eRuntimeError, "Matrix is not invertible");
    }
    double row_scale = row_scales[col];
    row_scales[col] = row_scales[pivot];
    row_scales[pivot] = row_scale;
    for (int i = 0; i < size * 2; i++) {
      double tmp = rows[col][i];
      rows[col][i] = rows[pivot][i];
      rows[pivot][i] = tmp;
    }
    double divisor = rows[col][col];
    for (int i = 0; i < size * 2; i++) {
      rows[col][i] /= divisor;
    }
    for (int row = 0; row < size; row++) {
      if (row == col) continue;
      double factor = rows[row][col];
      for (int i = 0; i < size * 2; i++) {
        rows[row][i] -= factor * rows[col][i];
      }
    }
  }

  for (int col = 0; col < size; col++) {
    for (int row = 0; row < size; row++) {
      double value = scalbn(rows[row][size + col],
                              -(column_exponents[row] + row_exponents[col]));
      if (!isfinite(value)) {
        rb_raise(rb_eRuntimeError, "Matrix is not invertible");
      }
      inverse[col * size + row] = value;
    }
  }
}

#endif
