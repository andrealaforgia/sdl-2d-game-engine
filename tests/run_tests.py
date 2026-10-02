"""Compile and exercise engine contracts without opening a window."""

import argparse
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import sys

ROOT = Path(__file__).resolve().parents[1]


def build(sources, destination, sanitize=False, extra_includes=()):
    compiler = shlex.split(os.environ.get("CC", "cc"))
    flags = ["-std=c99", "-O1", "-g", "-Wall", "-Wextra", "-Werror"]
    if sanitize:
        flags += ["-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
    includes = [
        "-I" + str(directory)
        for directory in sorted((ROOT / "core").iterdir())
        if directory.is_dir()
    ]
    includes += ["-I" + str(ROOT / "tests")] + ["-I" + str(p) for p in extra_includes]
    sdl_flags = shlex.split(
        subprocess.check_output(["sdl2-config", "--cflags"], text=True)
    )
    sdl_libs = shlex.split(
        subprocess.check_output(["sdl2-config", "--libs"], text=True)
    )
    objects = []
    for index, source in enumerate(sources):
        target = destination.parent / (str(index) + ".o")
        instrumentation = (
            []
            if source.name in ("test_allocator.c", "test_clock.c")
            else [
                "-include",
                str(ROOT / "tests/test_allocator.h"),
                "-Dmalloc=test_malloc",
                "-Dcalloc=test_calloc",
                "-Dfree=test_free",
            ]
        )
        subprocess.run(
            compiler
            + flags
            + includes
            + sdl_flags
            + instrumentation
            + ["-c", str(source), "-o", str(target)],
            check=True,
        )
        objects.append(str(target))
    subprocess.run(
        compiler
        + flags
        + objects
        + sdl_libs
        + ["-lSDL2_image", "-lSDL2_mixer", "-lSDL2_ttf", "-lm", "-o", str(destination)],
        check=True,
    )


def runtime_environment():
    environment = dict(os.environ)
    environment.setdefault("UBSAN_OPTIONS", "halt_on_error=1")
    if sys.platform == "darwin":
        # sdl2-compat loads SDL3 with dlopen; ASan needs an explicit search path.
        libraries = shlex.split(
            subprocess.check_output(["sdl2-config", "--libs"], text=True)
        )
        directories = [flag[2:] for flag in libraries if flag.startswith("-L")]
        if environment.get("DYLD_LIBRARY_PATH"):
            directories.append(environment["DYLD_LIBRARY_PATH"])
        environment["DYLD_LIBRARY_PATH"] = ":".join(directories)
        environment.setdefault("ASAN_OPTIONS", "detect_leaks=0")
    return environment


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--sanitize", action="store_true")
    args = parser.parse_args()
    sources = [
        ROOT / "core/utils/command_line.c",
        ROOT / "core/input/keyboard.c",
        ROOT / "core/input/events.c",
        ROOT / "core/memory/object_pool.c",
    ]
    sources += [
        ROOT / ("tests/" + name)
        for name in ("test_allocator.c", "test_clock.c", "test_engine.c")
    ]
    cases = [(name, [name], 0) for name in ("keys", "pool", "allocation", "events")]
    cases += [
        ("valid options", ["parse", "--fps=120", "--volume=64", "--window-mode=0"], 0)
    ]
    malformed = [
        "--fps=60junk",
        "--volume=garbage",
        "--window-mode=garbage",
        "--fps=0",
        "--volume=129",
        "--display=-1",
        "--display-mode=1junk",
        "--fps=9999999999999999999999999999",
    ]
    cases += [(value, ["parse", value], 1) for value in malformed]
    failures = 0
    with tempfile.TemporaryDirectory(prefix="asteroids-engine-tests-") as directory:
        binary = Path(directory) / "tests"
        build(sources, binary, args.sanitize)
        for name, arguments, expected in cases:
            result = subprocess.run(
                [str(binary)] + arguments,
                text=True,
                capture_output=True,
                timeout=10,
                env=runtime_environment(),
            )
            passed = result.returncode == expected
            print(("PASS " if passed else "FAIL ") + name)
            if not passed:
                failures += 1
                print(result.stdout + result.stderr)
    raise SystemExit(bool(failures))


if __name__ == "__main__":
    main()
