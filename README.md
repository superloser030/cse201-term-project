# CSE201 Term Project (Fall 2026) — ATM System

C++ console ATM simulator (Bank / Account / Card / ATM) built with object-oriented design.

## Quick start (Windows)
| What | How |
|---|---|
| Build | `build.cmd` |
| Run | `run.cmd` (builds first if needed) |
| Automated tests | `test.cmd` (all 49) / `test.cmd 23` (tests whose name contains "23") |
| Manual tests | follow `test/manual/manual_test_guide.md` |

All scripts can be double-clicked or run from cmd / PowerShell in the project root (`.\build.cmd`).

## Structure
```
build.cmd  run.cmd  test.cmd     build / run / test scripts
sample_initial_condition.txt      initial condition input file (default)
src/
  main.cpp
  system/       System (file loading, ATM selection, snapshot), ConsoleIO (input, '/', cancel)
  bank/         Bank, Account, Card (UserCard / AdminCard)
  atm/          ATM (SingleBankATM / MultiBankATM), Session
  transaction/  Transaction (4 kinds), TransactionLog, CashBundle, FeePolicy
  common/       Messages (English / Korean), Exceptions, TextUtil
test/
  auto/<name>/  input.txt, expect.txt, output.txt (created when tests run)
  data/         extra initial condition files used by tests
  manual/       manual_test_guide.md (scenarios for report screenshots)
  run_tests.ps1 / run_tests.sh   test runners (PowerShell / Git Bash)
docs/           decisions.md (design decisions, TA answers), test_report.md
reference/      provided example code (file_input.cpp)
build/          build outputs (ignored by git)
```

## Build (MSVC, C++17 required)
`build.cmd`, or in "Developer Command Prompt for VS":
```
cl /nologo /std:c++17 /EHsc /W4 /permissive- /utf-8 /I src /Fe:build\atm.exe /Fo:build\ src\main.cpp src\system\*.cpp src\bank\*.cpp src\atm\*.cpp src\transaction\*.cpp src\common\*.cpp
```
- `/std:c++17` is required, `/utf-8` is required (Korean messages in source), `/I src` for include paths.

## Run
Run from the project root so the input file is found (`run.cmd` does this):
1. Enter the initial condition file name (just press Enter for `sample_initial_condition.txt`).
2. Admin card numbers are printed at start: `ADMIN-<BankName>` (no password, usable only at ATMs whose primary bank issued it). The admin menu shows the transactions of that ATM only.
3. Select an ATM → (bilingual ATM: choose language) → insert card (enter card number) → password.

Special inputs:
- `/` at any prompt: print snapshot of all ATMs' cash and all accounts' balances (REQ10.1)
- `c` at any prompt inside a session: cancel
- Card / account numbers may be entered with or without `-`.

## Automated tests
Each `test/auto/<name>/input.txt` is fed as standard input; every line of `expect.txt` must appear in
the output (lines starting with `!` must not appear). The output is saved to `output.txt` in the same folder.
- PowerShell / cmd: `test.cmd`
- Git Bash: `bash test/run_tests.sh`
