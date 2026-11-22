# CSE201 Term Project (Fall 2026) — ATM System

C++ console ATM simulator (Bank / Account / ATM) built with object-oriented design.

## Structure
```
src/        source code to submit (.h / .cpp)
test/       local test code (not submitted)
reference/  provided example code (file_input.cpp)
docs/       report materials (screenshots, class diagram)
build/      build outputs (ignored by git)
sample_initial_condition.txt   initial condition input file
```

## Build (MSVC, Developer Command Prompt)
```
cl /nologo /EHsc /W4 /utf-8 /Fe:build\atm.exe /Fo:build\ src\*.cpp
```

## Run
Run from the project root so the input file is found:
```
build\atm.exe
```
Enter `/` at any input prompt to print the Account/ATM snapshot (REQ10.1).
