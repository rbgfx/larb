#include "color.h"

#include <math.h>

static void color_free(void *ptr) {
  xfree(ptr);
}

static size_t color_memsize(const void *ptr) {
  return sizeof(ColorData);
}

static const rb_data_type_t color_type = {
    "Color",
    {0, color_free, color_memsize},
    0,
    0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};

static VALUE cColor = Qnil;
static VALUE cVec3 = Qnil;
static VALUE cVec4 = Qnil;

static ColorData *color_get(VALUE obj) {
  ColorData *data = NULL;
  TypedData_Get_Struct(obj, ColorData, &color_type, data);
  return data;
}

static VALUE color_build(VALUE klass, double r, double g, double b, double a) {
  VALUE obj = color_alloc(klass);
  ColorData *data = color_get(obj);
  data->r = r;
  data->g = g;
  data->b = b;
  data->a = a;
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

static int color_byte(double value) {
  if (isnan(value)) rb_raise(rb_eArgError, "Cannot convert NaN to a color byte");
  return (int)lround(clamp_double(value, 0.0, 1.0) * 255.0);
}

static VALUE color_get_r(VALUE self) {
  ColorData *data = color_get(self);
  return DBL2NUM(data->r);
}

static VALUE color_set_r(VALUE self, VALUE value) {
  rb_check_frozen(self);
  ColorData *data = color_get(self);
  double component = NUM2DBL(value);
  rb_check_frozen(self);
  data->r = component;
  return value;
}

static VALUE color_get_g(VALUE self) {
  ColorData *data = color_get(self);
  return DBL2NUM(data->g);
}

static VALUE color_set_g(VALUE self, VALUE value) {
  rb_check_frozen(self);
  ColorData *data = color_get(self);
  double component = NUM2DBL(value);
  rb_check_frozen(self);
  data->g = component;
  return value;
}

static VALUE color_get_b(VALUE self) {
  ColorData *data = color_get(self);
  return DBL2NUM(data->b);
}

static VALUE color_set_b(VALUE self, VALUE value) {
  rb_check_frozen(self);
  ColorData *data = color_get(self);
  double component = NUM2DBL(value);
  rb_check_frozen(self);
  data->b = component;
  return value;
}

static VALUE color_get_a(VALUE self) {
  ColorData *data = color_get(self);
  return DBL2NUM(data->a);
}

static VALUE color_set_a(VALUE self, VALUE value) {
  rb_check_frozen(self);
  ColorData *data = color_get(self);
  double component = NUM2DBL(value);
  rb_check_frozen(self);
  data->a = component;
  return value;
}

VALUE color_alloc(VALUE klass) {
  ColorData *data = ALLOC(ColorData);
  data->r = 0.0;
  data->g = 0.0;
  data->b = 0.0;
  data->a = 1.0;
  return TypedData_Wrap_Struct(klass, &color_type, data);
}

VALUE color_initialize(int argc, VALUE *argv, VALUE self) {
  rb_check_frozen(self);
  VALUE vr = Qnil;
  VALUE vg = Qnil;
  VALUE vb = Qnil;
  VALUE va = Qnil;
  rb_scan_args(argc, argv, "04", &vr, &vg, &vb, &va);
  ColorData value = {
      argc > 0 ? NUM2DBL(vr) : 0.0,
      argc > 1 ? NUM2DBL(vg) : 0.0,
      argc > 2 ? NUM2DBL(vb) : 0.0,
      argc > 3 ? NUM2DBL(va) : 1.0
  };
  rb_check_frozen(self);
  *color_get(self) = value;
  return self;
}

static VALUE color_class_bracket(int argc, VALUE *argv, VALUE klass) {
  VALUE vr = Qnil;
  VALUE vg = Qnil;
  VALUE vb = Qnil;
  VALUE va = Qnil;

  rb_scan_args(argc, argv, "31", &vr, &vg, &vb, &va);
  return color_build(klass, NUM2DBL(vr), NUM2DBL(vg),
                     NUM2DBL(vb), argc < 4 ? 1.0 : NUM2DBL(va));
}

static VALUE color_class_rgb(VALUE klass, VALUE r, VALUE g, VALUE b) {
  return color_build(klass, NUM2DBL(r), NUM2DBL(g),
                     NUM2DBL(b), 1.0);
}

static VALUE color_class_rgba(VALUE klass, VALUE r, VALUE g, VALUE b, VALUE a) {
  return color_build(klass, NUM2DBL(r), NUM2DBL(g),
                     NUM2DBL(b), NUM2DBL(a));
}

static VALUE color_class_from_bytes(int argc, VALUE *argv, VALUE klass) {
  VALUE vr = Qnil;
  VALUE vg = Qnil;
  VALUE vb = Qnil;
  VALUE va = Qnil;

  rb_scan_args(argc, argv, "31", &vr, &vg, &vb, &va);
  double r = NUM2DBL(vr) / 255.0;
  double g = NUM2DBL(vg) / 255.0;
  double b = NUM2DBL(vb) / 255.0;
  double a = argc < 4 ? 1.0 : NUM2DBL(va) / 255.0;
  return color_build(klass, r, g, b, a);
}

static VALUE color_class_from_hex(VALUE klass, VALUE hex_value) {
  StringValue(hex_value);
  const char *hex = RSTRING_PTR(hex_value);
  long len = RSTRING_LEN(hex_value);
  if (len > 0 && hex[0] == '#') {
    hex++;
    len--;
  }
  if (len != 6 && len != 8) {
    rb_raise(rb_eArgError, "Expected 6 or 8 hexadecimal digits with an optional leading #");
  }
  unsigned int rgba = 0;
  for (long i = 0; i < len; i++) {
    unsigned int digit;
    if (hex[i] >= '0' && hex[i] <= '9') digit = hex[i] - '0';
    else if (hex[i] >= 'a' && hex[i] <= 'f') digit = hex[i] - 'a' + 10;
    else if (hex[i] >= 'A' && hex[i] <= 'F') digit = hex[i] - 'A' + 10;
    else rb_raise(rb_eArgError, "Invalid hexadecimal digit");
    rgba = (rgba << 4) | digit;
  }
  if (len == 6) rgba = (rgba << 8) | 255;
  return color_build(klass, ((rgba >> 24) & 255) / 255.0,
                     ((rgba >> 16) & 255) / 255.0, ((rgba >> 8) & 255) / 255.0,
                     (rgba & 255) / 255.0);
}

static VALUE color_class_black(VALUE klass) {
  return color_build(klass, 0.0, 0.0, 0.0, 1.0);
}

static VALUE color_class_white(VALUE klass) {
  return color_build(klass, 1.0, 1.0, 1.0, 1.0);
}

static VALUE color_class_red(VALUE klass) {
  return color_build(klass, 1.0, 0.0, 0.0, 1.0);
}

static VALUE color_class_green(VALUE klass) {
  return color_build(klass, 0.0, 1.0, 0.0, 1.0);
}

static VALUE color_class_blue(VALUE klass) {
  return color_build(klass, 0.0, 0.0, 1.0, 1.0);
}

static VALUE color_class_yellow(VALUE klass) {
  return color_build(klass, 1.0, 1.0, 0.0, 1.0);
}

static VALUE color_class_cyan(VALUE klass) {
  return color_build(klass, 0.0, 1.0, 1.0, 1.0);
}

static VALUE color_class_magenta(VALUE klass) {
  return color_build(klass, 1.0, 0.0, 1.0, 1.0);
}

static VALUE color_class_transparent(VALUE klass) {
  return color_build(klass, 0.0, 0.0, 0.0, 0.0);
}

VALUE color_class_from_vec4(VALUE klass, VALUE vec4) {
  double r = NUM2DBL(rb_funcall(vec4, rb_intern("x"), 0));
  double g = NUM2DBL(rb_funcall(vec4, rb_intern("y"), 0));
  double b = NUM2DBL(rb_funcall(vec4, rb_intern("z"), 0));
  double a = NUM2DBL(rb_funcall(vec4, rb_intern("w"), 0));
  return color_build(klass, r, g, b, a);
}

VALUE color_class_from_vec3(int argc, VALUE *argv, VALUE klass) {
  VALUE vec3 = Qnil;
  VALUE alpha = Qnil;
  rb_scan_args(argc, argv, "11", &vec3, &alpha);
  double r = NUM2DBL(rb_funcall(vec3, rb_intern("x"), 0));
  double g = NUM2DBL(rb_funcall(vec3, rb_intern("y"), 0));
  double b = NUM2DBL(rb_funcall(vec3, rb_intern("z"), 0));
  double a = argc < 2 ? 1.0 : NUM2DBL(alpha);
  return color_build(klass, r, g, b, a);
}

VALUE color_add(VALUE self, VALUE other) {
  ColorData *a = color_get(self);
  ColorData *b = color_get(other);
  return color_build(rb_obj_class(self), a->r + b->r, a->g + b->g, a->b + b->b,
                     a->a + b->a);
}

VALUE color_sub(VALUE self, VALUE other) {
  ColorData *a = color_get(self);
  ColorData *b = color_get(other);
  return color_build(rb_obj_class(self), a->r - b->r, a->g - b->g, a->b - b->b,
                     a->a - b->a);
}

VALUE color_mul(VALUE self, VALUE scalar) {
  ColorData *a = color_get(self);
  if (rb_obj_is_kind_of(scalar, cColor)) {
    ColorData *b = color_get(scalar);
    return color_build(rb_obj_class(self), a->r * b->r, a->g * b->g,
                       a->b * b->b, a->a * b->a);
  }
  if (rb_obj_is_kind_of(scalar, rb_cNumeric)) {
    double s = NUM2DBL(scalar);
    return color_build(rb_obj_class(self), a->r * s, a->g * s, a->b * s,
                       a->a * s);
  }
  rb_raise(rb_eTypeError, "Expected a Color or numeric scalar");
}

VALUE color_lerp(VALUE self, VALUE other, VALUE t) {
  ColorData *a = color_get(self);
  ColorData *b = color_get(other);
  double s = NUM2DBL(t);
  return color_build(rb_obj_class(self), a->r + (b->r - a->r) * s,
                     a->g + (b->g - a->g) * s, a->b + (b->b - a->b) * s,
                     a->a + (b->a - a->a) * s);
}

VALUE color_clamp(VALUE self) {
  ColorData *a = color_get(self);
  return color_build(rb_obj_class(self), clamp_double(a->r, 0.0, 1.0),
                     clamp_double(a->g, 0.0, 1.0),
                     clamp_double(a->b, 0.0, 1.0),
                     clamp_double(a->a, 0.0, 1.0));
}

VALUE color_to_bytes(VALUE self) {
  ColorData *a = color_get(self);
  return rb_ary_new_from_args(4, INT2NUM(color_byte(a->r)), INT2NUM(color_byte(a->g)),
                              INT2NUM(color_byte(a->b)), INT2NUM(color_byte(a->a)));
}

VALUE color_to_hex(VALUE self) {
  VALUE bytes = color_to_bytes(self);
  VALUE r = rb_ary_entry(bytes, 0);
  VALUE g = rb_ary_entry(bytes, 1);
  VALUE b = rb_ary_entry(bytes, 2);
  VALUE a = rb_ary_entry(bytes, 3);
  return rb_funcall(rb_mKernel, rb_intern("format"), 5,
                    rb_str_new_cstr("#%02x%02x%02x%02x"), r, g, b, a);
}

VALUE color_to_vec3(VALUE self) {
  ColorData *a = color_get(self);
  return rb_funcall(cVec3, rb_intern("new"), 3, DBL2NUM(a->r), DBL2NUM(a->g),
                    DBL2NUM(a->b));
}

VALUE color_to_vec4(VALUE self) {
  ColorData *a = color_get(self);
  return rb_funcall(cVec4, rb_intern("new"), 4, DBL2NUM(a->r), DBL2NUM(a->g),
                    DBL2NUM(a->b), DBL2NUM(a->a));
}

VALUE color_to_a(VALUE self) {
  ColorData *a = color_get(self);
  VALUE ary = rb_ary_new_capa(4);
  rb_ary_push(ary, DBL2NUM(a->r));
  rb_ary_push(ary, DBL2NUM(a->g));
  rb_ary_push(ary, DBL2NUM(a->b));
  rb_ary_push(ary, DBL2NUM(a->a));
  return ary;
}

VALUE color_aref(VALUE self, VALUE index) {
  VALUE ary = color_to_a(self);
  return rb_ary_entry(ary, NUM2LONG(index));
}

VALUE color_equal(VALUE self, VALUE other) {
  if (!rb_obj_is_kind_of(other, cColor)) {
    return Qfalse;
  }
  ColorData *a = color_get(self);
  ColorData *b = color_get(other);
  return (a->r == b->r && a->g == b->g && a->b == b->b && a->a == b->a)
             ? Qtrue
             : Qfalse;
}

VALUE color_near(int argc, VALUE *argv, VALUE self) {
  VALUE other = Qnil;
  VALUE epsilon = Qnil;

  rb_scan_args(argc, argv, "11", &other, &epsilon);
  ColorData *a = color_get(self);
  ColorData *b = color_get(other);
  double eps = argc < 2 ? 1e-6 : NUM2DBL(epsilon);
  if (!isfinite(eps) || eps <= 0.0) {
    rb_raise(rb_eArgError, "epsilon must be finite and positive");
  }

  if (fabs(a->r - b->r) < eps && fabs(a->g - b->g) < eps &&
      fabs(a->b - b->b) < eps && fabs(a->a - b->a) < eps) {
    return Qtrue;
  }
  return Qfalse;
}

VALUE color_inspect(VALUE self) {
  ColorData *a = color_get(self);
  VALUE sr = rb_funcall(DBL2NUM(a->r), rb_intern("to_s"), 0);
  VALUE sg = rb_funcall(DBL2NUM(a->g), rb_intern("to_s"), 0);
  VALUE sb = rb_funcall(DBL2NUM(a->b), rb_intern("to_s"), 0);
  VALUE sa = rb_funcall(DBL2NUM(a->a), rb_intern("to_s"), 0);
  VALUE str = rb_str_new_cstr("Color[");
  rb_str_concat(str, sr);
  rb_str_cat_cstr(str, ", ");
  rb_str_concat(str, sg);
  rb_str_cat_cstr(str, ", ");
  rb_str_concat(str, sb);
  rb_str_cat_cstr(str, ", ");
  rb_str_concat(str, sa);
  rb_str_cat_cstr(str, "]");
  return str;
}

static VALUE color_initialize_copy(VALUE self, VALUE other) {
  if (self == other) return self;
  rb_obj_init_copy(self, other);
  *color_get(self) = *color_get(other);
  return self;
}

void Init_color(VALUE module) {
  cColor = rb_define_class_under(module, "Color", rb_cObject);
  cVec3 = rb_const_get(mLarb, rb_intern("Vec3"));
  cVec4 = rb_const_get(mLarb, rb_intern("Vec4"));

  rb_define_alloc_func(cColor, color_alloc);
  rb_define_method(cColor, "initialize", color_initialize, -1);
  rb_define_method(cColor, "initialize_copy", color_initialize_copy, 1);

  rb_define_singleton_method(cColor, "[]", color_class_bracket, -1);
  rb_define_singleton_method(cColor, "rgb", color_class_rgb, 3);
  rb_define_singleton_method(cColor, "rgba", color_class_rgba, 4);
  rb_define_singleton_method(cColor, "from_bytes", color_class_from_bytes, -1);
  rb_define_singleton_method(cColor, "from_hex", color_class_from_hex, 1);
  rb_define_singleton_method(cColor, "black", color_class_black, 0);
  rb_define_singleton_method(cColor, "white", color_class_white, 0);
  rb_define_singleton_method(cColor, "red", color_class_red, 0);
  rb_define_singleton_method(cColor, "green", color_class_green, 0);
  rb_define_singleton_method(cColor, "blue", color_class_blue, 0);
  rb_define_singleton_method(cColor, "yellow", color_class_yellow, 0);
  rb_define_singleton_method(cColor, "cyan", color_class_cyan, 0);
  rb_define_singleton_method(cColor, "magenta", color_class_magenta, 0);
  rb_define_singleton_method(cColor, "transparent", color_class_transparent, 0);
  rb_define_singleton_method(cColor, "from_vec4", color_class_from_vec4, 1);
  rb_define_singleton_method(cColor, "from_vec3", color_class_from_vec3, -1);

  rb_define_method(cColor, "r", color_get_r, 0);
  rb_define_method(cColor, "r=", color_set_r, 1);
  rb_define_method(cColor, "g", color_get_g, 0);
  rb_define_method(cColor, "g=", color_set_g, 1);
  rb_define_method(cColor, "b", color_get_b, 0);
  rb_define_method(cColor, "b=", color_set_b, 1);
  rb_define_method(cColor, "a", color_get_a, 0);
  rb_define_method(cColor, "a=", color_set_a, 1);

  rb_define_method(cColor, "+", color_add, 1);
  rb_define_method(cColor, "-", color_sub, 1);
  rb_define_method(cColor, "*", color_mul, 1);
  rb_define_method(cColor, "lerp", color_lerp, 2);
  rb_define_method(cColor, "clamp", color_clamp, 0);
  rb_define_method(cColor, "to_bytes", color_to_bytes, 0);
  rb_define_method(cColor, "to_hex", color_to_hex, 0);
  rb_define_method(cColor, "to_vec3", color_to_vec3, 0);
  rb_define_method(cColor, "to_vec4", color_to_vec4, 0);
  rb_define_method(cColor, "to_a", color_to_a, 0);
  rb_define_method(cColor, "[]", color_aref, 1);
  rb_define_method(cColor, "==", color_equal, 1);
  rb_define_method(cColor, "near?", color_near, -1);
  rb_define_method(cColor, "inspect", color_inspect, 0);
  rb_define_alias(cColor, "to_s", "inspect");
}
