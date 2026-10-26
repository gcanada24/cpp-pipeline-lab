# Start here

## Your current lesson
**[Lesson 01 — Take names](lessons/01-take-names/LESSON.md)**: open that file and follow it top to bottom.

## How every lesson works
Each lesson is one folder, `lessons/NN-name/`, and **its `LESSON.md` is the whole lesson**. It's split into
short steps, and every step has the same parts:

1. **Learn** — the idea, explained next to the Python you already know
2. **Example** — complete working code for a similar problem
3. **Your turn** — the one function to write, and which file it's in
4. **Run it** — `.\tools\run.ps1 NN`, and what "done" looks like

When you run the tests you get a progress list, one line per step. It shows the failures for the step you're
on, and only that step:
```
Progress
  [x] Step 2: is_take_extension            7/7
  [ ] Step 3: file_name                    2/6   <-- you are here
  [ ] Step 4: parse_take_number            8/11
```

## Where Claude fits
The lesson file is complete on its own. Use chat when you want more:
- **"check"** — I build your code and review it the way an interviewer would
- **"explain ___"** — a different explanation of anything in the lesson
- **"hint for step N"** — a nudge (each step also has hidden hints in the lesson)
- **"quiz me"** — interview-style questions on what you've done

## The lessons
You're building **Capture++**, a C++ version of a capture pipeline: the data model, a database, events and
threads, and finally a Qt app. Each lesson adds one working piece.
Full syllabus with the reasoning behind each lesson: [COURSE.md](COURSE.md).

| # | Lesson | Status |
|---|---|---|
| 01 | Take names: headers, strings, `optional`, structs | **current** |
| 02 | IDs, statuses and timecode: value types, `enum class`, operators | written when 01 is done |
| 03 | The Entity base class: classes, `virtual`, abstract classes | |
| 04 | The Facility → Take hierarchy: ownership, `unique_ptr` | |
| 05 | Finding and sorting entities: recursion, maps, lambdas | |
| 06 | Copy vs move: why an entity isn't a value | |
| 07–12 | Making it a system: events, JSON, database (SQLite), threads, design | |
| 13–16 | Qt 6 app: windows, tree/table views, editing, undo | |
| 17 | Capstone: the Capture++ Browser + mock interviews | |
