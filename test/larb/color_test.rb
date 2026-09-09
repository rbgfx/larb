# frozen_string_literal: true

require_relative "../test_helper"

class ColorTest < Test::Unit::TestCase
  def test_bytes_clamp_before_integer_conversion
    [1e8, 1e300, Float::INFINITY].each do |large|
      assert_equal [255, 0, 128, 255], Larb::Color.new(large, -large, 0.5, 1).to_bytes
      assert_equal "#ff0080ff", Larb::Color.new(large, -large, 0.5, 1).to_hex
    end
    4.times do |i|
      values = [0, 0, 0, 1]
      values[i] = Float::NAN
      assert_raise(ArgumentError) { Larb::Color.new(*values).to_bytes }
    end
  end

  def test_from_hex_validates_the_entire_string
    ["#gg0000", "#ff0000zz", "#ff0000ffjunk", "f#f0000", "", "#fff",
     "#ffff", " ff0000", "#ff0000\n", "ff0000\0", "＃ff0000"].each do |hex|
      assert_raise(ArgumentError, hex.inspect) { Larb::Color.from_hex(hex) }
    end
    assert_raise(TypeError) { Larb::Color.from_hex(nil) }
    assert_equal [255, 128, 0, 170], Larb::Color.from_hex("#FF8000aA").to_bytes
  end

  def test_new_with_default_values
    c = Larb::Color.new
    assert_equal 0.0, c.r
    assert_equal 0.0, c.g
    assert_equal 0.0, c.b
    assert_equal 1.0, c.a
  end

  def test_new_with_given_values
    c = Larb::Color.new(0.1, 0.2, 0.3, 0.4)
    assert_equal 0.1, c.r
    assert_equal 0.2, c.g
    assert_equal 0.3, c.b
    assert_equal 0.4, c.a
  end

  def test_bracket_syntax
    c = Larb::Color[0.1, 0.2, 0.3, 0.4]
    assert_equal 0.1, c.r
    assert_equal 0.2, c.g
    assert_equal 0.3, c.b
    assert_equal 0.4, c.a
  end

  def test_bracket_syntax_default_alpha
    c = Larb::Color[0.1, 0.2, 0.3]
    assert_equal 1.0, c.a
  end

  def test_rgb
    c = Larb::Color.rgb(0.5, 0.6, 0.7)
    assert_equal 0.5, c.r
    assert_equal 0.6, c.g
    assert_equal 0.7, c.b
    assert_equal 1.0, c.a
  end

  def test_rgba
    c = Larb::Color.rgba(0.1, 0.2, 0.3, 0.4)
    assert_equal 0.1, c.r
    assert_equal 0.2, c.g
    assert_equal 0.3, c.b
    assert_equal 0.4, c.a
  end

  def test_from_bytes
    c = Larb::Color.from_bytes(255, 128, 0, 255)
    assert_equal 1.0, c.r
    assert_in_delta 0.5, c.g, 0.01
    assert_equal 0.0, c.b
    assert_equal 1.0, c.a
  end

  def test_from_bytes_default_alpha
    c = Larb::Color.from_bytes(255, 0, 0)
    assert_equal 1.0, c.a
  end

  def test_from_hex
    c = Larb::Color.from_hex("#ff8000")
    assert_equal 1.0, c.r
    assert_in_delta 0.5, c.g, 0.01
    assert_equal 0.0, c.b
    assert_equal 1.0, c.a
  end

  def test_from_hex_without_hash
    c = Larb::Color.from_hex("ff0000")
    assert_equal 1.0, c.r
    assert_equal 0.0, c.g
    assert_equal 0.0, c.b
  end

  def test_from_hex_with_alpha
    c = Larb::Color.from_hex("#ff000080")
    assert_equal 1.0, c.r
    assert_in_delta 0.5, c.a, 0.01
  end

  def test_black
    c = Larb::Color.black
    assert_equal 0.0, c.r
    assert_equal 0.0, c.g
    assert_equal 0.0, c.b
    assert_equal 1.0, c.a
  end

  def test_white
    c = Larb::Color.white
    assert_equal 1.0, c.r
    assert_equal 1.0, c.g
    assert_equal 1.0, c.b
  end

  def test_red
    c = Larb::Color.red
    assert_equal 1.0, c.r
    assert_equal 0.0, c.g
    assert_equal 0.0, c.b
  end

  def test_green
    c = Larb::Color.green
    assert_equal 0.0, c.r
    assert_equal 1.0, c.g
    assert_equal 0.0, c.b
  end

  def test_blue
    c = Larb::Color.blue
    assert_equal 0.0, c.r
    assert_equal 0.0, c.g
    assert_equal 1.0, c.b
  end

  def test_yellow
    c = Larb::Color.yellow
    assert_equal 1.0, c.r
    assert_equal 1.0, c.g
    assert_equal 0.0, c.b
  end

  def test_cyan
    c = Larb::Color.cyan
    assert_equal 0.0, c.r
    assert_equal 1.0, c.g
    assert_equal 1.0, c.b
  end

  def test_magenta
    c = Larb::Color.magenta
    assert_equal 1.0, c.r
    assert_equal 0.0, c.g
    assert_equal 1.0, c.b
  end

  def test_transparent
    c = Larb::Color.transparent
    assert_equal 0.0, c.r
    assert_equal 0.0, c.g
    assert_equal 0.0, c.b
    assert_equal 0.0, c.a
  end

  def test_add
    c1 = Larb::Color.new(0.1, 0.2, 0.3, 0.4)
    c2 = Larb::Color.new(0.2, 0.3, 0.4, 0.5)
    result = c1 + c2
    assert_in_delta 0.3, result.r, 1e-10
    assert_in_delta 0.5, result.g, 1e-10
    assert_in_delta 0.7, result.b, 1e-10
    assert_in_delta 0.9, result.a, 1e-10
  end

  def test_subtract
    c1 = Larb::Color.new(0.5, 0.6, 0.7, 0.8)
    c2 = Larb::Color.new(0.1, 0.2, 0.3, 0.4)
    result = c1 - c2
    assert_in_delta 0.4, result.r, 1e-10
    assert_in_delta 0.4, result.g, 1e-10
    assert_in_delta 0.4, result.b, 1e-10
    assert_in_delta 0.4, result.a, 1e-10
  end

  def test_multiply_by_scalar
    c = Larb::Color.new(0.2, 0.3, 0.4, 0.5)
    result = c * 2
    assert_in_delta 0.4, result.r, 1e-10
    assert_in_delta 0.6, result.g, 1e-10
    assert_in_delta 0.8, result.b, 1e-10
    assert_in_delta 1.0, result.a, 1e-10
  end

  def test_multiply_by_color
    c1 = Larb::Color.new(0.5, 0.5, 0.5, 1.0)
    c2 = Larb::Color.new(1.0, 0.5, 0.0, 1.0)
    result = c1 * c2
    assert_in_delta 0.5, result.r, 1e-10
    assert_in_delta 0.25, result.g, 1e-10
    assert_equal 0.0, result.b
  end

  def test_lerp
    c1 = Larb::Color.new(0, 0, 0, 0)
    c2 = Larb::Color.new(1, 1, 1, 1)
    result = c1.lerp(c2, 0.5)
    assert_equal 0.5, result.r
    assert_equal 0.5, result.g
    assert_equal 0.5, result.b
    assert_equal 0.5, result.a
  end

  def test_clamp
    c = Larb::Color.new(-0.5, 1.5, 0.5, 2.0)
    result = c.clamp
    assert_equal 0.0, result.r
    assert_equal 1.0, result.g
    assert_equal 0.5, result.b
    assert_equal 1.0, result.a
  end

  def test_to_bytes
    c = Larb::Color.new(1.0, 0.5, 0.0, 1.0)
    bytes = c.to_bytes
    assert_equal 255, bytes[0]
    assert_equal 128, bytes[1]
    assert_equal 0, bytes[2]
    assert_equal 255, bytes[3]
  end

  def test_to_hex
    c = Larb::Color.new(1.0, 0.0, 0.0, 1.0)
    assert_equal "#ff0000ff", c.to_hex
  end

  def test_to_vec3
    c = Larb::Color.new(0.1, 0.2, 0.3, 1.0)
    v = c.to_vec3
    assert_instance_of Larb::Vec3, v
    assert_equal 0.1, v.x
    assert_equal 0.2, v.y
    assert_equal 0.3, v.z
  end

  def test_to_vec4
    c = Larb::Color.new(0.1, 0.2, 0.3, 0.4)
    v = c.to_vec4
    assert_instance_of Larb::Vec4, v
    assert_equal 0.1, v.x
    assert_equal 0.2, v.y
    assert_equal 0.3, v.z
    assert_equal 0.4, v.w
  end

  def test_to_a
    c = Larb::Color.new(0.1, 0.2, 0.3, 0.4)
    assert_equal [0.1, 0.2, 0.3, 0.4], c.to_a
  end

  def test_from_vec4
    v = Larb::Vec4.new(0.1, 0.2, 0.3, 0.4)
    c = Larb::Color.from_vec4(v)
    assert_equal 0.1, c.r
    assert_equal 0.2, c.g
    assert_equal 0.3, c.b
    assert_equal 0.4, c.a
  end

  def test_from_vec3
    v = Larb::Vec3.new(0.1, 0.2, 0.3)
    c = Larb::Color.from_vec3(v)
    assert_equal 0.1, c.r
    assert_equal 0.2, c.g
    assert_equal 0.3, c.b
    assert_equal 1.0, c.a
  end

  def test_from_vec3_with_alpha
    v = Larb::Vec3.new(0.1, 0.2, 0.3)
    c = Larb::Color.from_vec3(v, 0.5)
    assert_equal 0.5, c.a
  end

  def test_inspect
    c = Larb::Color.new(0.1, 0.2, 0.3, 0.4)
    assert_equal "Color[0.1, 0.2, 0.3, 0.4]", c.inspect
  end

  def test_to_s
    c = Larb::Color.new(0.1, 0.2, 0.3, 0.4)
    assert_equal c.inspect, c.to_s
  end
end
