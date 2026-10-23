# cpp-pipeline-lab

Daily C++ study toward Unreal Engine pipeline tools. Each week's build is written in Python first, then
ported to C++ by hand, with a note on every decision C++ forced (types, ownership, copies, failure, cost).

Build theme: a catalog of motion capture takes (shoots, performers, files), growing into a small asset
pipeline and, later, Unreal editor tools.

## Layout
- `weekNN/python/` — the Python version of that week's build
- `weekNN/cpp/` — the C++ port
- `weekNN/NOTES.md` — forced decisions and what the Python runtime was doing for me
- `interview/` — daily interview reps and Friday mocks, with reviews
- `STUDY-GUIDE.md` — core principles in my own words

## Build (Windows, VS 2022, C++20)
```
cmake -S . -B build
cmake --build build
```
