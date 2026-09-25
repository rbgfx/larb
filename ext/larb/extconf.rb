# frozen_string_literal: true

require "mkmf"

# WASI already selects its target architecture; Ruby's cross-build config may
# also provide an empty -march= flag, which clang rejects.
$ARCH_FLAG = "" if RbConfig::CONFIG["host"].start_with?("wasm32")

# mathライブラリの確認
have_library("m", "sin")

# 最適化フラグ
$CFLAGS << " -O3 -funroll-loops"

create_makefile("larb/larb")
