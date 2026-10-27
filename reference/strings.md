# Strings: `std::string` and `std::string_view`

The one idea that matters most here, plus the functions you'll reach for and where to look up the rest.

---

## 1. The core idea: owning vs looking

| | `std::string` | `std::string_view` |
|---|---|---|
| Holds | the characters themselves | an **address** and a **length**, never the characters |
| Analogy | the take file on the server | a file path written in your notes |
| Copying it | copies all the characters | copies two numbers (free) |
| Keeps the text alive? | yes | **no** |
| Use it for | text you **create** or need to keep | text you only **read**, and someone else owns |
| Unreal | `FString` | `FStringView` |
| Python | `str` | (nothing like it: Python never lets text die while something points at it) |

```
std::string name = ".fbx";            std::string_view view = name;

  name                                  view
  ┌──────────────┐                      ┌───────────────────────┐
  │ . f b x      │ ◀─────────────────── │ address of name's text │
  └──────────────┘                      │ length 4               │
                                        └───────────────────────┘
```

## 2. The lifetime rule (the bug you hit twice)

> A `string_view` is only valid while the text it points at is still alive.

**Bug A: viewing a temporary.** The returned string has no owner, so it's destroyed at the `;`:
```cpp
std::string_view lowered = to_lower_copy(ext);   // WRONG: dangling at the ;
std::string lowered = to_lower_copy(ext);        // right: lowered owns the new text
```

**Bug B: returning a view of a local.** The local is destroyed at the function's closing `}`:
```cpp
std::string_view first_word(std::string_view text) {
    std::string copy(text);
    return copy;                       // WRONG: copy dies at }, the caller gets a dangling view
}

std::string_view first_word(std::string_view text) {
    return text.substr(0, text.find(' '));   // right: a slice of the caller's text, which is still alive
}
```

**Questions to ask yourself every time:**
1. Is this function *creating* new text? Then it needs a `std::string` to hold it.
2. Am I returning a `string_view`? Then it must point into text the **caller** owns, usually a slice of a
   parameter.
3. Does it print right inside the function but come out wrong outside? That's the classic sign of a dangling
   view. A print just before `return` runs while the text is still alive.

Dangling views are **undefined behavior**: sometimes it looks fine, sometimes garbage, sometimes a crash. The
compiler usually won't warn you.

## 3. `string_view` members, with Python next to them

Every one of these works on `std::string` too. "Returns a view" means no copy.

**Size and characters**
| Python | C++ | Notes |
|---|---|---|
| `len(s)` | `s.size()` | returns `std::size_t` (an unsigned whole number) |
| `not s` | `s.empty()` | |
| `s[0]` | `s[0]` | a `char`. No negative indexes: `s[-1]` is a bug |
| `s[-1]` | `s.back()` | last character. Check `!s.empty()` first |
| `s[0]` | `s.front()` | first character. Check `!s.empty()` first |
| `for c in s:` | `for (char c : s)` | |

**Checking**
| Python | C++ | Notes |
|---|---|---|
| `s.startswith("T")` | `s.starts_with("T")` or `s.starts_with('T')` | C++20 |
| `s.endswith(".fbx")` | `s.ends_with(".fbx")` | C++20 |
| `"_" in s` | `s.find('_') != std::string_view::npos` | (`s.contains('_')` is C++23, not available yet) |
| `s == "x"` | `s == "x"` | compares the characters |

**Searching** (all return an index, or `std::string_view::npos` if not found)
| Python | C++ | Finds |
|---|---|---|
| `s.find("_")` | `s.find('_')` | first `'_'` |
| `s.find("_", 5)` | `s.find('_', 5)` | first `'_'` at or after index 5 |
| `s.rfind("_")` | `s.rfind('_')` | last `'_'` |
| (no direct match) | `s.find_first_of("/\\")` | first character that is **any** of `/` or `\` |
| (no direct match) | `s.find_last_of("/\\")` | last character that is any of them |
| (no direct match) | `s.find_first_not_of(' ')` | first character that is **not** a space |

**Slicing** (returns a view into the same characters)
| Python | C++ | Notes |
|---|---|---|
| `s[i:]` | `s.substr(i)` | from `i` to the end |
| `s[i:i+n]` | `s.substr(i, n)` | **the second number is a count, not an end index** |
| `s[a:b]` | `s.substr(a, b - a)` | |
| `s[1:]` (in place) | `s.remove_prefix(1)` | changes the view itself, not the text |
| `s[:-4]` (in place) | `s.remove_suffix(4)` | changes the view itself, not the text |

`substr(i)` with `i` bigger than `size()` throws an exception. `i == size()` is fine and gives an empty view.

## 4. `npos`: C++'s "not found"

Python's `find` returns `-1`. C++ returns `std::string_view::npos`, a huge number (the largest `size_t`).
Always compare against it by name:
```cpp
std::size_t underscore = name.find('_');
if (underscore == std::string_view::npos) {
    // not found
}
```
Never write `if (underscore == -1)` or `if (underscore < 0)`. `size_t` can't be negative, so the second one
is always false.

## 5. Things only `std::string` can do (it owns the text, so it can change it)

| Python | C++ |
|---|---|
| `s = s + "x"` / `s += "x"` | `s += "x";` or `s.append("x");` |
| `s += c` (one char) | `s.push_back(c);` |
| `s = ""` | `s.clear();` |
| (pre-size a list) | `s.reserve(n);` (allocates space once, up front) |
| `s.replace("a", "b")` | no one-line equivalent: loop with `find` + `replace(pos, count, new_text)` |
| `s.upper()` / `s.lower()` | no member: loop over the characters with `std::toupper` / `std::tolower` |

## 6. Converting between them

```cpp
std::string owned = "BRAWL_T003_JDoe.fbx";

std::string_view view = owned;          // string -> view: free, and automatic
std::string copy(view);                 // view -> string: makes a copy (explicit on purpose)
std::string copy2 = std::string(view);  // same thing
```
- **String → view** happens automatically, which is why you can pass a `std::string` to any function that
  takes a `string_view`.
- **View → string** is never automatic, because it allocates and copies. C++ makes you write it so you
  can see the cost.
- **Number → text:** `std::to_string(42)` or `std::format("{}", 42)`. **Text → number:** `std::from_chars`
  (Lesson 01 Step 4).

## 7. Looking things up yourself

**cppreference.com** is the reference everybody uses, the C++ version of Python's docs.
- `string_view`: search "cppreference string_view" →
  `https://en.cppreference.com/w/cpp/string/basic_string_view`
- `string`: `https://en.cppreference.com/w/cpp/string/basic_string`
- How to read a page:
  - The **Member functions** table lists everything you can call with a `.`
  - **Non-member functions** are things like `operator==` and `operator<<`
  - Click a function: the **Return value** section says what comes back, and the **Example** at the bottom
    is code you can run
  - "(C++20)" or "(C++23)" next to a name means which version added it. **We're on C++20**, so anything
    marked C++23 won't compile yet
  - Top of every page: **"Defined in header `<...>`"** tells you what to `#include`

**In Visual Studio or Rider:**
- Type a variable name and `.` to get the list of members. **Ctrl+Space** brings it back.
- Hover over a function to see what it takes and returns.
- **F12** (Go to Definition) opens the standard library's own header. It's hard to read, so use cppreference.

**Unreal (later):** the same ideas have different names. `FString::Len()`, `Mid()`, `Find()`,
`FindLastChar()`, `StartsWith()`, `FPaths::GetCleanFilename()`. The docs are at dev.epicgames.com. Same
owner-vs-view question, different spelling.
