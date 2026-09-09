#include "mat3.h"
#include "matrix_utils.h"

#include <math.h>

static void mat3_free(void *ptr) {
  xfree(ptr);
}

static size_t mat3_memsize(const void *ptr) {
  return sizeof(Mat3Data);
}

static const rb_data_type_t mat3_type = {
    "Mat3",
    {0, mat3_free, mat3_memsize},
    0,
    0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};

static VALUE cMat3 = Qnil;
static VALUE cVec3 = Qnil;

static Mat3Data *mat3_get(VALUE obj) {
  Mat3Data *data = NULL;
  TypedData_Get_Struct(obj, Mat3Data, &mat3_type, data);
  return data;
}

static VALUE mat3_build(VALUE klass, const double *values) {
  VALUE obj = mat3_alloc(klass);
  Mat3Data *data = mat3_get(obj);
  for (int i = 0; i < 9; i++) {
    data->data[i] = values[i];
  }
  return obj;
}

static VALUE mat3_build9(VALUE klass, double v0, double v1, double v2,
                         double v3, double v4, double v5, double v6,
                         double v7, double v8) {
  double values[9] = {v0, v1, v2, v3, v4, v5, v6, v7, v8};
  return mat3_build(klass, values);
}

VALUE mat3_alloc(VALUE klass) {
  Mat3Data *data = ALLOC(Mat3Data);
  data->data[0] = 1.0;
  data->data[1] = 0.0;
  data->data[2] = 0.0;
  data->data[3] = 0.0;
  data->data[4] = 1.0;
  data->data[5] = 0.0;
  data->data[6] = 0.0;
  data->data[7] = 0.0;
  data->data[8] = 1.0;
  return TypedData_Wrap_Struct(klass, &mat3_type, data);
}

VALUE mat3_initialize(int argc, VALUE *argv, VALUE self) {
  rb_check_frozen(self);
  VALUE data_arg = Qnil;
  Mat3Data values = {{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}};
  rb_scan_args(argc, argv, "01", &data_arg);
  if (argc != 0) {
    VALUE ary = rb_check_array_type(data_arg);
    if (NIL_P(ary)) {
      rb_raise(rb_eTypeError, "expected Array");
    }
    if (RARRAY_LEN(ary) != 9) {
      rb_raise(rb_eArgError, "expected 9 elements");
    }
    for (int i = 0; i < 9; i++) {
      values.data[i] = NUM2DBL(rb_ary_entry(ary, i));
    }
  }
  rb_check_frozen(self);
  *mat3_get(self) = values;
  return self;
}

static VALUE mat3_initialize_copy(VALUE self, VALUE other) {
  if (self == other) return self;
  rb_obj_init_copy(self, other);
  *mat3_get(self) = *mat3_get(other);
  return self;
}

static VALUE mat3_class_identity(VALUE klass) {
  return mat3_alloc(klass);
}

static VALUE mat3_class_zero(VALUE klass) {
  return mat3_build9(klass, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}

static VALUE mat3_class_from_mat4(VALUE klass, VALUE mat4) {
  double values[9];
  values[0] = NUM2DBL(rb_funcall(mat4, rb_intern("[]"), 1, INT2NUM(0)));
  values[1] = NUM2DBL(rb_funcall(mat4, rb_intern("[]"), 1, INT2NUM(1)));
  values[2] = NUM2DBL(rb_funcall(mat4, rb_intern("[]"), 1, INT2NUM(2)));
  values[3] = NUM2DBL(rb_funcall(mat4, rb_intern("[]"), 1, INT2NUM(4)));
  values[4] = NUM2DBL(rb_funcall(mat4, rb_intern("[]"), 1, INT2NUM(5)));
  values[5] = NUM2DBL(rb_funcall(mat4, rb_intern("[]"), 1, INT2NUM(6)));
  values[6] = NUM2DBL(rb_funcall(mat4, rb_intern("[]"), 1, INT2NUM(8)));
  values[7] = NUM2DBL(rb_funcall(mat4, rb_intern("[]"), 1, INT2NUM(9)));
  values[8] = NUM2DBL(rb_funcall(mat4, rb_intern("[]"), 1, INT2NUM(10)));
  return mat3_build(klass, values);
}

static VALUE mat3_class_from_mat2d(VALUE klass, VALUE mat2d) {
  double values[9];
  values[0] = NUM2DBL(rb_funcall(mat2d, rb_intern("[]"), 1, INT2NUM(0)));
  values[1] = NUM2DBL(rb_funcall(mat2d, rb_intern("[]"), 1, INT2NUM(1)));
  values[2] = 0.0;
  values[3] = NUM2DBL(rb_funcall(mat2d, rb_intern("[]"), 1, INT2NUM(2)));
  values[4] = NUM2DBL(rb_funcall(mat2d, rb_intern("[]"), 1, INT2NUM(3)));
  values[5] = 0.0;
  values[6] = NUM2DBL(rb_funcall(mat2d, rb_intern("[]"), 1, INT2NUM(4)));
  values[7] = NUM2DBL(rb_funcall(mat2d, rb_intern("[]"), 1, INT2NUM(5)));
  values[8] = 1.0;
  return mat3_build(klass, values);
}

static VALUE mat3_class_from_quaternion(VALUE klass, VALUE quat) {
  double x = NUM2DBL(rb_funcall(quat, rb_intern("x"), 0));
  double y = NUM2DBL(rb_funcall(quat, rb_intern("y"), 0));
  double z = NUM2DBL(rb_funcall(quat, rb_intern("z"), 0));
  double w = NUM2DBL(rb_funcall(quat, rb_intern("w"), 0));
  double x2 = x + x;
  double y2 = y + y;
  double z2 = z + z;
  double xx = x * x2;
  double yx = y * x2;
  double yy = y * y2;
  double zx = z * x2;
  double zy = z * y2;
  double zz = z * z2;
  double wx = w * x2;
  double wy = w * y2;
  double wz = w * z2;

  return mat3_build9(klass, 1 - yy - zz, yx + wz, zx - wy, yx - wz,
                     1 - xx - zz, zy + wx, zx + wy, zy - wx,
                     1 - xx - yy);
}

static VALUE mat3_class_normal_from_mat4(VALUE klass, VALUE mat4) {
  VALUE m3 = mat3_class_from_mat4(klass, mat4);
  VALUE inv = rb_funcall(m3, rb_intern("inverse"), 0);
  return rb_funcall(inv, rb_intern("transpose"), 0);
}

static VALUE mat3_class_projection(VALUE klass, VALUE width, VALUE height) {
  double w = NUM2DBL(width);
  double h = NUM2DBL(height);
  return mat3_build9(klass, 2.0 / w, 0.0, 0.0, 0.0, -2.0 / h, 0.0, -1.0, 1.0,
                     1.0);
}

static VALUE mat3_class_translation(VALUE klass, VALUE x, VALUE y) {
  return mat3_build9(klass, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, NUM2DBL(x),
                     NUM2DBL(y), 1.0);
}

static VALUE mat3_class_rotation(VALUE klass, VALUE radians) {
  double r = NUM2DBL(radians);
  double c = cos(r);
  double s = sin(r);
  return mat3_build9(klass, c, s, 0.0, -s, c, 0.0, 0.0, 0.0, 1.0);
}

static VALUE mat3_class_scaling(VALUE klass, VALUE x, VALUE y) {
  return mat3_build9(klass, NUM2DBL(x), 0.0, 0.0, 0.0,
                     NUM2DBL(y), 0.0, 0.0, 0.0, 1.0);
}

VALUE mat3_aref(VALUE self, VALUE index) {
  Mat3Data *data = mat3_get(self);
  long idx = NUM2LONG(index);
  if (idx < 0 || idx > 8) {
    return Qnil;
  }
  return DBL2NUM(data->data[idx]);
}

VALUE mat3_aset(VALUE self, VALUE index, VALUE value) {
  rb_check_frozen(self);
  Mat3Data *data = mat3_get(self);
  long idx = NUM2LONG(index);
  if (idx < 0 || idx > 8) {
    rb_raise(rb_eIndexError, "index %ld out of range", idx);
  }
  double component = NUM2DBL(value);
  rb_check_frozen(self);
  data->data[idx] = component;
  return value;
}

VALUE mat3_mul(VALUE self, VALUE other) {
  Mat3Data *a = mat3_get(self);

  if (rb_obj_is_kind_of(other, cMat3)) {
    Mat3Data *b = mat3_get(other);
    return mat3_build9(
        rb_obj_class(self),
        a->data[0] * b->data[0] + a->data[3] * b->data[1] +
            a->data[6] * b->data[2],
        a->data[1] * b->data[0] + a->data[4] * b->data[1] +
            a->data[7] * b->data[2],
        a->data[2] * b->data[0] + a->data[5] * b->data[1] +
            a->data[8] * b->data[2],
        a->data[0] * b->data[3] + a->data[3] * b->data[4] +
            a->data[6] * b->data[5],
        a->data[1] * b->data[3] + a->data[4] * b->data[4] +
            a->data[7] * b->data[5],
        a->data[2] * b->data[3] + a->data[5] * b->data[4] +
            a->data[8] * b->data[5],
        a->data[0] * b->data[6] + a->data[3] * b->data[7] +
            a->data[6] * b->data[8],
        a->data[1] * b->data[6] + a->data[4] * b->data[7] +
            a->data[7] * b->data[8],
        a->data[2] * b->data[6] + a->data[5] * b->data[7] +
            a->data[8] * b->data[8]);
  }

  if (rb_obj_is_kind_of(other, cVec3)) {
    double x = NUM2DBL(rb_funcall(other, rb_intern("x"), 0));
    double y = NUM2DBL(rb_funcall(other, rb_intern("y"), 0));
    double z = NUM2DBL(rb_funcall(other, rb_intern("z"), 0));
    VALUE vec3_class = rb_const_get(mLarb, rb_intern("Vec3"));
    return rb_funcall(vec3_class, rb_intern("new"), 3,
                      DBL2NUM(a->data[0] * x + a->data[3] * y +
                              a->data[6] * z),
                      DBL2NUM(a->data[1] * x + a->data[4] * y +
                              a->data[7] * z),
                      DBL2NUM(a->data[2] * x + a->data[5] * y +
                              a->data[8] * z));
  }

  if (rb_obj_is_kind_of(other, rb_cNumeric)) {
    double s = NUM2DBL(other);
    return mat3_build9(rb_obj_class(self), a->data[0] * s, a->data[1] * s,
                       a->data[2] * s, a->data[3] * s, a->data[4] * s,
                       a->data[5] * s, a->data[6] * s, a->data[7] * s,
                       a->data[8] * s);
  }

  rb_raise(rb_eTypeError, "unsupported operand for Mat3 multiplication");
}

VALUE mat3_add(VALUE self, VALUE other) {
  Mat3Data *a = mat3_get(self);
  Mat3Data *b = mat3_get(other);
  return mat3_build9(rb_obj_class(self), a->data[0] + b->data[0],
                     a->data[1] + b->data[1], a->data[2] + b->data[2],
                     a->data[3] + b->data[3], a->data[4] + b->data[4],
                     a->data[5] + b->data[5], a->data[6] + b->data[6],
                     a->data[7] + b->data[7], a->data[8] + b->data[8]);
}

VALUE mat3_sub(VALUE self, VALUE other) {
  Mat3Data *a = mat3_get(self);
  Mat3Data *b = mat3_get(other);
  return mat3_build9(rb_obj_class(self), a->data[0] - b->data[0],
                     a->data[1] - b->data[1], a->data[2] - b->data[2],
                     a->data[3] - b->data[3], a->data[4] - b->data[4],
                     a->data[5] - b->data[5], a->data[6] - b->data[6],
                     a->data[7] - b->data[7], a->data[8] - b->data[8]);
}

VALUE mat3_determinant(VALUE self) {
  Mat3Data *a = mat3_get(self);
  double det = a->data[0] * (a->data[4] * a->data[8] - a->data[5] * a->data[7]) -
               a->data[3] * (a->data[1] * a->data[8] - a->data[2] * a->data[7]) +
               a->data[6] * (a->data[1] * a->data[5] - a->data[2] * a->data[4]);
  return DBL2NUM(det);
}

VALUE mat3_inverse(VALUE self) {
  double inverse[9];
  larb_matrix_inverse(mat3_get(self)->data, inverse, 3);
  return mat3_build(rb_obj_class(self), inverse);
}

VALUE mat3_transpose(VALUE self) {
  Mat3Data *a = mat3_get(self);
  return mat3_build9(rb_obj_class(self), a->data[0], a->data[3], a->data[6],
                     a->data[1], a->data[4], a->data[7], a->data[2],
                     a->data[5], a->data[8]);
}

VALUE mat3_adjoint(VALUE self) {
  Mat3Data *a = mat3_get(self);
  return mat3_build9(
      rb_obj_class(self), a->data[4] * a->data[8] - a->data[5] * a->data[7],
      a->data[2] * a->data[7] - a->data[1] * a->data[8],
      a->data[1] * a->data[5] - a->data[2] * a->data[4],
      a->data[5] * a->data[6] - a->data[3] * a->data[8],
      a->data[0] * a->data[8] - a->data[2] * a->data[6],
      a->data[2] * a->data[3] - a->data[0] * a->data[5],
      a->data[3] * a->data[7] - a->data[4] * a->data[6],
      a->data[1] * a->data[6] - a->data[0] * a->data[7],
      a->data[0] * a->data[4] - a->data[1] * a->data[3]);
}

VALUE mat3_frobenius_norm(VALUE self) {
  Mat3Data *a = mat3_get(self);
  double norm = 0.0;
  for (int i = 0; i < 9; i++) {
    norm = hypot(norm, a->data[i]);
  }
  return DBL2NUM(norm);
}

VALUE mat3_translate(VALUE self, VALUE x, VALUE y) {
  Mat3Data *a = mat3_get(self);
  double dx = NUM2DBL(x);
  double dy = NUM2DBL(y);
  return mat3_build9(rb_obj_class(self), a->data[0], a->data[1], a->data[2],
                     a->data[3], a->data[4], a->data[5],
                     dx * a->data[0] + dy * a->data[3] + a->data[6],
                     dx * a->data[1] + dy * a->data[4] + a->data[7],
                     dx * a->data[2] + dy * a->data[5] + a->data[8]);
}

VALUE mat3_rotate(VALUE self, VALUE radians) {
  VALUE rot = mat3_class_rotation(cMat3, radians);
  return mat3_mul(self, rot);
}

VALUE mat3_scale(VALUE self, VALUE x, VALUE y) {
  Mat3Data *a = mat3_get(self);
  double sx = NUM2DBL(x);
  double sy = NUM2DBL(y);
  return mat3_build9(rb_obj_class(self), a->data[0] * sx, a->data[1] * sx,
                     a->data[2] * sx, a->data[3] * sy, a->data[4] * sy,
                     a->data[5] * sy, a->data[6], a->data[7], a->data[8]);
}

VALUE mat3_to_a(VALUE self) {
  Mat3Data *a = mat3_get(self);
  VALUE ary = rb_ary_new_capa(9);
  for (int i = 0; i < 9; i++) {
    rb_ary_push(ary, DBL2NUM(a->data[i]));
  }
  return ary;
}

VALUE mat3_data(VALUE self) {
  return mat3_to_a(self);
}

VALUE mat3_equal(VALUE self, VALUE other) {
  if (!rb_obj_is_kind_of(other, cMat3)) {
    return Qfalse;
  }
  Mat3Data *a = mat3_get(self);
  Mat3Data *b = mat3_get(other);
  for (int i = 0; i < 9; i++) {
    if (a->data[i] != b->data[i]) {
      return Qfalse;
    }
  }
  return Qtrue;
}

VALUE mat3_near(int argc, VALUE *argv, VALUE self) {
  VALUE other = Qnil;
  VALUE epsilon = Qnil;

  rb_scan_args(argc, argv, "11", &other, &epsilon);
  Mat3Data *a = mat3_get(self);
  Mat3Data *b = mat3_get(other);
  double eps = argc < 2 ? 1e-6 : NUM2DBL(epsilon);
  if (!isfinite(eps) || eps <= 0.0) {
    rb_raise(rb_eArgError, "epsilon must be finite and positive");
  }

  for (int i = 0; i < 9; i++) {
    if (!(fabs(a->data[i] - b->data[i]) < eps)) {
      return Qfalse;
    }
  }
  return Qtrue;
}

static VALUE mat3_format_value(double value) {
  return rb_funcall(rb_mKernel, rb_intern("format"), 2,
                    rb_str_new_cstr("%8.4f"), DBL2NUM(value));
}

static VALUE mat3_row_string(const double *values, int offset) {
  VALUE str = mat3_format_value(values[offset]);
  for (int i = 1; i < 3; i++) {
    rb_str_cat_cstr(str, ", ");
    rb_str_concat(str, mat3_format_value(values[offset + i]));
  }
  return str;
}

VALUE mat3_inspect(VALUE self) {
  Mat3Data *a = mat3_get(self);
  VALUE str = rb_str_new_cstr("Mat3[\n  ");
  rb_str_concat(str, mat3_row_string(a->data, 0));
  rb_str_cat_cstr(str, "\n  ");
  rb_str_concat(str, mat3_row_string(a->data, 3));
  rb_str_cat_cstr(str, "\n  ");
  rb_str_concat(str, mat3_row_string(a->data, 6));
  rb_str_cat_cstr(str, "\n]");
  return str;
}

void Init_mat3(VALUE module) {
  cMat3 = rb_define_class_under(module, "Mat3", rb_cObject);
  cVec3 = rb_const_get(mLarb, rb_intern("Vec3"));

  rb_define_alloc_func(cMat3, mat3_alloc);
  rb_define_method(cMat3, "initialize", mat3_initialize, -1);
  rb_define_method(cMat3, "initialize_copy", mat3_initialize_copy, 1);

  rb_define_singleton_method(cMat3, "identity", mat3_class_identity, 0);
  rb_define_singleton_method(cMat3, "zero", mat3_class_zero, 0);
  rb_define_singleton_method(cMat3, "from_mat4", mat3_class_from_mat4, 1);
  rb_define_singleton_method(cMat3, "from_mat2d", mat3_class_from_mat2d, 1);
  rb_define_singleton_method(cMat3, "from_quaternion",
                             mat3_class_from_quaternion, 1);
  rb_define_singleton_method(cMat3, "normal_from_mat4",
                             mat3_class_normal_from_mat4, 1);
  rb_define_singleton_method(cMat3, "projection", mat3_class_projection, 2);
  rb_define_singleton_method(cMat3, "translation", mat3_class_translation, 2);
  rb_define_singleton_method(cMat3, "rotation", mat3_class_rotation, 1);
  rb_define_singleton_method(cMat3, "scaling", mat3_class_scaling, 2);

  rb_define_method(cMat3, "data", mat3_data, 0);
  rb_define_method(cMat3, "[]", mat3_aref, 1);
  rb_define_method(cMat3, "[]=", mat3_aset, 2);
  rb_define_method(cMat3, "*", mat3_mul, 1);
  rb_define_method(cMat3, "+", mat3_add, 1);
  rb_define_method(cMat3, "-", mat3_sub, 1);
  rb_define_method(cMat3, "determinant", mat3_determinant, 0);
  rb_define_method(cMat3, "inverse", mat3_inverse, 0);
  rb_define_method(cMat3, "transpose", mat3_transpose, 0);
  rb_define_method(cMat3, "adjoint", mat3_adjoint, 0);
  rb_define_method(cMat3, "frobenius_norm", mat3_frobenius_norm, 0);
  rb_define_method(cMat3, "translate", mat3_translate, 2);
  rb_define_method(cMat3, "rotate", mat3_rotate, 1);
  rb_define_method(cMat3, "scale", mat3_scale, 2);
  rb_define_method(cMat3, "to_a", mat3_to_a, 0);
  rb_define_method(cMat3, "==", mat3_equal, 1);
  rb_define_method(cMat3, "near?", mat3_near, -1);
  rb_define_method(cMat3, "inspect", mat3_inspect, 0);
  rb_define_alias(cMat3, "to_s", "inspect");
}
