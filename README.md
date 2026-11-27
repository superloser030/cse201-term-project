# CSE201 Term Project (Fall 2026) — ATM System

C++ console ATM simulator (Bank / Account / Card / ATM) built with object-oriented design.

> `prototype` branch: full reference implementation + automated tests,
> based on the provisional decisions in `docs/decisions.md`.

## Structure
```
src/        source code (.h / .cpp)
test/       automated tests (cases/*.in, cases/*.expect, data/, run_tests.sh, output/)
reference/  provided example code (file_input.cpp)
docs/       design decisions, test report
build/      build outputs (ignored by git)
sample_initial_condition.txt   initial condition input file
```

## Build (MSVC, C++17 required)
```
build.cmd
```
or, in "Developer Command Prompt for VS":
```
cl /nologo /std:c++17 /EHsc /W4 /permissive- /utf-8 /Fe:build\atm.exe /Fo:build\ src\*.cpp
```
- `/std:c++17` is required (`static constexpr` members, `std::plus<>`).
- `/utf-8` is required (Korean messages in source).

## Run
Run from the project root so the input file is found:
```
build\atm.exe
```
1. Enter the initial condition file name (just press Enter for `sample_initial_condition.txt`).
2. Admin card numbers are printed at start: `ADMIN-<BankName>` (no password, usable only at ATMs whose primary bank issued it). The admin menu shows the transactions of that ATM only.
3. Select an ATM → (bilingual ATM: choose language) → insert card (enter card number) → password.

Special inputs:
- `/` at any prompt: print snapshot of all ATMs' cash and all accounts' balances (REQ10.1)
- `c` at any prompt inside a session: cancel
- Card / account numbers may be entered with or without `-`.

## Test
From the project root (Git Bash):
```
bash test/run_tests.sh
```
Each `test/cases/<name>.in` is fed as standard input; every line of `<name>.expect` must appear in
the output (lines starting with `!` must not appear). Outputs are saved to `test/output/`.
