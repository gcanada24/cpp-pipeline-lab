# C++ for Pipeline & Mocap — Fast Track

C++ only, taught in chat. Every lesson is a real piece of a mocap / animation pipeline, and every lesson
ends with the interview questions that concept gets asked as. You already know how to program (Python, 6+
years of pipeline work), so lessons spend no time on "what is a loop": they spend it on what C++ makes you
decide that Python decided for you — **types, memory, ownership, copies, failure, cost.**

## How a session works
1. In chat I explain the lesson's concepts, mapped from the Python you already know.
2. You open `lessons/NN-*/main.cpp`. It compiles from the start and has tests built in; your job is the `TODO`s.
3. Build + run: `.\tools\run.ps1 NN` (any PowerShell, from the repo root). It prints `FAIL` lines and a score.
4. Say "check" in chat — I build it, read your code, and review it like an interviewer would.
5. Close with 3–5 rapid interview questions. Misses get logged and come back in the next session's warm-up.

You write the solutions; I give hints in three levels (question → pointer → example on a different problem).
Ask for a worked example any time speed matters more than struggle.

## The build: `takecheck`
Lessons stack into one tool you can show in an interview: point it at a folder of capture data and it
catalogs the takes, loads marker trajectories, flags occlusion gaps, fills them, solves a skeleton pose,
and exports BVH — batch-processed across threads.

## Map

### Part A — Core language (the interview floor)
| # | Lesson | Mocap / pipeline task | C++ concepts |
|---|---|---|---|
| 01 | Take parser | `BRAWL_DAY2_T003_JDoe.fbx` → `TakeRecord` | compile/link, `struct`, `std::string` vs `std::string_view`, `std::optional`, `from_chars`, `operator==` |
| 02 | Vec3 & markers | marker positions, distances, centroid of a rigid body | value types, references, `const`, pass-by-value vs `const&`, operator overloading, `constexpr` |
| 03 | Trajectories | per-frame marker data, detect occlusion gaps, linear gap fill | `std::vector`, `reserve`, iterators, range-for, `<algorithm>`, NaN handling, iterator invalidation |
| 04 | Take catalog | group takes by shoot/performer, latest take per performer | `std::map` vs `std::unordered_map`, hashing a custom key, `std::sort` with lambdas |
| 05 | Marker CSV reader | load an OptiTrack/Vicon-style CSV export | `std::ifstream`, RAII, `std::filesystem`, exceptions vs `optional` vs error codes |

### Part B — Memory & design (where pipeline interviews are won)
| # | Lesson | Mocap / pipeline task | C++ concepts |
|---|---|---|---|
| 06 | Skeleton hierarchy | joints with parents/children, find root, walk the chain | pointers vs references, `std::unique_ptr`, raw non-owning pointers, index-based hierarchies |
| 07 | Owning a big buffer | a `Clip` holding a million frames — copy vs move | stack vs heap, Rule of 0/3/5, copy/move constructors, `std::move`, RVO |
| 08 | Forward kinematics | local rotations → world joint positions | quaternions, 4×4 transforms, composing transforms down a hierarchy, float precision |
| 09 | Generic filters | smoothing any channel (float, Vec3, Quat) | templates, function templates, lambdas & captures, `std::function`, concepts (light) |
| 10 | Exporter plugins | BVH / CSV writers behind one interface | classes, `virtual`, `override`, pure virtual, virtual destructor, factories, vtable cost |

### Part C — Performance & scale
| # | Lesson | Mocap / pipeline task | C++ concepts |
|---|---|---|---|
| 11 | Fast frames | process 10M samples, measure it | cache & memory layout, AoS vs SoA, Debug vs Release, `<chrono>` timing, profiler basics |
| 12 | Batch the farm | process a folder of takes in parallel | `std::thread`, `std::async`, `std::mutex`, data races, thread pool idea, `std::atomic` |

### Part D — Unreal & capstone
| # | Lesson | Mocap / pipeline task | C++ concepts |
|---|---|---|---|
| 13 | Unreal C++ dialect | read/write the same take data the Unreal way | `UCLASS`/`UPROPERTY`/`UFUNCTION`, `TArray`/`TMap`/`FString`, UObject GC vs smart pointers, modules, editor utilities & commandlets |
| 14 | Capstone: `takecheck` | the full CLI, tests, README for GitHub | putting it together, code review, talking through design trade-offs |

### Interview track (runs through everything)
Each lesson ends with its questions. Every ~4 lessons: a 30-minute mock (whiteboard-style problem + C++
knowledge questions + "walk me through a pipeline tool you built"). Running list in `interview/`.

## Pace
Built for speed: one lesson is a 60–120 minute session. Two lessons a day finishes Part A–B in about a week.
