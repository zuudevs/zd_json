"""
run_benchmarks.py

Stage 1 of the benchmark reporter pipeline.

Runs the zd_json_*_benchmarks executables (built via CMake) with
--benchmark_out_format=json and stores one raw result file per suite
under out/results/raw/, all sharing the same batch timestamp so later
stages can group them into a single "run".

Usage:
    python scripts/run_benchmarks.py
    python scripts/run_benchmarks.py --build --preset release
    python scripts/run_benchmarks.py --suites parser lexer --min-time 0.5
"""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path
from typing import Optional

from bench_common import (
    BIN_DIR,
    PROJECT_ROOT,
    RAW_DIR,
    BENCHMARK_SUITES,
    ensure_dirs,
    now_timestamp,
    suite_executable,
)


def build_project(preset: str) -> None:
    print(f"[run_benchmarks] Configuring & building preset '{preset}' ...")
    subprocess.run(
        ["cmake", "--preset", preset, "-DZD_JSON_BUILD_BENCHMARKS=ON"],
        cwd=PROJECT_ROOT,
        check=True,
    )
    subprocess.run(
        ["cmake", "--build", "--preset", preset, "--parallel"],
        cwd=PROJECT_ROOT,
        check=True,
    )


def run_suite(
    suite: str,
    batch_ts: str,
    min_time: Optional[float],
    benchmark_filter: Optional[str],
    warmup_time: float,
    keep_going: bool,
) -> Optional[Path]:
    exe = BIN_DIR / suite_executable(suite)
    if not exe.exists():
        exe_win = exe.with_suffix(".exe")
        exe = exe_win if exe_win.exists() else exe

    if not exe.exists():
        message = f"[run_benchmarks] SKIP '{suite}': executable not found at {exe}"
        if keep_going:
            print(message + " (use --build to compile first)")
            return None
        raise FileNotFoundError(message + " (use --build to compile first)")

    out_path = RAW_DIR / f"{suite}_{batch_ts}.json"
    args = [
        str(exe),
        f"--benchmark_out={out_path}",
        "--benchmark_out_format=json",
        f"--benchmark_min_warmup_time={warmup_time}",
    ]
    if min_time is not None:
        args.append(f"--benchmark_min_time={min_time}s")
    if benchmark_filter:
        args.append(f"--benchmark_filter={benchmark_filter}")

    print(f"[run_benchmarks] Running suite '{suite}' -> {out_path.relative_to(PROJECT_ROOT)}")
    result = subprocess.run(args, cwd=PROJECT_ROOT)
    if result.returncode != 0:
        message = f"[run_benchmarks] Suite '{suite}' exited with code {result.returncode}"
        if keep_going:
            print(message + " (continuing)")
            return None
        raise RuntimeError(message)

    return out_path


def main(argv: Optional[list[str]] = None) -> tuple[str, list[Path]]:
    parser = argparse.ArgumentParser(description="Run zd_json benchmark suites and emit raw JSON.")
    parser.add_argument("--build", action="store_true", help="Configure & build before running")
    parser.add_argument("--preset", default="release", help="CMake preset used with --build (default: release)")
    parser.add_argument(
        "--suites",
        nargs="+",
        choices=BENCHMARK_SUITES,
        default=BENCHMARK_SUITES,
        help="Subset of suites to run (default: all)",
    )
    parser.add_argument("--min-time", type=float, default=None, help="--benchmark_min_time in seconds")
    parser.add_argument("--warmup-time", type=float, default=2.0, help="--benchmark_min_warmup_time in seconds")
    parser.add_argument("--filter", dest="benchmark_filter", default=None, help="--benchmark_filter regex")
    parser.add_argument(
        "--keep-going",
        action="store_true",
        help="Skip suites that fail/are missing instead of aborting the whole run",
    )
    args = parser.parse_args(argv)

    ensure_dirs(RAW_DIR)

    if args.build:
        build_project(args.preset)

    batch_ts = now_timestamp()
    produced: list[Path] = []
    for suite in args.suites:
        out_path = run_suite(
            suite,
            batch_ts,
            args.min_time,
            args.benchmark_filter,
            args.warmup_time,
            args.keep_going,
        )
        if out_path is not None:
            produced.append(out_path)

    if not produced:
        print("[run_benchmarks] No benchmark results were produced.", file=sys.stderr)
        sys.exit(1)

    print(f"[run_benchmarks] Batch '{batch_ts}' complete: {len(produced)} suite(s) produced.")
    return batch_ts, produced


if __name__ == "__main__":
    main()
