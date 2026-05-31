#!/usr/bin/env bash
# Run headless regression race and optionally compare to baseline winner.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
CONFIG="${REGRESSION_CONFIG:-$ROOT/test/regression/regression-race.xml}"
BASELINE="${REGRESSION_BASELINE:-$ROOT/test/regression/baselines/regression-race.winner}"
RUNTIME="${TORCS_RUNTIME:-$ROOT/runtime}"
DATA="${TORCS_DATA:-$ROOT/data}"
LIB="${TORCS_LIB:-$RUNTIME}"

if [[ -n "${TORCS_BIN:-}" ]]; then
  BIN="$TORCS_BIN"
elif command -v torcs >/dev/null 2>&1; then
  BIN="torcs"
elif command -v torcs-bin >/dev/null 2>&1; then
  BIN="torcs-bin"
else
  echo "error: torcs not found; set TORCS_BIN or install the simulator" >&2
  exit 1
fi

if [[ ! -f "$CONFIG" ]]; then
  echo "error: race config not found: $CONFIG" >&2
  exit 1
fi

echo "== Regression race =="
echo "  binary:  $BIN"
echo "  config:  $CONFIG"
echo "  runtime: $RUNTIME"
echo "  data:    $DATA"

"$BIN" -l "$RUNTIME/" -L "$LIB/" -D "$DATA/" -r "$CONFIG"

RESULTS_DIR="$RUNTIME/results/regression-race"
if [[ ! -d "$RESULTS_DIR" ]]; then
  echo "warning: no results directory at $RESULTS_DIR (race may have failed silently)" >&2
  exit 1
fi

LATEST="$(ls -1t "$RESULTS_DIR"/results-*.xml 2>/dev/null | head -1 || true)"
if [[ -z "$LATEST" ]]; then
  echo "error: no results-*.xml in $RESULTS_DIR" >&2
  exit 1
fi

echo "  results: $LATEST"

if [[ ! -f "$BASELINE" ]]; then
  echo "note: no baseline at $BASELINE — run record_baseline.sh after verifying output"
  exit 0
fi

EXPECTED="$(tr -d '[:space:]' < "$BASELINE")"
# First driver in standings section 1 is race winner in standard result files.
ACTUAL=""
if command -v xmllint >/dev/null 2>&1; then
  ACTUAL="$(xmllint --xpath 'string(//section[@name="1"]/attstr[@name="module"]/@val)' "$LATEST" 2>/dev/null || true)"
fi
if [[ -z "$ACTUAL" ]]; then
  ACTUAL="$(grep -oP '(?<=<attstr name="module" val=")[^"]+' "$LATEST" | head -1 || true)"
fi

if [[ -z "$ACTUAL" ]]; then
  echo "warning: could not parse winner module from results; baseline not checked" >&2
  exit 0
fi

echo "  winner:  $ACTUAL (baseline: $EXPECTED)"
if [[ "$ACTUAL" != "$EXPECTED" ]]; then
  echo "error: winner mismatch — update baseline only if physics/AI change is intentional" >&2
  exit 1
fi

echo "regression OK"
