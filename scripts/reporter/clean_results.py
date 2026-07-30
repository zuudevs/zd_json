"""
clean_results.py

Stage 2 of the benchmark reporter pipeline.

Reads the raw Google Benchmark JSON files for one "batch" (one run of
run_benchmarks.py, grouped by shared timestamp suffix) and keeps only
the actual benchmark measurements, dropping the "context" block
(hostname, CPU caches, load average, library build type, etc.) that
Google Benchmark embeds in every file.

Each benchmark entry is tagged with its source "suite" so later stages
can group/filter by suite.

Usage:
    python scripts/reporter/clean_results.py                # cleans the latest batch
    python scripts/reporter/clean_results.py --timestamp 20260730120000
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path
from typing import Optional

# Ensure project root is in sys.path when script is executed directly
_PROJECT_ROOT = Path(__file__).resolve().parents[2]
if str(_PROJECT_ROOT) not in sys.path:
    sys.path.insert(0, str(_PROJECT_ROOT))

from scripts.reporter.bench_common import (
    CLEAN_DIR,
    PROJECT_ROOT,
    RAW_DIR,
    ensure_dirs,
    git_commit,
    read_json,
    timestamp_to_iso,
    write_json,
)

_RAW_NAME_RE = re.compile(r"^(?P<suite>.+)_(?P<ts>\d{14})\.json$")


def discover_batches(raw_dir: Path) -> dict[str, list[Path]]:
    """Group raw result files in raw_dir by their shared timestamp suffix."""
    batches: dict[str, list[Path]] = {}
    for path in raw_dir.glob("*.json"):
        match = _RAW_NAME_RE.match(path.name)
        if not match:
            continue
        batches.setdefault(match.group("ts"), []).append(path)
    return batches


def clean_batch(timestamp: str, raw_files: list[Path]) -> dict:
    benchmarks: list[dict] = []
    suites: list[str] = []

    for raw_path in sorted(raw_files):
        match = _RAW_NAME_RE.match(raw_path.name)
        suite = match.group("suite") if match else raw_path.stem
        suites.append(suite)

        raw_data = read_json(raw_path)
        # This is the "cleaning" step: keep raw_data["benchmarks"] only,
        # dropping raw_data["context"] (host/CPU/build metadata).
        for entry in raw_data.get("benchmarks", []):
            tagged = {"suite": suite, **entry}
            benchmarks.append(tagged)

    return {
        "timestamp": timestamp,
        "datetime": timestamp_to_iso(timestamp),
        "git_commit": git_commit(),
        "suites": sorted(suites),
        "benchmarks": benchmarks,
    }


def main(argv: Optional[list[str]] = None) -> Path:
    parser = argparse.ArgumentParser(description="Clean raw benchmark JSON (strip metadata/context).")
    parser.add_argument(
        "--timestamp",
        default=None,
        help="Batch timestamp to clean (default: most recent batch found in assets/benchmark_results/results/raw)",
    )
    args = parser.parse_args(argv)

    ensure_dirs(CLEAN_DIR)

    batches = discover_batches(RAW_DIR)
    if not batches:
        print(f"[clean_results] No raw results found in {RAW_DIR}", file=sys.stderr)
        sys.exit(1)

    timestamp = args.timestamp or max(batches.keys())
    if timestamp not in batches:
        print(f"[clean_results] No raw results found for batch '{timestamp}'", file=sys.stderr)
        sys.exit(1)

    cleaned = clean_batch(timestamp, batches[timestamp])
    out_path = CLEAN_DIR / f"run_{timestamp}.json"
    write_json(out_path, cleaned)

    print(
        f"[clean_results] Cleaned {len(cleaned['benchmarks'])} benchmark(s) "
        f"from {len(batches[timestamp])} suite(s) -> {out_path.relative_to(PROJECT_ROOT)}"
    )
    return out_path


if __name__ == "__main__":
    main()
