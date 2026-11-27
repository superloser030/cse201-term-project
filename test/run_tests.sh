#!/usr/bin/env bash
# Automated tests (Git Bash). Same as test.cmd.
#   test/auto/<name>/input.txt  : standard input
#   test/auto/<name>/expect.txt : lines that must appear ('!' = must NOT appear)
#   test/auto/<name>/output.txt : output of this run
# Usage: bash test/run_tests.sh [name-filter]
cd "$(dirname "$0")/.." || exit 1

EXE=./build/atm.exe
[ -x "$EXE" ] || EXE=./build/atm
[ -x "$EXE" ] || { echo "build/atm.exe not found. Run build.cmd first."; exit 1; }
FILTER="$1"

pass=0
fail=0
for dir in test/auto/*/; do
    name=$(basename "$dir")
    [ -n "$FILTER" ] && [[ "$name" != *"$FILTER"* ]] && continue
    output="$dir/output.txt"
    rm -f ATM_*_transaction_history.txt
    "$EXE" < "$dir/input.txt" > "$output" 2>&1
    ok=1
    while IFS= read -r line || [ -n "$line" ]; do
        line=${line%$'\r'}
        [ -z "$line" ] && continue
        if [[ "$line" == '!'* ]]; then
            if grep -qF -- "${line:1}" "$output"; then
                echo "  [$name] should NOT appear: ${line:1}"
                ok=0
            fi
        else
            if ! grep -qF -- "$line" "$output"; then
                echo "  [$name] missing: $line"
                ok=0
            fi
        fi
    done < "$dir/expect.txt"
    if [ "$ok" = 1 ]; then
        echo "PASS $name"
        pass=$((pass + 1))
    else
        echo "FAIL $name"
        fail=$((fail + 1))
    fi
done
rm -f ATM_*_transaction_history.txt

echo "----------------------------------------"
echo "Total: $((pass + fail)), passed: $pass, failed: $fail"
[ "$fail" = 0 ]
