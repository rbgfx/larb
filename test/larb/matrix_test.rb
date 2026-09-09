# frozen_string_literal: true

require_relative "../test_helper"

class MatrixTest < Test::Unit::TestCase
  MATRIX_CLASSES = [Larb::Mat2, Larb::Mat2d, Larb::Mat3, Larb::Mat4].freeze

  def test_array_sizes_and_component_types
    MATRIX_CLASSES.each do |klass|
      size = klass.identity.to_a.size
      [[], Array.new(size - 1, 0), Array.new(size + 1, 0)].each do |values|
        assert_raise(ArgumentError) { klass.new(values) }
      end
      [nil, "1", Object.new].each do |value|
        assert_raise(TypeError) { klass.new(value) }
        values = klass.identity.to_a
        values[-1] = value
        assert_raise(TypeError) { klass.new(values) }
      end
    end
  end

  def test_numeric_scalar_multiplication_and_invalid_operands
    MATRIX_CLASSES.each do |klass|
      matrix = klass.identity
      assert_equal matrix.to_a.map { |value| value * 2 }, (matrix * 2).to_a
      [nil, "2", Object.new, []].each do |other|
        assert_raise(TypeError) { matrix * other }
      end
    end
  end

  def test_near_rejects_nonfinite_components_and_invalid_epsilon
    MATRIX_CLASSES.each do |klass|
      matrix = klass.identity
      [Float::NAN, Float::INFINITY, -Float::INFINITY].each do |value|
        other = klass.identity
        other[0] = value
        assert_false matrix.near?(other)
        assert_false other.near?(other)
      end
      [0, -1, Float::NAN, Float::INFINITY, -Float::INFINITY].each do |epsilon|
        assert_raise(ArgumentError) { matrix.near?(matrix, epsilon) }
      end
      assert_raise(TypeError) { matrix.near?(matrix, nil) }
    end
  end

  def test_inverse_does_not_depend_on_absolute_scale
    MATRIX_CLASSES.each do |klass|
      [1e-200, 1e-6, 1e200].each do |magnitude|
        matrix = klass.identity * magnitude
        inverse = matrix.inverse
        assert inverse.to_a.all?(&:finite?)
        assert_finite_close klass.identity.to_a, (matrix * inverse).to_a
        assert_finite_close klass.identity.to_a, (inverse * matrix).to_a
      end
    end
  end

  def test_inverse_uses_pivot_swaps_and_handles_different_axis_scales
    matrices = [
      Larb::Mat2.new([0, 2, 3, 4]),
      Larb::Mat2d.new([0, 2, 3, 4, 5, 6]),
      Larb::Mat3.new([0, 1, 0, 1, 2, 0, 0, 0, 3]),
      Larb::Mat4.new([0, 1, 0, 1, 1, 2, 1, 0, 2, 0, 3, 1, 1, 0, 0, 4]),
      Larb::Mat2.scaling(1e-200, 1e200),
      Larb::Mat2d.scaling(1e-200, 1e200),
      Larb::Mat3.scaling(1e-200, 1e200),
      Larb::Mat4.scaling(1e-200, 1e200, 1),
      Larb::Mat2d.translation(1e200, 0),
      Larb::Mat4.translation(1e200, 0, 0)
    ]
    matrices.each do |matrix|
      inverse = matrix.inverse
      assert inverse.to_a.all?(&:finite?)
      assert_finite_close matrix.class.identity.to_a, (matrix * inverse).to_a
      assert_finite_close matrix.class.identity.to_a, (inverse * matrix).to_a
    end
  end

  def test_inverse_rejects_rank_deficient_and_nonfinite_matrices
    MATRIX_CLASSES.each do |klass|
      assert_raise(RuntimeError) { klass.new(Array.new(klass.identity.to_a.size, 1)).inverse }
      [Float::NAN, Float::INFINITY].each do |value|
        matrix = klass.identity
        matrix[0] = value
        assert_raise(RuntimeError) { matrix.inverse }
      end
    end
    assert_raise(RuntimeError) { Larb::Mat3.new([1, 2, 3, 4, 5, 6, 5, 7, 9]).inverse }
    assert_raise(RuntimeError) do
      Larb::Mat4.new([1, 2, 3, 4, 5, 6, 7, 8, 6, 8, 10, 12, 0, 1, 0, 1]).inverse
    end
  end

  def test_inverse_preserves_small_components_in_rows_and_columns
    cases = [
      [[1e200, 1e-200, 1e200, -1e-200], [5e-201, 5e-201, 5e199, -5e199]],
      [[1e200, 1e200, 1e-200, -1e-200], [5e-201, 5e199, 5e-201, -5e199]]
    ]
    MATRIX_CLASSES.each do |klass|
      size = klass == Larb::Mat2d ? 2 : Math.sqrt(klass.identity.to_a.size).to_i
      indices = [0, 1, size, size + 1]
      cases.each do |values, expected|
        matrix = klass.identity
        indices.zip(values).each { |index, value| matrix[index] = value }
        inverse = matrix.inverse.to_a
        assert inverse.all?(&:finite?)
        indices.zip(expected).each do |index, value|
          assert_in_delta 1.0, inverse[index] / value, 1e-12
        end
      end
    end
  end

  def test_frobenius_norm_handles_extreme_magnitudes
    [Larb::Mat2, Larb::Mat2d, Larb::Mat3].each do |klass|
      [1e-200, 1e200].each do |magnitude|
        values = klass.zero.to_a
        values[0] = magnitude
        norm = klass.new(values).frobenius_norm
        assert norm.finite?
        assert_in_delta 1.0, norm / magnitude, 1e-12
      end
    end
  end

  private

  def assert_finite_close(expected, actual)
    assert actual.all?(&:finite?), actual.inspect
    expected.zip(actual).each { |a, b| assert_in_delta a, b, 1e-9 }
  end
end
