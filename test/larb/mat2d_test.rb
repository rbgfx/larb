# frozen_string_literal: true

require_relative "../test_helper"

class Mat2dTest < Test::Unit::TestCase
  def test_new_identity_by_default
    m = Larb::Mat2d.new
    assert_equal 1.0, m[0]
    assert_equal 0.0, m[1]
    assert_equal 0.0, m[2]
    assert_equal 1.0, m[3]
    assert_equal 0.0, m[4]
    assert_equal 0.0, m[5]
  end

  def test_new_with_data
    data = [1, 2, 3, 4, 5, 6]
    m = Larb::Mat2d.new(data)
    assert_equal [1, 2, 3, 4, 5, 6], m.data
  end

  def test_identity
    m = Larb::Mat2d.identity
    assert_equal 1.0, m[0]
    assert_equal 1.0, m[3]
  end

  def test_zero
    m = Larb::Mat2d.zero
    6.times { |i| assert_equal 0.0, m[i] }
  end

  def test_translation
    m = Larb::Mat2d.translation(10, 20)
    assert_equal 10.0, m[4]
    assert_equal 20.0, m[5]
  end

  def test_rotation
    m = Larb::Mat2d.rotation(Math::PI / 2)
    assert_in_delta 0.0, m[0], 1e-10
    assert_in_delta 1.0, m[1], 1e-10
  end

  def test_scaling
    m = Larb::Mat2d.scaling(2, 3)
    assert_equal 2.0, m[0]
    assert_equal 3.0, m[3]
  end

  def test_from_rotation_translation_scale
    m = Larb::Mat2d.from_rotation_translation_scale(
      0,
      Larb::Vec2.new(10, 20),
      Larb::Vec2.new(2, 3)
    )
    assert_equal 2.0, m[0]
    assert_equal 3.0, m[3]
    assert_equal 10.0, m[4]
    assert_equal 20.0, m[5]
  end

  def test_multiply_mat2d
    m1 = Larb::Mat2d.translation(10, 0)
    m2 = Larb::Mat2d.translation(5, 0)
    result = m1 * m2
    assert_equal 15.0, result[4]
  end

  def test_multiply_vec2
    m = Larb::Mat2d.translation(10, 20)
    v = Larb::Vec2.new(0, 0)
    result = m * v
    assert_equal 10.0, result.x
    assert_equal 20.0, result.y
  end

  def test_multiply_rotation_translation
    m = Larb::Mat2d.rotation(Math::PI / 2) * Larb::Mat2d.translation(1, 0)
    v = Larb::Vec2.new(0, 0)
    result = m * v
    assert_in_delta 0.0, result.x, 1e-10
    assert_in_delta 1.0, result.y, 1e-10
  end

  def test_determinant
    m = Larb::Mat2d.identity
    assert_equal 1.0, m.determinant
  end

  def test_determinant_scaling
    m = Larb::Mat2d.scaling(2, 3)
    assert_equal 6.0, m.determinant
  end

  def test_inverse
    m = Larb::Mat2d.translation(10, 20) * Larb::Mat2d.scaling(2, 2)
    inv = m.inverse
    result = m * inv
    assert result.near?(Larb::Mat2d.identity)
  end

  def test_inverse_singular
    m = Larb::Mat2d.zero
    assert_raise_message("Matrix is not invertible") { m.inverse }
  end

  def test_translate
    m = Larb::Mat2d.identity
    result = m.translate(5, 10)
    assert_equal 5.0, result[4]
    assert_equal 10.0, result[5]
  end

  def test_rotate
    m = Larb::Mat2d.identity
    result = m.rotate(Math::PI / 2)
    assert_in_delta 0.0, result[0], 1e-10
  end

  def test_scale
    m = Larb::Mat2d.identity
    result = m.scale(2, 3)
    assert_equal 2.0, result[0]
    assert_equal 3.0, result[3]
  end

  def test_extract_translation
    m = Larb::Mat2d.translation(10, 20)
    t = m.extract_translation
    assert_equal 10.0, t.x
    assert_equal 20.0, t.y
  end

  def test_extract_rotation
    m = Larb::Mat2d.rotation(Math::PI / 4)
    angle = m.extract_rotation
    assert_in_delta Math::PI / 4, angle, 1e-10
  end

  def test_extract_scale
    m = Larb::Mat2d.scaling(2, 3)
    s = m.extract_scale
    assert_in_delta 2.0, s.x, 1e-10
    assert_in_delta 3.0, s.y, 1e-10
  end

  def test_to_mat3
    m = Larb::Mat2d.translation(10, 20)
    m3 = m.to_mat3
    assert_instance_of Larb::Mat3, m3
    assert_equal 10.0, m3[6]
    assert_equal 20.0, m3[7]
  end

  def test_equality
    m1 = Larb::Mat2d.identity
    m2 = Larb::Mat2d.identity
    assert_equal m1, m2
  end

  def test_near
    m1 = Larb::Mat2d.identity
    m2 = Larb::Mat2d.new([1.0000001, 0, 0, 1.0000001, 0, 0])
    assert m1.near?(m2)
  end

  def test_inspect
    m = Larb::Mat2d.identity
    assert_match(/Mat2d/, m.inspect)
  end

  def test_decomposition_round_trips_signed_scales
    [-1, 1].repeated_permutation(2) do |signs|
      [0, Math::PI / 2, 1.2].each do |angle|
        matrix = Larb::Mat2d.from_rotation_translation_scale(
          angle, Larb::Vec2.new(1, 2), Larb::Vec2.new(signs[0] * 2, signs[1] * 3)
        )
        scale = matrix.extract_scale
        assert_equal signs.inject(:*) < 0, scale.x < 0
        rebuilt = Larb::Mat2d.from_rotation_translation_scale(
          matrix.extract_rotation, matrix.extract_translation, scale
        )
        assert rebuilt.to_a.all?(&:finite?)
        matrix.to_a.zip(rebuilt.to_a).each { |a, b| assert_in_delta a, b, 1e-9 }
      end
    end
  end

  def test_decomposition_rejects_shear_zero_scale_and_nonfinite_values
    [Larb::Mat2d.new([1, 0, 0.5, 1, 0, 0]), Larb::Mat2d.scaling(0, 1),
     Larb::Mat2d.scaling(Float::NAN, 1)].each do |matrix|
      assert_raise(ArgumentError) { matrix.extract_scale }
      assert_raise(ArgumentError) { matrix.extract_rotation }
    end
  end
end
