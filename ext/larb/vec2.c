#include "vec2.h"

#include <math.h>

static void vec2_free(void *ptr) {
  xfree(ptr);
}

static size_t vec2_memsize(const void *ptr) {
  return sizeof(Vec2Data);
}

static const rb_data_type_t vec2_type = {
    "Vec2",
    {0, vec2_free, vec2_memsize},
    0,
    0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};

static VALUE cVec2 = Qnil;

static Vec2Data *vec2_get(VALUE obj) {
  Vec2Data *data = NULL;
  TypedData_Get_Struct(obj, Vec2Data, &vec2_type, data);
  return data;
}

static VALUE vec2_build(VALUE klass, double x, double y) {
  VALUE obj = vec2_alloc(klass);
  Vec2Data *data = vec2_get(obj);
  data->x = x;
  data->y = y;
  return obj;
}

VALUE vec2_alloc(VALUE klass) {
  Vec2Data *data = ALLOC(Vec2Data);
  data->x = 0.0;
  data->y = 0.0;
  return TypedData_Wrap_Struct(klass, &vec2_type, data);
}

VALUE vec2_initialize(int argc, VALUE *argv, VALUE self) {
  rb_check_frozen(self);
  VALUE vx = Qnil;
  VALUE vy = Qnil;
  rb_scan_args(argc, argv, "02", &vx, &vy);
  Vec2Data value = {
      argc > 0 ? NUM2DBL(vx) : 0.0,
      argc > 1 ? NUM2DBL(vy) : 0.0
  };
  rb_check_frozen(self);
  *vec2_get(self) = value;
  return self;
}

static VALUE vec2_class_bracket(VALUE klass, VALUE x, VALUE y) {
  return vec2_build(klass, NUM2DBL(x), NUM2DBL(y));
}

static VALUE vec2_class_zero(VALUE klass) {
  return vec2_build(klass, 0.0, 0.0);
}

static VALUE vec2_class_one(VALUE klass) {
  return vec2_build(klass, 1.0, 1.0);
}

static VALUE vec2_get_x(VALUE self) {
  Vec2Data *data = vec2_get(self);
  return DBL2NUM(data->x);
}

static VALUE vec2_set_x(VALUE self, VALUE value) {
  rb_check_frozen(self);
  Vec2Data *data = vec2_get(self);
  double component = NUM2DBL(value);
  rb_check_frozen(self);
  data->x = component;
  return value;
}

static VALUE vec2_get_y(VALUE self) {
  Vec2Data *data = vec2_get(self);
  return DBL2NUM(data->y);
}

static VALUE vec2_set_y(VALUE self, VALUE value) {
  rb_check_frozen(self);
  Vec2Data *data = vec2_get(self);
  double component = NUM2DBL(value);
  rb_check_frozen(self);
  data->y = component;
  return value;
}

VALUE vec2_add(VALUE self, VALUE other) {
  Vec2Data *a = vec2_get(self);
  Vec2Data *b = vec2_get(other);
  return vec2_build(rb_obj_class(self), a->x + b->x, a->y + b->y);
}

VALUE vec2_sub(VALUE self, VALUE other) {
  Vec2Data *a = vec2_get(self);
  Vec2Data *b = vec2_get(other);
  return vec2_build(rb_obj_class(self), a->x - b->x, a->y - b->y);
}

VALUE vec2_mul(VALUE self, VALUE scalar) {
  Vec2Data *a = vec2_get(self);
  double s = NUM2DBL(scalar);
  return vec2_build(rb_obj_class(self), a->x * s, a->y * s);
}

VALUE vec2_div(VALUE self, VALUE scalar) {
  Vec2Data *a = vec2_get(self);
  double s = NUM2DBL(scalar);
  return vec2_build(rb_obj_class(self), a->x / s, a->y / s);
}

VALUE vec2_negate(VALUE self) {
  Vec2Data *a = vec2_get(self);
  return vec2_build(rb_obj_class(self), -a->x, -a->y);
}

VALUE vec2_dot(VALUE self, VALUE other) {
  Vec2Data *a = vec2_get(self);
  Vec2Data *b = vec2_get(other);
  return DBL2NUM(a->x * b->x + a->y * b->y);
}

VALUE vec2_length(VALUE self) {
  Vec2Data *a = vec2_get(self);
  return DBL2NUM(hypot(a->x, a->y));
}

VALUE vec2_length_squared(VALUE self) {
  Vec2Data *a = vec2_get(self);
  return DBL2NUM(a->x * a->x + a->y * a->y);
}

VALUE vec2_normalize(VALUE self) {
  Vec2Data *a = vec2_get(self);
  double values[] = {a->x, a->y};
  larb_normalize(values, 2);
  return vec2_build(rb_obj_class(self), values[0], values[1]);
}

VALUE vec2_normalize_bang(VALUE self) {
  rb_check_frozen(self);
  Vec2Data *a = vec2_get(self);
  double values[] = {a->x, a->y};
  larb_normalize(values, 2);
  a->x = values[0];
  a->y = values[1];
  return self;
}

VALUE vec2_lerp(VALUE self, VALUE other, VALUE t) {
  Vec2Data *a = vec2_get(self);
  Vec2Data *b = vec2_get(other);
  double s = NUM2DBL(t);
  return vec2_build(rb_obj_class(self), a->x + (b->x - a->x) * s,
                    a->y + (b->y - a->y) * s);
}

VALUE vec2_to_a(VALUE self) {
  Vec2Data *a = vec2_get(self);
  VALUE ary = rb_ary_new_capa(2);
  rb_ary_push(ary, DBL2NUM(a->x));
  rb_ary_push(ary, DBL2NUM(a->y));
  return ary;
}

VALUE vec2_aref(VALUE self, VALUE index) {
  VALUE ary = vec2_to_a(self);
  return rb_ary_entry(ary, NUM2LONG(index));
}

VALUE vec2_aset(VALUE self, VALUE index, VALUE value) {
  rb_check_frozen(self);
  Vec2Data *a = vec2_get(self);
  long idx = NUM2LONG(index);
  if (idx < 0) idx += 2;
  if (idx < 0 || idx >= 2) rb_raise(rb_eIndexError, "Index out of range");
  double component = NUM2DBL(value);
  rb_check_frozen(self);
  if (idx == 0) {
    a->x = component;
  } else {
    a->y = component;
  }
  return value;
}

VALUE vec2_equal(VALUE self, VALUE other) {
  if (!rb_obj_is_kind_of(other, cVec2)) {
    return Qfalse;
  }
  Vec2Data *a = vec2_get(self);
  Vec2Data *b = vec2_get(other);
  return (a->x == b->x && a->y == b->y) ? Qtrue : Qfalse;
}

VALUE vec2_near(int argc, VALUE *argv, VALUE self) {
  VALUE other = Qnil;
  VALUE epsilon = Qnil;

  rb_scan_args(argc, argv, "11", &other, &epsilon);
  Vec2Data *a = vec2_get(self);
  Vec2Data *b = vec2_get(other);
  double eps = argc < 2 ? 1e-6 : NUM2DBL(epsilon);
  if (!isfinite(eps) || eps <= 0.0) {
    rb_raise(rb_eArgError, "epsilon must be finite and positive");
  }

  if (fabs(a->x - b->x) < eps && fabs(a->y - b->y) < eps) {
    return Qtrue;
  }
  return Qfalse;
}

VALUE vec2_angle(VALUE self) {
  Vec2Data *a = vec2_get(self);
  return DBL2NUM(atan2(a->y, a->x));
}

VALUE vec2_angle_to(VALUE self, VALUE other) {
  Vec2Data *a = vec2_get(self);
  Vec2Data *b = vec2_get(other);
  return DBL2NUM(atan2(b->y - a->y, b->x - a->x));
}

VALUE vec2_rotate(VALUE self, VALUE radians) {
  Vec2Data *a = vec2_get(self);
  double r = NUM2DBL(radians);
  double c = cos(r);
  double s = sin(r);
  return vec2_build(rb_obj_class(self), a->x * c - a->y * s,
                    a->x * s + a->y * c);
}

VALUE vec2_distance(VALUE self, VALUE other) {
  Vec2Data *a = vec2_get(self);
  Vec2Data *b = vec2_get(other);
  double dx = a->x - b->x;
  double dy = a->y - b->y;
  return DBL2NUM(hypot(dx, dy));
}

VALUE vec2_distance_squared(VALUE self, VALUE other) {
  Vec2Data *a = vec2_get(self);
  Vec2Data *b = vec2_get(other);
  double dx = a->x - b->x;
  double dy = a->y - b->y;
  return DBL2NUM(dx * dx + dy * dy);
}

VALUE vec2_cross(VALUE self, VALUE other) {
  Vec2Data *a = vec2_get(self);
  Vec2Data *b = vec2_get(other);
  return DBL2NUM(a->x * b->y - a->y * b->x);
}

VALUE vec2_perpendicular(VALUE self) {
  Vec2Data *a = vec2_get(self);
  return vec2_build(rb_obj_class(self), -a->y, a->x);
}

VALUE vec2_reflect(VALUE self, VALUE normal) {
  Vec2Data *a = vec2_get(self);
  Vec2Data *n = vec2_get(normal);
  double dot = a->x * n->x + a->y * n->y;
  return vec2_build(rb_obj_class(self), a->x - n->x * (2.0 * dot),
                    a->y - n->y * (2.0 * dot));
}

VALUE vec2_clamp_length(VALUE self, VALUE max_length) {
  Vec2Data *a = vec2_get(self);
  double max_len = NUM2DBL(max_length);
  if (!isfinite(max_len) || max_len < 0.0) {
    rb_raise(rb_eArgError, "max_length must be finite and nonnegative");
  }
  if (hypot(a->x, a->y) <= max_len) return self;
  double values[] = {a->x, a->y};
  larb_normalize(values, 2);
  return vec2_build(rb_obj_class(self), values[0] * max_len, values[1] * max_len);
}

VALUE vec2_to_vec3(int argc, VALUE *argv, VALUE self) {
  VALUE vz = Qnil;
  Vec2Data *a = vec2_get(self);

  rb_scan_args(argc, argv, "01", &vz);
  VALUE vec3_class = rb_const_get(mLarb, rb_intern("Vec3"));
  return rb_funcall(vec3_class, rb_intern("new"), 3, DBL2NUM(a->x),
                    DBL2NUM(a->y), argc == 0 ? DBL2NUM(0.0) : vz);
}

VALUE vec2_inspect(VALUE self) {
  Vec2Data *a = vec2_get(self);
  VALUE sx = rb_funcall(DBL2NUM(a->x), rb_intern("to_s"), 0);
  VALUE sy = rb_funcall(DBL2NUM(a->y), rb_intern("to_s"), 0);
  VALUE str = rb_str_new_cstr("Vec2[");
  rb_str_concat(str, sx);
  rb_str_cat_cstr(str, ", ");
  rb_str_concat(str, sy);
  rb_str_cat_cstr(str, "]");
  return str;
}

static VALUE vec2_initialize_copy(VALUE self, VALUE other) {
  if (self == other) return self;
  rb_obj_init_copy(self, other);
  *vec2_get(self) = *vec2_get(other);
  return self;
}

void Init_vec2(VALUE module) {
  cVec2 = rb_define_class_under(module, "Vec2", rb_cObject);

  rb_define_alloc_func(cVec2, vec2_alloc);
  rb_define_method(cVec2, "initialize", vec2_initialize, -1);
  rb_define_method(cVec2, "initialize_copy", vec2_initialize_copy, 1);

  rb_define_singleton_method(cVec2, "[]", vec2_class_bracket, 2);
  rb_define_singleton_method(cVec2, "zero", vec2_class_zero, 0);
  rb_define_singleton_method(cVec2, "one", vec2_class_one, 0);

  rb_define_method(cVec2, "x", vec2_get_x, 0);
  rb_define_method(cVec2, "x=", vec2_set_x, 1);
  rb_define_method(cVec2, "y", vec2_get_y, 0);
  rb_define_method(cVec2, "y=", vec2_set_y, 1);

  rb_define_method(cVec2, "+", vec2_add, 1);
  rb_define_method(cVec2, "-", vec2_sub, 1);
  rb_define_method(cVec2, "*", vec2_mul, 1);
  rb_define_method(cVec2, "/", vec2_div, 1);
  rb_define_method(cVec2, "-@", vec2_negate, 0);

  rb_define_method(cVec2, "dot", vec2_dot, 1);
  rb_define_method(cVec2, "length", vec2_length, 0);
  rb_define_method(cVec2, "length_squared", vec2_length_squared, 0);
  rb_define_method(cVec2, "normalize", vec2_normalize, 0);
  rb_define_method(cVec2, "normalize!", vec2_normalize_bang, 0);
  rb_define_method(cVec2, "lerp", vec2_lerp, 2);
  rb_define_method(cVec2, "to_a", vec2_to_a, 0);
  rb_define_method(cVec2, "[]", vec2_aref, 1);
  rb_define_method(cVec2, "[]=", vec2_aset, 2);
  rb_define_method(cVec2, "==", vec2_equal, 1);
  rb_define_method(cVec2, "near?", vec2_near, -1);
  rb_define_method(cVec2, "angle", vec2_angle, 0);
  rb_define_method(cVec2, "angle_to", vec2_angle_to, 1);
  rb_define_method(cVec2, "rotate", vec2_rotate, 1);
  rb_define_method(cVec2, "distance", vec2_distance, 1);
  rb_define_method(cVec2, "distance_squared", vec2_distance_squared, 1);
  rb_define_method(cVec2, "cross", vec2_cross, 1);
  rb_define_method(cVec2, "perpendicular", vec2_perpendicular, 0);
  rb_define_method(cVec2, "reflect", vec2_reflect, 1);
  rb_define_method(cVec2, "clamp_length", vec2_clamp_length, 1);
  rb_define_method(cVec2, "to_vec3", vec2_to_vec3, -1);
  rb_define_method(cVec2, "inspect", vec2_inspect, 0);
  rb_define_alias(cVec2, "to_s", "inspect");
}
