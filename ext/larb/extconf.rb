# frozen_string_literal: true

require "mkmf"

# mathライブラリの確認
have_library("m", "sin")

# 最適化フラグ
$CFLAGS << " -O3 -march=native -funroll-loops"

create_makefile("larb/larb")
