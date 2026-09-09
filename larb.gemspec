# frozen_string_literal: true

require_relative "lib/larb/version"

Gem::Specification.new do |spec|
  spec.name = "larb"
  spec.version = Larb::VERSION
  spec.authors = ["Yudai Takada"]
  spec.email = ["t.yudai92@gmail.com"]

  spec.summary = "Linear algebra library for 2D/3D graphics in Ruby"
  spec.description = "Larb provides vectors, matrices, quaternions, and Color classes for 2D/3D graphics and mathematical computations"
  spec.homepage = "https://github.com/rbgfx/larb"
  spec.license = "MIT"
  spec.required_ruby_version = ">= 3.1.0"

  spec.metadata["source_code_uri"] = spec.homepage
  spec.metadata["changelog_uri"] = "#{spec.homepage}/blob/main/CHANGELOG.md"

  spec.extensions = ["ext/larb/extconf.rb"]
  spec.files = Dir["lib/**/*.rb"] +
               Dir["ext/**/*.{c,h,rb}"] +
               Dir["test/**/*.rb"] +
               %w[README.md LICENSE CHANGELOG.md]
  spec.require_paths = ["lib"]

  spec.add_development_dependency "rake-compiler", "~> 1.2"
end
