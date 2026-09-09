# frozen_string_literal: true

require_relative "../test_helper"

class Vec4Test < Test::Unit::TestCase
  def test_new_with_default_values
    v = Larb::Vec4.new
    assert_equal 0.0, v.x
    assert_equal 0.0, v.y
    assert_equal 0.0, v.z
    assert_equal 1.0, v.w
  end

  def test_new_with_given_values
    v = Larb::Vec4.new(1, 2, 3, 4)
    assert_equal 1.0, v.x
    assert_equal 2.0, v.y
    assert_equal 3.0, v.z
    assert_equal 4.0, v.w
  end

  def test_bracket_syntax
    v = Larb::Vec4[1, 2, 3, 4]
    assert_equal 1.0, v.x
    assert_equal 2.0, v.y
    assert_equal 3.0, v.z
    assert_equal 4.0, v.w
  end

  def test_bracket_syntax_default_w
    v = Larb::Vec4[1, 2, 3]
    assert_equal 1.0, v.w
  end

  def test_zero
    assert_equal Larb::Vec4.new(0, 0, 0, 0), Larb::Vec4.zero
  end

  def test_one
    assert_equal Larb::Vec4.new(1, 1, 1, 1), Larb::Vec4.one
  end

  def test_add
    v1 = Larb::Vec4.new(1, 2, 3, 4)
    v2 = Larb::Vec4.new(5, 6, 7, 8)
    assert_equal Larb::Vec4.new(6, 8, 10, 12), v1 + v2
  end

  def test_subtract
    v1 = Larb::Vec4.new(5, 6, 7, 8)
    v2 = Larb::Vec4.new(1, 2, 3, 4)
    assert_equal Larb::Vec4.new(4, 4, 4, 4), v1 - v2
  end

  def test_multiply_by_scalar
    v = Larb::Vec4.new(1, 2, 3, 4)
    assert_equal Larb::Vec4.new(2, 4, 6, 8), v * 2
  end

  def test_divide_by_scalar
    v = Larb::Vec4.new(2, 4, 6, 8)
    assert_equal Larb::Vec4.new(1, 2, 3, 4), v / 2
  end

  def test_perspective_divide
    v = Larb::Vec4.new(2, 4, 6, 2)
    result = v.perspective_divide
    assert_instance_of Larb::Vec3, result
    assert_equal 1.0, result.x
    assert_equal 2.0, result.y
    assert_equal 3.0, result.z
  end

  def test_perspective_divide_w_is_1
    v = Larb::Vec4.new(1, 2, 3, 1)
    assert_equal Larb::Vec3.new(1, 2, 3), v.perspective_divide
  end

  def test_perspective_divide_w_is_0
    v = Larb::Vec4.new(1, 2, 3, 0)
    assert_raise(ArgumentError) { v.perspective_divide }
    assert_equal Larb::Vec3.new(1, 2, 3), v.xyz
  end

  def test_perspective_divide_rejects_nonfinite_w
    [Float::NAN, Float::INFINITY].each do |w|
      assert_raise(ArgumentError) { Larb::Vec4.new(1, 2, 3, w).perspective_divide }
    end
  end

  def test_xyz
    v = Larb::Vec4.new(1, 2, 3, 4)
    result = v.xyz
    assert_instance_of Larb::Vec3, result
    assert_equal Larb::Vec3.new(1, 2, 3), result
  end

  def test_xy
    v = Larb::Vec4.new(1, 2, 3, 4)
    result = v.xy
    assert_instance_of Larb::Vec2, result
    assert_equal Larb::Vec2.new(1, 2), result
  end

  def test_rgb
    v = Larb::Vec4.new(0.5, 0.6, 0.7, 1.0)
    result = v.rgb
    assert_instance_of Larb::Vec3, result
    assert_equal Larb::Vec3.new(0.5, 0.6, 0.7), result
  end

  def test_to_a
    v = Larb::Vec4.new(1, 2, 3, 4)
    assert_equal [1.0, 2.0, 3.0, 4.0], v.to_a
  end

  def test_index_access
    v = Larb::Vec4.new(1, 2, 3, 4)
    assert_equal 1.0, v[0]
    assert_equal 2.0, v[1]
    assert_equal 3.0, v[2]
    assert_equal 4.0, v[3]
  end

  def test_negate
    v = Larb::Vec4.new(1, -2, 3, -4)
    assert_equal Larb::Vec4.new(-1, 2, -3, 4), -v
  end

  def test_dot
    v1 = Larb::Vec4.new(1, 2, 3, 4)
    v2 = Larb::Vec4.new(5, 6, 7, 8)
    assert_equal 70.0, v1.dot(v2)
  end

  def test_length
    v = Larb::Vec4.new(1, 2, 2, 0)
    assert_equal 3.0, v.length
  end

  def test_length_squared
    v = Larb::Vec4.new(1, 2, 2, 0)
    assert_equal 9.0, v.length_squared
  end

  def test_normalize
    v = Larb::Vec4.new(1, 2, 2, 0)
    result = v.normalize
    assert_in_delta 1.0, result.length, 1e-10
  end

  def test_normalize!
    v = Larb::Vec4.new(1, 2, 2, 0)
    v.normalize!
    assert_in_delta 1.0, v.length, 1e-10
  end

  def test_equality
    v1 = Larb::Vec4.new(1, 2, 3, 4)
    v2 = Larb::Vec4.new(1, 2, 3, 4)
    assert_equal v1, v2
  end

  def test_inequality
    v1 = Larb::Vec4.new(1, 2, 3, 4)
    v2 = Larb::Vec4.new(1, 2, 3, 5)
    assert_not_equal v1, v2
  end

  def test_not_equal_to_array
    v = Larb::Vec4.new(1, 2, 3, 4)
    assert_not_equal v, [1, 2, 3, 4]
  end

  def test_near
    v1 = Larb::Vec4.new(1.0, 2.0, 3.0, 4.0)
    v2 = Larb::Vec4.new(1.0000001, 2.0000001, 3.0000001, 4.0000001)
    assert v1.near?(v2)
  end

  def test_lerp
    v1 = Larb::Vec4.new(0, 0, 0, 0)
    v2 = Larb::Vec4.new(10, 20, 30, 40)
    assert_equal Larb::Vec4.new(5, 10, 15, 20), v1.lerp(v2, 0.5)
  end

  def test_inspect
    v = Larb::Vec4.new(1, 2, 3, 4)
    assert_equal "Vec4[1.0, 2.0, 3.0, 4.0]", v.inspect
  end

  def test_to_s
    v = Larb::Vec4.new(1, 2, 3, 4)
    assert_equal v.inspect, v.to_s
  end
end
