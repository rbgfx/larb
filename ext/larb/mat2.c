#include "mat2.h"
#include "matrix_utils.h"

#include <math.h>

static void mat2_free(void *ptr) {
  xfree(ptr);
}

static size_t mat2_memsize(const void *ptr) {
  return sizeof(Mat2Data);
}

static const rb_data_type_t mat2_type = {
    "Mat2",
    {0, mat2_free, mat2_memsize},
    0,
    0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};

static VALUE cMat2 = Qnil;
static VALUE cVec2 = Qnil;

static Mat2Data *mat2_get(VALUE obj) {
  Mat2Data *data = NULL;
  TypedData_Get_Struct(obj, Mat2Data, &mat2_type, data);
  return data;
}

static VALUE mat2_build(VALUE klass, double a0, double a1, double a2,
                        double a3) {
  VALUE obj = mat2_alloc(klass);
  Mat2Data *data = mat2_get(obj);
  data->data[0] = a0;
  data->data[1] = a1;
  data->data[2] = a2;
  data->data[3] = a3;
  return obj;
}

VALUE mat2_alloc(VALUE klass) {
  Mat2Data *data = ALLOC(Mat2Data);
  data->data[0] = 1.0;
  data->data[1] = 0.0;
  data->data[2] = 0.0;
  data->data[3] = 1.0;
  return TypedData_Wrap_Struct(klass, &mat2_type, data);
}

VALUE mat2_initialize(int argc, VALUE *argv, VALUE self) {
  rb_check_frozen(self);
  VALUE data_arg = Qnil;
  Mat2Data values = {{1.0, 0.0, 0.0, 1.0}};
  rb_scan_args(argc, argv, "01", &data_arg);
  if (argc != 0) {
    VALUE ary = rb_check_array_type(data_arg);
    if (NIL_P(ary)) {
      rb_raise(rb_eTypeError, "expected Array");
    }
    if (RARRAY_LEN(ary) != 4) {
      rb_raise(rb_eArgError, "expected 4 elements");
    }
    for (int i = 0; i < 4; i++) {
      values.data[i] = NUM2DBL(rb_ary_entry(ary, i));
    }
  }
  rb_check_frozen(self);
  *mat2_get(self) = values;
  return self;
}

static VALUE mat2_initialize_copy(VALUE self, VALUE other) {
  if (self == other) return self;
  rb_obj_init_copy(self, other);
  *mat2_get(self) = *mat2_get(other);
  return self;
}

static VALUE mat2_class_identity(VALUE klass) {
  return mat2_build(klass, 1.0, 0.0, 0.0, 1.0);
}

static VALUE mat2_class_zero(VALUE klass) {
  return mat2_build(klass, 0.0, 0.0, 0.0, 0.0);
}

static VALUE mat2_class_rotation(VALUE klass, VALUE radians) {
  double r = NUM2DBL(radians);
  double c = cos(r);
  double s = sin(r);
  return mat2_build(klass, c, s, -s, c);
}

static VALUE mat2_class_scaling(VALUE klass, VALUE x, VALUE y) {
  return mat2_build(klass, NUM2DBL(x), 0.0, 0.0, NUM2DBL(y));
}

static VALUE mat2_class_from_vec2(VALUE klass, VALUE v1, VALUE v2) {
  double x1 = NUM2DBL(rb_funcall(v1, rb_intern("x"), 0));
  double y1 = NUM2DBL(rb_funcall(v1, rb_intern("y"), 0));
  double x2 = NUM2DBL(rb_funcall(v2, rb_intern("x"), 0));
  double y2 = NUM2DBL(rb_funcall(v2, rb_intern("y"), 0));
  return mat2_build(klass, x1, y1, x2, y2);
}

VALUE mat2_aref(VALUE self, VALUE index) {
  Mat2Data *data = mat2_get(self);
  long idx = NUM2LONG(index);
  if (idx < 0 || idx > 3) {
    return Qnil;
  }
  return DBL2NUM(data->data[idx]);
}

VALUE mat2_aset(VALUE self, VALUE index, VALUE value) {
  rb_check_frozen(self);
  Mat2Data *data = mat2_get(self);
  long idx = NUM2LONG(index);
  if (idx < 0 || idx > 3) {
    rb_raise(rb_eIndexError, "index %ld out of range", idx);
  }
  double component = NUM2DBL(value);
  rb_check_frozen(self);
  data->data[idx] = component;
  return value;
}

VALUE mat2_mul(VALUE self, VALUE other) {
  Mat2Data *a = mat2_get(self);

  if (rb_obj_is_kind_of(other, cMat2)) {
    Mat2Data *b = mat2_get(other);
    return mat2_build(rb_obj_class(self),
                      a->data[0] * b->data[0] + a->data[2] * b->data[1],
                      a->data[1] * b->data[0] + a->data[3] * b->data[1],
                      a->data[0] * b->data[2] + a->data[2] * b->data[3],
                      a->data[1] * b->data[2] + a->data[3] * b->data[3]);
  }

  if (rb_obj_is_kind_of(other, cVec2)) {
    double x = NUM2DBL(rb_funcall(other, rb_intern("x"), 0));
    double y = NUM2DBL(rb_funcall(other, rb_intern("y"), 0));
    VALUE vec2_class = rb_const_get(mLarb, rb_intern("Vec2"));
    return rb_funcall(vec2_class, rb_intern("new"), 2,
                      DBL2NUM(a->data[0] * x + a->data[2] * y),
                      DBL2NUM(a->data[1] * x + a->data[3] * y));
  }

  if (rb_obj_is_kind_of(other, rb_cNumeric)) {
    double s = NUM2DBL(other);
    return mat2_build(rb_obj_class(self), a->data[0] * s, a->data[1] * s,
                      a->data[2] * s, a->data[3] * s);
  }

  rb_raise(rb_eTypeError, "unsupported operand for Mat2 multiplication");
}

VALUE mat2_add(VALUE self, VALUE other) {
  Mat2Data *a = mat2_get(self);
  Mat2Data *b = mat2_get(other);
  return mat2_build(rb_obj_class(self), a->data[0] + b->data[0],
                    a->data[1] + b->data[1], a->data[2] + b->data[2],
                    a->data[3] + b->data[3]);
}

VALUE mat2_sub(VALUE self, VALUE other) {
  Mat2Data *a = mat2_get(self);
  Mat2Data *b = mat2_get(other);
  return mat2_build(rb_obj_class(self), a->data[0] - b->data[0],
                    a->data[1] - b->data[1], a->data[2] - b->data[2],
                    a->data[3] - b->data[3]);
}

VALUE mat2_determinant(VALUE self) {
  Mat2Data *a = mat2_get(self);
  return DBL2NUM(a->data[0] * a->data[3] - a->data[2] * a->data[1]);
}

VALUE mat2_inverse(VALUE self) {
  double inverse[4];
  larb_matrix_inverse(mat2_get(self)->data, inverse, 2);
  return mat2_build(rb_obj_class(self), inverse[0], inverse[1], inverse[2],
                    inverse[3]);
}

VALUE mat2_transpose(VALUE self) {
  Mat2Data *a = mat2_get(self);
  return mat2_build(rb_obj_class(self), a->data[0], a->data[2], a->data[1],
                    a->data[3]);
}

VALUE mat2_adjoint(VALUE self) {
  Mat2Data *a = mat2_get(self);
  return mat2_build(rb_obj_class(self), a->data[3], -a->data[1], -a->data[2],
                    a->data[0]);
}

VALUE mat2_frobenius_norm(VALUE self) {
  Mat2Data *a = mat2_get(self);
  double norm = 0.0;
  for (int i = 0; i < 4; i++) {
    norm = hypot(norm, a->data[i]);
  }
  return DBL2NUM(norm);
}

VALUE mat2_to_a(VALUE self) {
  Mat2Data *a = mat2_get(self);
  VALUE ary = rb_ary_new_capa(4);
  rb_ary_push(ary, DBL2NUM(a->data[0]));
  rb_ary_push(ary, DBL2NUM(a->data[1]));
  rb_ary_push(ary, DBL2NUM(a->data[2]));
  rb_ary_push(ary, DBL2NUM(a->data[3]));
  return ary;
}

VALUE mat2_data(VALUE self) {
  return mat2_to_a(self);
}

VALUE mat2_equal(VALUE self, VALUE other) {
  if (!rb_obj_is_kind_of(other, cMat2)) {
    return Qfalse;
  }
  Mat2Data *a = mat2_get(self);
  Mat2Data *b = mat2_get(other);
  return (a->data[0] == b->data[0] && a->data[1] == b->data[1] &&
          a->data[2] == b->data[2] && a->data[3] == b->data[3])
             ? Qtrue
             : Qfalse;
}

VALUE mat2_near(int argc, VALUE *argv, VALUE self) {
  VALUE other = Qnil;
  VALUE epsilon = Qnil;

  rb_scan_args(argc, argv, "11", &other, &epsilon);
  Mat2Data *a = mat2_get(self);
  Mat2Data *b = mat2_get(other);
  double eps = argc < 2 ? 1e-6 : NUM2DBL(epsilon);
  if (!isfinite(eps) || eps <= 0.0) {
    rb_raise(rb_eArgError, "epsilon must be finite and positive");
  }

  if (fabs(a->data[0] - b->data[0]) < eps &&
      fabs(a->data[1] - b->data[1]) < eps &&
      fabs(a->data[2] - b->data[2]) < eps &&
      fabs(a->data[3] - b->data[3]) < eps) {
    return Qtrue;
  }
  return Qfalse;
}

VALUE mat2_inspect(VALUE self) {
  Mat2Data *a = mat2_get(self);
  VALUE s0 = rb_funcall(DBL2NUM(a->data[0]), rb_intern("to_s"), 0);
  VALUE s1 = rb_funcall(DBL2NUM(a->data[1]), rb_intern("to_s"), 0);
  VALUE s2 = rb_funcall(DBL2NUM(a->data[2]), rb_intern("to_s"), 0);
  VALUE s3 = rb_funcall(DBL2NUM(a->data[3]), rb_intern("to_s"), 0);
  VALUE str = rb_str_new_cstr("Mat2[");
  rb_str_concat(str, s0);
  rb_str_cat_cstr(str, ", ");
  rb_str_concat(str, s1);
  rb_str_cat_cstr(str, ", ");
  rb_str_concat(str, s2);
  rb_str_cat_cstr(str, ", ");
  rb_str_concat(str, s3);
  rb_str_cat_cstr(str, "]");
  return str;
}

void Init_mat2(VALUE module) {
  cMat2 = rb_define_class_under(module, "Mat2", rb_cObject);
  cVec2 = rb_const_get(mLarb, rb_intern("Vec2"));

  rb_define_alloc_func(cMat2, mat2_alloc);
  rb_define_method(cMat2, "initialize", mat2_initialize, -1);
  rb_define_method(cMat2, "initialize_copy", mat2_initialize_copy, 1);

  rb_define_singleton_method(cMat2, "identity", mat2_class_identity, 0);
  rb_define_singleton_method(cMat2, "zero", mat2_class_zero, 0);
  rb_define_singleton_method(cMat2, "rotation", mat2_class_rotation, 1);
  rb_define_singleton_method(cMat2, "scaling", mat2_class_scaling, 2);
  rb_define_singleton_method(cMat2, "from_vec2", mat2_class_from_vec2, 2);

  rb_define_method(cMat2, "data", mat2_data, 0);
  rb_define_method(cMat2, "[]", mat2_aref, 1);
  rb_define_method(cMat2, "[]=", mat2_aset, 2);
  rb_define_method(cMat2, "*", mat2_mul, 1);
  rb_define_method(cMat2, "+", mat2_add, 1);
  rb_define_method(cMat2, "-", mat2_sub, 1);
  rb_define_method(cMat2, "determinant", mat2_determinant, 0);
  rb_define_method(cMat2, "inverse", mat2_inverse, 0);
  rb_define_method(cMat2, "transpose", mat2_transpose, 0);
  rb_define_method(cMat2, "adjoint", mat2_adjoint, 0);
  rb_define_method(cMat2, "frobenius_norm", mat2_frobenius_norm, 0);
  rb_define_method(cMat2, "to_a", mat2_to_a, 0);
  rb_define_method(cMat2, "==", mat2_equal, 1);
  rb_define_method(cMat2, "near?", mat2_near, -1);
  rb_define_method(cMat2, "inspect", mat2_inspect, 0);
  rb_define_alias(cMat2, "to_s", "inspect");
}
