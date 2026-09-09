#include "mat2d.h"
#include "matrix_utils.h"

#include <math.h>

static void mat2d_free(void *ptr) {
  xfree(ptr);
}

static size_t mat2d_memsize(const void *ptr) {
  return sizeof(Mat2dData);
}

static const rb_data_type_t mat2d_type = {
    "Mat2d",
    {0, mat2d_free, mat2d_memsize},
    0,
    0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};

static VALUE cMat2d = Qnil;
static VALUE cVec2 = Qnil;
static VALUE cMat3 = Qnil;

static Mat2dData *mat2d_get(VALUE obj) {
  Mat2dData *data = NULL;
  TypedData_Get_Struct(obj, Mat2dData, &mat2d_type, data);
  return data;
}

static VALUE mat2d_build(VALUE klass, const double *values) {
  VALUE obj = mat2d_alloc(klass);
  Mat2dData *data = mat2d_get(obj);
  for (int i = 0; i < 6; i++) {
    data->data[i] = values[i];
  }
  return obj;
}

static VALUE mat2d_build6(VALUE klass, double v0, double v1, double v2,
                          double v3, double v4, double v5) {
  double values[6] = {v0, v1, v2, v3, v4, v5};
  return mat2d_build(klass, values);
}

VALUE mat2d_alloc(VALUE klass) {
  Mat2dData *data = ALLOC(Mat2dData);
  data->data[0] = 1.0;
  data->data[1] = 0.0;
  data->data[2] = 0.0;
  data->data[3] = 1.0;
  data->data[4] = 0.0;
  data->data[5] = 0.0;
  return TypedData_Wrap_Struct(klass, &mat2d_type, data);
}

VALUE mat2d_initialize(int argc, VALUE *argv, VALUE self) {
  rb_check_frozen(self);
  VALUE data_arg = Qnil;
  Mat2dData values = {{1.0, 0.0, 0.0, 1.0, 0.0, 0.0}};
  rb_scan_args(argc, argv, "01", &data_arg);
  if (argc != 0) {
    VALUE ary = rb_check_array_type(data_arg);
    if (NIL_P(ary)) {
      rb_raise(rb_eTypeError, "expected Array");
    }
    if (RARRAY_LEN(ary) != 6) {
      rb_raise(rb_eArgError, "expected 6 elements");
    }
    for (int i = 0; i < 6; i++) {
      values.data[i] = NUM2DBL(rb_ary_entry(ary, i));
    }
  }
  rb_check_frozen(self);
  *mat2d_get(self) = values;
  return self;
}

static VALUE mat2d_initialize_copy(VALUE self, VALUE other) {
  if (self == other) return self;
  rb_obj_init_copy(self, other);
  *mat2d_get(self) = *mat2d_get(other);
  return self;
}

static VALUE mat2d_class_identity(VALUE klass) {
  return mat2d_alloc(klass);
}

static VALUE mat2d_class_zero(VALUE klass) {
  return mat2d_build6(klass, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}

static VALUE mat2d_class_translation(VALUE klass, VALUE x, VALUE y) {
  return mat2d_build6(klass, 1.0, 0.0, 0.0, 1.0, NUM2DBL(x),
                      NUM2DBL(y));
}

static VALUE mat2d_class_rotation(VALUE klass, VALUE radians) {
  double r = NUM2DBL(radians);
  double c = cos(r);
  double s = sin(r);
  return mat2d_build6(klass, c, s, -s, c, 0.0, 0.0);
}

static VALUE mat2d_class_scaling(VALUE klass, VALUE x, VALUE y) {
  return mat2d_build6(klass, NUM2DBL(x), 0.0, 0.0,
                      NUM2DBL(y), 0.0, 0.0);
}

static VALUE mat2d_class_from_rotation_translation_scale(VALUE klass,
                                                         VALUE rotation,
                                                         VALUE translation,
                                                         VALUE scale) {
  double r = NUM2DBL(rotation);
  double c = cos(r);
  double s = sin(r);
  double sx = NUM2DBL(rb_funcall(scale, rb_intern("x"), 0));
  double sy = NUM2DBL(rb_funcall(scale, rb_intern("y"), 0));
  double tx = NUM2DBL(rb_funcall(translation, rb_intern("x"), 0));
  double ty = NUM2DBL(rb_funcall(translation, rb_intern("y"), 0));

  return mat2d_build6(klass, c * sx, s * sx, -s * sy, c * sy, tx, ty);
}

VALUE mat2d_aref(VALUE self, VALUE index) {
  Mat2dData *data = mat2d_get(self);
  long idx = NUM2LONG(index);
  if (idx < 0 || idx > 5) {
    return Qnil;
  }
  return DBL2NUM(data->data[idx]);
}

VALUE mat2d_aset(VALUE self, VALUE index, VALUE value) {
  rb_check_frozen(self);
  Mat2dData *data = mat2d_get(self);
  long idx = NUM2LONG(index);
  if (idx < 0 || idx > 5) {
    rb_raise(rb_eIndexError, "index %ld out of range", idx);
  }
  double component = NUM2DBL(value);
  rb_check_frozen(self);
  data->data[idx] = component;
  return value;
}

VALUE mat2d_mul(VALUE self, VALUE other) {
  Mat2dData *a = mat2d_get(self);

  if (rb_obj_is_kind_of(other, cMat2d)) {
    Mat2dData *b = mat2d_get(other);
    return mat2d_build6(
        rb_obj_class(self),
        a->data[0] * b->data[0] + a->data[2] * b->data[1],
        a->data[1] * b->data[0] + a->data[3] * b->data[1],
        a->data[0] * b->data[2] + a->data[2] * b->data[3],
        a->data[1] * b->data[2] + a->data[3] * b->data[3],
        a->data[0] * b->data[4] + a->data[2] * b->data[5] + a->data[4],
        a->data[1] * b->data[4] + a->data[3] * b->data[5] + a->data[5]);
  }

  if (rb_obj_is_kind_of(other, cVec2)) {
    double x = NUM2DBL(rb_funcall(other, rb_intern("x"), 0));
    double y = NUM2DBL(rb_funcall(other, rb_intern("y"), 0));
    VALUE vec2_class = rb_const_get(mLarb, rb_intern("Vec2"));
    return rb_funcall(vec2_class, rb_intern("new"), 2,
                      DBL2NUM(a->data[0] * x + a->data[2] * y + a->data[4]),
                      DBL2NUM(a->data[1] * x + a->data[3] * y + a->data[5]));
  }

  if (rb_obj_is_kind_of(other, rb_cNumeric)) {
    double s = NUM2DBL(other);
    return mat2d_build6(rb_obj_class(self), a->data[0] * s, a->data[1] * s,
                        a->data[2] * s, a->data[3] * s, a->data[4] * s,
                        a->data[5] * s);
  }

  rb_raise(rb_eTypeError, "unsupported operand for Mat2d multiplication");
}

VALUE mat2d_add(VALUE self, VALUE other) {
  Mat2dData *a = mat2d_get(self);
  Mat2dData *b = mat2d_get(other);
  return mat2d_build6(rb_obj_class(self), a->data[0] + b->data[0],
                      a->data[1] + b->data[1], a->data[2] + b->data[2],
                      a->data[3] + b->data[3], a->data[4] + b->data[4],
                      a->data[5] + b->data[5]);
}

VALUE mat2d_sub(VALUE self, VALUE other) {
  Mat2dData *a = mat2d_get(self);
  Mat2dData *b = mat2d_get(other);
  return mat2d_build6(rb_obj_class(self), a->data[0] - b->data[0],
                      a->data[1] - b->data[1], a->data[2] - b->data[2],
                      a->data[3] - b->data[3], a->data[4] - b->data[4],
                      a->data[5] - b->data[5]);
}

VALUE mat2d_determinant(VALUE self) {
  Mat2dData *a = mat2d_get(self);
  return DBL2NUM(a->data[0] * a->data[3] - a->data[1] * a->data[2]);
}

VALUE mat2d_inverse(VALUE self) {
  const double *a = mat2d_get(self)->data;
  double values[9] = {a[0], a[1], 0.0, a[2], a[3], 0.0, a[4], a[5], 1.0};
  double inverse[9];
  larb_matrix_inverse(values, inverse, 3);
  return mat2d_build6(rb_obj_class(self), inverse[0], inverse[1], inverse[3],
                      inverse[4], inverse[6], inverse[7]);
}

VALUE mat2d_translate(VALUE self, VALUE x, VALUE y) {
  Mat2dData *a = mat2d_get(self);
  double dx = NUM2DBL(x);
  double dy = NUM2DBL(y);
  return mat2d_build6(rb_obj_class(self), a->data[0], a->data[1], a->data[2],
                      a->data[3], a->data[0] * dx + a->data[2] * dy + a->data[4],
                      a->data[1] * dx + a->data[3] * dy + a->data[5]);
}

VALUE mat2d_rotate(VALUE self, VALUE radians) {
  VALUE rot = mat2d_class_rotation(cMat2d, radians);
  return mat2d_mul(self, rot);
}

VALUE mat2d_scale(VALUE self, VALUE x, VALUE y) {
  Mat2dData *a = mat2d_get(self);
  double sx = NUM2DBL(x);
  double sy = NUM2DBL(y);
  return mat2d_build6(rb_obj_class(self), a->data[0] * sx, a->data[1] * sx,
                      a->data[2] * sy, a->data[3] * sy, a->data[4], a->data[5]);
}

VALUE mat2d_extract_translation(VALUE self) {
  Mat2dData *a = mat2d_get(self);
  VALUE vec2_class = rb_const_get(mLarb, rb_intern("Vec2"));
  return rb_funcall(vec2_class, rb_intern("new"), 2, DBL2NUM(a->data[4]),
                    DBL2NUM(a->data[5]));
}

static void mat2d_decompose(const Mat2dData *a, double *sx, double *sy) {
  for (int i = 0; i < 6; i++) {
    if (!isfinite(a->data[i])) {
      rb_raise(rb_eArgError, "Cannot decompose non-finite components");
    }
  }
  *sx = hypot(a->data[0], a->data[1]);
  *sy = hypot(a->data[2], a->data[3]);
  if (*sx == 0.0 || *sy == 0.0 || !isfinite(*sx) || !isfinite(*sy)) {
    rb_raise(rb_eArgError, "Cannot decompose zero or non-finite scale");
  }
  double x0 = a->data[0] / *sx, y0 = a->data[1] / *sx;
  double x1 = a->data[2] / *sy, y1 = a->data[3] / *sy;
  if (fabs(x0 * x1 + y0 * y1) > 1e-6) {
    rb_raise(rb_eArgError, "Cannot decompose shear");
  }
  if (x0 * y1 - y0 * x1 < 0.0) *sx = -*sx;
}

VALUE mat2d_extract_rotation(VALUE self) {
  Mat2dData *a = mat2d_get(self);
  double sx, sy;
  mat2d_decompose(a, &sx, &sy);
  return DBL2NUM(atan2(a->data[1] / sx, a->data[0] / sx));
}

VALUE mat2d_extract_scale(VALUE self) {
  double sx, sy;
  mat2d_decompose(mat2d_get(self), &sx, &sy);
  return rb_funcall(cVec2, rb_intern("new"), 2, DBL2NUM(sx), DBL2NUM(sy));
}

VALUE mat2d_frobenius_norm(VALUE self) {
  Mat2dData *a = mat2d_get(self);
  double norm = 0.0;
  for (int i = 0; i < 6; i++) {
    norm = hypot(norm, a->data[i]);
  }
  return DBL2NUM(norm);
}

VALUE mat2d_to_mat3(VALUE self) {
  Mat2dData *a = mat2d_get(self);
  VALUE mat3_class = rb_const_get(mLarb, rb_intern("Mat3"));
  return rb_funcall(mat3_class, rb_intern("new"), 1,
                    rb_ary_new3(9, DBL2NUM(a->data[0]), DBL2NUM(a->data[1]),
                                DBL2NUM(0.0), DBL2NUM(a->data[2]),
                                DBL2NUM(a->data[3]), DBL2NUM(0.0),
                                DBL2NUM(a->data[4]), DBL2NUM(a->data[5]),
                                DBL2NUM(1.0)));
}

VALUE mat2d_to_a(VALUE self) {
  Mat2dData *a = mat2d_get(self);
  VALUE ary = rb_ary_new_capa(6);
  for (int i = 0; i < 6; i++) {
    rb_ary_push(ary, DBL2NUM(a->data[i]));
  }
  return ary;
}

VALUE mat2d_data(VALUE self) {
  return mat2d_to_a(self);
}

VALUE mat2d_equal(VALUE self, VALUE other) {
  if (!rb_obj_is_kind_of(other, cMat2d)) {
    return Qfalse;
  }
  Mat2dData *a = mat2d_get(self);
  Mat2dData *b = mat2d_get(other);
  for (int i = 0; i < 6; i++) {
    if (a->data[i] != b->data[i]) {
      return Qfalse;
    }
  }
  return Qtrue;
}

VALUE mat2d_near(int argc, VALUE *argv, VALUE self) {
  VALUE other = Qnil;
  VALUE epsilon = Qnil;

  rb_scan_args(argc, argv, "11", &other, &epsilon);
  Mat2dData *a = mat2d_get(self);
  Mat2dData *b = mat2d_get(other);
  double eps = argc < 2 ? 1e-6 : NUM2DBL(epsilon);
  if (!isfinite(eps) || eps <= 0.0) {
    rb_raise(rb_eArgError, "epsilon must be finite and positive");
  }

  for (int i = 0; i < 6; i++) {
    if (!(fabs(a->data[i] - b->data[i]) < eps)) {
      return Qfalse;
    }
  }
  return Qtrue;
}

VALUE mat2d_inspect(VALUE self) {
  Mat2dData *a = mat2d_get(self);
  VALUE str = rb_str_new_cstr("Mat2d[");
  for (int i = 0; i < 6; i++) {
    VALUE formatted = rb_funcall(rb_mKernel, rb_intern("format"), 2,
                                 rb_str_new_cstr("%.4f"), DBL2NUM(a->data[i]));
    if (i > 0) {
      rb_str_cat_cstr(str, ", ");
    }
    rb_str_concat(str, formatted);
  }
  rb_str_cat_cstr(str, "]");
  return str;
}

void Init_mat2d(VALUE module) {
  cMat2d = rb_define_class_under(module, "Mat2d", rb_cObject);
  cVec2 = rb_const_get(mLarb, rb_intern("Vec2"));
  cMat3 = rb_const_get(mLarb, rb_intern("Mat3"));

  rb_define_alloc_func(cMat2d, mat2d_alloc);
  rb_define_method(cMat2d, "initialize", mat2d_initialize, -1);
  rb_define_method(cMat2d, "initialize_copy", mat2d_initialize_copy, 1);

  rb_define_singleton_method(cMat2d, "identity", mat2d_class_identity, 0);
  rb_define_singleton_method(cMat2d, "zero", mat2d_class_zero, 0);
  rb_define_singleton_method(cMat2d, "translation", mat2d_class_translation,
                             2);
  rb_define_singleton_method(cMat2d, "rotation", mat2d_class_rotation, 1);
  rb_define_singleton_method(cMat2d, "scaling", mat2d_class_scaling, 2);
  rb_define_singleton_method(cMat2d, "from_rotation_translation_scale",
                             mat2d_class_from_rotation_translation_scale, 3);

  rb_define_method(cMat2d, "data", mat2d_data, 0);
  rb_define_method(cMat2d, "[]", mat2d_aref, 1);
  rb_define_method(cMat2d, "[]=", mat2d_aset, 2);
  rb_define_method(cMat2d, "*", mat2d_mul, 1);
  rb_define_method(cMat2d, "+", mat2d_add, 1);
  rb_define_method(cMat2d, "-", mat2d_sub, 1);
  rb_define_method(cMat2d, "determinant", mat2d_determinant, 0);
  rb_define_method(cMat2d, "inverse", mat2d_inverse, 0);
  rb_define_method(cMat2d, "translate", mat2d_translate, 2);
  rb_define_method(cMat2d, "rotate", mat2d_rotate, 1);
  rb_define_method(cMat2d, "scale", mat2d_scale, 2);
  rb_define_method(cMat2d, "extract_translation", mat2d_extract_translation, 0);
  rb_define_method(cMat2d, "extract_rotation", mat2d_extract_rotation, 0);
  rb_define_method(cMat2d, "extract_scale", mat2d_extract_scale, 0);
  rb_define_method(cMat2d, "frobenius_norm", mat2d_frobenius_norm, 0);
  rb_define_method(cMat2d, "to_mat3", mat2d_to_mat3, 0);
  rb_define_method(cMat2d, "to_a", mat2d_to_a, 0);
  rb_define_method(cMat2d, "==", mat2d_equal, 1);
  rb_define_method(cMat2d, "near?", mat2d_near, -1);
  rb_define_method(cMat2d, "inspect", mat2d_inspect, 0);
  rb_define_alias(cMat2d, "to_s", "inspect");
}
