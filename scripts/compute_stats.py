"""
compute_stats.py

Stage 4 of the benchmark reporter pipeline.

Compares the latest run in out/history/history.json against the
baseline run. Baseline = the OLDEST run ever recorded (history["runs"][0]),
per project convention: performance is always tracked relative to the
first measurement taken, not the previous run.

For every benchmark present in both runs, computes the percent change
in real_time (converted to nanoseconds so runs using different
time_unit values remain comparable) and classifies it as:

    regressed  -> slower than baseline by more than --threshold percent
    improved   -> faster than baseline by more than --threshold percent
    stable     -> within +/- --threshold percent

Benchmarks only present in one of the two runs are reported separately
as "new" or "removed".

Usage:
    python scripts/compute_stats.py
    python scripts/compute_stats.py --threshold 3.0
    python scripts/compute_stats.py --current-index -2   # compare an older run instead of latest
"""

from __future__ import annotations

import argparse
import sys
from typing import Optional

from bench_common import (
    COMPARISON_DIR,
    HISTORY_FILE,
    PROJECT_ROOT,
    BenchmarkEntry,
    ensure_dirs,
    read_json,
    write_json,
)

DEFAULT_THRESHOLD_PERCENT = 5.0


def classify(percent_change: float, threshold: float) -> str:
    if percent_change > threshold:
        return "regressed"
    if percent_change < -threshold:
        return "improved"
    return "stable"


def compute(baseline_run: dict, current_run: dict, threshold: float) -> dict:
    baseline_entries = {e.name: e for e in map(BenchmarkEntry.from_dict, baseline_run["benchmarks"])}
    current_entries = {e.name: e for e in map(BenchmarkEntry.from_dict, current_run["benchmarks"])}

    baseline_names = set(baseline_entries)
    current_names = set(current_entries)

    records = []
    for name in sorted(baseline_names & current_names):
        base = baseline_entries[name]
        curr = current_entries[name]
        base_ns = base.real_time_ns
        curr_ns = curr.real_time_ns
        percent_change = ((curr_ns - base_ns) / base_ns * 100.0) if base_ns else 0.0

        records.append(
            {
                "suite": curr.suite,
                "name": name,
                "baseline_real_time_ns": base_ns,
                "current_real_time_ns": curr_ns,
                "percent_change": percent_change,
                "status": classify(percent_change, threshold),
            }
        )

    summary = {
        "total_compared": len(records),
        "regressed": sum(1 for r in records if r["status"] == "regressed"),
        "improved": sum(1 for r in records if r["status"] == "improved"),
        "stable": sum(1 for r in records if r["status"] == "stable"),
        "new": sorted(current_names - baseline_names),
        "removed": sorted(baseline_names - current_names),
    }

    return {
        "baseline_timestamp": baseline_run["timestamp"],
        "baseline_git_commit": baseline_run.get("git_commit"),
        "current_timestamp": current_run["timestamp"],
        "current_git_commit": current_run.get("git_commit"),
        "threshold_percent": threshold,
        "summary": summary,
        "records": records,
    }


def main(argv: Optional[list[str]] = None):
    parser = argparse.ArgumentParser(description="Compare latest benchmark run against the baseline run.")
    parser.add_argument("--threshold", type=float, default=DEFAULT_THRESHOLD_PERCENT, help="Regression threshold in percent (default: 5.0)")
    parser.add_argument("--baseline-index", type=int, default=0, help="Index into history['runs'] used as baseline (default: 0 = oldest)")
    parser.add_argument("--current-index", type=int, default=-1, help="Index into history['runs'] used as current run (default: -1 = latest)")
    parser.add_argument("--fail-on-regression", action="store_true", help="Exit with code 1 if any benchmark regressed beyond the threshold")
    args = parser.parse_args(argv)

    ensure_dirs(COMPARISON_DIR)

    if not HISTORY_FILE.exists():
        print(f"[compute_stats] No history file found at {HISTORY_FILE}", file=sys.stderr)
        sys.exit(1)

    history = read_json(HISTORY_FILE)
    runs = history.get("runs", [])
    if len(runs) < 2:
        print(
            f"[compute_stats] Only {len(runs)} run(s) in history; need at least 2 "
            f"(this run establishes the baseline). Nothing to compare yet.",
            file=sys.stderr,
        )
        sys.exit(0)

    baseline_run = runs[args.baseline_index]
    current_run = runs[args.current_index]

    if baseline_run["timestamp"] == current_run["timestamp"]:
        print("[compute_stats] Baseline and current run resolve to the same run; nothing to compare.", file=sys.stderr)
        sys.exit(0)

    comparison = compute(baseline_run, current_run, args.threshold)

    out_path = COMPARISON_DIR / f"comparison_{current_run['timestamp']}.json"
    write_json(out_path, comparison)

    summary = comparison["summary"]
    print(
        f"[compute_stats] baseline={comparison['baseline_timestamp']} "
        f"current={comparison['current_timestamp']} threshold=+/-{args.threshold}%"
    )
    print(
        f"[compute_stats] regressed={summary['regressed']} improved={summary['improved']} "
        f"stable={summary['stable']} new={len(summary['new'])} removed={len(summary['removed'])}"
    )
    print(f"[compute_stats] -> {out_path.relative_to(PROJECT_ROOT)}")

    if args.fail_on_regression and summary["regressed"] > 0:
        sys.exit(1)

    return out_path


if __name__ == "__main__":
    main()
