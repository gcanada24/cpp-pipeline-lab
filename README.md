# cpp-pipeline-lab

Fast-track C++ study for pipeline / mocap / Unreal tools roles. C++ only; lessons are taught in a chat with
Claude and each one builds a real piece of a motion capture pipeline. See **[COURSE.md](COURSE.md)**.

## Layout
- `lessons/NN-name/main.cpp` — the exercise (compiles from the start, tests built in, you fill the `TODO`s)
- `lessons/NN-name/NOTES.md` — reference card: Python → C++ mapping, traps, interview questions
- `common/check.hpp` — the tiny `CHECK()` test helper every lesson uses
- `interview/` — interview reps and mocks
- `STUDY-GUIDE.md` — principles in my own words
- `archive/` — the earlier Python-first attempt (kept for reference)

## Build + run a lesson (Windows, VS 2026, MSVC, C++20)
From the repo root in any PowerShell:
```
.\tools\run.ps1 01
```
It loads the MSVC environment itself, configures with the `debug` preset (Ninja), builds `lesson01`, runs it.
`-Release` builds optimized. Visual Studio / Rider: File > Open > Folder, pick the `debug` preset.
