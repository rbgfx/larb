#include "mat4.h"
#include "matrix_utils.h"

#include <math.h>

static void mat4_free(void *ptr) {
  xfree(ptr);
}

static size_t mat4_memsize(const void *ptr) {
  return sizeof(Mat4Data);
}

static const rb_data_type_t mat4_type = {
    "Mat4",
    {0, mat4_free, mat4_memsize},
    0,
    0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};

static VALUE cMat4 = Qnil;
static VALUE cVec3 = Qnil;
static VALUE cVec4 = Qnil;
static VALUE cQuat = Qnil;

static Mat4Data *mat4_get(VALUE obj) {
  Mat4Data *data = NULL;
  TypedData_Get_Struct(obj, Mat4Data, &mat4_type, data);
  return data;
}

static VALUE mat4_build(VALUE klass, const double *values) {
  VALUE obj = mat4_alloc(klass);
  Mat4Data *data = mat4_get(obj);
  for (int i = 0; i < 16; i++) {
    data->data[i] = values[i];
  }
  return obj;
}

static VALUE mat4_build16(VALUE klass, double v0, double v1, double v2,
                          double v3, double v4, double v5, double v6, double v7,
                          double v8, double v9, double v10, double v11,
                          double v12, double v13, double v14, double v15) {
  double values[16] = {v0,  v1,  v2,  v3,  v4,  v5,  v6,  v7,
                       v8,  v9,  v10, v11, v12, v13, v14, v15};
  return mat4_build(klass, values);
}

static inline void vec3_normalize(double *x, double *y, double *z) {
  double values[3] = {*x, *y, *z};
  larb_normalize(values, 3);
  *x = values[0];
  *y = values[1];
  *z = values[2];
}

static inline void vec3_cross(double ax, double ay, double az, double bx,
                              double by, double bz, double *rx, double *ry,
                              double *rz) {
  *rx = ay * bz - az * by;
  *ry = az * bx - ax * bz;
  *rz = ax * by - ay * bx;
}

VALUE mat4_alloc(VALUE klass) {
  Mat4Data *data = ALLOC(Mat4Data);
  for (int i = 0; i < 16; i++) {
    data->data[i] = 0.0;
  }
  data->data[0] = 1.0;
  data->data[5] = 1.0;
  data->data[10] = 1.0;
  data->data[15] = 1.0;
  return TypedData_Wrap_Struct(klass, &mat4_type, data);
}

VALUE mat4_initialize(int argc, VALUE *argv, VALUE self) {
  rb_check_frozen(self);
  VALUE data_arg = Qnil;
  Mat4Data values = {{1.0, 0.0, 0.0, 0.0,
                      0.0, 1.0, 0.0, 0.0,
                      0.0, 0.0, 1.0, 0.0,
                      0.0, 0.0, 0.0, 1.0}};
  rb_scan_args(argc, argv, "01", &data_arg);
  if (argc != 0) {
    VALUE ary = rb_check_array_type(data_arg);
    if (NIL_P(ary)) {
      rb_raise(rb_eTypeError, "expected Array");
    }
    if (RARRAY_LEN(ary) != 16) {
      rb_raise(rb_eArgError, "expected 16 elements");
    }
    for (int i = 0; i < 16; i++) {
      values.data[i] = NUM2DBL(rb_ary_entry(ary, i));
    }
  }
  rb_check_frozen(self);
  *mat4_get(self) = values;
  return self;
}

static VALUE mat4_initialize_copy(VALUE self, VALUE other) {
  if (self == other) return self;
  rb_obj_init_copy(self, other);
  *mat4_get(self) = *mat4_get(other);
  return self;
}

static VALUE mat4_class_identity(VALUE klass) {
  return mat4_alloc(klass);
}

static VALUE mat4_class_zero(VALUE klass) {
  return mat4_build16(klass, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}

static VALUE mat4_class_translation(VALUE klass, VALUE x, VALUE y, VALUE z) {
  VALUE m = mat4_class_identity(klass);
  mat4_aset(m, INT2NUM(12), x);
  mat4_aset(m, INT2NUM(13), y);
  mat4_aset(m, INT2NUM(14), z);
  return m;
}

static VALUE mat4_class_scaling(VALUE klass, VALUE x, VALUE y, VALUE z) {
  double sx = NUM2DBL(x);
  double sy = NUM2DBL(y);
  double sz = NUM2DBL(z);
  return mat4_build16(klass, sx, 0.0, 0.0, 0.0, 0.0, sy, 0.0, 0.0, 0.0, 0.0,
                      sz, 0.0, 0.0, 0.0, 0.0, 1.0);
}

static VALUE mat4_class_rotation_x(VALUE klass, VALUE radians) {
  double r = NUM2DBL(radians);
  double c = cos(r);
  double s = sin(r);
  return mat4_build16(klass, 1.0, 0.0, 0.0, 0.0, 0.0, c, s, 0.0, 0.0, -s, c,
                      0.0, 0.0, 0.0, 0.0, 1.0);
}

static VALUE mat4_class_rotation_y(VALUE klass, VALUE radians) {
  double r = NUM2DBL(radians);
  double c = cos(r);
  double s = sin(r);
  return mat4_build16(klass, c, 0.0, -s, 0.0, 0.0, 1.0, 0.0, 0.0, s, 0.0, c,
                      0.0, 0.0, 0.0, 0.0, 1.0);
}

static VALUE mat4_class_rotation_z(VALUE klass, VALUE radians) {
  double r = NUM2DBL(radians);
  double c = cos(r);
  double s = sin(r);
  return mat4_build16(klass, c, s, 0.0, 0.0, -s, c, 0.0, 0.0, 0.0, 0.0, 1.0,
                      0.0, 0.0, 0.0, 0.0, 1.0);
}

static VALUE mat4_class_rotation(VALUE klass, VALUE axis, VALUE radians) {
  VALUE normalized = rb_funcall(axis, rb_intern("normalize"), 0);
  double x = NUM2DBL(rb_funcall(normalized, rb_intern("x"), 0));
  double y = NUM2DBL(rb_funcall(normalized, rb_intern("y"), 0));
  double z = NUM2DBL(rb_funcall(normalized, rb_intern("z"), 0));
  double r = NUM2DBL(radians);
  double c = cos(r);
  double s = sin(r);
  double t = 1.0 - c;

  return mat4_build16(klass, t * x * x + c, t * x * y + s * z,
                      t * x * z - s * y, 0.0, t * x * y - s * z,
                      t * y * y + c, t * y * z + s * x, 0.0,
                      t * x * z + s * y, t * y * z - s * x,
                      t * z * z + c, 0.0, 0.0, 0.0, 0.0, 1.0);
}

static VALUE mat4_class_look_at(VALUE klass, VALUE eye, VALUE target,
                                VALUE up) {
  double ex = NUM2DBL(rb_funcall(eye, rb_intern("x"), 0));
  double ey = NUM2DBL(rb_funcall(eye, rb_intern("y"), 0));
  double ez = NUM2DBL(rb_funcall(eye, rb_intern("z"), 0));
  double tx = NUM2DBL(rb_funcall(target, rb_intern("x"), 0));
  double ty = NUM2DBL(rb_funcall(target, rb_intern("y"), 0));
  double tz = NUM2DBL(rb_funcall(target, rb_intern("z"), 0));
  double ux = NUM2DBL(rb_funcall(up, rb_intern("x"), 0));
  double uy = NUM2DBL(rb_funcall(up, rb_intern("y"), 0));
  double uz = NUM2DBL(rb_funcall(up, rb_intern("z"), 0));

  double fx = tx - ex;
  double fy = ty - ey;
  double fz = tz - ez;
  vec3_normalize(&fx, &fy, &fz);

  vec3_normalize(&ux, &uy, &uz);
  double rx, ry, rz;
  vec3_cross(fx, fy, fz, ux, uy, uz, &rx, &ry, &rz);
  vec3_normalize(&rx, &ry, &rz);

  double ux2, uy2, uz2;
  vec3_cross(rx, ry, rz, fx, fy, fz, &ux2, &uy2, &uz2);

  return mat4_build16(klass, rx, ux2, -fx, 0.0, ry, uy2, -fy, 0.0, rz, uz2,
                      -fz, 0.0, -(rx * ex + ry * ey + rz * ez),
                      -(ux2 * ex + uy2 * ey + uz2 * ez),
                      (fx * ex + fy * ey + fz * ez), 1.0);
}

static VALUE mat4_class_perspective(VALUE klass, VALUE fov_y, VALUE aspect,
                                    VALUE near, VALUE far) {
  double f = 1.0 / tan(NUM2DBL(fov_y) / 2.0);
  double nf = 1.0 / (NUM2DBL(near) - NUM2DBL(far));
  double a = NUM2DBL(aspect);
  double n = NUM2DBL(near);
  double fr = NUM2DBL(far);

  return mat4_build16(klass, f / a, 0.0, 0.0, 0.0, 0.0, f, 0.0, 0.0, 0.0, 0.0,
                      (fr + n) * nf, -1.0, 0.0, 0.0, 2.0 * fr * n * nf, 0.0);
}

static VALUE mat4_class_orthographic(VALUE klass, VALUE left, VALUE right,
                                     VALUE bottom, VALUE top, VALUE near,
                                     VALUE far) {
  double rl = 1.0 / (NUM2DBL(right) - NUM2DBL(left));
  double tb = 1.0 / (NUM2DBL(top) - NUM2DBL(bottom));
  double fn = 1.0 / (NUM2DBL(far) - NUM2DBL(near));
  double r = NUM2DBL(right);
  double l = NUM2DBL(left);
  double t = NUM2DBL(top);
  double b = NUM2DBL(bottom);
  double f = NUM2DBL(far);
  double n = NUM2DBL(near);

  return mat4_build16(klass, 2 * rl, 0.0, 0.0, 0.0, 0.0, 2 * tb, 0.0, 0.0,
                      0.0, 0.0, -2 * fn, 0.0, -(r + l) * rl, -(t + b) * tb,
                      -(f + n) * fn, 1.0);
}

static VALUE mat4_class_frustum(VALUE klass, VALUE left, VALUE right,
                                VALUE bottom, VALUE top, VALUE near,
                                VALUE far) {
  double rl = 1.0 / (NUM2DBL(right) - NUM2DBL(left));
  double tb = 1.0 / (NUM2DBL(top) - NUM2DBL(bottom));
  double nf = 1.0 / (NUM2DBL(near) - NUM2DBL(far));
  double r = NUM2DBL(right);
  double l = NUM2DBL(left);
  double t = NUM2DBL(top);
  double b = NUM2DBL(bottom);
  double n = NUM2DBL(near);
  double f = NUM2DBL(far);

  return mat4_build16(klass, 2 * n * rl, 0.0, 0.0, 0.0, 0.0, 2 * n * tb, 0.0,
                      0.0, (r + l) * rl, (t + b) * tb, (f + n) * nf, -1.0, 0.0,
                      0.0, 2 * f * n * nf, 0.0);
}

static VALUE mat4_class_from_quaternion(VALUE klass, VALUE quat) {
  double x = NUM2DBL(rb_funcall(quat, rb_intern("x"), 0));
  double y = NUM2DBL(rb_funcall(quat, rb_intern("y"), 0));
  double z = NUM2DBL(rb_funcall(quat, rb_intern("z"), 0));
  double w = NUM2DBL(rb_funcall(quat, rb_intern("w"), 0));

  double x2 = x + x;
  double y2 = y + y;
  double z2 = z + z;
  double xx = x * x2;
  double xy = x * y2;
  double xz = x * z2;
  double yy = y * y2;
  double yz = y * z2;
  double zz = z * z2;
  double wx = w * x2;
  double wy = w * y2;
  double wz = w * z2;

  return mat4_build16(klass, 1 - (yy + zz), xy + wz, xz - wy, 0.0, xy - wz,
                      1 - (xx + zz), yz + wx, 0.0, xz + wy, yz - wx,
                      1 - (xx + yy), 0.0, 0.0, 0.0, 0.0, 1.0);
}

static VALUE mat4_class_trs(VALUE klass, VALUE translation, VALUE rotation,
                            VALUE scale) {
  VALUE rot = mat4_class_from_quaternion(klass, rotation);
  double sx = NUM2DBL(rb_funcall(scale, rb_intern("x"), 0));
  double sy = NUM2DBL(rb_funcall(scale, rb_intern("y"), 0));
  double sz = NUM2DBL(rb_funcall(scale, rb_intern("z"), 0));
  VALUE scale_m = mat4_class_scaling(klass, DBL2NUM(sx), DBL2NUM(sy), DBL2NUM(sz));
  double tx = NUM2DBL(rb_funcall(translation, rb_intern("x"), 0));
  double ty = NUM2DBL(rb_funcall(translation, rb_intern("y"), 0));
  double tz = NUM2DBL(rb_funcall(translation, rb_intern("z"), 0));
  VALUE trans_m =
      mat4_class_translation(klass, DBL2NUM(tx), DBL2NUM(ty), DBL2NUM(tz));
  VALUE tmp = mat4_mul(rot, scale_m);
  return mat4_mul(trans_m, tmp);
}

VALUE mat4_aref(VALUE self, VALUE index) {
  Mat4Data *data = mat4_get(self);
  long idx = NUM2LONG(index);
  if (idx < 0 || idx > 15) {
    return Qnil;
  }
  return DBL2NUM(data->data[idx]);
}

VALUE mat4_aset(VALUE self, VALUE index, VALUE value) {
  rb_check_frozen(self);
  Mat4Data *data = mat4_get(self);
  long idx = NUM2LONG(index);
  if (idx < 0 || idx > 15) {
    rb_raise(rb_eIndexError, "index %ld out of range", idx);
  }
  double component = NUM2DBL(value);
  rb_check_frozen(self);
  data->data[idx] = component;
  return value;
}

VALUE mat4_mul(VALUE self, VALUE other) {
  Mat4Data *a = mat4_get(self);

  if (rb_obj_is_kind_of(other, cMat4)) {
    Mat4Data *b = mat4_get(other);
    double result[16];
    const double *ad = a->data;
    const double *bd = b->data;

    result[0] = ad[0] * bd[0] + ad[4] * bd[1] + ad[8] * bd[2] + ad[12] * bd[3];
    result[1] = ad[1] * bd[0] + ad[5] * bd[1] + ad[9] * bd[2] + ad[13] * bd[3];
    result[2] = ad[2] * bd[0] + ad[6] * bd[1] + ad[10] * bd[2] + ad[14] * bd[3];
    result[3] = ad[3] * bd[0] + ad[7] * bd[1] + ad[11] * bd[2] + ad[15] * bd[3];

    result[4] = ad[0] * bd[4] + ad[4] * bd[5] + ad[8] * bd[6] + ad[12] * bd[7];
    result[5] = ad[1] * bd[4] + ad[5] * bd[5] + ad[9] * bd[6] + ad[13] * bd[7];
    result[6] = ad[2] * bd[4] + ad[6] * bd[5] + ad[10] * bd[6] + ad[14] * bd[7];
    result[7] = ad[3] * bd[4] + ad[7] * bd[5] + ad[11] * bd[6] + ad[15] * bd[7];

    result[8] = ad[0] * bd[8] + ad[4] * bd[9] + ad[8] * bd[10] + ad[12] * bd[11];
    result[9] = ad[1] * bd[8] + ad[5] * bd[9] + ad[9] * bd[10] + ad[13] * bd[11];
    result[10] = ad[2] * bd[8] + ad[6] * bd[9] + ad[10] * bd[10] + ad[14] * bd[11];
    result[11] = ad[3] * bd[8] + ad[7] * bd[9] + ad[11] * bd[10] + ad[15] * bd[11];

    result[12] = ad[0] * bd[12] + ad[4] * bd[13] + ad[8] * bd[14] + ad[12] * bd[15];
    result[13] = ad[1] * bd[12] + ad[5] * bd[13] + ad[9] * bd[14] + ad[13] * bd[15];
    result[14] = ad[2] * bd[12] + ad[6] * bd[13] + ad[10] * bd[14] + ad[14] * bd[15];
    result[15] = ad[3] * bd[12] + ad[7] * bd[13] + ad[11] * bd[14] + ad[15] * bd[15];
    return mat4_build(rb_obj_class(self), result);
  }

  if (rb_obj_is_kind_of(other, cVec4)) {
    double x = NUM2DBL(rb_funcall(other, rb_intern("x"), 0));
    double y = NUM2DBL(rb_funcall(other, rb_intern("y"), 0));
    double z = NUM2DBL(rb_funcall(other, rb_intern("z"), 0));
    double w = NUM2DBL(rb_funcall(other, rb_intern("w"), 0));
    VALUE vec4_class = rb_const_get(mLarb, rb_intern("Vec4"));
    return rb_funcall(
        vec4_class, rb_intern("new"), 4,
        DBL2NUM(a->data[0] * x + a->data[4] * y + a->data[8] * z +
                a->data[12] * w),
        DBL2NUM(a->data[1] * x + a->data[5] * y + a->data[9] * z +
                a->data[13] * w),
        DBL2NUM(a->data[2] * x + a->data[6] * y + a->data[10] * z +
                a->data[14] * w),
        DBL2NUM(a->data[3] * x + a->data[7] * y + a->data[11] * z +
                a->data[15] * w));
  }

  if (rb_obj_is_kind_of(other, cVec3)) {
    VALUE vec4 = rb_funcall(other, rb_intern("to_vec4"), 0);
    return mat4_mul(self, vec4);
  }

  if (rb_obj_is_kind_of(other, rb_cNumeric)) {
    double scalar = NUM2DBL(other);
    double values[16];
    for (int i = 0; i < 16; i++) {
      values[i] = a->data[i] * scalar;
    }
    return mat4_build(rb_obj_class(self), values);
  }

  rb_raise(rb_eTypeError, "unsupported operand for Mat4 multiplication");
}

VALUE mat4_transpose(VALUE self) {
  Mat4Data *a = mat4_get(self);
  return mat4_build16(rb_obj_class(self), a->data[0], a->data[4], a->data[8],
                      a->data[12], a->data[1], a->data[5], a->data[9],
                      a->data[13], a->data[2], a->data[6], a->data[10],
                      a->data[14], a->data[3], a->data[7], a->data[11],
                      a->data[15]);
}

VALUE mat4_inverse(VALUE self) {
  double inverse[16];
  larb_matrix_inverse(mat4_get(self)->data, inverse, 4);
  return mat4_build(rb_obj_class(self), inverse);
}

VALUE mat4_to_a(VALUE self) {
  Mat4Data *a = mat4_get(self);
  VALUE ary = rb_ary_new_capa(16);
  for (int i = 0; i < 16; i++) {
    rb_ary_push(ary, DBL2NUM(a->data[i]));
  }
  return ary;
}

VALUE mat4_data(VALUE self) {
  return mat4_to_a(self);
}

VALUE mat4_determinant(VALUE self) {
  Mat4Data *a = mat4_get(self);
  double *m = a->data;
  double det =
      m[0] *
          (m[5] * (m[10] * m[15] - m[11] * m[14]) -
           m[9] * (m[6] * m[15] - m[7] * m[14]) +
           m[13] * (m[6] * m[11] - m[7] * m[10])) -
      m[4] *
          (m[1] * (m[10] * m[15] - m[11] * m[14]) -
           m[9] * (m[2] * m[15] - m[3] * m[14]) +
           m[13] * (m[2] * m[11] - m[3] * m[10])) +
      m[8] *
          (m[1] * (m[6] * m[15] - m[7] * m[14]) -
           m[5] * (m[2] * m[15] - m[3] * m[14]) +
           m[13] * (m[2] * m[7] - m[3] * m[6])) -
      m[12] *
          (m[1] * (m[6] * m[11] - m[7] * m[10]) -
           m[5] * (m[2] * m[11] - m[3] * m[10]) +
           m[9] * (m[2] * m[7] - m[3] * m[6]));
  return DBL2NUM(det);
}

VALUE mat4_add(VALUE self, VALUE other) {
  Mat4Data *a = mat4_get(self);
  Mat4Data *b = mat4_get(other);
  double values[16];
  for (int i = 0; i < 16; i++) {
    values[i] = a->data[i] + b->data[i];
  }
  return mat4_build(rb_obj_class(self), values);
}

VALUE mat4_sub(VALUE self, VALUE other) {
  Mat4Data *a = mat4_get(self);
  Mat4Data *b = mat4_get(other);
  double values[16];
  for (int i = 0; i < 16; i++) {
    values[i] = a->data[i] - b->data[i];
  }
  return mat4_build(rb_obj_class(self), values);
}

VALUE mat4_equal(VALUE self, VALUE other) {
  if (!rb_obj_is_kind_of(other, cMat4)) {
    return Qfalse;
  }
  Mat4Data *a = mat4_get(self);
  Mat4Data *b = mat4_get(other);
  for (int i = 0; i < 16; i++) {
    if (a->data[i] != b->data[i]) {
      return Qfalse;
    }
  }
  return Qtrue;
}

VALUE mat4_near(int argc, VALUE *argv, VALUE self) {
  VALUE other = Qnil;
  VALUE epsilon = Qnil;

  rb_scan_args(argc, argv, "11", &other, &epsilon);
  Mat4Data *a = mat4_get(self);
  Mat4Data *b = mat4_get(other);
  double eps = argc < 2 ? 1e-6 : NUM2DBL(epsilon);
  if (!isfinite(eps) || eps <= 0.0) {
    rb_raise(rb_eArgError, "epsilon must be finite and positive");
  }

  for (int i = 0; i < 16; i++) {
    if (!(fabs(a->data[i] - b->data[i]) < eps)) {
      return Qfalse;
    }
  }
  return Qtrue;
}

VALUE mat4_extract_translation(VALUE self) {
  Mat4Data *a = mat4_get(self);
  VALUE vec3_class = rb_const_get(mLarb, rb_intern("Vec3"));
  return rb_funcall(vec3_class, rb_intern("new"), 3, DBL2NUM(a->data[12]),
                    DBL2NUM(a->data[13]), DBL2NUM(a->data[14]));
}

static void mat4_decompose(const Mat4Data *a, double *scale, double *rotation) {
  for (int i = 0; i < 16; i++) {
    if (!isfinite(a->data[i])) {
      rb_raise(rb_eArgError, "Cannot decompose non-finite components");
    }
  }
  if (a->data[3] != 0.0 || a->data[7] != 0.0 || a->data[11] != 0.0 ||
      a->data[15] != 1.0) {
    rb_raise(rb_eArgError, "Cannot decompose a perspective matrix");
  }
  for (int col = 0; col < 3; col++) {
    const double *v = a->data + col * 4;
    scale[col] = hypot(hypot(v[0], v[1]), v[2]);
    if (scale[col] == 0.0 || !isfinite(scale[col])) {
      rb_raise(rb_eArgError, "Cannot decompose zero or non-finite scale");
    }
    for (int row = 0; row < 3; row++) {
      rotation[col * 3 + row] = v[row] / scale[col];
    }
  }
  for (int col = 0; col < 3; col++) {
    for (int other = col + 1; other < 3; other++) {
      double dot = 0.0;
      for (int row = 0; row < 3; row++) {
        dot += rotation[col * 3 + row] * rotation[other * 3 + row];
      }
      if (fabs(dot) > 1e-6) {
        rb_raise(rb_eArgError, "Cannot decompose shear");
      }
    }
  }
  double det = rotation[0] * (rotation[4] * rotation[8] - rotation[5] * rotation[7]) -
               rotation[3] * (rotation[1] * rotation[8] - rotation[2] * rotation[7]) +
               rotation[6] * (rotation[1] * rotation[5] - rotation[2] * rotation[4]);
  if (det < 0.0) {
    scale[0] = -scale[0];
    for (int row = 0; row < 3; row++) rotation[row] = -rotation[row];
  }
}

VALUE mat4_extract_scale(VALUE self) {
  double scale[3], rotation[9];
  mat4_decompose(mat4_get(self), scale, rotation);
  return rb_funcall(cVec3, rb_intern("new"), 3, DBL2NUM(scale[0]),
                    DBL2NUM(scale[1]), DBL2NUM(scale[2]));
}

VALUE mat4_extract_rotation(VALUE self) {
  double scale[3], rotation[9], q[4];
  mat4_decompose(mat4_get(self), scale, rotation);
  double m00 = rotation[0], m01 = rotation[1], m02 = rotation[2];
  double m10 = rotation[3], m11 = rotation[4], m12 = rotation[5];
  double m20 = rotation[6], m21 = rotation[7], m22 = rotation[8];
  double trace = m00 + m11 + m22;
  if (trace > 0.0) {
    double s = 0.5 / sqrt(trace + 1.0);
    q[0] = (m12 - m21) * s;
    q[1] = (m20 - m02) * s;
    q[2] = (m01 - m10) * s;
    q[3] = 0.25 / s;
  } else if (m00 > m11 && m00 > m22) {
    double s = 2.0 * sqrt(1.0 + m00 - m11 - m22);
    q[0] = 0.25 * s;
    q[1] = (m10 + m01) / s;
    q[2] = (m20 + m02) / s;
    q[3] = (m12 - m21) / s;
  } else if (m11 > m22) {
    double s = 2.0 * sqrt(1.0 + m11 - m00 - m22);
    q[0] = (m10 + m01) / s;
    q[1] = 0.25 * s;
    q[2] = (m21 + m12) / s;
    q[3] = (m20 - m02) / s;
  } else {
    double s = 2.0 * sqrt(1.0 + m22 - m00 - m11);
    q[0] = (m20 + m02) / s;
    q[1] = (m21 + m12) / s;
    q[2] = 0.25 * s;
    q[3] = (m01 - m10) / s;
  }
  larb_normalize(q, 4);
  return rb_funcall(cQuat, rb_intern("new"), 4,
                    DBL2NUM(q[0]), DBL2NUM(q[1]), DBL2NUM(q[2]), DBL2NUM(q[3]));
}

static VALUE mat4_format_value(double value) {
  return rb_funcall(rb_mKernel, rb_intern("format"), 2,
                    rb_str_new_cstr("%8.4f"), DBL2NUM(value));
}

static VALUE mat4_row_string(const double *values, int offset) {
  VALUE str = mat4_format_value(values[offset]);
  for (int i = 1; i < 4; i++) {
    rb_str_cat_cstr(str, ", ");
    rb_str_concat(str, mat4_format_value(values[offset + i]));
  }
  return str;
}

VALUE mat4_inspect(VALUE self) {
  Mat4Data *a = mat4_get(self);
  VALUE str = rb_str_new_cstr("Mat4[\n  ");
  rb_str_concat(str, mat4_row_string(a->data, 0));
  rb_str_cat_cstr(str, "\n  ");
  rb_str_concat(str, mat4_row_string(a->data, 4));
  rb_str_cat_cstr(str, "\n  ");
  rb_str_concat(str, mat4_row_string(a->data, 8));
  rb_str_cat_cstr(str, "\n  ");
  rb_str_concat(str, mat4_row_string(a->data, 12));
  rb_str_cat_cstr(str, "\n]");
  return str;
}

void Init_mat4(VALUE module) {
  cMat4 = rb_define_class_under(module, "Mat4", rb_cObject);
  cVec3 = rb_const_get(mLarb, rb_intern("Vec3"));
  cVec4 = rb_const_get(mLarb, rb_intern("Vec4"));
  cQuat = rb_const_get(mLarb, rb_intern("Quat"));

  rb_define_alloc_func(cMat4, mat4_alloc);
  rb_define_method(cMat4, "initialize", mat4_initialize, -1);
  rb_define_method(cMat4, "initialize_copy", mat4_initialize_copy, 1);

  rb_define_singleton_method(cMat4, "identity", mat4_class_identity, 0);
  rb_define_singleton_method(cMat4, "zero", mat4_class_zero, 0);
  rb_define_singleton_method(cMat4, "translation", mat4_class_translation, 3);
  rb_define_singleton_method(cMat4, "scaling", mat4_class_scaling, 3);
  rb_define_singleton_method(cMat4, "rotation_x", mat4_class_rotation_x, 1);
  rb_define_singleton_method(cMat4, "rotation_y", mat4_class_rotation_y, 1);
  rb_define_singleton_method(cMat4, "rotation_z", mat4_class_rotation_z, 1);
  rb_define_singleton_method(cMat4, "rotation", mat4_class_rotation, 2);
  rb_define_singleton_method(cMat4, "look_at", mat4_class_look_at, 3);
  rb_define_singleton_method(cMat4, "perspective", mat4_class_perspective, 4);
  rb_define_singleton_method(cMat4, "orthographic", mat4_class_orthographic,
                             6);
  rb_define_singleton_method(cMat4, "frustum", mat4_class_frustum, 6);
  rb_define_singleton_method(cMat4, "from_quaternion",
                             mat4_class_from_quaternion, 1);
  rb_define_singleton_method(cMat4, "trs", mat4_class_trs, 3);

  rb_define_method(cMat4, "data", mat4_data, 0);
  rb_define_method(cMat4, "[]", mat4_aref, 1);
  rb_define_method(cMat4, "[]=", mat4_aset, 2);
  rb_define_method(cMat4, "*", mat4_mul, 1);
  rb_define_method(cMat4, "transpose", mat4_transpose, 0);
  rb_define_method(cMat4, "inverse", mat4_inverse, 0);
  rb_define_method(cMat4, "to_a", mat4_to_a, 0);
  rb_define_method(cMat4, "determinant", mat4_determinant, 0);
  rb_define_method(cMat4, "+", mat4_add, 1);
  rb_define_method(cMat4, "-", mat4_sub, 1);
  rb_define_method(cMat4, "==", mat4_equal, 1);
  rb_define_method(cMat4, "near?", mat4_near, -1);
  rb_define_method(cMat4, "extract_translation", mat4_extract_translation, 0);
  rb_define_method(cMat4, "extract_scale", mat4_extract_scale, 0);
  rb_define_method(cMat4, "extract_rotation", mat4_extract_rotation, 0);
  rb_define_method(cMat4, "inspect", mat4_inspect, 0);
}
