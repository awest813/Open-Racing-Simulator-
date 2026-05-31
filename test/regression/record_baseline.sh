#!/usr/bin/env bash
# Record regression winner module from the latest results file.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
RUNTIME="${TORCS_RUNTIME:-$ROOT/runtime}"
BASELINE_DIR="$ROOT/test/regression/baselines"
BASELINE="$BASELINE_DIR/regression-race.winner"
RESULTS_DIR="$RUNTIME/results/regression-race"

LATEST="$(ls -1t "$RESULTS_DIR"/results-*.xml 2>/dev/null | head -1 || true)"
if [[ -z "$LATEST" ]]; then
  echo "error: run ./test/regression/run_regression.sh first" >&2
  exit 1
fi

mkdir -p "$BASELINE_DIR"
WINNER=""
if command -v xmllint >/dev/null 2>&1; then
  WINNER="$(xmllint --xpath 'string(//section[@name="Drivers"]/section[@name="1"]/attstr[@name="dll name"]/@val)' "$LATEST" 2>/dev/null || true)"
fi
if [[ -z "$WINNER" ]]; then
  WINNER="$(grep -oP '(?<=<attstr name="dll name" val=")[^"]+' "$LATEST" | head -1 || true)"
fi
if [[ -z "$WINNER" ]]; then
  echo "error: could not parse winner from $LATEST" >&2
  exit 1
fi

printf '%s\n' "$WINNER" > "$BASELINE"
echo "wrote baseline: $BASELINE ($WINNER from $LATEST)"
