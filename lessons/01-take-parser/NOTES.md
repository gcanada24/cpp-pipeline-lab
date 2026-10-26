# Lesson 01 — reference card

## How a C++ program becomes an .exe
`main.cpp` → **preprocessor** (pastes in every `#include`, expands macros) → **compiler** (`cl`, turns one
.cpp into an .obj of machine code; all type errors happen here) → **linker** (joins .obj files and libraries
into `lesson01.exe`; "unresolved external symbol" errors happen here). Python skips all of this.

## Python → C++ for this lesson
| Python | C++ | Note |
|---|---|---|
| `@dataclass` | `struct` | public fields, aggregate init: `TakeRecord{"BRAWL", 3, "JDoe"}` |
| dataclass `__eq__` | `bool operator==(const T&) const = default;` | C++ does **not** generate `==` unless you ask |
| `str` | `std::string` | owns its characters (heap copy) |
| a slice of a `str` | `std::string_view` | pointer + length into someone else's chars; **no copy, no ownership** |
| `s.rfind("_")` | `sv.rfind('_')` | returns `std::string_view::npos` when not found (not -1) |
| `s[a:b]` | `sv.substr(a, count)` | 2nd arg is a **count**, not an end index |
| `None` / raise | `std::optional<T>` / `std::nullopt` | "a T or nothing" — check before use |
| `int("007")` | `std::from_chars(first, last, value)` | no exceptions, no allocation; check `ec` **and** `ptr == last` |
| `s.lower()` | `std::tolower((unsigned char)c)` per char | cast to `unsigned char` first (UB otherwise) |

## Traps
- A `string_view` must never outlive the string it points into. Returning a view into a local
  `std::string` is a dangling reference: compiles fine, reads garbage.
- `optional<T>::operator*` on an empty optional is undefined behaviour. `.value()` throws instead.
- `int` overflow is undefined behaviour; `from_chars` reports `errc::result_out_of_range` instead.

## Interview questions
1. What's the difference between `std::string` and `std::string_view`? When is a `string_view` dangerous?
2. What does the compiler generate for a `struct` for free? (ctor/dtor/copy/move/assign — not `==`.)
3. Compile error vs link error — give one cause of each.
4. Why might you return `std::optional` instead of throwing? When would you still throw?
5. What is undefined behaviour? Name two examples from this lesson.
