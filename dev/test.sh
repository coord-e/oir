#!/usr/bin/env bash

set -euo pipefail

readonly ACCEPT=${ACCEPT:-false}
readonly OIR_BIN=${OIR_BIN:-./build/oir}

readonly PROJECT_DIR="$(git rev-parse --show-toplevel)"
readonly TEST_DIR="$PROJECT_DIR/test"

function run_command() {
  local -r target="$1"

  local head_line
  head_line="$(head -n1 "$target")"
  local -r command="${head_line#\# test: *}"

  if [ "$head_line" == "$command" ]; then
    echo "no test command specified for \"$target\"" >&2
    return 1
  fi

  eval "$command -i \"$target\""
}

function main() {
  local fail=0

  shopt -s globstar nullglob
  for target_path in "$TEST_DIR"/**/*.oir; do
    local target_file="${target_path##*/}"
    local case_name="${target_file%.*}"
    local golden_path="$TEST_DIR/golden/$case_name.golden"

    local tmp_path
    tmp_path="$(mktemp)"

    if [ ! -f "$golden_path" ]; then
      touch "$golden_path"
    fi

    run_command "$target_path" > "$tmp_path"

    local diff_exit=0
    local diff_result
    diff_result="$(diff --color=always "$golden_path" "$tmp_path")" || diff_exit=$?

    if [ $diff_exit -ne 0 ]; then
      if $ACCEPT; then
        echo "accept changes for \"$case_name\""
        cp "$tmp_path" "$golden_path"
      else
        echo "\"$case_name\" failed:"
        echo "$diff_result"
        fail=1
      fi
    fi
  done

  if [ $fail -ne 0 ]; then
    echo "test failed"
    exit 1
  fi
}

>&2 main "$@"
