#!/usr/bin/env bash
# 자동 테스트: test/cases/<name>.in 을 표준 입력으로 실행하고
# test/cases/<name>.expect 의 각 줄이 출력에 있는지 확인한다.
#   - 일반 줄: 출력에 반드시 있어야 함
#   - '!'로 시작하는 줄: 출력에 없어야 함
# 실행: 프로젝트 최상위에서  bash test/run_tests.sh
cd "$(dirname "$0")/.." || exit 1

EXE=./build/atm.exe
[ -x "$EXE" ] || EXE=./build/atm
mkdir -p test/output

pass=0
fail=0
for input in test/cases/*.in; do
    name=$(basename "$input" .in)
    output="test/output/$name.out"
    rm -f ATM_*_transaction_history.txt
    "$EXE" < "$input" > "$output" 2>&1
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
    done < "test/cases/$name.expect"
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
