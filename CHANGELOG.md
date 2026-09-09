# Changelog

## Unreleased

- Preserve native components in `dup`/`clone` and enforce `freeze` on all mutations.
- Correct indexed Vec2 assignment, transform composition, quaternion interpolation, dual quaternion normalization/inversion, and reflection decomposition.
- Stabilize lengths, normalization, matrix inversion, and small axis-angle rotations; prevent NaN values from passing approximate comparisons.
- Validate constructor inputs, multiplication operands, tolerances, degenerate directions, length limits, perspective division, and hexadecimal colors.
- Clamp color components before integer conversion and preserve IEEE floating-point behavior in native builds.

## 1.0.0 - 2026-01-10

- Reimplemented in C for better performance.

## 0.1.0 - 2026-01-02

- Initial release.
