"""Spec tests for take_record.py. Run from the repo root:  python week01/python/test_take_record.py"""
import dataclasses
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from take_record import TakeRecord, parse_take  # noqa: E402

results = []


def check(name, fn):
    try:
        fn()
        results.append(True)
        print(f"PASS  {name}")
    except Exception as exc:  # report every failure, keep going
        results.append(False)
        print(f"FAIL  {name}: {type(exc).__name__}: {exc}")


def expect(path, shoot, take, performer):
    def run():
        rec = parse_take(path)
        got = (rec.shoot, rec.take, rec.performer)
        assert got == (shoot, take, performer), f"got {got}"
        assert rec.path == Path(path), f"path should be kept as given, got {rec.path!r}"
        assert rec.fps == 120.0, f"default fps should be 120.0, got {rec.fps}"
    return run


def expect_error(path):
    def run():
        try:
            parse_take(path)
        except ValueError as exc:
            assert Path(path).name in str(exc), "error message should include the filename"
            return
        raise AssertionError("expected ValueError, nothing was raised")
    return run


def frozen():
    rec = parse_take("BRAWL_T003_JDoe.fbx")
    try:
        rec.take = 4
    except dataclasses.FrozenInstanceError:
        return
    raise AssertionError("TakeRecord should be frozen")


def equality_and_hash():
    a = parse_take("BRAWL_T003_JDoe.fbx")
    b = parse_take("BRAWL_T003_JDoe.fbx")
    assert a == b, "same file should give equal records"
    assert len({a, b}) == 1, "equal records should hash the same"


def custom_fps():
    assert parse_take("BRAWL_T003_JDoe.fbx", fps=60.0).fps == 60.0


check("simple name", expect("BRAWL_T003_JDoe.fbx", "BRAWL", 3, "JDoe"))
check("shoot with underscore", expect("BRAWL_DAY2_T012_ASmith.c3d", "BRAWL_DAY2", 12, "ASmith"))
check("full path, upper-case ext", expect("D:/mocap/brawl/BRAWL_T001_JDoe.FBX", "BRAWL", 1, "JDoe"))
check("missing T", expect_error("BRAWL_003_JDoe.fbx"))
check("take not digits", expect_error("BRAWL_TABC_JDoe.fbx"))
check("empty take digits", expect_error("BRAWL_T_JDoe.fbx"))
check("take zero", expect_error("BRAWL_T000_JDoe.fbx"))
check("missing performer", expect_error("BRAWL_T003.fbx"))
check("empty performer", expect_error("BRAWL_T003_.fbx"))
check("empty shoot", expect_error("_T003_JDoe.fbx"))
check("bad extension", expect_error("BRAWL_T003_JDoe.mov"))
check("frozen", frozen)
check("equality and hash", equality_and_hash)
check("custom fps", custom_fps)

print(f"\n{sum(results)}/{len(results)} passed")
sys.exit(0 if all(results) else 1)
