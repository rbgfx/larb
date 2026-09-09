# frozen_string_literal: true

require_relative "../test_helper"

class NativeValueTest < Test::Unit::TestCase
  TYPES = [Larb::Vec2, Larb::Vec3, Larb::Vec4, Larb::Mat2, Larb::Mat2d,
           Larb::Mat3, Larb::Mat4, Larb::Quat, Larb::Quat2, Larb::Color].freeze
  ARRAY_TYPES = [Larb::Mat2, Larb::Mat2d, Larb::Mat3, Larb::Mat4, Larb::Quat2].freeze

  def arguments(klass, values)
    ARRAY_TYPES.include?(klass) ? [values] : values
  end

  def sample(klass)
    klass.new(*arguments(klass, (1..klass.new.to_a.length).to_a))
  end

  def setters(value)
    %i[x= y= z= w= r= g= b= a=].select { |method| value.respond_to?(method) }
  end

  def coercion(&block)
    Object.new.tap { |object| object.define_singleton_method(:to_f, &block) }
  end

  def test_copies_preserve_components_and_have_independent_storage
    TYPES.each do |klass|
      original = sample(klass)
      [original.dup, original.clone, original.freeze.clone(freeze: false)].each do |copy|
        assert_equal original.to_a, copy.to_a, klass.name
        copy.send(:initialize)
        assert_equal (1..original.to_a.length).to_a, original.to_a
      end
      assert_predicate original.clone, :frozen?
      subclass = Class.new(klass)
      assert_instance_of subclass, subclass.new.dup
      assert_raise(TypeError) { klass.new.send(:initialize_copy, Object.new) }
    end
  end

  def test_frozen_values_reject_every_mutator
    TYPES.each do |klass|
      value = sample(klass).freeze
      before = value.to_a
      setters(value).each { |method| assert_raise(FrozenError) { value.public_send(method, 99) } }
      assert_raise(FrozenError) { value[0] = 99 } if value.respond_to?(:[]=)
      assert_raise(FrozenError) { value.normalize! } if value.respond_to?(:normalize!)
      assert_raise(FrozenError) { value.send(:initialize) }
      assert_raise(FrozenError) { value.send(:initialize_copy, sample(klass)) }
      assert_equal before, value.to_a
    end
  end

  def test_setters_recheck_freeze_after_numeric_conversion
    TYPES.each do |klass|
      setters(sample(klass)).each do |method|
        value = sample(klass)
        before = value.to_a
        number = coercion { value.freeze; 99.0 }
        assert_raise(FrozenError) { value.public_send(method, number) }
        assert_equal before, value.to_a
      end
      next unless sample(klass).respond_to?(:[]=)

      value = sample(klass)
      before = value.to_a
      number = coercion { value.freeze; 99.0 }
      assert_raise(FrozenError) { value[0] = number }
      assert_equal before, value.to_a

      value = sample(klass)
      index = Object.new
      index.define_singleton_method(:to_int) { value.freeze; 0 }
      assert_raise(FrozenError) { value[index] = 99 }
      assert_equal before, value.to_a
    end
  end

  def test_reinitialization_is_atomic_when_conversion_freezes_or_fails
    TYPES.each do |klass|
      value = sample(klass)
      before = value.to_a
      input = Array.new(before.length, 99)
      input[-1] = coercion { value.freeze; 99.0 }
      assert_raise(FrozenError) { value.send(:initialize, *arguments(klass, input)) }
      assert_equal before, value.to_a

      value = sample(klass)
      input[-1] = Object.new
      assert_raise(TypeError) { value.send(:initialize, *arguments(klass, input)) }
      assert_equal before, value.to_a
    end
  end

  def test_numeric_inputs_reject_strings_and_nil
    TYPES.each do |klass|
      [nil, "not a number"].each do |invalid|
        input = sample(klass).to_a
        input[0] = invalid
        assert_raise(TypeError) { klass.new(*arguments(klass, input)) }
        assert_raise(TypeError) { sample(klass) * invalid }
        setters(sample(klass)).each do |method|
          assert_raise(TypeError) { sample(klass).public_send(method, invalid) }
        end
      end
    end
  end

  def test_array_constructors_require_exact_component_counts
    ARRAY_TYPES.each do |klass|
      count = klass.new.to_a.length
      [0, count - 1, count + 1].each do |length|
        assert_raise(ArgumentError) { klass.new(Array.new(length, 0)) }
      end
      assert_raise(TypeError) { klass.new(nil) }
    end
  end

  def test_near_rejects_nan_components_and_invalid_tolerances
    TYPES.each do |klass|
      good = sample(klass)
      values = good.to_a
      values.each_index do |i|
        bad_values = values.dup
        bad_values[i] = Float::NAN
        bad = klass.new(*arguments(klass, bad_values))
        assert_false good.near?(bad), klass.name
        assert_false bad.near?(good), klass.name
      end
      [0, -1, Float::NAN, Float::INFINITY].each do |epsilon|
        assert_raise(ArgumentError) { good.near?(good, epsilon) }
      end
      assert_raise(TypeError) { good.near?(good, nil) }
    end
  end

  def test_vector_lengths_and_normalization_across_magnitudes
    [Larb::Vec2, Larb::Vec3, Larb::Vec4].each do |klass|
      count = klass.zero.to_a.length
      [1e200, 1e-200, Float::MAX, Float::MIN * Float::EPSILON].each do |magnitude|
        value = klass.new(magnitude, *Array.new(count - 1, 0))
        assert_equal magnitude, value.length
        assert_equal magnitude, value.distance(klass.zero) if value.respond_to?(:distance)
        assert_equal [1.0, *Array.new(count - 1, 0.0)], value.normalize.to_a
        value.normalize!
        assert_equal [1.0, *Array.new(count - 1, 0.0)], value.to_a
      end
      value = klass.new(*Array.new(count, Float::MAX)).normalize
      value.to_a.each { |v| assert_in_delta 1.0 / Math.sqrt(count), v, 1e-15 }
      [0.0, Float::INFINITY, Float::NAN].each do |invalid|
        value = klass.new(invalid, *Array.new(count - 1, 0))
        assert_raise(ArgumentError) { value.normalize }
        assert_raise(ArgumentError) { value.normalize! }
      end
    end
  end
end
