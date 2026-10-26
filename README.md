# cpp-pipeline-lab — Capture++

Learning C++ for pipeline / mocap / Unreal tools roles by building **Capture++**: a C++ capture-data
pipeline (entity data model, database providers, events, threads, and a Qt 6 browser), taught lesson by
lesson with Claude.

**New here or picking back up? Open [START-HERE.md](START-HERE.md).**

## Layout
- `core/` — `capture_core` static library (no Qt). Headers in `core/include/capture/`, code in `core/src/`
- `apps/` — executables on top of core (CLI, Qt browser), added as the course reaches them
- `lessons/NN-name/` — `LESSON.md` (the lesson: steps, explanations, examples) + `main.cpp` (its tests)
- `common/check.h` — the tiny `CHECK()` test helper
- `interview/` — interview reps and mocks
- `STUDY-GUIDE.md` — principles in my own words
- `archive/` — the earlier Python-first attempt

## Build + run a lesson (Windows, VS 2026, MSVC, C++20)
From the repo root in any PowerShell:
```
.\tools\run.ps1 01
```
It loads the MSVC environment itself, configures the `debug` preset (Ninja), builds `capture_core` and
`lesson01`, and runs the tests. `-Release` builds optimized. Visual Studio / Rider: File > Open > Folder,
pick the `debug` preset.
