# frozen_string_literal: true

require_relative "../test_helper"

class Vec2Test < Test::Unit::TestCase
  def test_index_assignment_bounds_and_negative_indices
    v = Larb::Vec2.new(1, 2)
    v[-2] = 3
    v[-1] = 4
    assert_equal [3.0, 4.0], v.to_a
    [-3, 2, 100].each { |index| assert_raise(IndexError) { v[index] = 99 } }
    assert_equal [3.0, 4.0], v.to_a
  end

  def test_clamp_length_validates_limits_and_handles_extreme_magnitudes
    [Larb::Vec2, Larb::Vec3].each do |klass|
      v = klass.new(1e200, 0)
      [-1, Float::NAN, Float::INFINITY].each do |limit|
        assert_raise(ArgumentError) { v.clamp_length(limit) }
      end
      assert_equal klass.new(2, 0), v.clamp_length(2)
      assert_equal klass.zero, v.clamp_length(0)
      assert_equal klass.zero, klass.zero.clamp_length(0)
      tiny = klass.new(1e-200, 0)
      assert_equal klass.new(1e-201, 0), tiny.clamp_length(1e-201)
    end
  end

  def test_new_with_default_values
    v = Larb::Vec2.new
    assert_equal 0.0, v.x
    assert_equal 0.0, v.y
  end

  def test_new_with_given_values
    v = Larb::Vec2.new(3, 4)
    assert_equal 3.0, v.x
    assert_equal 4.0, v.y
  end

  def test_bracket_syntax
    v = Larb::Vec2[1, 2]
    assert_equal 1.0, v.x
    assert_equal 2.0, v.y
  end

  def test_zero
    v = Larb::Vec2.zero
    assert_equal 0.0, v.x
    assert_equal 0.0, v.y
  end

  def test_one
    v = Larb::Vec2.one
    assert_equal 1.0, v.x
    assert_equal 1.0, v.y
  end

  def test_add
    v1 = Larb::Vec2.new(1, 2)
    v2 = Larb::Vec2.new(3, 4)
    result = v1 + v2
    assert_equal 4.0, result.x
    assert_equal 6.0, result.y
  end

  def test_subtract
    v1 = Larb::Vec2.new(5, 7)
    v2 = Larb::Vec2.new(2, 3)
    result = v1 - v2
    assert_equal 3.0, result.x
    assert_equal 4.0, result.y
  end

  def test_multiply_by_scalar
    v = Larb::Vec2.new(2, 3)
    result = v * 2
    assert_equal 4.0, result.x
    assert_equal 6.0, result.y
  end

  def test_divide_by_scalar
    v = Larb::Vec2.new(6, 8)
    result = v / 2
    assert_equal 3.0, result.x
    assert_equal 4.0, result.y
  end

  def test_negate
    v = Larb::Vec2.new(1, -2)
    result = -v
    assert_equal(-1.0, result.x)
    assert_equal 2.0, result.y
  end

  def test_dot
    v1 = Larb::Vec2.new(1, 2)
    v2 = Larb::Vec2.new(3, 4)
    assert_equal 11.0, v1.dot(v2)
  end

  def test_length
    v = Larb::Vec2.new(3, 4)
    assert_equal 5.0, v.length
  end

  def test_length_squared
    v = Larb::Vec2.new(3, 4)
    assert_equal 25.0, v.length_squared
  end

  def test_normalize
    v = Larb::Vec2.new(3, 4)
    result = v.normalize
    assert_in_delta 0.6, result.x, 1e-10
    assert_in_delta 0.8, result.y, 1e-10
  end

  def test_normalize!
    v = Larb::Vec2.new(3, 4)
    v.normalize!
    assert_in_delta 0.6, v.x, 1e-10
    assert_in_delta 0.8, v.y, 1e-10
  end

  def test_lerp
    v1 = Larb::Vec2.new(0, 0)
    v2 = Larb::Vec2.new(10, 20)
    result = v1.lerp(v2, 0.5)
    assert_equal 5.0, result.x
    assert_equal 10.0, result.y
  end

  def test_to_a
    v = Larb::Vec2.new(1, 2)
    assert_equal [1.0, 2.0], v.to_a
  end

  def test_index_access
    v = Larb::Vec2.new(1, 2)
    assert_equal 1.0, v[0]
    assert_equal 2.0, v[1]
  end

  def test_index_assignment
    v = Larb::Vec2.new(0, 0)
    v[0] = 5
    v[1] = 6
    assert_equal 5, v.x
    assert_equal 6, v.y
  end

  def test_equality
    v1 = Larb::Vec2.new(1, 2)
    v2 = Larb::Vec2.new(1, 2)
    assert_equal v1, v2
  end

  def test_inequality
    v1 = Larb::Vec2.new(1, 2)
    v2 = Larb::Vec2.new(1, 3)
    assert_not_equal v1, v2
  end

  def test_not_equal_to_array
    v = Larb::Vec2.new(1, 2)
    assert_not_equal v, [1, 2]
  end

  def test_near_true
    v1 = Larb::Vec2.new(1.0, 2.0)
    v2 = Larb::Vec2.new(1.0000001, 2.0000001)
    assert v1.near?(v2)
  end

  def test_near_false
    v1 = Larb::Vec2.new(1.0, 2.0)
    v2 = Larb::Vec2.new(1.1, 2.1)
    assert_false v1.near?(v2)
  end

  def test_angle
    v = Larb::Vec2.new(1, 0)
    assert_equal 0.0, v.angle
  end

  def test_angle_up
    v = Larb::Vec2.new(0, 1)
    assert_in_delta Math::PI / 2, v.angle, 1e-10
  end

  def test_angle_to
    v1 = Larb::Vec2.new(0, 0)
    v2 = Larb::Vec2.new(1, 0)
    assert_equal 0.0, v1.angle_to(v2)
  end

  def test_rotate
    v = Larb::Vec2.new(1, 0)
    result = v.rotate(Math::PI / 2)
    assert_in_delta 0.0, result.x, 1e-10
    assert_in_delta 1.0, result.y, 1e-10
  end

  def test_distance
    v1 = Larb::Vec2.new(0, 0)
    v2 = Larb::Vec2.new(3, 4)
    assert_equal 5.0, v1.distance(v2)
  end

  def test_distance_squared
    v1 = Larb::Vec2.new(0, 0)
    v2 = Larb::Vec2.new(3, 4)
    assert_equal 25.0, v1.distance_squared(v2)
  end

  def test_cross
    v1 = Larb::Vec2.new(1, 0)
    v2 = Larb::Vec2.new(0, 1)
    assert_equal 1.0, v1.cross(v2)
  end

  def test_perpendicular
    v = Larb::Vec2.new(1, 0)
    result = v.perpendicular
    assert_equal 0.0, result.x
    assert_equal 1.0, result.y
  end

  def test_reflect
    v = Larb::Vec2.new(1, -1).normalize
    normal = Larb::Vec2.new(0, 1)
    result = v.reflect(normal)
    assert_in_delta v.x, result.x, 1e-10
    assert_in_delta(-v.y, result.y, 1e-10)
  end

  def test_clamp_length
    v = Larb::Vec2.new(3, 4)
    result = v.clamp_length(2.5)
    assert_in_delta 2.5, result.length, 1e-10
  end

  def test_clamp_length_under_limit
    v = Larb::Vec2.new(1, 1)
    result = v.clamp_length(10)
    assert_equal v, result
  end

  def test_to_vec3
    v = Larb::Vec2.new(1, 2)
    result = v.to_vec3
    assert_instance_of Larb::Vec3, result
    assert_equal 1.0, result.x
    assert_equal 2.0, result.y
    assert_equal 0.0, result.z
  end

  def test_to_vec3_with_custom_z
    v = Larb::Vec2.new(1, 2)
    result = v.to_vec3(5)
    assert_equal 5.0, result.z
  end

  def test_inspect
    v = Larb::Vec2.new(1, 2)
    assert_equal "Vec2[1.0, 2.0]", v.inspect
  end

  def test_to_s
    v = Larb::Vec2.new(1, 2)
    assert_equal v.inspect, v.to_s
  end
end
