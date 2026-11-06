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
RECONFIGURE=0

usage() {
  cat <<'EOF'
Usage: ./build-ubuntu.sh [--clean] [--debug] [--reconfigure]

Options:
  --clean   Remove build output before building
  --debug   Build with -g -O0
  --reconfigure  Regenerate Autotools files and rerun configure
EOF
}

for arg in "$@"; do
  case "$arg" in
    --clean) CLEAN=1 ;;
    --debug) DEBUG=1 ;;
    --reconfigure) RECONFIGURE=1 ;;
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

build_component() {
  local name=$1
  local source_dir=$2
  local build_dir=$3
  local stamp_file="$build_dir/.config-signature"
  local signature
  shift 3

  mkdir -p "$build_dir"
  signature=$(printf '%s\n' "$source_dir" "$PREFIX" "${CC:-cc}" "${CXX:-c++}" \
    "$COMMON_CPPFLAGS" "$COMMON_LDFLAGS" "$EXTRA_CFLAGS" "$EXTRA_CXXFLAGS" "$@")

  if [[ "$RECONFIGURE" == 1 || ! -x "$source_dir/configure" ]]; then
    echo "Preparing Autotools for $name..."
    (cd "$source_dir" && autoreconf -fvi)
  fi

  if [[ "$RECONFIGURE" == 1 || ! -f "$build_dir/Makefile" || ! -f "$stamp_file" || "$(<"$stamp_file")" != "$signature" ]]; then
    echo "Configuring $name..."
    (cd "$build_dir" && "$source_dir/configure" --srcdir="$source_dir" --prefix="$PREFIX" \
      "$@" CPPFLAGS="$COMMON_CPPFLAGS" LDFLAGS="$COMMON_LDFLAGS" \
      CFLAGS="$EXTRA_CFLAGS" CXXFLAGS="$EXTRA_CXXFLAGS")
    printf '%s' "$signature" > "$stamp_file"
  else
    echo "Reusing $name configuration"
  fi

  echo "Building $name..."
  (cd "$build_dir" && make -j"$BUILD_JOBS")
  echo "Installing $name to $PREFIX..."
  (cd "$build_dir" && make install)
}

generate_protocols
build_component simpleshell "$ROOT_DIR/simpleshell" "$BUILD_ROOT/simpleshell"
build_component westeros-gl-drm "$DRM_REPO" "$DRM_BUILD_DIR" --enable-gbm-modifiers
build_component Westeros "$ROOT_DIR" "$MAIN_BUILD_DIR" \
  --enable-app --enable-test --enable-rendergl --enable-embedded \
  --disable-essos --enable-xdgstable

echo "Build complete"
echo "  compositor: $PREFIX/bin/westeros"
echo "  client:     $PREFIX/bin/westeros_test"
echo "  renderer:   $PREFIX/lib/libwesteros_render_gl.so"
echo "  drm adaptr: $PREFIX/lib/libwesteros_gl.so"
