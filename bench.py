#!/usr/bin/env python3
"""
BF interpreter benchmark — runs mandelbrot.b against every dayN directory.
Compiles with aggressive flags before timing.

Usage:
    python bench.py               # benchmark all days
    python bench.py day1 day3     # benchmark specific days
"""

import os
import re
import sys
import subprocess
import time

ROOT = os.path.dirname(os.path.abspath(__file__))
BENCH_PROGRAM = os.path.join(ROOT, "tests", "mandelbrot.b")
RUNS = 3  # average over this many runs

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


def bench_once(binary):
    start = time.perf_counter()
    result = subprocess.run([binary, BENCH_PROGRAM], capture_output=True, timeout=120)
    elapsed = time.perf_counter() - start
    if result.returncode != 0:
        raise RuntimeError(f"interpreter exited {result.returncode}: {result.stderr.decode()[:200]}")
    return elapsed, result.stdout


def benchmark(day_dir):
    name = os.path.basename(day_dir)
    binary = os.path.join(day_dir, "bf")

    print(f"\n{name}")
    print(f"  building with fast flags...", end=" ", flush=True)
    if not build(day_dir):
        return

    print("ok")

    times = []
    output = None
    for i in range(RUNS):
        print(f"  run {i+1}/{RUNS}...", end=" ", flush=True)
        try:
            t, out = bench_once(binary)
            times.append(t)
            output = out
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
    print(f"  output ({len(output)} bytes): first line = {output.split(b'\\n')[0].decode(errors='replace')!r}")


if __name__ == "__main__":
    if not os.path.exists(BENCH_PROGRAM):
        print(f"benchmark program not found: {BENCH_PROGRAM}")
        sys.exit(1)

    days = [os.path.join(ROOT, a) for a in sys.argv[1:]] if sys.argv[1:] else find_days()

    print(f"benchmark: {BENCH_PROGRAM}")
    print(f"runs per interpreter: {RUNS}")

    for day in days:
        benchmark(day)

    print()
