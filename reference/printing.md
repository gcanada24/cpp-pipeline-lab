# Printing to the console

How to see what your code is doing while it runs: the C++ version of dropping `print()` calls into Python.
We're on C++20, so everything here uses `std::cout` and `std::format`. (C++23 adds `std::println`, which is
closer to Python. We'll switch when the course moves to C++23.)

---

## 1. The basics: `std::cout`

```cpp
#include <iostream>   // needed for std::cout

std::cout << "hello" << '\n';
```

| Python | C++ |
|---|---|
| `print("hello")` | `std::cout << "hello" << '\n';` |
| `print("take", take)` | `std::cout << "take " << take << '\n';` |
| `print(a, b, sep="")` | `std::cout << a << b << '\n';` |

- `std::cout` is the console ("character output"). `<<` means "send this to it". Chain as many as you like.
- **No automatic spaces and no automatic newline.** Python's `print` adds both, so you add them yourself:
  `" "` between values, `'\n'` at the end.
- `'\n'` vs `std::endl`: both end the line. `std::endl` also forces the output to the screen right away, which
  is slower. Use `'\n'`. (See section 8 for when output seems to be missing.)

## 2. Printing the types from Lesson 01

```cpp
std::string_view ext = ".fbx";
std::string shoot = "BRAWL_DAY2";
int take = 3;
char c = 'F';
bool found = true;

std::cout << "ext: " << ext << '\n';        // ext: .fbx
std::cout << "shoot: " << shoot << '\n';    // shoot: BRAWL_DAY2
std::cout << "take: " << take << '\n';      // take: 3
std::cout << "c: " << c << '\n';            // c: F
std::cout << "size: " << ext.size() << '\n';  // size: 4
```

**`bool` prints as `1` or `0`**, not `True`/`False`. To get words, send `std::boolalpha` once:
```cpp
std::cout << "found: " << found << '\n';                    // found: 1
std::cout << std::boolalpha << "found: " << found << '\n';  // found: true   (stays on after this)
```

**A `char` prints as the letter.** To see its number (handy when checking upper vs lower case), convert it:
```cpp
std::cout << c << " is " << static_cast<int>(c) << '\n';    // F is 70
```

## 3. Put brackets around text

When you're slicing strings, the bugs are usually "one character too many", a space, or text that's empty.
None of those show up in plain output. Brackets make the edges visible:
```cpp
std::string_view part = "";
std::cout << "part: [" << part << "]\n";    // part: []      <- clearly empty
std::cout << "part: " << part << '\n';      // part:         <- empty, or did it not print?
```
Make this a habit for every `string_view` you print.

## 4. `std::format`: f-strings for C++

When a line has several values, `std::format` is easier to read than a long `<<` chain. `{}` works like
Python's `{}` in an f-string, except the values come after the text instead of inside it:
```cpp
#include <format>

std::cout << std::format("shoot=[{}] take={} ext=[{}]\n", shoot, take, ext);
// shoot=[BRAWL_DAY2] take=3 ext=[.fbx]
```

| Python f-string | `std::format` | Output |
|---|---|---|
| `f"{take}"` | `std::format("{}", take)` | `3` |
| `f"{take:03}"` | `std::format("{:03}", take)` | `003` |
| `f"{name:>10}"` | `std::format("{:>10}", name)` | `      JDoe` |
| `f"{name:<10}\|"` | `std::format("{:<10}\|", name)` | `JDoe      \|` |
| `f"{found}"` | `std::format("{}", found)` | `true` (`format` prints bools as words) |

Values fill the `{}` in order. If the number of `{}` doesn't match the number of values, it's a **compile
error**. Python only finds that mistake when the line runs.

## 5. Printing an `optional`: check it first

You can't print a `std::optional` directly. Try it and you get a compile error, `C2678: binary '<<': no operator found`.
Check first, the same way you'd check for `None`:
```cpp
std::optional<int> number = std::nullopt;   // or: std::optional<int> number = 3;

if (number.has_value()) {
    std::cout << "number: " << number.value() << '\n';
} else {
    std::cout << "number: (nothing)\n";
}
```
Python version: `print("number:", number if number is not None else "(nothing)")`.

## 6. Printing a struct: write a small helper

C++ doesn't know how to print your structs (Python gives you a default `repr`; C++ doesn't). Write a helper
function that prints each field. Put it in the unnamed `namespace { }` at the top of your `.cpp`, next to your
other helpers:
```cpp
struct Marker {
    std::string name;
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
};

namespace {
void print_marker(const Marker& marker) {
    std::cout << std::format("Marker{{ name=[{}] x={} y={} z={} }}\n", marker.name, marker.x, marker.y,
                             marker.z);
}
}  // namespace

// print_marker(Marker{"LFHD", 1.5, 0.0, 172.25});
// Marker{ name=[LFHD] x=1.5 y=0 z=172.25 }
```
- `const Marker&` means "look at the caller's struct, don't copy it, don't change it". You'll see this
  everywhere; Lesson 01 Step 7 uses it too.
- `{{` and `}}` print a literal `{` and `}` inside `std::format`, same as an f-string.
- `TakeName` works the same way: one helper, one `{}` per field.

## 7. Tracing the flow through a function

The pattern: print what came **in**, print **each decision**, print what goes **out**. Shown here on the
`is_valid_unit` example from Lesson 01 Step 2:
```cpp
bool is_valid_unit(std::string_view unit) {
    std::cout << "is_valid_unit(\"" << unit << "\")\n";               // what came in

    for (std::string_view valid : kValidUnits) {
        std::cout << "  compare [" << unit << "] to [" << valid << "]\n";   // each step of the loop
        if (unit == valid) {
            std::cout << "  -> true\n";                                // the way out
            return true;
        }
    }
    std::cout << "  -> false (no match)\n";                            // the other way out
    return false;
}
```
Output for `is_valid_unit("mm")`:
```
is_valid_unit("mm")
  compare [mm] to [cm]
  compare [mm] to [m]
  compare [mm] to [mm]
  -> true
```
- Indent the inner lines with two spaces. When one function calls another (Step 5 calls Steps 2–4), the
  indentation shows who called whom.
- **Print before every `return`.** A function with three exits gets three prints, so you always know which
  door it left by.
- `\"` puts a `"` inside a string, so the output looks like the call you'd write.

## 8. Printing in the lesson tests

**Where your prints show up:** above the progress list. The tests run first and the list prints at the end.

**The tests call your function many times**, so one print line becomes a wall of output. Print for one input
only:
```cpp
if (unit == "mm") {
    std::cout << "unit is mm, size " << unit.size() << '\n';
}
```

**Output seems to be missing, or comes out in a strange order?** That happens if the program crashes before
the output reaches the screen. Use `std::cerr` instead of `std::cout`: it goes to the screen immediately.
```cpp
std::cerr << "got here\n";
```

**Turning prints off without deleting them.** One switch at the top of the `.cpp`:
```cpp
namespace {
constexpr bool kDebugPrint = true;   // set to false to silence every debug line
}

if (kDebugPrint) {
    std::cout << "unit: [" << unit << "]\n";
}
```
When you set it to `false`, every `if (kDebugPrint)` block is skipped. Before you say "check", remove your
prints or set the switch to `false`. Debug prints in finished code count against you in a code review.

## 9. Errors you'll hit

| What you see | Why | Fix |
|---|---|---|
| `error C2039: 'cout': is not a member of 'std'` | forgot the include | `#include <iostream>` |
| `error C2039: 'format': is not a member of 'std'` | forgot the include | `#include <format>` |
| `error C2678: binary '<<': no operator found` (notes mention `std::optional<int>`) | printed an `optional` | section 5 |
| `error C2678: binary '<<': no operator found` (notes mention `Marker`) | printed a struct | section 6 |
| Compile error pointing into `<format>` | `{}` count doesn't match the values | count the `{}` |
| `true` shows as `1` | `bool` with `<<` | `std::boolalpha`, or use `std::format` |

## Printing vs the debugger
Printing shows you the **story** of a whole run: every call, in order. The debugger (VS **F5**, breakpoint,
**F10** to step) **freezes** one moment so you can look at every variable. Use prints to find roughly where
things go wrong, then a breakpoint to look closely at that spot. Unreal developers use both all day: prints
become `UE_LOG`.
