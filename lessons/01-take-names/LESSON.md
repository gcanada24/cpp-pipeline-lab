# Lesson 01 — Take names

**Time:** about 2–3 hours, in 8 short steps. Stop after any step; your progress is saved in the code.

## What you'll build
The first piece of Capture++: code that reads a capture file's name and pulls out its parts.

```
"D:/mocap/brawl/BRAWL_DAY2_T003_JDoe.fbx"
        │
        ▼  parse_take_name()
TakeName { shoot = "BRAWL_DAY2", take = 3, performer = "JDoe", extension = ".fbx" }
        │
        ▼  format_take_name()
"BRAWL_DAY2_T003_JDoe.fbx"
```

You've written this kind of code in Python many times. That's the point: the *problem* is familiar, so all
your attention can go on the *C++*.

## What you'll learn
1. How C++ code is split into header (`.h`) and source (`.cpp`) files, and how it becomes an `.exe`
2. `std::string` vs `std::string_view` — owning text vs looking at text
3. Searching and slicing strings
4. `std::optional` — returning "a value, or nothing" (C++'s answer to returning `None`)
5. Building a struct, and making structs comparable with `==`
6. Formatting strings with `std::format` (very close to Python f-strings)

## How each step works
Every step has the same four parts:
- **Learn** — the idea, compared to the Python you already know
- **Example** — complete working code for a *similar* problem, so you can see the idea in action
- **Your turn** — exactly which function to write, in which file
- **Run it** — the command, and what you should see when the step is done

The files you'll have open:

| File | What it is | You edit it? |
|---|---|---|
| `lessons/01-take-names/LESSON.md` | this guide | no — read it |
| `core/include/capture/naming/take_name.h` | the **header**: declares the functions and their rules | only in Step 6 |
| `core/src/naming/take_name.cpp` | the **source**: where you write the function bodies | **yes**, steps 2–7 |
| `lessons/01-take-names/main.cpp` | the tests | no |

---

## Step 0 — Run it once (5 min)

1. Open a PowerShell window in the repo folder (`Documents\Claude Files\cpp-pipeline-lab`).
2. Run:
   ```
   .\tools\run.ps1 01
   ```
3. You'll see some build lines, then a progress list like this:
   ```
   Progress
     [ ] Step 2: is_take_extension            4/7   <-- you are here
     [ ] Step 3: file_name                    2/6
     [ ] Step 4: parse_take_number            8/11
     ...
   Failing checks in "Step 2: is_take_extension":
     main.cpp(37): is_take_extension(".fbx")
   ```
   Some checks already "pass" because the empty starter functions return `false` or nothing, and some tests
   *expect* `false`. Ignore those numbers; work on one step at a time and watch it go to `[x]`.

**Done when:** you can run the command and see the progress list.

---

## Step 1 — How C++ code is organised (15 min)

### Learn
In Python, one `.py` file is both the "what exists" and the "how it works". C++ splits those into two files:

- **Header (`take_name.h`)** — *declarations*. It lists the functions with their names, inputs and output
  type, but no bodies. It's the menu.
- **Source (`take_name.cpp`)** — *definitions*. The function bodies. It's the kitchen.

Other code that wants to call your functions does `#include <capture/naming/take_name.h>`, which literally
pastes the header's text in. So the caller knows *what* exists, and the compiler can check every call has the
right types, without seeing *how* it works.

Building happens in two stages, and knowing which stage complained tells you where to look:

```
take_name.cpp ──compile──▶ take_name.obj ─┐
                                          ├──link──▶ lesson01.exe
main.cpp      ──compile──▶ main.obj ──────┘
```
- **Compiler** — turns one `.cpp` into machine code (`.obj`). Type errors, typos and missing `;` show up here.
  Error codes start with `C` (e.g. `error C2065: undeclared identifier`).
- **Linker** — joins all the `.obj` files into one `.exe`, connecting each call to its function body.
  If a function was declared but never defined, the linker can't connect it.
  Error codes start with `LNK` (e.g. `LNK2019: unresolved external symbol`). **You'll see these a lot in Unreal.**

Two more things you'll notice in the header:
- `#pragma once` — "only paste this file in once per `.cpp`", even if it's included twice.
- `namespace capture::naming { ... }` — a name prefix, like a Python package path. From outside, the full
  name is `capture::naming::parse_take_name`. The tests write `using namespace capture::naming;` so they can
  skip the prefix.

### Your turn
1. Open `take_name.h` and read it. For each function, find: its name, what it takes in, what it returns.
2. **Make a linker error on purpose.** In `take_name.cpp`, select the whole `is_take_extension` function
   (from `bool` to its closing `}`) and comment it out with `/* ... */`. Run `.\tools\run.ps1 01`.
   You should see `LNK2019: unresolved external symbol ... is_take_extension`. The compile worked; the link
   failed, because the header promised a function that doesn't exist.
3. Undo the comment, run again, and you're back to the progress list.

**Done when:** you've seen an `LNK` error and can say in one sentence why it was a link error, not a compile
error.

---

## Step 2 — Looking at text without copying it: `std::string_view` (20 min)

### Learn
C++ has two common ways to hold text:

| Type | What it is | Python comparison |
|---|---|---|
| `std::string` | **owns** its characters. It has its own copy in memory and frees it when done. | a `str` |
| `std::string_view` | **looks at** characters someone else owns. Just a pointer to the first character and a length. Making one costs nothing. | (no equivalent: Python always makes a new `str` when you slice) |

Why it matters: our functions only *read* a filename. Copying it would waste time and memory, which adds up
when you scan 50,000 files. So the inputs are `std::string_view`.

Things you can do with a `std::string_view sv`:
```cpp
sv.size()          // number of characters               (Python: len(s))
sv.empty()         // true if size() == 0                (Python: not s)
sv[0]              // first character, a char            (Python: s[0])
for (char c : sv)  // loop over every character          (Python: for c in s)
sv == ".fbx"       // compare with text
```

A `char` is a single character, written with **single** quotes: `'.'`. Double quotes `".fbx"` are text.

To lower-case one character, use `std::tolower` from `<cctype>`. It has an old C quirk: you must cast the
`char` to `unsigned char` first, or characters like `é` can crash it:
```cpp
char lower = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
```
`static_cast<T>(x)` means "convert x to type T"; it's C++'s explicit, searchable way to write a type
conversion (Python: `int(x)`, `str(x)`).

### Example
A different problem with the same tools: is the user's answer "y" or "yes", in any case?
```cpp
#include <cctype>
#include <string>
#include <string_view>

bool is_yes(std::string_view answer) {
    if (answer.size() != 1 && answer.size() != 3) {
        return false;                       // can't be "y" or "yes"
    }
    std::string lower;                      // a small owned copy we're allowed to change
    for (char c : answer) {
        lower += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return lower == "y" || lower == "yes";
}
// is_yes("YES") -> true    is_yes("Yeah") -> false    is_yes("") -> false
```
Notice that `answer` is a `string_view` (we only read it), but `lower` is a `std::string`, because we build
new text and need to own it.

### Your turn
In `take_name.cpp`, write the body of **`is_take_extension(std::string_view ext)`**.
Rules (also in the header): true only for `.fbx` or `.c3d`, ignoring upper and lower case. `"fbx"` without
the dot is false. Delete the `(void)ext;` line once you use `ext`.

### Run it
`.\tools\run.ps1 01` → `[x] Step 2: is_take_extension    7/7`

<details><summary>Stuck? Hint 1</summary>

Both valid extensions are exactly 4 characters long. What can you rule out before looking at any letters?
</details>
<details><summary>Hint 2</summary>

Follow the `is_yes` shape: check the size, build a lower-case copy, compare the copy with `".fbx"` and `".c3d"`.
</details>

---

## Step 3 — Searching and slicing (20 min)

### Learn
Searching and slicing a `string_view`:
```cpp
std::string_view sv = "D:/mocap/a.fbx";

sv.find('/')              // index of the FIRST '/'                     -> 2
sv.rfind('/')             // index of the LAST '/'                      -> 8
sv.find_last_of("/\\")    // index of the last character that is ANY of '/' or '\'
sv.substr(9)              // from index 9 to the end                    -> "a.fbx"
sv.substr(0, 8)           // from index 0, 8 characters                 -> "D:/mocap"
```

Three things that differ from Python:
1. **`substr(start, count)`** — the second number is *how many characters*, not an end index.
   Python `s[2:5]` is C++ `s.substr(2, 3)`.
2. **"Not found" is `std::string_view::npos`**, not `-1`. It's a special huge number. Always compare against
   it: `if (pos == std::string_view::npos)`.
3. **`"\\"` is one backslash.** In C++ text, `\` starts an escape (like `\n`), so a real backslash is written
   `\\`. Windows paths use `\`, other tools use `/`, so we look for either.

`substr` on a `string_view` gives another `string_view` into the *same* characters: still no copy.

**The one danger of `string_view`:** it doesn't own anything, so the text it looks at must still exist when
you use it. `file_name` below returns a view into the caller's `path`, which is safe because the caller still
owns `path`. Returning a view into a local `std::string` created *inside* your function would point at freed
memory. It compiles, then reads garbage. Interviewers love asking about this.

### Example
The opposite of what you'll write: get the *folder* part of a path.
```cpp
std::string_view folder_of(std::string_view path) {
    auto slash = path.find_last_of("/\\");     // last '/' or '\'
    if (slash == std::string_view::npos) {
        return {};                             // no slash: no folder. {} means "an empty value"
    }
    return path.substr(0, slash);              // from the start, `slash` characters long
}
// folder_of("D:/mocap/a.fbx") -> "D:/mocap"     folder_of("a.fbx") -> ""
```
`auto` lets the compiler work out the type from the right-hand side (here `size_t`, an unsigned integer for
sizes and positions). Use it when the type is obvious or long to write.

### Your turn
Write **`file_name(std::string_view path)`**: everything *after* the last `/` or `\`. If there's no slash, the
whole path is the file name. If the path ends with a slash, the result is empty.

### Run it
`[x] Step 3: file_name    6/6`

<details><summary>Hint 1</summary>

Start from `folder_of`. Which part of the path do you want instead, and where does it start relative to `slash`?
</details>
<details><summary>Hint 2</summary>

It starts at `slash + 1` and runs to the end: `path.substr(slash + 1)`. Handle the "not found" case first.
</details>

---

## Step 4 — "A value, or nothing": `std::optional` (25 min)

### Learn
In Python, a parse function that fails either returns `None` or raises. In C++, the cleanest equivalent of
"returns an int or None" is **`std::optional<int>`**: a box that holds either an `int` or nothing.

```cpp
std::optional<int> a = 42;            // holds 42
std::optional<int> b = std::nullopt;  // holds nothing (like None)

if (a) { ... }                        // true if it holds a value        (Python: if a is not None)
*a                                    // the value inside -> 42
a.value_or(0)                         // the value, or 0 if empty         (Python: a if a is not None else 0)
```
In a function returning `std::optional<int>`, you write `return 42;` for success and `return std::nullopt;`
for failure. Only use `*a` after checking `if (a)`: reading an empty optional with `*` is undefined
behaviour, meaning the program can do anything, including appearing to work.

To turn digits into a number, use **`std::from_chars`** (from `<charconv>`). It's the C++ version of `int("42")`,
but it doesn't throw. Instead it tells you what happened:
```cpp
std::string_view digits = "042";
int value = 0;
auto [ptr, ec] = std::from_chars(digits.data(), digits.data() + digits.size(), value);
```
- You give it the start and end of the characters (`.data()` is a pointer to the first character).
- It writes the number into `value`.
- `auto [ptr, ec] = ...` unpacks the two results, like Python's `ptr, ec = f()`:
  - `ec` is the error code. `ec == std::errc{}` means success. It fails on no digits, or a number too big for an `int`.
  - `ptr` points to where it **stopped** reading. For `"12a"` it reads `12`, stops at `a`, and still says success!
    So you must also check `ptr == digits.data() + digits.size()` ("did it read everything?").

### Example
Parse a frame rate like `"120fps"`:
```cpp
#include <charconv>
#include <optional>
#include <string_view>

std::optional<int> parse_fps(std::string_view text) {
    if (!text.ends_with("fps")) {                       // C++20; there's also starts_with
        return std::nullopt;
    }
    std::string_view digits = text.substr(0, text.size() - 3);   // drop "fps"
    int value = 0;
    auto [ptr, ec] = std::from_chars(digits.data(), digits.data() + digits.size(), value);
    if (ec != std::errc{} || ptr != digits.data() + digits.size()) {
        return std::nullopt;                            // not a number, or junk after it
    }
    return value;
}
// parse_fps("120fps") -> 120     parse_fps("12xfps") -> nothing     parse_fps("fps") -> nothing
```

### Your turn
Write **`parse_take_number(std::string_view token)`**. Rules: the first character must be `'T'` (capital),
everything after it must be digits only, and the number must be 1 or more. `"T003"` gives `3`.

### Run it
`[x] Step 4: parse_take_number    11/11`

<details><summary>Hint 1</summary>

Check it starts with `'T'` (`token.starts_with('T')`), then take `token.substr(1)` as the digits and follow `parse_fps`.
</details>
<details><summary>Hint 2</summary>

`from_chars` already rejects an empty string and numbers too big for an `int`. After it succeeds, there's one more rule left to check.
</details>

---

## Step 5 — Putting it together: `parse_take_name` (30 min)

### Learn
**Structs.** `TakeName` is a `struct`: a group of named fields, like a Python `@dataclass`. You can create
one by listing the fields in order inside braces:
```cpp
TakeName t{"BRAWL", 3, "JDoe", ".fbx", "D:/mocap/BRAWL_T003_JDoe.fbx"};
t.shoot      // "BRAWL"
```

**Turning a view into owned text.** The struct's fields are `std::string` (the record must own its text,
because it will outlive the filename it came from). A `string_view` doesn't turn into a `std::string` by
itself; you ask for the copy explicitly: `std::string(some_view)`. C++ makes copies visible on purpose.

**Early return.** Check one rule at a time and `return std::nullopt;` as soon as one fails. The last line
only runs when everything passed. Same habit as Python guard clauses.

**Finding the second-to-last `_`.** `rfind` takes an optional second argument: where to start searching
backwards. `name.rfind('_', pos - 1)` finds the last `_` *before* position `pos`.

### Example
Same shape, different problem: parse `"fps=120"` into a setting.
```cpp
struct Setting {
    std::string key;
    std::string value;
};

std::optional<Setting> parse_setting(std::string_view line) {
    auto eq = line.find('=');
    if (eq == std::string_view::npos) {
        return std::nullopt;                          // no '='
    }
    std::string_view key = line.substr(0, eq);
    std::string_view value = line.substr(eq + 1);
    if (key.empty() || value.empty()) {
        return std::nullopt;
    }
    return Setting{std::string(key), std::string(value)};   // copy into owned strings once, at the end
}
```

### Your turn
Write **`parse_take_name(std::string_view path)`** using your step 2–4 functions. A plan:
1. `name = file_name(path)`
2. Find the last `.` → split into the base name and the extension (`".fbx"`). Check the extension.
3. In the base name (`"BRAWL_DAY2_T003_JDoe"`), find the **last** `_` and the one **before** it.
4. Slice out the three parts: shoot / take token / performer. Check shoot and performer aren't empty, and
   parse the take token.
5. Return the `TakeName`, with `path` set to the full path you were given.

### Run it
`[x] Step 5: parse_take_name    12/12`

<details><summary>Hint 1</summary>

With `last = base.rfind('_')`, the previous one is `base.rfind('_', last - 1)`. What if `last` is `npos`, or `0`?
</details>
<details><summary>Hint 2</summary>

With positions `first` and `last` of the two underscores: shoot = `base.substr(0, first)`, token =
`base.substr(first + 1, last - first - 1)`, performer = `base.substr(last + 1)`.
</details>

---

## Step 6 — Comparing structs: `operator==` (10 min)

### Learn
A Python `@dataclass` gets `__eq__` for free, so two records with the same fields are equal. A C++ struct
does **not** get `==` automatically: `a == b` won't compile. You ask for it with one line inside the struct:
```cpp
struct Vec3 {
    float x, y, z;
    bool operator==(const Vec3&) const = default;   // compare every field, in order
};
```
Reading that line left to right:
- `bool operator==` — "I'm defining what `==` means for this type; it returns a bool."
- `(const Vec3&)` — the other object, passed by **reference** (no copy) and **const** (we promise not to change it).
- the `const` after the `)` — calling `==` doesn't change *this* object either.
- `= default` — "compiler, write the obvious version: compare every field." C++20 then gives you `!=` for free too.

What the compiler *does* write for every struct without being asked: creating it, copying it, moving it,
assigning it, destroying it. Not `==`. "What does the compiler generate for a class?" is a classic interview
question.

### Your turn
In **`take_name.h`**, replace the `// STEP 6` comment inside `TakeName` with the `operator==` line for `TakeName`.

### Run it
`[x] Step 6: operator==    4/4`

---

## Step 7 — Building strings: `std::format` (10 min)

### Learn
`std::format` (C++20, `#include <format>`) uses the same mini-language as Python's `str.format` and f-strings:
```cpp
std::format("{} has {} takes", "BRAWL", 12)      // "BRAWL has 12 takes"
std::format("{:03}", 7)                          // "007"   (pad with zeros to width 3)
std::format("{:.2f}", 59.9401)                   // "59.94"
```
Python: `f"{n:03}"` → C++: `std::format("{:03}", n)`.

### Example
```cpp
std::string asset_file(std::string_view asset, int version) {
    return std::format("{}_v{:03}.ma", asset, version);
}
// asset_file("hero", 7) -> "hero_v007.ma"     asset_file("hero", 1200) -> "hero_v1200.ma"
```

### Your turn
Add `#include <format>` at the top of `take_name.cpp`, then write **`format_take_name(const TakeName& name)`**:
`SHOOT_T###_PERFORMER.ext`, with the take padded to at least 3 digits. No folders.

### Run it
All steps `[x]`, and: `All steps pass. Say "check" in chat for a review.`

---

## Wrap-up

### What you can now say in an interview
- **Header vs source:** headers declare, `.cpp` files define. `#include` pastes text. The compiler handles one
  `.cpp` at a time; the linker joins them, and `LNK2019` means something was declared but never defined.
- **`string` vs `string_view`:** a string owns its text; a view just looks at someone else's. Views make reading
  and slicing free, but must never outlive the text they point into.
- **`std::optional`:** the type says "this might be empty", and the caller has to check. Use it when failure is
  normal (a badly named file); throw an exception when failure is truly unexpected.
- **Free struct operations:** construct, copy, move, assign, destroy. Not `==`: add `= default` for it.
- **Undefined behaviour:** code that breaks a language rule (reading an empty optional with `*`, a dangling
  `string_view`, overflowing an `int`). It may seem to work, then fail somewhere else.

### Quick quiz (answers hidden)
<details><summary>1. A function is declared in a header but has no body. Compile error or link error?</summary>

Link error (`LNK2019`). Each `.cpp` compiles fine against the declaration; the linker can't find the body.
</details>
<details><summary>2. Why is this function dangerous? <code>std::string_view f() { std::string s = "abc"; return s; }</code></summary>

The view points into `s`, which is destroyed when `f` returns. The caller gets a dangling view.
</details>
<details><summary>3. <code>std::from_chars</code> on "12a" — what does it return?</summary>

Success, with value 12, and `ptr` pointing at `a`. You must also check that `ptr` reached the end.
</details>
<details><summary>4. Python <code>s[3:7]</code> in C++?</summary>

`s.substr(3, 4)`: start 3, count 4.
</details>

### Cheat sheet
| Python | C++ |
|---|---|
| `len(s)` / `not s` | `s.size()` / `s.empty()` |
| `s.rfind("_")`, returns -1 | `s.rfind('_')`, returns `std::string_view::npos` |
| `s[a:b]` | `s.substr(a, b - a)` |
| `s.startswith("T")` | `s.starts_with('T')` (C++20) |
| `int("42")` | `std::from_chars(...)` + check `ec` and `ptr` |
| `None` | `std::nullopt` in a `std::optional<T>` |
| `@dataclass` | `struct` + `bool operator==(const T&) const = default;` |
| `f"{n:03}"` | `std::format("{:03}", n)` |
