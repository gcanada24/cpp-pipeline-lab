# Lesson 02 — Take details: status, timecode and IDs

**Time:** about 3½ hours, in 10 short steps. Stop after any step; your progress is saved in the code.

## What you'll build
Three small types that every take in Capture++ will carry, starting in Lesson 03:

```
Take "BRAWL_T003_JDoe"
  ├── id      Uuid          "3f2a9c1e-77b0-4d1a-9e2f-0b6c5d4e3a21"   which take, exactly (Steps 7-9)
  ├── status  CircleStatus  CircleA                                   did the director circle it? (Steps 1-3)
  └── start   Timecode      01:02:03:04 @ 30 fps                      when it was recorded (Steps 4-6)
```

In Python these were an `Enum`, a timecode string and `uuid4().int`. Here each one becomes a proper C++ type
of its own, so the compiler can stop you from mixing them up.

## What's new in this lesson: you write parts of the headers
In Lesson 01 the header was finished for you and you wrote the function bodies. This time the headers have a
**spec** in comments (what each piece must do), and **you write the declarations** as well as the bodies.
That's the real job: deciding what a type looks like from the outside. The tests tell you if something is
missing or declared wrong.

## What you'll learn
1. `enum class` — a fixed set of named values (Python's `Enum`), and `switch`
2. `class` — private data, public functions, constructors
3. `const` member functions — "calling this doesn't change the object"
4. Operator overloading — making `+`, `-` and `==` work on your own types (Python's `__add__`, `__sub__`, `__eq__`)
5. `constexpr` constants, integer division and `%`
6. `static` member functions and `<random>` — making new random IDs

| File | What it is | You edit it? |
|---|---|---|
| `lessons/02-take-details/LESSON.md` | this guide | no — read it |
| `core/include/capture/model/circle_status.h` + `core/src/model/circle_status.cpp` | `CircleStatus` | **yes**, steps 1–3 |
| `core/include/capture/model/timecode.h` + `core/src/model/timecode.cpp` | `Timecode` | **yes**, steps 4–6 |
| `core/include/capture/model/uuid.h` + `core/src/model/uuid.cpp` | `Uuid` | **yes**, steps 7–9 |
| `lessons/02-take-details/main.cpp` | the tests | no |

Your reference notes are in [`reference/`](../../reference/): printing and strings so far.

---

## Step 0 — Run it once (5 min)

Run the lesson: in Visual Studio / Rider pick **lesson02** as the startup item, or in PowerShell run
`.\tools\run.ps1 02`. You should see:
```
Progress
  [ ] Step 1: CircleStatus values             0/1   <-- you are here
  [ ] Step 2: to_string(CircleStatus)         0/1
  ...
Failing checks in "Step 1: CircleStatus values":
  CircleStatus is missing values: add all six from the spec in circle_status.h
```
Each step shows a plain message like that until the pieces it needs exist, then the real checks run.

**Done when:** you see the progress list for lesson02.

---

## Step 1 — A fixed set of values: `enum class` (15 min)

### Learn
Your Python version:
```python
class CircleStatus(Enum):
    NOT_CAPTURED = 0
    PENDING = 1
    ...
status = CircleStatus.PENDING
```
The C++ version is an **`enum class`**:
```cpp
enum class CameraKind {
    Optical = 0,
    Inertial = 1,
    Video = 2,
};

CameraKind kind = CameraKind::Optical;   // Python: kind = CameraKind.OPTICAL
if (kind == CameraKind::Video) { ... }
```
- The values are written with `::`, like a namespace: `CameraKind::Optical`.
- Naming style: types and enum values in `PascalCase` is common in modern C++ (and Unreal). `ALL_CAPS` is
  usually kept for macros.
- `= 0`, `= 1`... set each value's number. Without them C++ numbers them 0, 1, 2... in order, but writing them
  out makes it clear the numbers matter (they'll be saved to a database later).

**Why `enum class` and not plain `enum`?** C++ also has an older plain `enum`, inherited from C:
```cpp
enum OldKind { Optical, Inertial };   // plain enum: don't use for new code
int n = Optical;        // compiles: silently turns into the number 0
bool odd = (Optical == 0);  // compiles: compares a camera kind with a number
```
Its names leak into the surrounding code (`Optical` on its own, with no `OldKind::`), and it turns into an `int`
without being asked. An `enum class` does neither: `CameraKind::Optical == 0` is a compile error. That's the
point: a status is a status, not a number. When you really want the number, you ask for it:
```cpp
int number = static_cast<int>(CameraKind::Inertial);   // 1
```

### Your turn
Open **`circle_status.h`** and write the six values inside `enum class CircleStatus { };`, with the names
and numbers from the spec comment above it.

### Run it
`[x] Step 1: CircleStatus values    6/6`

<details><summary>Hint 1</summary>

One value per line, `Name = number,`. A comma after the last one is allowed (and makes adding a value later a
one-line change).
</details>

---

## Step 2 — Choosing by value: `switch` (20 min)

### Learn
To turn a status into its text name, Python might use a dict or `match`:
```python
match status:
    case CircleStatus.PENDING: return "pending"
    ...
```
C++ has **`switch`**, which works on enums and whole numbers:
```cpp
switch (kind) {
    case CameraKind::Optical:
        return "optical";
    case CameraKind::Inertial:
        return "inertial";
    case CameraKind::Video:
        return "video";
}
return "unknown";
```
- Each `case` is one value. When it matches, the code under it runs.
- **Watch out:** without a `return` (or `break`), C++ keeps going into the *next* case. This is called
  *fall-through* and is a classic bug. Here every case returns, so there's nothing to fall into.
- **Why the `return "unknown"` after the switch?** An `enum class` can hold numbers that aren't any of its
  names: `static_cast<CameraKind>(99)` compiles. It happens in real life when data comes from a file or a
  database written by a newer version. So the function needs an answer for that too.

**Returning `std::string_view` here is safe.** Remember the lifetime rule from Lesson 01: a view must point
at text that stays alive. Text in quotes, like `"optical"`, is a **string literal**: it's stored inside the
program itself and lives as long as the program runs. So a view of it can never dangle. (Contrast: a view of a
`std::string` you built inside the function dies at `}`.)

### Example
```cpp
std::string_view to_string(CameraKind kind) {
    switch (kind) {
        case CameraKind::Optical:
            return "optical";
        case CameraKind::Inertial:
            return "inertial";
        case CameraKind::Video:
            return "video";
    }
    return "unknown";   // a number that isn't one of the names
}
```

### Your turn
In **`circle_status.cpp`**, write **`to_string(CircleStatus status)`**, using the names from the spec in
`circle_status.h`.

### Run it
`[x] Step 2: to_string(CircleStatus)    7/7`

---

## Step 3 — Text back to a status (25 min)

### Learn
Going the other way, `"circle_a"` → `CircleStatus::CircleA`, can fail: the text might be anything. That's
exactly what `std::optional` is for (Lesson 01, Step 4): **"a value, or nothing"**, where nothing is a normal,
expected answer. Returning a plain `CircleStatus` wouldn't work: there's no value that means "that wasn't a
status", and making one up (`NotCaptured`?) would quietly turn bad data into real-looking data.

You already have the names in `to_string`. Rather than writing them all a second time (two lists that can
drift apart), loop over every status and ask `to_string` for each name. For that you need a list of every
status: a `constexpr std::array`, like `kValidExt` in Lesson 01.

### Example
Same shape, different job: find the frame rate that matches a number.
```cpp
enum class FrameRate {
    Fps30 = 30,
    Fps60 = 60,
    Fps120 = 120,
};

constexpr std::array kAllFrameRates{FrameRate::Fps30, FrameRate::Fps60, FrameRate::Fps120};

std::optional<FrameRate> frame_rate_from_number(int fps) {
    for (FrameRate rate : kAllFrameRates) {
        if (static_cast<int>(rate) == fps) {
            return rate;          // found it: the optional holds rate
        }
    }
    return std::nullopt;          // nothing matched
}
// frame_rate_from_number(60) -> FrameRate::Fps60      frame_rate_from_number(24) -> nothing
```

### Your turn
In **`circle_status.cpp`**: put a list of all six statuses in the unnamed namespace at the top (you'll need
`#include <array>`), then write **`parse_circle_status(std::string_view text)`**.

### Run it
`[x] Step 3: parse_circle_status    10/10`

<details><summary>Hint 1</summary>

Inside the loop, compare `to_string(status)` with `text`. Both are `std::string_view`, so `==` compares the
characters.
</details>

---

## Step 4 — Your first class: `Timecode` (35 min)

This is the biggest step: four small ideas, then you declare and define the class yourself.

### Learn 1: `class`, `public` and `private`
A `struct` (like `TakeName`) is a bag of fields anyone can change. A **`class`** keeps its data **private** and
offers **public** functions to work with it:
```cpp
class FrameRange {
public:                          // anyone can use what's below
    FrameRange(int first, int last);
    int first() const;
    int last() const;
    int length() const;

private:                         // only FrameRange's own functions can touch these
    int first_ = 0;
    int last_ = 0;
};
```
Python has the idea as a convention (`self._first` means "don't touch"); C++ enforces it. Code outside the
class that writes `range.first_ = 5` gets a compile error.

**Why bother?** Because the class can then promise things about its data. A `Timecode` that anyone can edit
can be put into a weird state from anywhere in the codebase; one with private data can only change through
the functions you wrote. (Lesson 03 goes further with this.)

The only real difference between `struct` and `class` is the default: `struct` starts `public`, `class` starts
`private`. Convention: `struct` for plain bags of data, `class` when there's private data.

**Naming:** a trailing underscore (`first_`) marks a data member, so it can't be confused with a function
`first()` or a parameter `first`. It's Google's style; Unreal doesn't use one. Pick one style per codebase.

### Learn 2: the constructor
The **constructor** is the function that runs when the object is created. Python's `__init__`. It has the
class's name and no return type. Declared in the header, defined in the `.cpp`:
```cpp
// frame_range.cpp
FrameRange::FrameRange(int first, int last)
    : first_(first), last_(last)          // the member initializer list
{
}
```
- **`FrameRange::`** in front says "this is FrameRange's function". Every member function defined in the
  `.cpp` needs it. Forget it and you've defined a plain free function instead, and the linker complains the
  member is missing (`LNK2019`, your old friend).
- **`: first_(first), last_(last)`** is the **member initializer list**: it sets each member *as it's
  created*. Python: `self._first = first`. You could also assign inside `{ }`, but the list is standard
  practice: it initializes instead of "create, then overwrite".
- Creating one: `FrameRange shot(100, 250);` (Python: `shot = FrameRange(100, 250)`).

### Learn 3: `const` member functions
Remember the trailing `const` from the Lesson 01 quiz? This is where it lives:
```cpp
int length() const;      // header: "calling length() doesn't change the FrameRange"
```
```cpp
int FrameRange::length() const {         // .cpp: the const is repeated, it's part of the function's type
    return last_ - first_ + 1;
}
```
Why it matters: if you have a **const** object (or a `const FrameRange&` parameter, which you'll use all the
time), C++ only lets you call its `const` functions. Forget the `const` on a getter and this happens:
```cpp
void print_length(const FrameRange& range) {
    std::cout << range.length();   // error C2662: cannot convert 'this' pointer from 'const FrameRange'
}                                  //              to 'FrameRange &'   <- means: length() isn't const
```
The rule of thumb: **every function that only reads gets `const`.** Getters, `to_string`, comparisons:
all `const`. The tests for this step check that your getters are `const`.

### Learn 4: `==` on a class
Same as Lesson 01, Step 6, now inside a class (it can compare private members, because it's a member):
```cpp
bool operator==(const FrameRange&) const = default;   // equal when every data member matches
```

### Example
The whole `FrameRange`, both files:
```cpp
// frame_range.h
#pragma once

class FrameRange {
public:
    FrameRange(int first, int last);

    int first() const;
    int last() const;
    int length() const;

    bool operator==(const FrameRange&) const = default;

private:
    int first_ = 0;
    int last_ = 0;
};
```
```cpp
// frame_range.cpp
#include "frame_range.h"

FrameRange::FrameRange(int first, int last)
    : first_(first), last_(last)
{
}

int FrameRange::first() const {
    return first_;
}

int FrameRange::last() const {
    return last_;
}

int FrameRange::length() const {
    return last_ - first_ + 1;   // both ends count: 100..102 is 3 frames
}
```
(`= 0` on each member is a **default member initializer**: the value used if a constructor doesn't set it.
Always give members a starting value; an `int` without one holds whatever garbage was in memory.)

### Your turn
1. In **`timecode.h`**, under `public:`, declare what the Step 4 spec asks for: the constructor, the two
   getters (const!) and `==`. Under `private:`, declare the two data members, each with a starting value.
2. In **`timecode.cpp`**, define the constructor and the two getters.

### Run it
`[x] Step 4: Timecode class    7/7`

<details><summary>Hint 1</summary>

Follow `FrameRange` line by line: it has the same shape. Two `int` members, a constructor taking two `int`s,
getters that return them.
</details>
<details><summary>Hint 2: "add const after the ()"</summary>

That message means a getter is declared without `const`. It goes in both places:
`int frame_rate() const;` in the header and `int Timecode::frame_rate() const { ... }` in the `.cpp`.
</details>

---

## Step 5 — Showing a timecode: `to_string` (25 min)

### Learn: integer division and `%`
Turning a frame count into hours, minutes, seconds and frames is division with remainders. With two `int`s:
```cpp
int whole = 125 / 60;     // 2    division of ints drops the fraction (Python: 125 // 60)
int rest  = 125 % 60;     // 5    the remainder                       (Python: 125 % 60)
```
So 125 seconds is `2` minutes and `5` seconds. (Careful: for **negative** numbers C++ and Python disagree.
`-7 / 2` is `-3` in C++ but `-7 // 2` is `-4` in Python. Timecodes here are never negative, so it doesn't
matter in this step, but it's a classic interview gotcha.)

### Learn: `constexpr` constants
"60 seconds in a minute" should have a name, not be a bare `60` in the middle of math (a "magic number"):
```cpp
namespace {
constexpr int kSecondsPerMinute = 60;
}
```
`constexpr` = fixed at compile time and can never change (Lesson 01, Step 2). Unlike a Python constant, it's
enforced, and it costs nothing at run time: the compiler puts the number straight into the code.

### Example
A take's length in seconds as `"MM:SS"`:
```cpp
namespace {
constexpr int kSecondsPerMinute = 60;
}

std::string format_duration(int total_seconds) {
    int minutes = total_seconds / kSecondsPerMinute;
    int seconds = total_seconds % kSecondsPerMinute;
    return std::format("{:02}:{:02}", minutes, seconds);
}
// format_duration(125) -> "02:05"      format_duration(59) -> "00:59"      format_duration(3600) -> "60:00"
```

### Your turn
1. In **`timecode.h`**, declare `to_string()` (it returns `std::string` and is `const`).
2. In **`timecode.cpp`**, add your constants to the unnamed namespace and define **`Timecode::to_string()`**.
   You'll need `#include <format>`.

Work it out in order: frames first (what's left over after whole seconds), then seconds, minutes and hours.
Hours just keep counting past 24 (`"25:00:00:00"` is fine).

### Run it
`[x] Step 5: Timecode::to_string    7/7`

<details><summary>Hint 1</summary>

`total_frames_ / frame_rate_` is the number of whole seconds, and `total_frames_ % frame_rate_` is the frames
left over. Then split the whole seconds the same way `format_duration` does, one more level up for hours.
</details>
<details><summary>Hint 2</summary>

Write one named variable per line: `frames`, `total_seconds`, `seconds`, `total_minutes`, `minutes`, `hours`.
Print them (your printing reference, section 7) if a test fails.
</details>

---

## Step 6 — Making `+` and `-` work: operator overloading (20 min)

### Learn
In Python you make `a + b` work on your own class with `__add__`. In C++, you write a function named
**`operator+`**. That's the whole trick; you saw it with `operator==` already.
```cpp
class FrameRange {
public:
    FrameRange operator+(int offset) const;    // range + 10 -> the same range, 10 frames later
    ...
};
```
```cpp
FrameRange FrameRange::operator+(int offset) const {
    return FrameRange(first_ + offset, last_ + offset);   // a NEW FrameRange
}
```
Reading the declaration: returns a `FrameRange`, is called `operator+`, takes the thing on the right of the
`+` (an `int`), and is `const` because the thing on the left doesn't change.

When you write `shot + 10`, C++ calls `shot.operator+(10)`.

**`+` makes a new value, it doesn't change the old one.** Just like `int`: `a + 1` doesn't change `a`. Types that
behave like numbers this way are called **value types**, and they're easy to reason about: nothing changes
behind your back. The tests check that `start + 5` leaves `start` alone.

The rule when overloading operators: **make them mean what people expect.** `+` adds, `-` subtracts, `==`
compares. An `operator+` that deletes files compiles fine and is a firing offense.

The return type is your choice, and it should match the meaning: a timecode **plus** some frames is another
timecode, but a timecode **minus** a timecode is a *distance*, a number of frames. Python works the same way
with `datetime`: `datetime - datetime` gives a `timedelta`, not a `datetime`.

### Your turn
1. In **`timecode.h`**, declare `operator+` (Timecode + int → Timecode) and `operator-` (Timecode − Timecode →
   int), both `const`.
2. In **`timecode.cpp`**, define them.

### Run it
`[x] Step 6: Timecode + and -    8/8`

<details><summary>Hint 1</summary>

`operator-` takes the Timecode on the right as `const Timecode& other` (no copy, not changed), and reads
`other.total_frames_` directly: a member function can see the private members of any object of its own class.
</details>

---

## Step 7 — A second class from a spec: `Uuid` (25 min)

Same skills as Step 4, plus three small new things.

### Learn 1: exact-size integers
A UUID is 128 bits. The biggest standard integer is 64 bits, so `Uuid` stores two halves:
```cpp
#include <cstdint>

std::uint64_t half = 0;                  // unsigned (never negative), exactly 64 bits
std::uint64_t big = 0xFFFFFFFFFFFFFFFF;  // 0x... is a hex number: here the largest 64-bit value
```
Python's `int` grows as big as it needs to, so `uuid4().int` is one 128-bit number. C++ integers have fixed
sizes, and you pick them: `std::uint8_t`, `std::uint16_t`, `std::uint32_t`, `std::uint64_t` (unsigned), and the
same without `u` (signed). `int` is "usually 32 bits"; when the size matters, say it exactly.

### Learn 2: two constructors (overloading)
A class can have several constructors, as long as their parameters differ. C++ picks the one that matches:
```cpp
class Color {
public:
    Color() = default;                                          // Color black;           -> all 0
    Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue);  // Color red(255, 0, 0);
    ...
private:
    std::uint8_t red_ = 0;
    std::uint8_t green_ = 0;
    std::uint8_t blue_ = 0;
};
```
`Color() = default;` means "the constructor with no arguments does the obvious thing": each member gets its
default member initializer (`= 0`). Writing your own version that sets all three to 0 would work too, but
`= default` says it in one line and can't get out of sync with the members.

(Python can't overload; it uses default arguments: `def __init__(self, red=0, green=0, blue=0)`.)

### Learn 3: two checks, one answer
`is_nil()` is true only when **both** halves are 0. Written the "one check per `if`" way:
```cpp
bool Color::is_black() const {
    if (red_ != 0) {
        return false;
    }
    if (green_ != 0) {
        return false;
    }
    if (blue_ != 0) {
        return false;
    }
    return true;        // nothing was lit
}
```
(The short form you'll see elsewhere is `return red_ == 0 && green_ == 0 && blue_ == 0;`.)

### Your turn
1. In **`uuid.h`**, declare what the Step 7 spec asks for, and the two data members (each starting at 0).
2. In **`uuid.cpp`**, define the two-argument constructor, `high()`, `low()` and `is_nil()`. (The default
   constructor and `==` are `= default`, so they have nothing to define.)

### Run it
`[x] Step 7: Uuid class    11/11`

<details><summary>Hint 1</summary>

It's `Timecode` again with `std::uint64_t` instead of `int`, plus `Uuid() = default;` and `is_nil()`.
</details>

---

## Step 8 — Hex text: `Uuid::to_string` (20 min)

### Learn
`std::format` can write numbers in hex, the same way Python's f-strings do:
```cpp
std::format("{:x}", 255)            // "ff"                 Python: f"{255:x}"
std::format("{:08x}", 255)          // "000000ff"           pad with zeros to 8 characters
std::format("{:016x}", 255ull)      // "00000000000000ff"   a full 64-bit value is 16 hex digits
```
Each hex digit is 4 bits, so a 64-bit half is always exactly **16** digits once padded, and the two halves are
**32**. The standard UUID text adds dashes after the 8th, 12th, 16th and 20th digit: `8-4-4-4-12`.

A plan with no tricks: write all 32 digits into one `std::string`, then build the result piece by piece with
`substr` (Lesson 01) and `+=` (your strings reference, section 5).

**Careful with `substr`:** the second number is *how many characters*, not where to stop. Each piece's
count is right there in `8-4-4-4-12`.

### Example
Hex in pairs, like a network address: `0xdeadbeef` → `"de:ad:be:ef"`.
```cpp
std::string hex_pairs(std::uint32_t value) {
    std::string hex = std::format("{:08x}", value);   // "deadbeef": always 8 digits

    std::string result;
    result += hex.substr(0, 2);    // "de"   start 0, 2 characters
    result += ':';
    result += hex.substr(2, 2);    // "ad"   start 2, 2 characters
    result += ':';
    result += hex.substr(4, 2);    // "be"
    result += ':';
    result += hex.substr(6, 2);    // "ef"
    return result;
}
```
(Here `substr` is called on a `std::string`, so each piece is a new `std::string`, and `result` owns all
of its text. No views, nothing to dangle.)

### Your turn
Declare **`to_string()`** in `uuid.h` (returns `std::string`, `const`), and define it in `uuid.cpp`
(`#include <format>`).

### Run it
`[x] Step 8: Uuid::to_string    5/5`

<details><summary>Hint 1</summary>

`std::format("{:016x}{:016x}", high_, low_)` gives all 32 digits. The pieces start at 0, 8, 12, 16 and 20.
</details>

---

## Step 9 — New random IDs: `static` and `<random>` (25 min)

### Learn 1: `static` member functions
`generate()` makes a brand-new `Uuid`, so there's no existing `Uuid` to call it on. A **`static`** member function
belongs to the class, not to any one object. Python: `@staticmethod` (or a `@classmethod` factory).
```cpp
class Uuid {
public:
    static Uuid generate();       // header: `static` here only
};

Uuid Uuid::generate() { ... }     // .cpp: no `static` here

Uuid id = Uuid::generate();       // called on the class, with ::
```
A static function has no `this` object, so it has no `high_` or `low_` of its own. It makes a new `Uuid` and
returns it. (It's also why it can't be `const`: there's no object to keep unchanged.)

### Learn 2: random numbers in C++
Python's `random.getrandbits(64)` is one call. C++ splits the job into three parts, from `<random>`:
```cpp
std::random_device seed_source;                     // 1. truly unpredictable bits from the OS, but slow
std::mt19937_64 engine(seed_source());              // 2. a fast generator, started ("seeded") from 1
std::uniform_int_distribution<int> die(1, 6);       // 3. the shape you want: whole numbers 1 to 6
int roll = die(engine);                             //    ask the distribution, using the engine
```
- **`std::random_device`** asks the operating system for real randomness. Use it once, to seed.
- **`std::mt19937_64`** (the "Mersenne Twister", 64-bit) is the workhorse generator: fast, and good enough for
  IDs and games. Not for passwords or encryption.
- **`std::uniform_int_distribution<T>`** turns raw bits into numbers in a range. With no range given,
  `std::uniform_int_distribution<std::uint64_t>` covers every 64-bit value.

### Learn 3: `static` local variables
Making a new engine on every call would be slow, and two engines seeded at nearly the same moment can give
similar numbers. A **`static` local variable** is created **once**, the first time the function runs, and then
kept between calls:
```cpp
int roll_die() {
    static std::random_device seed_source;            // created on the first call only
    static std::mt19937_64 engine(seed_source());     // seeded once, then reused forever
    std::uniform_int_distribution<int> die(1, 6);     // cheap: fine to make each time
    int roll = die(engine);
    return roll;
}
```
Python equivalent: a module-level `_engine = random.Random()` that the function uses. (Note for Lesson 11:
two threads using the same engine at once is a bug. We're single-threaded for now.)

One more standard-practice detail: call the distribution into **named variables**, one per line:
```cpp
int first_roll = die(engine);
int second_roll = die(engine);
```
If you write `make_pair(die(engine), die(engine))`, C++ doesn't promise which of the two calls runs first.
For random numbers it doesn't break anything, but it's a habit worth having: one call, one line, one name.

### Your turn
Declare **`static Uuid generate();`** in `uuid.h`, and define it in `uuid.cpp` (`#include <random>`): a
static engine, a distribution over every `std::uint64_t`, two named halves, and return a new `Uuid`.

### Run it
All steps `[x]`, and: `All steps pass. Say "check" in chat for a review.`

<details><summary>Hint 1</summary>

Follow `roll_die`, with `std::uniform_int_distribution<std::uint64_t>` and no range. Then
`return Uuid(high, low);`.
</details>

(Real UUIDs also set a few fixed bits to mark "version 4, random". We skip that; ours are 128 random bits
written in UUID form, which is all Capture++ needs.)

---

## Wrap-up

### What you can now say in an interview
- **`enum class` vs `enum`:** an `enum class` keeps its names scoped (`Status::Done`) and never turns into an
  `int` by itself; a plain `enum` does both. Use `enum class` for new code, and `static_cast` when you need
  the number.
- **`class` vs `struct`:** the same thing except the default access (`private` vs `public`). Private data plus
  public functions lets a type keep its own data valid.
- **Member initializer list:** members are initialized before the constructor body runs; the list sets them
  directly instead of "create, then overwrite".
- **`const` member functions:** a promise not to change the object. Only `const` functions can be called on a
  `const` object or through a `const&`, so every read-only function should be `const`.
- **Operator overloading:** `a + b` is `a.operator+(b)`. Keep the expected meaning, return new values from
  arithmetic operators, and pick return types that match the meaning (timecode − timecode = frames).
- **Value types:** small types like `Timecode` and `Uuid` that copy cheaply, compare by value, and never change
  behind your back. They're the building blocks the entities in Lesson 03 hold.
- **`static`:** on a member function, "belongs to the class, no object needed"; on a local variable,
  "created once, kept between calls".

### Quick quiz (answers hidden)
<details><summary>1. Why does <code>CircleStatus::Burnt == 5</code> fail to compile, and how do you get the number?</summary>

An `enum class` never converts to an `int` by itself. `static_cast<int>(CircleStatus::Burnt)` gives 5.
</details>
<details><summary>2. A getter is declared <code>int frame_rate();</code> (no const). What breaks?</summary>

It can't be called on a `const Timecode` or through a `const Timecode&`: error C2662. Read-only functions
should always be `const`.
</details>
<details><summary>3. Is it safe for <code>to_string(CircleStatus)</code> to return a <code>std::string_view</code>? Why?</summary>

Yes: it points at string literals, which live for the whole program. A view of a local `std::string` would
dangle.
</details>
<details><summary>4. In C++, what is <code>-7 / 2</code>? And in Python, <code>-7 // 2</code>?</summary>

C++ gives `-3` (it drops the fraction, toward zero). Python gives `-4` (it rounds down).
</details>
<details><summary>5. Why is the random engine in <code>generate()</code> <code>static</code>?</summary>

So it's created and seeded once and then reused. A new engine per call is slow, and engines seeded at nearly
the same time can repeat each other.
</details>

### Cheat sheet
| Python | C++ |
|---|---|
| `class Status(Enum): DONE = 1` | `enum class Status { Done = 1 };` |
| `Status.DONE.value` | `static_cast<int>(Status::Done)` |
| `match x: case ...:` | `switch (x) { case ...: return ...; }` |
| `def __init__(self, a): self._a = a` | `Thing(int a);` + `Thing::Thing(int a) : a_(a) {}` |
| `self._a` (private by convention) | `private: int a_ = 0;` (enforced) |
| `@property def a(self)` | `int a() const;` |
| `__eq__` | `bool operator==(const Thing&) const = default;` |
| `__add__(self, n)` | `Thing operator+(int n) const;` |
| `@staticmethod def make()` | `static Thing make();` called as `Thing::make()` |
| `a // b`, `a % b` (positive numbers) | `a / b`, `a % b` with `int`s |
| `f"{n:08x}"` | `std::format("{:08x}", n)` |
| `random.getrandbits(64)` | `std::mt19937_64` + `std::uniform_int_distribution<std::uint64_t>` |
| `uuid.uuid4()` | `Uuid::generate()` (yours!) |
