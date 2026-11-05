#!/usr/bin/env bash
set -Eeuo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
BUILD_ROOT=${WESTEROS_BUILD_ROOT:-"$ROOT_DIR/build/ubuntu"}
PREFIX=${WESTEROS_PREFIX:-"$BUILD_ROOT/install"}
MAIN_BUILD_DIR="$BUILD_ROOT/westeros"
DRM_BUILD_DIR="$BUILD_ROOT/westeros-gl-drm"
DRM_REPO=${WESTEROS_DRM_REPO:-"$ROOT_DIR/../westeros-gl-drm"}
BUILD_JOBS=${BUILD_JOBS:-$(nproc 2>/dev/null || echo 4)}
CLEAN=0
DEBUG=0

usage() {
  cat <<'EOF'
Usage: ./build-ubuntu.sh [--clean] [--debug]

Options:
  --clean   Remove build output before building
  --debug   Build with -g -O0
EOF
}

for arg in "$@"; do
  case "$arg" in
    --clean) CLEAN=1 ;;
    --debug) DEBUG=1 ;;
    -h|--help) usage; exit 0 ;;
    *) echo "Unknown option: $arg" >&2; usage; exit 2 ;;
  esac
done

[[ -d "$DRM_REPO" ]] || {
  echo "Missing DRM adapter repository: $DRM_REPO" >&2
  echo "Set WESTEROS_DRM_REPO to your westeros-gl-drm path." >&2
  exit 1
}

if [[ "$CLEAN" == 1 ]]; then
  echo "Cleaning $BUILD_ROOT"
  rm -rf "$BUILD_ROOT"
fi

mkdir -p "$MAIN_BUILD_DIR" "$DRM_BUILD_DIR" "$PREFIX"

COMMON_CPPFLAGS="-I$PREFIX/include -I/usr/include/libdrm"
COMMON_LDFLAGS="-L$PREFIX/lib -Wl,-rpath,$PREFIX/lib"
EXTRA_CFLAGS=""
EXTRA_CXXFLAGS=""
if [[ "$DEBUG" == 1 ]]; then
  EXTRA_CFLAGS="-g -O0"
  EXTRA_CXXFLAGS="-g -O0"
fi

SCANNER_TOOL=${SCANNER_TOOL:-$(command -v wayland-scanner || true)}
[[ -n "$SCANNER_TOOL" ]] || {
  echo "Missing required tool: wayland-scanner" >&2
  exit 1
}

generate_protocols() {
  echo "Generating wayland protocol sources..."
  (cd "$ROOT_DIR" && SCANNER_TOOL="$SCANNER_TOOL" make -C protocol)
  (cd "$ROOT_DIR" && SCANNER_TOOL="$SCANNER_TOOL" make -C simpleshell/protocol)
  (cd "$ROOT_DIR" && SCANNER_TOOL="$SCANNER_TOOL" make -C linux-dmabuf/protocol)
  (cd "$ROOT_DIR" && SCANNER_TOOL="$SCANNER_TOOL" make -C linux-expsync/protocol)
}

build_main() {
  echo "Preparing Westeros autotools..."
  (cd "$ROOT_DIR" && autoreconf -fvi)

  echo "Configuring Westeros embedded build..."
  (cd "$MAIN_BUILD_DIR" && \
    "$ROOT_DIR/configure" \
      --srcdir="$ROOT_DIR" \
      --prefix="$PREFIX" \
      --enable-app \
      --enable-test \
      --enable-rendergl \
      --enable-embedded \
      --disable-essos \
      --enable-xdgstable \
      CPPFLAGS="$COMMON_CPPFLAGS" \
      LDFLAGS="$COMMON_LDFLAGS" \
      CFLAGS="$EXTRA_CFLAGS" \
      CXXFLAGS="$EXTRA_CXXFLAGS")

  echo "Building Westeros..."
  (cd "$MAIN_BUILD_DIR" && make -j"$BUILD_JOBS")

  echo "Installing Westeros to $PREFIX..."
  (cd "$MAIN_BUILD_DIR" && make install)
}

build_simpleshell() {
  local shell_src="$ROOT_DIR/simpleshell"
  local shell_build="$BUILD_ROOT/simpleshell"

  mkdir -p "$shell_build"

  echo "Preparing simpleshell autotools..."
  (cd "$shell_src" && autoreconf -fvi)

  echo "Configuring simpleshell..."
  (cd "$shell_build" && \
    "$shell_src/configure" \
      --srcdir="$shell_src" \
      --prefix="$PREFIX" \
      CPPFLAGS="$COMMON_CPPFLAGS" \
      LDFLAGS="$COMMON_LDFLAGS" \
      CFLAGS="$EXTRA_CFLAGS" \
      CXXFLAGS="$EXTRA_CXXFLAGS")

  echo "Building simpleshell..."
  (cd "$shell_build" && make -j"$BUILD_JOBS")

  echo "Installing simpleshell to $PREFIX..."
  (cd "$shell_build" && make install)
}

build_drm_adapter() {
  echo "Preparing DRM adapter autotools..."
  (cd "$DRM_REPO" && autoreconf -fvi)

  echo "Configuring westeros-gl-drm..."
  (cd "$DRM_BUILD_DIR" && \
    "$DRM_REPO/configure" \
      --srcdir="$DRM_REPO" \
      --prefix="$PREFIX" \
      --enable-gbm-modifiers \
      CPPFLAGS="$COMMON_CPPFLAGS" \
      LDFLAGS="$COMMON_LDFLAGS" \
      CFLAGS="$EXTRA_CFLAGS" \
      CXXFLAGS="$EXTRA_CXXFLAGS")

  echo "Building DRM adapter..."
  (cd "$DRM_BUILD_DIR" && make -j"$BUILD_JOBS")

  echo "Installing DRM adapter to $PREFIX..."
  (cd "$DRM_BUILD_DIR" && make install)
}

generate_protocols
build_simpleshell
build_drm_adapter
build_main

echo "Build complete"
echo "  compositor: $PREFIX/bin/westeros"
echo "  client:     $PREFIX/bin/westeros_test"
echo "  renderer:   $PREFIX/lib/libwesteros_render_gl.so"
echo "  drm adaptr: $PREFIX/lib/libwesteros_gl.so"
