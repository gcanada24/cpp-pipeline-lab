# Capture++ — learning C++ by rebuilding a capture pipeline

C++ only, taught in chat. Over 17 lessons you build **Capture++**: a C++ re-design of the Python capture
system you shipped (entity data model → database → events/threads → Qt browser). It is not a line-by-line
port: every lesson takes one idea from the Python system, names the design principle behind it, and makes
the decisions C++ forces — **types, ownership, copies, failure, cost, threads.**

The Python source is a **local reference only** (`Downloads\My Work-...\python\capture\core`). It is never
copied into this repo; Capture++ is original code with generic names.

## Architecture (where it ends up)
```
capture_core (static lib, no Qt)            apps/
  naming/     take filename parse/format      capture_cli/  ingest + query from the command line
  model/      Entity base, Facility→Take      capture_ui/   Qt 6 browser: tree, table, edit, undo
  events/     Signal<Args...>
  serialize/  JSON, Type@uuid references
  db/         IDatabaseProvider, JSON + SQLite providers
  process/    background saver, task processor
  services/   selection, repository, processing (replaces the engine inheritance chain)
```
Rule carried over from the Python design: **core never includes Qt**; views talk to core through services
and signals.

## How a session works
1. In chat I explain the concepts, mapped from the Python you already wrote.
2. You write the code in `core/...` (headers in `core/include/capture/`, bodies in `core/src/`). Each lesson's
   tests live in `lessons/NN-*/main.cpp` and compile from the start.
3. `.\tools\run.ps1 NN` builds `capture_core` and the lesson's tests and runs them (FAIL lines + a score).
4. Say "check": I build it, read your code and review it like an interviewer.
5. Close with rapid interview questions; misses come back in the next session's warm-up.

You write the solutions. Hints come in three levels (question → pointer → example on a different problem);
ask for a worked example whenever speed matters more than struggle.

## Lessons

### Phase 1 — C++ through the data model (no frameworks)
| # | Capture++ piece | From the Python system | C++ concepts |
|---|---|---|---|
| 01 | `naming`: parse/format take filenames | naming / path helpers | header vs source, compile vs link, namespaces, `string_view`, `optional`, `from_chars`, `std::format`, `operator==` |
| 02 | `Uuid`, `CircleStatus`, `Timecode` | `uuid4().int`, `CircleStatus(Enum)`, timecode strings, frames @120/60/30 | value types, `enum class`, operator overloading, `const`, `constexpr`, `<random>`, `std::hash` |
| 03 | `Entity` base class | `EntityBase`: uuid, name, note, `safely_deleted`, `_modified_properties` | `class` vs `struct`, access control, abstract classes, virtual destructor, getters/setters, change tracking |
| 04 | Facility → Project → Day → Session → Shot → Take | `add_take()`, `parent_entity`, `get_parent_of_type()` | ownership: `unique_ptr` children + raw non-owning parent, shared Subjects, templates + `dynamic_cast` |
| 05 | Traversal, lookup, sorting | `children_entity_bases`, `get_child_with_uuid`, `SortTypes` | recursion/visitors, `unordered_map` index, `std::sort` + lambdas, iterators |
| 06 | Entities vs values | (Python never made you choose) | copy vs move, Rule of 0/3/5, `= delete`, identity vs value objects |

### Phase 2 — Making it a system
| # | Capture++ piece | From the Python system | C++ concepts |
|---|---|---|---|
| 07 | `Signal<Args...>` | `EventBase.register/dispatch`, `inspect.signature` checks | variadic templates, `std::function`, lambdas & captures, connection lifetime — compile-time checked |
| 08 | JSON serialization | `_entity_to_dict_formation`, `Type@uuid`, `replace_source_object_serializations...` | vcpkg + nlohmann/json, two-pass reconstruction, factory registry (no reflection in C++) |
| 09 | `IDatabaseProvider` + JSON-file provider | `DatabaseProvider`, `DatabaseFactory`, safe delete | pure virtual interfaces, factories returning `unique_ptr`, exception hierarchy |
| 10 | SQLite provider | Shotgun provider (simplified) | wrapping a C API with RAII, `unique_ptr` custom deleters, prepared statements, transactions, save only dirty entities |
| 11 | Background saver + task processor | `ThreadRunnerBase` pushers, `ProcessorBase` + status events | `std::jthread`, `stop_token`, `mutex`, `condition_variable`, work queues, why `RLock` is a smell |
| 12 | Services instead of the engine chain | `TakeEngine(ShotEngine(SessionEngine(...)))` — 12 levels | composition over inheritance, dependency injection, interfaces for testing, library/app split, `capture_cli` |

### Phase 3 — Qt 6 (needs Qt installed by here)
| # | Capture++ piece | From the Python system | C++ concepts |
|---|---|---|---|
| 13 | Main window shell | `windowBase`, `mainBase` | `find_package(Qt6)`, `QMainWindow`, layouts, signals/slots, moc / `Q_OBJECT`, Qt parent-child ownership |
| 14 | Hierarchy tree + take table | `EntityViewWidget` (`QStandardItemModel`) | `QAbstractItemModel` / `QAbstractTableModel`: `index`, `parent`, `rowCount`, `data`, roles, `dataChanged` |
| 15 | Edit form, circle takes, undo/redo | `entityEditWidget` | delegates, binding core `Signal`s to Qt, `QUndoStack` (Command pattern) |
| 16 | Responsive UI | threaded pushers + UI callbacks | `QThread` / `QtConcurrent`, `QFutureWatcher`, queued connections, never touching widgets off the GUI thread |

### Phase 4 — Capstone & interviews
| # | Capture++ piece | |
|---|---|---|
| 17 | **Capture++ Browser**: open a DB, browse the hierarchy, scan a folder and ingest takes (Lesson 01), circle takes, save | README + screenshots for GitHub; mock system-design interview ("design a capture data catalog"); C++ knowledge mock |

**Optional:** live stream receiver (`CaptureStream`/`CaptureTransform`: binary packets, frame buffers,
performance); Unreal C++ dialect (`UCLASS`, `TArray`, UObject GC vs smart pointers).

## Setup milestones
- Now: VS 2026 (MSVC), CMake, Ninja — already working via `tools\run.ps1`.
- Lesson 08: vcpkg (bundled with VS) pulls nlohmann-json and sqlite3 through a `vcpkg.json` manifest.
- Lesson 13: Qt 6.8 LTS, "MSVC 2022 64-bit" component, from the Qt online installer (free Qt account).
