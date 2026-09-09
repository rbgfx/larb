#include "vec3.h"

#include <math.h>

static void vec3_free(void *ptr) {
  xfree(ptr);
}

static size_t vec3_memsize(const void *ptr) {
  return sizeof(Vec3Data);
}

static const rb_data_type_t vec3_type = {
    "Vec3",
    {0, vec3_free, vec3_memsize},
    0,
    0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};

static VALUE cVec3 = Qnil;

static Vec3Data *vec3_get(VALUE obj) {
  Vec3Data *data = NULL;
  TypedData_Get_Struct(obj, Vec3Data, &vec3_type, data);
  return data;
}

static VALUE vec3_build(VALUE klass, double x, double y, double z) {
  VALUE obj = vec3_alloc(klass);
  Vec3Data *data = vec3_get(obj);
  data->x = x;
  data->y = y;
  data->z = z;
  return obj;
}

static double clamp_double(double value, double min, double max) {
  if (value < min) {
    return min;
  }
  if (value > max) {
    return max;
  }
  return value;
}

VALUE vec3_alloc(VALUE klass) {
  Vec3Data *data = ALLOC(Vec3Data);
  data->x = 0.0;
  data->y = 0.0;
  data->z = 0.0;
  return TypedData_Wrap_Struct(klass, &vec3_type, data);
}

VALUE vec3_initialize(int argc, VALUE *argv, VALUE self) {
  rb_check_frozen(self);
  VALUE vx = Qnil;
  VALUE vy = Qnil;
  VALUE vz = Qnil;
  rb_scan_args(argc, argv, "03", &vx, &vy, &vz);
  Vec3Data value = {
      argc > 0 ? NUM2DBL(vx) : 0.0,
      argc > 1 ? NUM2DBL(vy) : 0.0,
      argc > 2 ? NUM2DBL(vz) : 0.0
  };
  rb_check_frozen(self);
  *vec3_get(self) = value;
  return self;
}

static VALUE vec3_class_bracket(VALUE klass, VALUE x, VALUE y, VALUE z) {
  return vec3_build(klass, NUM2DBL(x), NUM2DBL(y),
                    NUM2DBL(z));
}

static VALUE vec3_class_zero(VALUE klass) {
  return vec3_build(klass, 0.0, 0.0, 0.0);
}

static VALUE vec3_class_one(VALUE klass) {
  return vec3_build(klass, 1.0, 1.0, 1.0);
}

static VALUE vec3_class_up(VALUE klass) {
  return vec3_build(klass, 0.0, 1.0, 0.0);
}

static VALUE vec3_class_down(VALUE klass) {
  return vec3_build(klass, 0.0, -1.0, 0.0);
}

static VALUE vec3_class_forward(VALUE klass) {
  return vec3_build(klass, 0.0, 0.0, -1.0);
}

static VALUE vec3_class_back(VALUE klass) {
  return vec3_build(klass, 0.0, 0.0, 1.0);
}

static VALUE vec3_class_right(VALUE klass) {
  return vec3_build(klass, 1.0, 0.0, 0.0);
}

static VALUE vec3_class_left(VALUE klass) {
  return vec3_build(klass, -1.0, 0.0, 0.0);
}

static VALUE vec3_get_x(VALUE self) {
  Vec3Data *data = vec3_get(self);
  return DBL2NUM(data->x);
}

static VALUE vec3_set_x(VALUE self, VALUE value) {
  rb_check_frozen(self);
  Vec3Data *data = vec3_get(self);
  double component = NUM2DBL(value);
  rb_check_frozen(self);
  data->x = component;
  return value;
}

static VALUE vec3_get_y(VALUE self) {
  Vec3Data *data = vec3_get(self);
  return DBL2NUM(data->y);
}

static VALUE vec3_set_y(VALUE self, VALUE value) {
  rb_check_frozen(self);
  Vec3Data *data = vec3_get(self);
  double component = NUM2DBL(value);
  rb_check_frozen(self);
  data->y = component;
  return value;
}

static VALUE vec3_get_z(VALUE self) {
  Vec3Data *data = vec3_get(self);
  return DBL2NUM(data->z);
}

static VALUE vec3_set_z(VALUE self, VALUE value) {
  rb_check_frozen(self);
  Vec3Data *data = vec3_get(self);
  double component = NUM2DBL(value);
  rb_check_frozen(self);
  data->z = component;
  return value;
}

VALUE vec3_add(VALUE self, VALUE other) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  return vec3_build(rb_obj_class(self), a->x + b->x, a->y + b->y,
                    a->z + b->z);
}

VALUE vec3_sub(VALUE self, VALUE other) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  return vec3_build(rb_obj_class(self), a->x - b->x, a->y - b->y,
                    a->z - b->z);
}

VALUE vec3_mul(VALUE self, VALUE scalar) {
  Vec3Data *a = vec3_get(self);

  if (rb_obj_is_kind_of(scalar, cVec3)) {
    Vec3Data *b = vec3_get(scalar);
    return vec3_build(rb_obj_class(self), a->x * b->x, a->y * b->y,
                      a->z * b->z);
  }

  double s = NUM2DBL(scalar);
  return vec3_build(rb_obj_class(self), a->x * s, a->y * s, a->z * s);
}

VALUE vec3_div(VALUE self, VALUE scalar) {
  Vec3Data *a = vec3_get(self);
  double s = NUM2DBL(scalar);
  return vec3_build(rb_obj_class(self), a->x / s, a->y / s, a->z / s);
}

VALUE vec3_negate(VALUE self) {
  Vec3Data *a = vec3_get(self);
  return vec3_build(rb_obj_class(self), -a->x, -a->y, -a->z);
}

VALUE vec3_dot(VALUE self, VALUE other) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  return DBL2NUM(a->x * b->x + a->y * b->y + a->z * b->z);
}

VALUE vec3_cross(VALUE self, VALUE other) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  return vec3_build(rb_obj_class(self), a->y * b->z - a->z * b->y,
                    a->z * b->x - a->x * b->z,
                    a->x * b->y - a->y * b->x);
}

VALUE vec3_length(VALUE self) {
  Vec3Data *a = vec3_get(self);
  return DBL2NUM(hypot(hypot(a->x, a->y), a->z));
}

VALUE vec3_length_squared(VALUE self) {
  Vec3Data *a = vec3_get(self);
  return DBL2NUM(a->x * a->x + a->y * a->y + a->z * a->z);
}

VALUE vec3_normalize(VALUE self) {
  Vec3Data *a = vec3_get(self);
  double values[] = {a->x, a->y, a->z};
  larb_normalize(values, 3);
  return vec3_build(rb_obj_class(self), values[0], values[1], values[2]);
}

VALUE vec3_reflect(VALUE self, VALUE normal) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *n = vec3_get(normal);
  double dot = a->x * n->x + a->y * n->y + a->z * n->z;
  return vec3_build(rb_obj_class(self), a->x - n->x * (2.0 * dot),
                    a->y - n->y * (2.0 * dot), a->z - n->z * (2.0 * dot));
}

VALUE vec3_xy(VALUE self) {
  Vec3Data *a = vec3_get(self);
  VALUE vec2_class = rb_const_get(mLarb, rb_intern("Vec2"));
  return rb_funcall(vec2_class, rb_intern("new"), 2, DBL2NUM(a->x),
                    DBL2NUM(a->y));
}

VALUE vec3_xz(VALUE self) {
  Vec3Data *a = vec3_get(self);
  VALUE vec2_class = rb_const_get(mLarb, rb_intern("Vec2"));
  return rb_funcall(vec2_class, rb_intern("new"), 2, DBL2NUM(a->x),
                    DBL2NUM(a->z));
}

VALUE vec3_yz(VALUE self) {
  Vec3Data *a = vec3_get(self);
  VALUE vec2_class = rb_const_get(mLarb, rb_intern("Vec2"));
  return rb_funcall(vec2_class, rb_intern("new"), 2, DBL2NUM(a->y),
                    DBL2NUM(a->z));
}

VALUE vec3_lerp(VALUE self, VALUE other, VALUE t) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  double s = NUM2DBL(t);
  return vec3_build(rb_obj_class(self), a->x + (b->x - a->x) * s,
                    a->y + (b->y - a->y) * s,
                    a->z + (b->z - a->z) * s);
}

VALUE vec3_to_a(VALUE self) {
  Vec3Data *a = vec3_get(self);
  VALUE ary = rb_ary_new_capa(3);
  rb_ary_push(ary, DBL2NUM(a->x));
  rb_ary_push(ary, DBL2NUM(a->y));
  rb_ary_push(ary, DBL2NUM(a->z));
  return ary;
}

VALUE vec3_to_vec4(int argc, VALUE *argv, VALUE self) {
  VALUE vw = Qnil;
  Vec3Data *a = vec3_get(self);

  rb_scan_args(argc, argv, "01", &vw);
  VALUE vec4_class = rb_const_get(mLarb, rb_intern("Vec4"));
  return rb_funcall(vec4_class, rb_intern("new"), 4, DBL2NUM(a->x),
                    DBL2NUM(a->y), DBL2NUM(a->z),
                    argc == 0 ? DBL2NUM(1.0) : vw);
}

VALUE vec3_aref(VALUE self, VALUE index) {
  VALUE ary = vec3_to_a(self);
  return rb_ary_entry(ary, NUM2LONG(index));
}

VALUE vec3_equal(VALUE self, VALUE other) {
  if (!rb_obj_is_kind_of(other, cVec3)) {
    return Qfalse;
  }
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  return (a->x == b->x && a->y == b->y && a->z == b->z) ? Qtrue : Qfalse;
}

VALUE vec3_near(int argc, VALUE *argv, VALUE self) {
  VALUE other = Qnil;
  VALUE epsilon = Qnil;

  rb_scan_args(argc, argv, "11", &other, &epsilon);
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  double eps = argc < 2 ? 1e-6 : NUM2DBL(epsilon);
  if (!isfinite(eps) || eps <= 0.0) {
    rb_raise(rb_eArgError, "epsilon must be finite and positive");
  }

  if (fabs(a->x - b->x) < eps && fabs(a->y - b->y) < eps &&
      fabs(a->z - b->z) < eps) {
    return Qtrue;
  }
  return Qfalse;
}

VALUE vec3_distance(VALUE self, VALUE other) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  double dx = a->x - b->x;
  double dy = a->y - b->y;
  double dz = a->z - b->z;
  return DBL2NUM(hypot(hypot(dx, dy), dz));
}

VALUE vec3_distance_squared(VALUE self, VALUE other) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  double dx = a->x - b->x;
  double dy = a->y - b->y;
  double dz = a->z - b->z;
  return DBL2NUM(dx * dx + dy * dy + dz * dz);
}

VALUE vec3_angle_between(VALUE self, VALUE other) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  double av[] = {a->x, a->y, a->z};
  double bv[] = {b->x, b->y, b->z};
  larb_normalize(av, 3);
  larb_normalize(bv, 3);
  double d = av[0] * bv[0] + av[1] * bv[1] + av[2] * bv[2];
  return DBL2NUM(acos(clamp_double(d, -1.0, 1.0)));
}

VALUE vec3_project(VALUE self, VALUE onto) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(onto);
  double direction[] = {b->x, b->y, b->z};
  larb_normalize(direction, 3);
  double scale = a->x * direction[0] + a->y * direction[1] + a->z * direction[2];
  return vec3_build(rb_obj_class(self), direction[0] * scale,
                    direction[1] * scale, direction[2] * scale);
}

VALUE vec3_reject(VALUE self, VALUE from) {
  return vec3_sub(self, vec3_project(self, from));
}

VALUE vec3_slerp(VALUE self, VALUE other, VALUE t) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  double s = NUM2DBL(t);
  double len = hypot(hypot(a->x, a->y), a->z);
  double other_len = hypot(hypot(b->x, b->y), b->z);
  if (!isfinite(s) || !isfinite(len) || !isfinite(other_len)) {
    rb_raise(rb_eArgError, "slerp requires finite lengths and interpolation parameter");
  }
  double av[] = {a->x, a->y, a->z};
  double bv[] = {b->x, b->y, b->z};
  larb_normalize(av, 3);
  larb_normalize(bv, 3);
  if (s == 0.0) return vec3_build(rb_obj_class(self), a->x, a->y, a->z);
  if (s == 1.0) return vec3_build(rb_obj_class(self), b->x, b->y, b->z);

  double dot = clamp_double(av[0] * bv[0] + av[1] * bv[1] + av[2] * bv[2], -1.0, 1.0);
  double direction[3];
  if (dot > 1.0 - 1e-12) {
    for (int i = 0; i < 3; i++) direction[i] = av[i] + (bv[i] - av[i]) * s;
  } else {
    double normal[] = {av[1] * bv[2] - av[2] * bv[1],
                       av[2] * bv[0] - av[0] * bv[2],
                       av[0] * bv[1] - av[1] * bv[0]};
    double tangent[] = {normal[1] * av[2] - normal[2] * av[1],
                        normal[2] * av[0] - normal[0] * av[2],
                        normal[0] * av[1] - normal[1] * av[0]};
    double sine = hypot(hypot(normal[0], normal[1]), normal[2]);
    if (sine < 1e-12) {
      // Antipodes have no unique arc; choose the least-aligned coordinate axis.
      int axis = 0;
      for (int i = 1; i < 3; i++) if (fabs(av[i]) < fabs(av[axis])) axis = i;
      for (int i = 0; i < 3; i++) tangent[i] = (i == axis ? 1.0 : 0.0) - av[i] * av[axis];
    }
    larb_normalize(tangent, 3);
    double theta = atan2(sine, dot) * s;
    for (int i = 0; i < 3; i++) direction[i] = av[i] * cos(theta) + tangent[i] * sin(theta);
  }
  larb_normalize(direction, 3);
  double length = (1.0 - s) * len + s * other_len;
  return vec3_build(rb_obj_class(self), direction[0] * length,
                    direction[1] * length, direction[2] * length);
}

VALUE vec3_clamp_length(VALUE self, VALUE max_length) {
  Vec3Data *a = vec3_get(self);
  double max_len = NUM2DBL(max_length);
  if (!isfinite(max_len) || max_len < 0.0) {
    rb_raise(rb_eArgError, "max_length must be finite and nonnegative");
  }
  if (hypot(hypot(a->x, a->y), a->z) <= max_len) return self;
  double values[] = {a->x, a->y, a->z};
  larb_normalize(values, 3);
  return vec3_build(rb_obj_class(self), values[0] * max_len, values[1] * max_len, values[2] * max_len);
}

VALUE vec3_normalize_bang(VALUE self) {
  rb_check_frozen(self);
  Vec3Data *a = vec3_get(self);
  double values[] = {a->x, a->y, a->z};
  larb_normalize(values, 3);
  a->x = values[0];
  a->y = values[1];
  a->z = values[2];
  return self;
}

VALUE vec3_min(VALUE self, VALUE other) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  return vec3_build(rb_obj_class(self), fmin(a->x, b->x), fmin(a->y, b->y),
                    fmin(a->z, b->z));
}

VALUE vec3_max(VALUE self, VALUE other) {
  Vec3Data *a = vec3_get(self);
  Vec3Data *b = vec3_get(other);
  return vec3_build(rb_obj_class(self), fmax(a->x, b->x), fmax(a->y, b->y),
                    fmax(a->z, b->z));
}

VALUE vec3_abs(VALUE self) {
  Vec3Data *a = vec3_get(self);
  return vec3_build(rb_obj_class(self), fabs(a->x), fabs(a->y), fabs(a->z));
}

VALUE vec3_floor(VALUE self) {
  Vec3Data *a = vec3_get(self);
  return vec3_build(rb_obj_class(self), floor(a->x), floor(a->y), floor(a->z));
}

VALUE vec3_ceil(VALUE self) {
  Vec3Data *a = vec3_get(self);
  return vec3_build(rb_obj_class(self), ceil(a->x), ceil(a->y), ceil(a->z));
}

VALUE vec3_round(VALUE self) {
  Vec3Data *a = vec3_get(self);
  return vec3_build(rb_obj_class(self), round(a->x), round(a->y), round(a->z));
}

VALUE vec3_inspect(VALUE self) {
  Vec3Data *a = vec3_get(self);
  VALUE sx = rb_funcall(DBL2NUM(a->x), rb_intern("to_s"), 0);
  VALUE sy = rb_funcall(DBL2NUM(a->y), rb_intern("to_s"), 0);
  VALUE sz = rb_funcall(DBL2NUM(a->z), rb_intern("to_s"), 0);
  VALUE str = rb_str_new_cstr("Vec3[");
  rb_str_concat(str, sx);
  rb_str_cat_cstr(str, ", ");
  rb_str_concat(str, sy);
  rb_str_cat_cstr(str, ", ");
  rb_str_concat(str, sz);
  rb_str_cat_cstr(str, "]");
  return str;
}

static VALUE vec3_initialize_copy(VALUE self, VALUE other) {
  if (self == other) return self;
  rb_obj_init_copy(self, other);
  *vec3_get(self) = *vec3_get(other);
  return self;
}

void Init_vec3(VALUE module) {
  cVec3 = rb_define_class_under(module, "Vec3", rb_cObject);

  rb_define_alloc_func(cVec3, vec3_alloc);
  rb_define_method(cVec3, "initialize", vec3_initialize, -1);
  rb_define_method(cVec3, "initialize_copy", vec3_initialize_copy, 1);

  rb_define_singleton_method(cVec3, "[]", vec3_class_bracket, 3);
  rb_define_singleton_method(cVec3, "zero", vec3_class_zero, 0);
  rb_define_singleton_method(cVec3, "one", vec3_class_one, 0);
  rb_define_singleton_method(cVec3, "up", vec3_class_up, 0);
  rb_define_singleton_method(cVec3, "down", vec3_class_down, 0);
  rb_define_singleton_method(cVec3, "forward", vec3_class_forward, 0);
  rb_define_singleton_method(cVec3, "back", vec3_class_back, 0);
  rb_define_singleton_method(cVec3, "right", vec3_class_right, 0);
  rb_define_singleton_method(cVec3, "left", vec3_class_left, 0);

  rb_define_method(cVec3, "x", vec3_get_x, 0);
  rb_define_method(cVec3, "x=", vec3_set_x, 1);
  rb_define_method(cVec3, "y", vec3_get_y, 0);
  rb_define_method(cVec3, "y=", vec3_set_y, 1);
  rb_define_method(cVec3, "z", vec3_get_z, 0);
  rb_define_method(cVec3, "z=", vec3_set_z, 1);

  rb_define_method(cVec3, "+", vec3_add, 1);
  rb_define_method(cVec3, "-", vec3_sub, 1);
  rb_define_method(cVec3, "*", vec3_mul, 1);
  rb_define_method(cVec3, "/", vec3_div, 1);
  rb_define_method(cVec3, "-@", vec3_negate, 0);

  rb_define_method(cVec3, "dot", vec3_dot, 1);
  rb_define_method(cVec3, "cross", vec3_cross, 1);
  rb_define_method(cVec3, "length", vec3_length, 0);
  rb_define_method(cVec3, "length_squared", vec3_length_squared, 0);
  rb_define_method(cVec3, "normalize", vec3_normalize, 0);
  rb_define_method(cVec3, "reflect", vec3_reflect, 1);
  rb_define_method(cVec3, "xy", vec3_xy, 0);
  rb_define_method(cVec3, "xz", vec3_xz, 0);
  rb_define_method(cVec3, "yz", vec3_yz, 0);
  rb_define_method(cVec3, "lerp", vec3_lerp, 2);
  rb_define_method(cVec3, "to_a", vec3_to_a, 0);
  rb_define_method(cVec3, "to_vec4", vec3_to_vec4, -1);
  rb_define_method(cVec3, "[]", vec3_aref, 1);
  rb_define_method(cVec3, "==", vec3_equal, 1);
  rb_define_method(cVec3, "near?", vec3_near, -1);
  rb_define_method(cVec3, "distance", vec3_distance, 1);
  rb_define_method(cVec3, "distance_squared", vec3_distance_squared, 1);
  rb_define_method(cVec3, "angle_between", vec3_angle_between, 1);
  rb_define_method(cVec3, "project", vec3_project, 1);
  rb_define_method(cVec3, "reject", vec3_reject, 1);
  rb_define_method(cVec3, "slerp", vec3_slerp, 2);
  rb_define_method(cVec3, "clamp_length", vec3_clamp_length, 1);
  rb_define_method(cVec3, "normalize!", vec3_normalize_bang, 0);
  rb_define_method(cVec3, "min", vec3_min, 1);
  rb_define_method(cVec3, "max", vec3_max, 1);
  rb_define_method(cVec3, "abs", vec3_abs, 0);
  rb_define_method(cVec3, "floor", vec3_floor, 0);
  rb_define_method(cVec3, "ceil", vec3_ceil, 0);
  rb_define_method(cVec3, "round", vec3_round, 0);
  rb_define_method(cVec3, "inspect", vec3_inspect, 0);
  rb_define_alias(cVec3, "to_s", "inspect");
}
