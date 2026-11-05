#!/usr/bin/env bash
set -Eeuo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
BUILD_ROOT=${WESTEROS_BUILD_ROOT:-"$ROOT_DIR/build/ubuntu"}
PREFIX=${WESTEROS_PREFIX:-"$BUILD_ROOT/install"}
RUNTIME_DIR=${WESTEROS_RUNTIME_DIR:-"$BUILD_ROOT/runtime"}
LOG_DIR="$BUILD_ROOT"
DISPLAY_NAME=${WESTEROS_DISPLAY:-westeros}
DRM_CARD=${WESTEROS_DRM_CARD:-/dev/dri/card0}
RUN_TTY=tty3

TEST_TIMEOUT=${WESTEROS_TEST_TIMEOUT:-20}
SW_CLIENT=${RUN_LINUX_SW_CLIENT:-1}
FORCE_GUI=${RUN_LINUX_FORCE_GUI:-0}
ALLOW_HEADLESS=${RUN_LINUX_ALLOW_HEADLESS:-0}
INTERNAL_ENGINE=${RUN_LINUX_INTERNAL:-0}

usage() {
  cat <<'EOF'
Usage: ./run-linux.sh [options]

Options:
  --timeout SEC      westeros_test timeout in seconds (default: 20)
  --sw-client        run test client in software mode (default)
  --hw-client        run test client in hardware mode
  --force-gui        allow engine execution in non-tty sessions
  --allow-headless   do not fail when scanout evidence is missing
  --internal-engine  internal flag used by wrapper mode (do not use directly)
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --timeout)
      TEST_TIMEOUT=${2:-}
      shift 2
      ;;
    --sw-client)
      SW_CLIENT=1
      shift
      ;;
    --hw-client)
      SW_CLIENT=0
      shift
      ;;
    --force-gui)
      FORCE_GUI=1
      shift
      ;;
    --allow-headless)
      ALLOW_HEADLESS=1
      shift
      ;;
    --internal-engine)
      INTERNAL_ENGINE=1
      shift
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "Unknown option: $1" >&2
      usage
      exit 2
      ;;
  esac
done

if [[ ! "$TEST_TIMEOUT" =~ ^[0-9]+$ ]]; then
  echo "Timeout must be numeric seconds" >&2
  exit 2
fi

run_engine() {
  if [[ "$FORCE_GUI" == 0 ]]; then
    session_type=${XDG_SESSION_TYPE:-unknown}
    if [[ "$session_type" != "tty" ]]; then
      echo "Refusing to run DRM path in non-tty session (XDG_SESSION_TYPE=$session_type)." >&2
      echo "Switch to VT first, for example: sudo chvt 3" >&2
      echo "Or re-run with --force-gui if you explicitly want to test inside GUI." >&2
      exit 1
    fi
  fi

  [[ -x "$PREFIX/bin/westeros" ]] || { echo "Missing compositor: $PREFIX/bin/westeros" >&2; exit 1; }
  [[ -x "$PREFIX/bin/westeros_test" ]] || { echo "Missing client: $PREFIX/bin/westeros_test" >&2; exit 1; }
  [[ -f "$PREFIX/lib/libwesteros_render_gl.so" ]] || { echo "Missing renderer lib" >&2; exit 1; }
  [[ -f "$PREFIX/lib/libwesteros_gl.so" ]] || { echo "Missing DRM adapter lib" >&2; exit 1; }
  [[ -e "$DRM_CARD" ]] || { echo "DRM device unavailable: $DRM_CARD" >&2; exit 1; }
  command -v stdbuf >/dev/null 2>&1 || { echo "Missing stdbuf" >&2; exit 1; }
  command -v timeout >/dev/null 2>&1 || { echo "Missing timeout" >&2; exit 1; }

  mkdir -p "$RUNTIME_DIR"
  chmod 700 "$RUNTIME_DIR"
  export XDG_RUNTIME_DIR="$RUNTIME_DIR"

  mkdir -p "$LOG_DIR"

  comp_log="$LOG_DIR/westeros-tty.log"
  client_log="$LOG_DIR/westeros_test.log"
  rm -f "$comp_log" "$client_log"
  rm -f "$XDG_RUNTIME_DIR/$DISPLAY_NAME" "$XDG_RUNTIME_DIR/$DISPLAY_NAME.lock"

  compositor_pid=''
  cleanup_engine() {
    [[ -n "$compositor_pid" ]] && kill "$compositor_pid" 2>/dev/null || true
    [[ -n "$compositor_pid" ]] && wait "$compositor_pid" 2>/dev/null || true
  }
  trap cleanup_engine EXIT INT TERM

  echo "Starting compositor on DRM card $DRM_CARD (display=$DISPLAY_NAME)..."
  LD_PRELOAD="$PREFIX/lib/libwesteros_gl.so${LD_PRELOAD:+:$LD_PRELOAD}" \
  WESTEROS_DRM_CARD="$DRM_CARD" \
  WESTEROS_GL_NO_PLANES=${WESTEROS_GL_NO_PLANES:-1} \
  WESTEROS_GL_NO_VBLANK=${WESTEROS_GL_NO_VBLANK:-1} \
  WESTEROS_GL_NO_VIDEOSERVER=${WESTEROS_GL_NO_VIDEOSERVER:-1} \
  WESTEROS_GL_DEBUG=${WESTEROS_GL_DEBUG:-3} \
  WESTEROS_GL_GRAPHICS_MAX_SIZE=${WESTEROS_GL_GRAPHICS_MAX_SIZE:-1920x1080} \
  EGL_LOG_LEVEL=${EGL_LOG_LEVEL:-debug} \
  stdbuf -oL -eL "$PREFIX/bin/westeros" \
    --renderer "$PREFIX/lib/libwesteros_render_gl.so" \
    --display "$DISPLAY_NAME" \
    --window-size 1920x1080 >"$comp_log" 2>&1 &
  compositor_pid=$!

  for _ in {1..80}; do
    kill -0 "$compositor_pid" 2>/dev/null || {
      echo "Compositor exited early. Log: $comp_log" >&2
      tail -n 80 "$comp_log" >&2 || true
      exit 1
    }
    [[ -S "$XDG_RUNTIME_DIR/$DISPLAY_NAME" ]] && break
    sleep 0.1
  done

  [[ -S "$XDG_RUNTIME_DIR/$DISPLAY_NAME" ]] || {
    echo "Wayland socket not created: $XDG_RUNTIME_DIR/$DISPLAY_NAME" >&2
    tail -n 80 "$comp_log" >&2 || true
    exit 1
  }

  echo "Compositor started. Key startup lines:"
  grep -E "master|eglInitiialize|eglSurface|unable" "$comp_log" | head -n 20 || true

  if [[ "$SW_CLIENT" == 1 ]]; then
    export LIBGL_ALWAYS_SOFTWARE=1
    echo "Running westeros_test with software GL"
  else
    unset LIBGL_ALWAYS_SOFTWARE
    echo "Running westeros_test with hardware GL"
  fi

  export EGL_LOG_LEVEL=${EGL_LOG_LEVEL:-debug}
  set +e
  timeout --foreground --signal=INT "$TEST_TIMEOUT" \
    "$PREFIX/bin/westeros_test" --display "$DISPLAY_NAME" 2>&1 | tee "$client_log"
  client_rc=${PIPESTATUS[0]}
  set -e

  if [[ "$client_rc" == 124 ]]; then
    echo "westeros_test timed out after ${TEST_TIMEOUT}s (expected for long-running render loop)."
    client_rc=0
  fi

  master_state=$(grep -m1 -o 'master [01]' "$comp_log" | awk '{print $2}' || true)
  bad_surface_count=$(grep -c 'EGL_BAD_SURFACE' "$comp_log" || true)
  no_surface_count=$(grep -c 'outputMode: no surfaces' "$client_log" || true)
  open_card_fail_count=$(grep -c 'failed to open card' "$comp_log" || true)
  egl_surface_count=$(grep -c 'eglSurface ' "$comp_log" || true)
  client_surface_count=$(grep -c 'createWindowSurface success' "$client_log" || true)
  client_log_lines=$(wc -l < "$client_log" 2>/dev/null || echo 0)

  bad_surface_hard_fail=0
  if [[ "$bad_surface_count" -gt 50 ]]; then
    bad_surface_hard_fail=1
  fi

  no_surface_hard_fail=0
  if [[ "$no_surface_count" -gt 0 && "$client_surface_count" -eq 0 ]]; then
    no_surface_hard_fail=1
  fi

  if [[ "$ALLOW_HEADLESS" != 1 ]]; then
    if [[ "$master_state" != "1" || "$bad_surface_hard_fail" -eq 1 || "$no_surface_hard_fail" -eq 1 || "$open_card_fail_count" -gt 0 || "$egl_surface_count" -eq 0 || "$client_surface_count" -eq 0 || "$client_log_lines" -eq 0 ]]; then
      echo "Run failed visible-render gate:" >&2
      [[ "$master_state" != "1" ]] && echo "  - DRM master is not 1 (current: ${master_state:-unknown})" >&2
      [[ "$bad_surface_hard_fail" -eq 1 ]] && echo "  - compositor log has excessive EGL_BAD_SURFACE ($bad_surface_count lines)" >&2
      [[ "$no_surface_hard_fail" -eq 1 ]] && echo "  - client reports outputMode: no surfaces without any successful surface creation" >&2
      [[ "$open_card_fail_count" -gt 0 ]] && echo "  - compositor could not open DRM card ($DRM_CARD)" >&2
      [[ "$egl_surface_count" -eq 0 ]] && echo "  - compositor did not report EGL surface creation" >&2
      [[ "$client_surface_count" -eq 0 ]] && echo "  - client did not report createWindowSurface success" >&2
      [[ "$client_log_lines" -eq 0 ]] && echo "  - client produced no output" >&2
      exit 1
    else
      [[ "$bad_surface_count" -gt 0 ]] && echo "Warning: transient EGL_BAD_SURFACE seen ($bad_surface_count lines)" >&2
      [[ "$no_surface_count" -gt 0 ]] && echo "Warning: transient outputMode: no surfaces seen before surface was created" >&2
    fi
  fi

  exit "$client_rc"
}

run_wrapper() {
  command -v openvt >/dev/null 2>&1 || { echo "Missing openvt" >&2; exit 1; }
  command -v chvt >/dev/null 2>&1 || { echo "Missing chvt" >&2; exit 1; }

  ACTIVE_TTY=$(cat /sys/class/tty/tty0/active 2>/dev/null || true)
  if [[ -z "$ACTIVE_TTY" || ! "$ACTIVE_TTY" =~ ^tty[0-9]+$ ]]; then
    ACTIVE_TTY=tty2
  fi

  if ! sudo -n true >/dev/null 2>&1; then
    echo "Passwordless sudo is required for VT switching/openvt in this script." >&2
    echo "Either configure sudo -n for this user or run from root shell." >&2
    exit 1
  fi

  cleanup_wrapper() {
    sudo -n pkill -INT -x westeros_test 2>/dev/null || true
    sudo -n pkill -INT -x westeros 2>/dev/null || true
  }
  trap cleanup_wrapper EXIT INT TERM

  cd "$ROOT_DIR"
  echo "Running Westeros on $RUN_TTY with timeout ${TEST_TIMEOUT}s"

  RUN_TTY_NUM=${RUN_TTY#tty}
  RETURN_TTY_NUM=${ACTIVE_TTY#tty}
  echo "Switching to $RUN_TTY"
  sudo -n chvt "$RUN_TTY_NUM" || true

  RUN_AS_USER=${SUDO_USER:-$USER}
  SW_ENV=$SW_CLIENT
  FG_ENV=$FORCE_GUI
  AH_ENV=$ALLOW_HEADLESS
  TO_ENV=$TEST_TIMEOUT
  RL_CMD="exec sudo -n -u '$RUN_AS_USER' env XDG_SESSION_TYPE=tty RUN_LINUX_INTERNAL=1 RUN_LINUX_SW_CLIENT='$SW_ENV' RUN_LINUX_FORCE_GUI='$FG_ENV' RUN_LINUX_ALLOW_HEADLESS='$AH_ENV' WESTEROS_TEST_TIMEOUT='$TO_ENV' '$ROOT_DIR/run-linux.sh'"

  set +e
  sudo -n openvt -f -w -c "$RUN_TTY_NUM" -- /bin/bash -lc "$RL_CMD"
  RC=$?
  set -e

  echo "Switching back to $ACTIVE_TTY"
  sudo -n chvt "$RETURN_TTY_NUM" || true

  exit "$RC"
}

if [[ "$INTERNAL_ENGINE" == 1 ]]; then
  run_engine
else
  run_wrapper
fi