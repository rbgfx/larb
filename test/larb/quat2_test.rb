# frozen_string_literal: true

require_relative "../test_helper"

class Quat2Test < Test::Unit::TestCase
  def test_new_identity_by_default
    q = Larb::Quat2.new
    assert_equal 1.0, q[3]
    assert_equal 0.0, q[7]
  end

  def test_new_with_data
    data = [0, 0, 0, 1, 0, 0, 0, 0]
    q = Larb::Quat2.new(data)
    assert_equal data, q.data
  end

  def test_identity
    q = Larb::Quat2.identity
    assert_equal Larb::Quat.identity, q.real
  end

  def test_from_rotation_translation
    rotation = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 2)
    translation = Larb::Vec3.new(1, 2, 3)
    q = Larb::Quat2.from_rotation_translation(rotation, translation)
    assert q.rotation.near?(rotation)
    assert q.translation.near?(translation)
  end

  def test_from_translation
    translation = Larb::Vec3.new(5, 10, 15)
    q = Larb::Quat2.from_translation(translation)
    assert q.translation.near?(translation)
    assert q.rotation.near?(Larb::Quat.identity)
  end

  def test_from_rotation
    rotation = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 2)
    q = Larb::Quat2.from_rotation(rotation)
    assert q.rotation.near?(rotation)
    assert q.translation.near?(Larb::Vec3.zero)
  end

  def test_real
    q = Larb::Quat2.identity
    assert_instance_of Larb::Quat, q.real
  end

  def test_dual
    q = Larb::Quat2.identity
    assert_instance_of Larb::Quat, q.dual
  end

  def test_multiply_quat2
    q1 = Larb::Quat2.from_translation(Larb::Vec3.new(1, 0, 0))
    q2 = Larb::Quat2.from_translation(Larb::Vec3.new(0, 1, 0))
    result = q1 * q2
    assert result.translation.near?(Larb::Vec3.new(1, 1, 0))
  end

  def test_multiply_vec3
    q = Larb::Quat2.from_translation(Larb::Vec3.new(10, 0, 0))
    point = Larb::Vec3.new(0, 0, 0)
    result = q * point
    assert result.near?(Larb::Vec3.new(10, 0, 0))
  end

  def test_add
    q1 = Larb::Quat2.identity
    q2 = Larb::Quat2.identity
    result = q1 + q2
    assert_equal 2.0, result[3]
  end

  def test_subtract
    q1 = Larb::Quat2.identity
    q2 = Larb::Quat2.identity
    result = q1 - q2
    assert_equal 0.0, result[3]
  end

  def test_dot
    q = Larb::Quat2.identity
    assert_equal 1.0, q.dot(q)
  end

  def test_length
    q = Larb::Quat2.identity
    assert_equal 1.0, q.length
  end

  def test_normalize
    q = Larb::Quat2.new([0, 0, 0, 2, 0, 0, 0, 0])
    result = q.normalize
    assert_in_delta 1.0, result.length, 1e-10
  end

  def test_conjugate
    rotation = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 2)
    q = Larb::Quat2.from_rotation(rotation)
    conj = q.conjugate
    result = q * conj
    assert result.rotation.near?(Larb::Quat.identity)
  end

  def test_inverse
    rotation = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 4)
    translation = Larb::Vec3.new(1, 2, 3)
    q = Larb::Quat2.from_rotation_translation(rotation, translation)
    inv = q.inverse
    result = q * inv
    assert result.rotation.near?(Larb::Quat.identity)
  end

  def test_translation
    t = Larb::Vec3.new(5, 10, 15)
    q = Larb::Quat2.from_translation(t)
    assert q.translation.near?(t)
  end

  def test_rotation
    r = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 2)
    q = Larb::Quat2.from_rotation(r)
    assert q.rotation.near?(r)
  end

  def test_transform_point
    rotation = Larb::Quat.from_axis_angle(Larb::Vec3.new(0, 1, 0), Math::PI / 2)
    translation = Larb::Vec3.new(0, 0, 5)
    q = Larb::Quat2.from_rotation_translation(rotation, translation)
    point = Larb::Vec3.new(1, 0, 0)
    result = q.transform_point(point)
    assert_in_delta 0.0, result.x, 1e-10
    assert_in_delta 4.0, result.z, 1e-10
  end

  def test_lerp
    q1 = Larb::Quat2.from_translation(Larb::Vec3.new(0, 0, 0))
    q2 = Larb::Quat2.from_translation(Larb::Vec3.new(10, 0, 0))
    result = q1.lerp(q2, 0.5)
    assert_in_delta 5.0, result.translation.x, 1e-6
  end

  def test_equality
    q1 = Larb::Quat2.identity
    q2 = Larb::Quat2.identity
    assert_equal q1, q2
  end

  def test_near
    q1 = Larb::Quat2.identity
    q2 = Larb::Quat2.new([0, 0, 0, 1.0000001, 0, 0, 0, 0])
    assert q1.near?(q2)
  end

  def test_inspect
    q = Larb::Quat2.identity
    assert_match(/Quat2/, q.inspect)
  end

  def test_matrix_and_point_transform_agree
    rotation = Larb::Quat.from_axis_angle(Larb::Vec3.forward, Math::PI / 2)
    q = Larb::Quat2.from_rotation_translation(rotation, Larb::Vec3.new(1, 2, 3))
    [Larb::Vec3.zero, Larb::Vec3.new(4, -2, 7)].each do |point|
      actual = (q.to_mat4 * Larb::Vec4.new(*point.to_a, 1)).xyz
      assert_components q.transform_point(point).to_a, actual
    end
    assert_components [1, 2, 3], q.to_mat4.extract_translation
  end

  def test_lerp_preserves_antipodal_rigid_transforms
    rotation = Larb::Quat.from_axis_angle(Larb::Vec3.up, 0.7)
    q = Larb::Quat2.from_rotation_translation(rotation, Larb::Vec3.new(1, 2, 3))
    [0, 0.5, 1].each do |t|
      result = (q * 1e200).lerp(q * -1e-200, t)
      assert_components q.to_a, result
      assert_components q.transform_point(Larb::Vec3.right).to_a,
                        result.transform_point(Larb::Vec3.right)
    end
  end

  def test_lerp_preserves_real_dual_orthogonality
    a = Larb::Quat2.identity
    b = Larb::Quat2.from_rotation_translation(Larb::Quat.new(0, 0, 1, 0), Larb::Vec3.new(0, 0, 2))
    [0, 0.25, 0.5, 0.75, 1].each do |t|
      q = a.lerp(b, t)
      assert q.to_a.all?(&:finite?)
      assert_in_delta 1.0, q.real.length, 1e-12
      assert_in_delta 0.0, q.real.dot(q.dual), 1e-12
      assert_components Larb::Quat2.identity.to_a, q * q.inverse
    end
  end

  def test_normalization_removes_parallel_dual_component
    q = Larb::Quat2.new([0, 0, 0, 2, 4, 6, 8, 10])
    assert_components [0, 0, 0, 1, 2, 3, 4, 0], q.normalize
    assert_same q, q.normalize!
    assert_components [0, 0, 0, 1, 2, 3, 4, 0], q
    assert_components Larb::Quat2.identity.to_a,
                      Larb::Quat2.new([0, 0, 0, 1e-200, 0, 0, 0, 1e200]).normalize
  end

  def test_normalization_and_inverse_across_extreme_magnitudes
    q = Larb::Quat2.from_translation(Larb::Vec3.new(1, 2, 3))
    [1e200, 1e-200].each do |magnitude|
      scaled = q * magnitude
      assert_equal magnitude, scaled.length
      assert_components q.to_a, scaled.normalize
      assert_components Larb::Quat2.identity.to_a, scaled * scaled.inverse
      assert_same scaled, scaled.normalize!
      assert_components q.to_a, scaled
    end
    assert_components [0.5, 0.5, 0.5, 0.5, 0, 0, 0, 0],
                      Larb::Quat2.new([Float::MAX] * 4 + [0] * 4).normalize
  end

  def test_normalization_preserves_subnormal_dual_components
    magnitude = Float::MIN * Float::EPSILON
    q = Larb::Quat2.new([magnitude] * 4 + [magnitude, -magnitude] * 2)
    expected = [0.5] * 4 + [0.5, -0.5] * 2
    assert_components expected, q.normalize
    assert_same q, q.normalize!
    assert_components expected, q
  end

  def test_inverse_supports_nonorthogonal_dual_quaternions
    q = Larb::Quat2.new([2, 3, 4, 5, 6, 7, 8, 9])
    assert_components Larb::Quat2.identity.to_a, q * q.inverse
    assert_components Larb::Quat2.identity.to_a, q.inverse * q
  end

  def test_degenerate_dual_quaternions_are_rejected_atomically
    [[0] * 8, [0, 0, 0, Float::INFINITY, 0, 0, 0, 0],
     [0, 0, 0, 2, 3, Float::INFINITY, 0, 0]].each do |values|
      q = Larb::Quat2.new(values)
      %i[normalize normalize! inverse].each do |method|
        assert_raise(ArgumentError) { q.public_send(method) }
        assert_equal values, q.to_a
      end
      assert_raise(ArgumentError) { Larb::Quat2.identity.lerp(q, 0.5) }
    end
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
