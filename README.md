# Larb [![Gem Version](https://badge.fury.io/rb/larb.svg)](https://badge.fury.io/rb/larb) [![CI](https://github.com/ydah/larb/actions/workflows/ci.yml/badge.svg)](https://github.com/ydah/larb/actions/workflows/ci.yml)

Linear algebra library for 2D/3D graphics in Ruby.

## Installation

Add this line to your application's Gemfile:

```ruby
gem 'larb'
```

And then execute:

```bash
bundle install
```

Or install it yourself as:

```bash
gem install larb
```

## Usage

```ruby
require 'larb'

# Vectors
v2 = Larb::Vec2.new(1, 2)
v3 = Larb::Vec3.new(1, 2, 3)
v4 = Larb::Vec4.new(1, 2, 3, 1)

# Vector operations
sum = v3 + Larb::Vec3.new(4, 5, 6)
dot = v3.dot(Larb::Vec3.new(1, 0, 0))
cross = v3.cross(Larb::Vec3.up)
normalized = v3.normalize

# Matrices
identity = Larb::Mat4.identity
translation = Larb::Mat4.translation(10, 20, 30)
rotation = Larb::Mat4.rotation_y(Math::PI / 4)
scale = Larb::Mat4.scaling(2, 2, 2)

# Matrix multiplication
transform = translation * rotation * scale
transformed_point = transform * v4

# Quaternions
quat = Larb::Quat.from_axis_angle(Larb::Vec3.up, Math::PI / 2)
rotated = quat * Larb::Vec3.new(1, 0, 0)

# Colors
red = Larb::Color.red
custom = Larb::Color.new(0.5, 0.3, 0.8, 1.0)
hex_color = Larb::Color.from_hex("#ff8800")
```

### Value and input behavior

All types support independent `dup`/`clone` copies and respect `freeze`, including
setters, indexed assignment, `normalize!`, and reinitialization. Failed numeric
conversion during reinitialization leaves the components unchanged.

Numeric arguments use Ruby's native numeric conversion. Strings and explicit
`nil` are rejected with `TypeError`; omitted optional arguments retain their
defaults. Array constructors require exactly 4 (`Mat2`), 6 (`Mat2d`), 9 (`Mat3`),
16 (`Mat4`), or 8 (`Quat2`) components. Unsupported multiplication operands raise
`TypeError`; all matrix types support numeric scalar multiplication.

`near?` uses an absolute tolerance (default `1e-6`) that must be finite and
positive. NaN components never compare near another value. Normalization uses
scaled components to handle very large and very small finite values; zero-length
or non-finite inputs raise `ArgumentError`. `length_squared` can still overflow
or underflow when the squared result is outside the range of a Float.

Direction operations (`Vec3#project`, `reject`, `angle_between`, and `slerp`)
reject a zero or non-finite direction with `ArgumentError`. `Vec3#slerp`
interpolates direction along an arc and length linearly; its endpoint lengths
and interpolation parameter must be finite. Opposite directions use a
deterministic perpendicular direction to select an arc. `clamp_length` requires
a finite, nonnegative limit. Degenerate `Mat4.look_at` and `Quat.look_rotation`
inputs also raise `ArgumentError`.

`Quat#lerp`, `Quat#slerp`, and `Quat2#lerp` choose the shorter rotation path,
including between `q` and `-q`. Dual quaternion normalization enforces both unit
real length and real/dual orthogonality. Quaternion rotation factories and
matrix/point conversion APIs expect unit rotations; normalize manually
constructed quaternions before using them as rotations.

`Mat4.trs` composes `translation * rotation * scale`. `Mat4` and `Mat2d`
scale/rotation extraction support finite affine transforms with nonzero scales
and no shear (normalized-column dot tolerance `1e-6`). Reflections are assigned
to the X scale, so the extracted values reconstruct the original transform;
the original individual scale signs are not uniquely recoverable. Unsupported
decompositions raise `ArgumentError`. Matrix inversion uses scaled pivoting;
singular, numerically singular, non-finite, or non-finitely representable inverse
results raise `RuntimeError`.

`Vec4#perspective_divide` requires finite, nonzero `w` and raises `ArgumentError`
otherwise. Use `xyz` to read a direction vector's three spatial components.

`Color.from_hex` accepts 6 or 8 hexadecimal digits, optionally prefixed with one
`#`; malformed strings raise `ArgumentError`. `to_bytes` and `to_hex` clamp
components to `[0, 1]` before rounding: infinities saturate to the corresponding
endpoint and NaN raises `ArgumentError`.

## Development

```bash
# Install dependencies
bundle install

# Compile native extension
bundle exec rake compile

# Run tests
bundle exec rake test
```

## License

The gem is available as open source under the terms of the [MIT License](https://opensource.org/licenses/MIT).
