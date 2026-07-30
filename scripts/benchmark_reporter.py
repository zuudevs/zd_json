"""
benchmark_reporter.py

Orchestrator for the benchmark reporter pipeline described in the
project roadmap (v0.9.0):

    run benchmark
        -> generate benchmark result (raw json)
        -> clean benchmark result (strip metadata)
        -> combine with previous results (history.json)
        -> compute benchmark result (compare vs. baseline)
        -> generate chart & summary (markdown report + PNG charts)

Each stage is also runnable standalone via its own script
(run_benchmarks.py, clean_results.py, combine_history.py,
compute_stats.py, generate_report.py) for debugging or CI use.

Usage:
    python scripts/benchmark_reporter.py                     # full pipeline, uses already-built binaries
    python scripts/benchmark_reporter.py --build              # configure+build, then full pipeline
    python scripts/benchmark_reporter.py --suites parser lexer
    python scripts/benchmark_reporter.py --threshold 3 --fail-on-regression
"""

from __future__ import annotations

import argparse
import sys

import clean_results
import combine_history
import compute_stats
import generate_report
import run_benchmarks
from bench_common import BENCHMARK_SUITES


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Run the full benchmark reporter pipeline: run -> clean -> combine -> compute -> report."
    )
    parser.add_argument("--build", action="store_true", help="Configure & build before running benchmarks")
    parser.add_argument("--preset", default="release", help="CMake preset used with --build (default: release)")
    parser.add_argument("--suites", nargs="+", choices=BENCHMARK_SUITES, default=BENCHMARK_SUITES)
    parser.add_argument("--min-time", type=float, default=None, help="--benchmark_min_time in seconds")
    parser.add_argument("--warmup-time", type=float, default=2.0)
    parser.add_argument("--filter", dest="benchmark_filter", default=None, help="--benchmark_filter regex")
    parser.add_argument("--keep-going", action="store_true", help="Skip suites that fail/are missing")
    parser.add_argument("--threshold", type=float, default=compute_stats.DEFAULT_THRESHOLD_PERCENT, help="Regression threshold in percent")
    parser.add_argument("--fail-on-regression", action="store_true", help="Exit non-zero if any benchmark regressed")
    parser.add_argument(
        "--skip-run",
        action="store_true",
        help="Skip stage 1 (run_benchmarks) and start from the raw results already on disk",
    )
    args = parser.parse_args()

    # Stage 1: run benchmark -> generate raw benchmark result (json)
    if not args.skip_run:
        run_benchmarks.main(
            [
                *(["--build"] if args.build else []),
                "--preset", args.preset,
                "--suites", *args.suites,
                *(["--min-time", str(args.min_time)] if args.min_time is not None else []),
                "--warmup-time", str(args.warmup_time),
                *(["--filter", args.benchmark_filter] if args.benchmark_filter else []),
                *(["--keep-going"] if args.keep_going else []),
            ]
        )

    # Stage 2: cleaning benchmark result (grab benchmark result only, drop metadata)
    clean_results.main([])

    # Stage 3: combine with previous benchmark results (history.json)
    combine_history.main([])

    # Stage 4: compute benchmark result (compare current vs. baseline).
    # Note: exits 0 early (no report) if history has fewer than 2 runs --
    # this run just became the new baseline.
    comparison_path = compute_stats.main(["--threshold", str(args.threshold)])
    if comparison_path is None:
        return

    # Stage 5: generate chart & summary based on baseline
    generate_report.main(["--comparison-file", str(comparison_path)])

    if args.fail_on_regression:
        from bench_common import read_json

        comparison = read_json(comparison_path)
        if comparison["summary"]["regressed"] > 0:
            sys.exit(1)


if __name__ == "__main__":
    main()
