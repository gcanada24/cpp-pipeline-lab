# Lesson 1: TakeRecord in Python, then why C++ will make it harder

Time: about 45 minutes reading and coding. You write `week01/python/take_record.py`.
The test file `week01/python/test_take_record.py` is already written for you; it is the spec.

---

## Part 1. What we are building and why

Over the next sessions you build a small **take catalog**: point it at a folder of mocap files and it tells
you which shoots, takes and performers are there. Today is the smallest piece: turning one filename into
one structured record.

```
"BRAWL_DAY2_T003_JDoe.fbx"   ->   TakeRecord(shoot="BRAWL_DAY2", take=3, performer="JDoe", ...)
```

This is the most common job in pipeline code: **take messy outside input (a string), check it, and turn it
into a typed object the rest of the program can trust.** You did this at 2K every time you read shoot data
from SQL. Today you learn the names for it, because interviewers use the names.

---

## Part 2. The principles you already use (with their names)

### 2.1 Value object (a "record type")
A `TakeRecord` has no behaviour worth talking about. It is just data, and two records with the same fields
are the same take. That is called a **value object**. In Python, `@dataclass` is the tool for it.

What `@dataclass` writes for you, silently:

```python
from dataclasses import dataclass

@dataclass
class Point:
    x: int
    y: int

# Python generated, behind your back:
#   __init__(self, x, y)          -> the constructor
#   __repr__(self)                -> "Point(x=1, y=2)" when printed
#   __eq__(self, other)           -> Point(1, 2) == Point(1, 2) is True (compares fields, not identity)
```

Remember this list. **In C++ you will get some of these for free and have to write others yourself.**
That difference is a classic interview question ("what does the compiler generate for a struct?").

### 2.2 Immutability: `frozen=True`
```python
@dataclass(frozen=True)
class Point:
    x: int
    y: int

p = Point(1, 2)
p.x = 5          # raises dataclasses.FrozenInstanceError
```
Why bother? A take's identity should not change after you parse it. If a record can't be changed, any code
that holds it can trust it, and you can safely use it as a dict key or put it in a set (frozen dataclasses
are **hashable**). In C++ the same idea is spelled `const`, and you will see it everywhere.

Use `frozen=True` for `TakeRecord`.

### 2.3 Type hints are promises, not checks
```python
@dataclass
class Point:
    x: int
    y: int

Point("hello", None)   # Python happily creates this. Nothing checks the hints at run time.
```
In Python, `x: int` is documentation plus a hint for PyCharm. In C++, `int x;` is **enforced by the
compiler** and the program won't even build if you put a string there. That is the single biggest mindset
shift: **in C++, types are checked before the program runs (static typing); in Python, mostly never
(dynamic typing).**

### 2.4 Parse, don't validate (validate at the boundary)
There are two ways to handle bad input:

- Bad: pass the raw string around and check bits of it wherever you use it.
- Good: check everything **once**, at the edge of the program, and produce an object that can only exist
  if the input was valid. After that, nobody re-checks.

`parse_take()` is that edge. Either it returns a valid `TakeRecord`, or it raises. There is no "half valid"
record. This is also called **fail fast**.

### 2.5 Exceptions for failure
Python's convention: when a function can't do its job because the input is wrong, raise `ValueError`
with a message that says *what* was wrong. `int("abc")` already does this for you:

```python
int("42")     # 42
int("abc")    # ValueError: invalid literal for int() with base 10: 'abc'
```

In C++ you will have to choose between exceptions, returning an "empty" result (`std::optional`), or an
error code. Python made that choice for you. That's one of the forced decisions you'll write down later.

---

## Part 3. The spec (exact rules)

A take file is named:

```
<SHOOT>_T<NNN>_<PERFORMER>.<ext>
```

| Part | Rule |
|---|---|
| `SHOOT` | Everything before the last two underscores. **May contain underscores** (`BRAWL_DAY2`). Must not be empty. |
| `T<NNN>` | The letter `T` followed by digits only. The number must be 1 or higher. Leading zeros are fine (`T003` is take 3). |
| `PERFORMER` | Must not be empty. |
| `ext` | `.fbx` or `.c3d` only, compared case-insensitively (`.FBX` is fine). |

`TakeRecord` fields, in this order:

| Field | Type | Meaning |
|---|---|---|
| `shoot` | `str` | e.g. `"BRAWL_DAY2"` |
| `take` | `int` | e.g. `3` |
| `performer` | `str` | e.g. `"JDoe"` |
| `path` | `pathlib.Path` | the path you were given, unchanged |
| `fps` | `float` | not in the filename; default `120.0` |

Function signature:

```python
def parse_take(path: str | Path, fps: float = 120.0) -> TakeRecord: ...
```

It accepts a full path (`"D:/mocap/BRAWL_T003_JDoe.fbx"`) or just a filename. Only the filename part is parsed.
On any rule violation it raises `ValueError` with a message that includes the filename.

### Examples

| Input | Result |
|---|---|
| `BRAWL_T003_JDoe.fbx` | shoot `BRAWL`, take `3`, performer `JDoe` |
| `BRAWL_DAY2_T012_ASmith.c3d` | shoot `BRAWL_DAY2`, take `12`, performer `ASmith` |
| `D:/mocap/brawl/BRAWL_T001_JDoe.FBX` | shoot `BRAWL`, take `1`, performer `JDoe` (folders ignored, `.FBX` ok) |
| `BRAWL_003_JDoe.fbx` | `ValueError` (no `T`) |
| `BRAWL_TABC_JDoe.fbx` | `ValueError` (not digits) |
| `BRAWL_T000_JDoe.fbx` | `ValueError` (take must be >= 1) |
| `BRAWL_T003.fbx` | `ValueError` (missing performer) |
| `_T003_JDoe.fbx` | `ValueError` (empty shoot) |
| `BRAWL_T003_JDoe.mov` | `ValueError` (bad extension) |

---

## Part 4. Tools you'll need, shown on a *different* problem

These examples parse a made-up `name-version.ext` format, not yours. Read them, then apply the ideas.

**Splitting a path into folder, name and extension with `pathlib`:**
```python
from pathlib import Path

p = Path("C:/builds/tool-v7.zip")
p.name      # 'tool-v7.zip'   (filename, no folders)
p.stem      # 'tool-v7'       (filename without the last extension)
p.suffix    # '.zip'          (includes the dot)
```

**Splitting from the right, a limited number of times:**
```python
"a-b-c-d".split("-")        # ['a', 'b', 'c', 'd']
"a-b-c-d".rsplit("-", 1)    # ['a-b-c', 'd']        split once, starting from the right
"a-b-c-d".rsplit("-", 2)    # ['a-b', 'c', 'd']
"abc".rsplit("-", 2)        # ['abc']               fewer parts than you hoped: check len()!
```
Think about why `rsplit` with a limit is exactly what the "shoot may contain underscores" rule needs.

**Checking characters:**
```python
"v7"[0]            # 'v'
"v7"[1:]           # '7'         slice: from index 1 to the end
"007".isdigit()    # True
"".isdigit()       # False       (careful: empty string)
int("007")         # 7
```

**Raising with a useful message:**
```python
def parse_version(name: str) -> int:
    stem, _, ver = name.rpartition("-")
    if not ver.startswith("v") or not ver[1:].isdigit():
        raise ValueError(f"bad version in {name!r}: expected v<digits>")
    return int(ver[1:])
```
`{name!r}` puts quotes around the value in the message, so an empty string shows up as `''` instead of
being invisible.

**Default argument + frozen dataclass:**
```python
@dataclass(frozen=True)
class Build:
    name: str
    version: int
    path: Path
    channel: str = "stable"     # fields with defaults must come after fields without
```

---

## Part 5. Steps

1. Create `week01/python/take_record.py`.
2. Write the `TakeRecord` frozen dataclass with the five fields above.
3. Write `parse_take()`. Suggested order of checks: extension, then split, then shoot, take, performer.
   Each failed check raises its own `ValueError` with a specific message.
4. Add a small `if __name__ == "__main__":` block that parses two or three of the example names and
   prints the records.
5. Run the tests from the repo root:
   ```
   python week01/python/test_take_record.py
   ```
   It prints `PASS` or `FAIL` per case. Get to all `PASS`.
6. Paste your file here. I'll review it the way an interviewer would.

### Hints (use only if stuck, in order)
1. **Question:** after `rsplit` with a limit of 2, how many pieces must you have, and what is each one?
2. **Pointer:** check the `T` prefix and the digits on the middle piece before calling `int()`, so the
   error message is yours, not Python's.
3. **Example:** see `parse_version` above. Your take check is the same shape.

---

## Part 6. Think ahead (answer in a sentence each, we use these in the C++ port)

1. `fps` is a `float`. What could go wrong if someone passes `fps=-30`? Should `parse_take` care?
2. `TakeRecord` stores the `Path`. If you parse 10,000 files, how many string copies do you think Python
   makes? (Guess. C++ will make you know.)
3. If `parse_take` gets a bad name, the caller gets an exception. In a folder scan of 10,000 files where 3
   are badly named, do you want the whole scan to stop? What would you do instead?

---

## Words to own after this lesson

- **Value object / record type**: data identified by its fields, not by its memory address.
- **Immutability**: can't change after creation (`frozen=True`, C++ `const`).
- **Static vs dynamic typing**: types checked at build time vs at run time (or never).
- **Parse, don't validate / fail fast**: check input once at the boundary, then trust the object.
- **Invariant**: a rule that is always true for every valid object (here: `take >= 1`, `shoot` not empty).
