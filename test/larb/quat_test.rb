# frozen_string_literal: true

require_relative "../test_helper"

class QuatTest < Test::Unit::TestCase
  def test_new_with_default_values
    q = Larb::Quat.new
    assert_equal 0.0, q.x
    assert_equal 0.0, q.y
    assert_equal 0.0, q.z
    assert_equal 1.0, q.w
  end

  def test_new_with_given_values
    q = Larb::Quat.new(1, 2, 3, 4)
    assert_equal 1.0, q.x
    assert_equal 2.0, q.y
    assert_equal 3.0, q.z
    assert_equal 4.0, q.w
  end

  def test_bracket_syntax
    q = Larb::Quat[1, 2, 3, 4]
    assert_equal 1.0, q.x
    assert_equal 2.0, q.y
    assert_equal 3.0, q.z
    assert_equal 4.0, q.w
  end

  def test_identity
    q = Larb::Quat.identity
    assert_equal 0.0, q.x
    assert_equal 0.0, q.y
    assert_equal 0.0, q.z
    assert_equal 1.0, q.w
  end

  def test_from_axis_angle
    q = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 2)
    assert_in_delta 1.0, q.length, 1e-10
    assert_in_delta Math.sin(Math::PI / 4), q.y, 1e-10
    assert_in_delta Math.cos(Math::PI / 4), q.w, 1e-10
  end

  def test_from_euler
    q = Larb::Quat.from_euler(0, Math::PI / 2, 0)
    assert_in_delta 1.0, q.length, 1e-10
  end

  def test_multiply_quaternions
    q1 = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 4)
    q2 = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 4)
    result = q1 * q2
    expected = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 2)
    assert result.near?(expected)
  end

  def test_multiply_rotates_vector
    q = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 2)
    v = Larb::Vec3.new(1, 0, 0)
    result = q * v
    assert_in_delta 0.0, result.x, 1e-10
    assert_in_delta(-1.0, result.z, 1e-10)
  end

  def test_multiply_by_scalar
    q = Larb::Quat.new(1, 2, 3, 4)
    result = q * 2
    assert_equal 2.0, result.x
    assert_equal 4.0, result.y
    assert_equal 6.0, result.z
    assert_equal 8.0, result.w
  end

  def test_add
    q1 = Larb::Quat.new(1, 2, 3, 4)
    q2 = Larb::Quat.new(5, 6, 7, 8)
    result = q1 + q2
    assert_equal 6.0, result.x
    assert_equal 8.0, result.y
    assert_equal 10.0, result.z
    assert_equal 12.0, result.w
  end

  def test_subtract
    q1 = Larb::Quat.new(5, 6, 7, 8)
    q2 = Larb::Quat.new(1, 2, 3, 4)
    result = q1 - q2
    assert_equal 4.0, result.x
    assert_equal 4.0, result.y
    assert_equal 4.0, result.z
    assert_equal 4.0, result.w
  end

  def test_negate
    q = Larb::Quat.new(1, 2, 3, 4)
    result = -q
    assert_equal(-1.0, result.x)
    assert_equal(-2.0, result.y)
    assert_equal(-3.0, result.z)
    assert_equal(-4.0, result.w)
  end

  def test_dot
    q1 = Larb::Quat.new(1, 2, 3, 4)
    q2 = Larb::Quat.new(5, 6, 7, 8)
    assert_equal 70.0, q1.dot(q2)
  end

  def test_length
    q = Larb::Quat.new(1, 2, 2, 0)
    assert_equal 3.0, q.length
  end

  def test_length_squared
    q = Larb::Quat.new(1, 2, 2, 0)
    assert_equal 9.0, q.length_squared
  end

  def test_normalize
    q = Larb::Quat.new(1, 2, 2, 0)
    result = q.normalize
    assert_in_delta 1.0, result.length, 1e-10
  end

  def test_normalize!
    q = Larb::Quat.new(1, 2, 2, 0)
    q.normalize!
    assert_in_delta 1.0, q.length, 1e-10
  end

  def test_conjugate
    q = Larb::Quat.new(1, 2, 3, 4)
    result = q.conjugate
    assert_equal(-1.0, result.x)
    assert_equal(-2.0, result.y)
    assert_equal(-3.0, result.z)
    assert_equal 4.0, result.w
  end

  def test_inverse
    q = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 4)
    inv = q.inverse
    result = q * inv
    assert result.near?(Larb::Quat.identity)
  end

  def test_lerp
    q1 = Larb::Quat.identity
    q2 = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 2)
    result = q1.lerp(q2, 0.5)
    assert_in_delta 1.0, result.length, 1e-10
  end

  def test_slerp
    q1 = Larb::Quat.identity
    q2 = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 2)
    result = q1.slerp(q2, 0.5)
    expected = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 4)
    assert result.near?(expected, 1e-6)
  end

  def test_to_axis_angle
    q = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 2)
    axis, angle = q.to_axis_angle
    assert_in_delta Math::PI / 2, angle, 1e-10
    assert_in_delta 1.0, axis.y, 1e-10
  end

  def test_to_euler
    q = Larb::Quat.from_euler(0.1, 0.2, 0.3)
    euler = q.to_euler
    assert_in_delta 0.1, euler.x, 1e-6
    assert_in_delta 0.2, euler.y, 1e-6
    assert_in_delta 0.3, euler.z, 1e-6
  end

  def test_to_mat4
    q = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 2)
    m = q.to_mat4
    assert_instance_of Larb::Mat4, m
  end

  def test_to_a
    q = Larb::Quat.new(1, 2, 3, 4)
    assert_equal [1.0, 2.0, 3.0, 4.0], q.to_a
  end

  def test_index_access
    q = Larb::Quat.new(1, 2, 3, 4)
    assert_equal 1.0, q[0]
    assert_equal 2.0, q[1]
    assert_equal 3.0, q[2]
    assert_equal 4.0, q[3]
  end

  def test_equality
    q1 = Larb::Quat.new(1, 2, 3, 4)
    q2 = Larb::Quat.new(1, 2, 3, 4)
    assert_equal q1, q2
  end

  def test_inequality
    q1 = Larb::Quat.new(1, 2, 3, 4)
    q2 = Larb::Quat.new(1, 2, 3, 5)
    assert_not_equal q1, q2
  end

  def test_near
    q1 = Larb::Quat.new(1.0, 2.0, 3.0, 4.0)
    q2 = Larb::Quat.new(1.0000001, 2.0000001, 3.0000001, 4.0000001)
    assert q1.near?(q2)
  end

  def test_inspect
    q = Larb::Quat.new(1, 2, 3, 4)
    assert_equal "Quat[1.0, 2.0, 3.0, 4.0]", q.inspect
  end

  def test_to_s
    q = Larb::Quat.new(1, 2, 3, 4)
    assert_equal q.inspect, q.to_s
  end

  def test_interpolation_preserves_antipodal_rotations
    q = Larb::Quat.from_axis_angle(Larb::Vec3.up, 0.7)
    %i[lerp slerp].each do |method|
      [0, 0.5, 1].each do |t|
        result = (q * 1e200).public_send(method, q * -1e-200, t)
        assert_components q.to_a, result
        assert_in_delta 1.0, result.length, 1e-12
      end
      assert_raise(ArgumentError) { q.public_send(method, Larb::Quat.new(0, 0, 0, 0), 0.5) }
      assert_raise(ArgumentError) { q.public_send(method, q, Float::NAN) }
    end
  end

  def test_normalization_and_inverse_across_extreme_magnitudes
    [1e200, 1e-200, Float::MAX, Float::MIN].each do |magnitude|
      q = Larb::Quat.new(magnitude, 0, 0, 0)
      assert_equal magnitude, q.length
      assert_components [1, 0, 0, 0], q.normalize
      assert_components [0, 0, 0, 1], q * q.inverse
      assert_same q, q.normalize!
      assert_components [1, 0, 0, 0], q
    end
    assert_components [0.5, 0.5, 0.5, 0.5], Larb::Quat.new(*[Float::MAX] * 4).normalize
  end

  def test_degenerate_quaternions_cannot_be_normalized_or_inverted
    [0, Float::INFINITY, Float::NAN].each do |value|
      q = Larb::Quat.new(value, 0, 0, 0)
      %i[normalize normalize! inverse to_axis_angle].each do |method|
        assert_raise(ArgumentError) { q.public_send(method) }
      end
      assert_equal [0.0, 0.0, 0.0], q.to_a.drop(1)
      value.is_a?(Float) && value.nan? ? assert(q.x.nan?) : assert_equal(value, q.x)
    end
  end

  def test_small_axis_angles_round_trip
    [1e-3, 1e-12, 1e-200].each do |angle|
      q = Larb::Quat.from_axis_angle(Larb::Vec3.up, angle)
      axis, actual_angle = q.to_axis_angle
      assert_components [0, 1, 0], axis
      assert actual_angle.finite?
      assert_in_delta angle, actual_angle, angle * 1e-12
      assert_components q.to_a, Larb::Quat.from_axis_angle(axis, actual_angle)
    end
    axis, angle = Larb::Quat.identity.to_axis_angle
    assert_components [1, 0, 0], axis
    assert_equal 0.0, angle
  end

  private

  def assert_components(expected, actual)
    assert_equal expected.size, actual.to_a.size
    expected.zip(actual.to_a).each do |wanted, value|
      assert value.finite?, "expected finite component, got #{value.inspect}"
      assert_in_delta wanted, value, 1e-10
    end
  end
end
