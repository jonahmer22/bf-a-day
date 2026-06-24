#!/usr/bin/env python3
"""
BF interpreter benchmark — compiles each dayN with aggressive flags and times it.

Usage:
    python3 bench.py                     # tight loop, all days
    python3 bench.py --mandelbrot        # mandelbrot (I/O suppressed), all days
    python3 bench.py day1 day3           # specific days
    python3 bench.py --mandelbrot day2   # flags and days can be combined
"""

import os
import re
import sys
import subprocess
import time

ROOT = os.path.dirname(os.path.abspath(__file__))
RUNS = 3

PROGRAMS = {
    "tight_loop": os.path.join(ROOT, "tests", "tight_loop.b"),
    "mandelbrot": os.path.join(ROOT, "tests", "mandelbrot.b"),
}

FAST_FLAGS = [
    "gcc", "-O3", "-march=native",
    "-funroll-loops", "-fomit-frame-pointer",
    "-fno-stack-protector", "-ffast-math",
    "-o", "bf", "bf.c"
]


def find_days():
    return sorted(
        os.path.join(ROOT, d)
        for d in os.listdir(ROOT)
        if re.fullmatch(r"day\d+", d)
    )


def build(day_dir):
    result = subprocess.run(FAST_FLAGS, cwd=day_dir, capture_output=True)
    if result.returncode != 0:
        print(f"  [build failed]\n{result.stderr.decode()}")
        return False
    return True


def bench_once(binary, program):
    start = time.perf_counter()
    result = subprocess.run(
        [binary, program],
        stdin=subprocess.DEVNULL,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.PIPE,
        timeout=120,
    )
    elapsed = time.perf_counter() - start
    if result.returncode != 0:
        raise RuntimeError(f"interpreter exited {result.returncode}: {result.stderr.decode()[:200]}")
    return elapsed


def benchmark(day_dir, program):
    name = os.path.basename(day_dir)
    binary = os.path.join(day_dir, "bf")

    print(f"\n{name}")
    print(f"  building...", end=" ", flush=True)
    if not build(day_dir):
        return

    print("ok")

    times = []
    for i in range(RUNS):
        print(f"  run {i+1}/{RUNS}...", end=" ", flush=True)
        try:
            t = bench_once(binary, program)
            times.append(t)
            print(f"{t:.3f}s")
        except subprocess.TimeoutExpired:
            print("TIMEOUT (>120s)")
            return
        except RuntimeError as e:
            print(f"ERROR: {e}")
            return

    avg = sum(times) / len(times)
    best = min(times)
    print(f"  avg: {avg:.3f}s  best: {best:.3f}s")


if __name__ == "__main__":
    args = sys.argv[1:]

    mode = "tight_loop"
    if "--mandelbrot" in args:
        mode = "mandelbrot"
        args.remove("--mandelbrot")

    program = PROGRAMS[mode]
    if not os.path.exists(program):
        print(f"benchmark program not found: {program}")
        sys.exit(1)

    days = [os.path.join(ROOT, a) for a in args] if args else find_days()

    print(f"benchmark : {os.path.basename(program)} (output suppressed)")
    print(f"runs each : {RUNS}")

    for day in days:
        benchmark(day, program)

    print()
