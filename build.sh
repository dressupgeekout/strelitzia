#!/bin/sh

set -e
set -o pipefail

PROGNAME=$(basename $0)

MRUBY_SRC=$(pwd)/vendor/mruby
ONIGURUMA_SRC=$(pwd)/vendor/oniguruma

PLATFORM=""
MAKE=make
MAKE_JOBS=1
CLEAN=no

########## ########## ##########

run_make() {
  ${MAKE} -j ${MAKE_JOBS} "$@"
}

usage() {
  echo "${PROGNAME}: usage: ${PROGNAME} [options]"
  echo "  --help             Show this help message"
  echo "  --clean            Remove build artifacts and exit"
  echo "  --platform         Use an alternative CMAKE_SYTEM_NAME (default: unset)"
  echo "  --make-program     Default: ${MAKE}"
  echo "  --make-jobs        Default: ${MAKE_JOBS}"
}

while [ $# -gt 0 ]; do
  case "$1" in
  --help)
    usage
    exit 0
    ;;
  --clean)
    CLEAN=yes
    shift 1
    ;;
  --platform)
    PLATFORM=$2
    shift 2
    ;;
  --make-program)
    MAKE=$2
    shift 2
    ;;
  --make-jobs)
    MAKE_JOBS=$2
    shift 2
    ;;
  *)
    echo "${PROGNAME}: unknown option: $1" 1>&2
    usage
    exit 1
    ;;
  esac
done

PLATFORM=${PLATFORM:-default}

BUILD_DIR=./build-${PLATFORM}
MRUBY_CONFIG="default"

if [ "${CLEAN}" = "yes" ]; then
  for repo in ${MRUBY_SRC} ${ONIGURUMA_SRC}; do
    git -C ${repo} clean -fdx
    git -C ${repo} reset --hard HEAD
  done
  rm -rf ${BUILD_DIR}
  exit 0
fi

if [ "${PLATFORM##mingw}" != "${PLATFORM}" ]; then
  cmake_system_name="-DCMAKE_SYSTEM_NAME=Windows"
  export CC=x86_64-w64-mingw32-gcc
  export LD=x86_64-w64-mingw32-gcc
  MRUBY_CONFIG="${PLATFORM}"
fi

mkdir -p "${BUILD_DIR}"

# Build Oniguruma first.
ONIG_BUILD_DIR="${ONIGURUMA_SRC}/build"
mkdir -p "${ONIG_BUILD_DIR}"

(cd "${ONIG_BUILD_DIR}" && cmake \
  ${cmake_system_name} \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_SHARED_LIBS=OFF \
  ..)

run_make -C ${ONIG_BUILD_DIR}

# Build mruby now:
cp -vf ./build_config/build_config_${MRUBY_CONFIG}.rb ${MRUBY_SRC}/build_config.rb
run_make -C ${MRUBY_SRC}

# Finally:
(cd "${BUILD_DIR}" && cmake \
  ${cmake_system_name} \
  -DCMAKE_BUILD_TYPE=Debug \
  -DMRUBY_SRC:PATH=${MRUBY_SRC} \
  -DONIGURUMA_SRC:PATH=${ONIGURUMA_SRC} \
  ..)

run_make -C ${BUILD_DIR}
