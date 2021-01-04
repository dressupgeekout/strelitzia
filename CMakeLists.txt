cmake_minimum_required(VERSION 3.0)
project(strelitzia)

include(FindPkgConfig)
include(GNUInstallDirs)

set(MRUBY_SRC "${CMAKE_CURRENT_LIST_DIR}/vendor/mruby" CACHE PATH "mruby source code location")
set(ONIGURUMA_SRC "${CMAKE_CURRENT_LIST_DIR}/vendor/oniguruma" CACHE PATH "Oniguruma source code location")

# Bundled mruby:
set(MRUBY_INCLUDE_DIR "${MRUBY_SRC}/include" CACHE PATH "Location of mruby headers")
set(MRBC "${MRUBY_SRC}/build/host/bin/mrbc" CACHE PATH "Location of mrbc(1)")
find_library(LIBMRUBY NAMES mruby PATHS ${MRUBY_SRC}/build/host/lib REQUIRED NO_DEFAULT_PATH)

# Bundled Oniguruma:
find_library(LIBONIG NAMES onig PATHS ${ONIGURUMA_SRC}/build REQUIRED NO_DEFAULT_PATH)

# ...but SDL2 and friends need to already be installed:
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

# Good to go:
add_subdirectory(src)
