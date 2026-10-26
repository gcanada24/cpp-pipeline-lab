# Lesson 01 — Take names (reference card)

**Capture++ piece:** `capture::naming` — parse `BRAWL_DAY2_T003_JDoe.fbx` into a `TakeName`, and build
filenames back from one. Later the ingest step (Lesson 17) turns every parsed file into a Take entity.
Python ancestor: the naming / path-generation helpers in `core/utilities` (shot name curation, `modelTools`).

## Where the code goes
| File | Role | Python analogy |
|---|---|---|
| `core/include/capture/naming/take_name.hpp` | **declarations**: the struct and function signatures | the "interface" of a module |
| `core/src/naming/take_name.cpp` | **definitions**: function bodies | the module's code |
| `lessons/01-take-names/main.cpp` | tests, links against `capture_core` | `unit_test/...UnitTest.py` |

## How it becomes an .exe
**preprocessor** (pastes each `#include`d file in) → **compiler** (one .cpp → one .obj; type errors here)
→ **linker** (joins .obj files + `capture_core.lib` into `lesson01.exe`). Delete a function body from the
.cpp and the compile still succeeds — the **link** fails with `unresolved external symbol`. Python has no
link step; this is the error class you'll see most in Unreal.

- `#pragma once` — include this header at most once per .cpp (stops duplicate-definition errors).
- `namespace capture::naming { }` — like a Python package path; call as `capture::naming::parse_take_name`.
- A default argument or a doc comment goes in the **header**; the .cpp just repeats the signature.

## Python → C++ for this lesson
| Python | C++ | Note |
|---|---|---|
| `@dataclass` | `struct` | public fields; aggregate init `TakeName{"BRAWL", 3, "JDoe", ".fbx", path}` |
| dataclass `__eq__` | `bool operator==(const TakeName&) const = default;` | not generated unless you ask; `!=` comes free in C++20 |
| `str` | `std::string` | owns its characters (a heap copy) |
| slicing a `str` | `std::string_view` | pointer + length into someone else's chars; no copy, no ownership |
| `s.rfind("_")` | `sv.rfind('_')` | returns `std::string_view::npos` when not found (not -1) |
| `s[a:b]` | `sv.substr(a, count)` | 2nd argument is a **count**, not an end index |
| `None` / `raise` | `std::optional<T>` / `std::nullopt` | "a T or nothing"; check before use |
| `int("007")` | `std::from_chars(first, last, value)` | no throw, no allocation; check `ec` **and** that `ptr == last` |
| `s.lower()` | `std::tolower(static_cast<unsigned char>(c))` per char | the cast matters (UB for negative chars) |
| `f"{n:03d}"` | `std::format("{:03}", n)` (`<format>`) | C++20; same mini-language as Python |

## Traps
- A `string_view` must never outlive the string it points into. Returning a view into a local
  `std::string` compiles fine and reads garbage (a dangling reference).
- `*opt` on an empty optional is undefined behaviour; `opt.value()` throws instead.
- Overflowing an `int` is undefined behaviour; `from_chars` reports `std::errc::result_out_of_range`.

## Interview questions
1. Declaration vs definition? What goes in a header, what goes in a .cpp, and why?
2. Compile error vs link error — one cause of each.
3. `std::string` vs `std::string_view` — when is a `string_view` dangerous?
4. What does the compiler generate for a struct for free? (ctor/dtor/copy/move/assign — not `==`.)
5. `std::optional` vs throwing an exception — when would you pick each?
6. What is undefined behaviour? Two examples from this lesson.
