#!/usr/bin/env bash
# Headless smoke test: one Rookie career round (career-rookie.xml on forza).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
CONFIG="${CAREER_CONFIG:-$ROOT/test/career/career-rookie-headless.xml}"
case "$CONFIG" in
  /*) ;;
  *) CONFIG="$ROOT/$CONFIG" ;;
esac
RUNTIME="${TORCS_RUNTIME:-$ROOT/runtime}"
case "$RUNTIME" in /*) ;; *) RUNTIME="$ROOT/$RUNTIME" ;; esac
RUNTIME="$(cd "$RUNTIME" && pwd)"
DATA="${TORCS_DATA:-$ROOT}"
LIB="${TORCS_LIB:-$ROOT/export}"
case "$LIB" in /*) ;; *) LIB="$ROOT/$LIB" ;; esac

if [[ -n "${TORCS_BIN:-}" ]]; then
  BIN="$TORCS_BIN"
elif command -v torcs-bin >/dev/null 2>&1; then
  BIN="torcs-bin"
elif command -v torcs >/dev/null 2>&1; then
  BIN="torcs"
else
  echo "error: torcs not found; set TORCS_BIN" >&2
  exit 1
fi
case "$BIN" in
  /*) ;;
  *) BIN="$(cd "$(dirname "$BIN")" && pwd)/$(basename "$BIN")" ;;
esac
if [[ ! -x "$BIN" ]]; then
  echo "error: torcs binary not executable: $BIN" >&2
  exit 1
fi

if [[ ! -f "$CONFIG" ]]; then
  echo "error: career config not found: $CONFIG" >&2
  exit 1
fi

echo "== Career Rookie race (headless) =="
echo "  binary:  $BIN"
echo "  config:  $CONFIG"
echo "  runtime: $RUNTIME"

if [[ -d "${LIB}/lib" ]]; then
  export LD_LIBRARY_PATH="${LIB}/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
fi

(
  cd "$RUNTIME"
  "$BIN" -l "./" -L "$LIB/" -D "$DATA/" -r "$CONFIG"
)

RACEMAN_STEM="$(basename "$CONFIG" .xml)"
RESULTS_DIR="$RUNTIME/results/$RACEMAN_STEM"
if [[ ! -d "$RESULTS_DIR" ]]; then
  echo "error: expected results in $RESULTS_DIR" >&2
  exit 1
fi

LATEST="$(ls -1t "$RESULTS_DIR"/results-*.xml 2>/dev/null | head -1 || true)"
if [[ -z "$LATEST" ]]; then
  echo "error: no results-*.xml in $RESULTS_DIR" >&2
  exit 1
fi

echo "  results: $LATEST"
echo "Career Rookie headless race OK"
