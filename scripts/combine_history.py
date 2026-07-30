"""
combine_history.py

Stage 3 of the benchmark reporter pipeline.

Appends one cleaned run (produced by clean_results.py) into the
persistent history file at out/history/history.json, which accumulates
every run ever recorded, oldest first.

Usage:
    python scripts/combine_history.py                     # combines latest clean run
    python scripts/combine_history.py --clean-file out/results/clean/run_20260730120000.json
    python scripts/combine_history.py --force              # overwrite if timestamp already recorded
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Optional

from bench_common import (
    CLEAN_DIR,
    HISTORY_FILE,
    PROJECT_ROOT,
    ensure_dirs,
    latest_file,
    read_json,
    write_json,
)


def load_history() -> dict:
    if HISTORY_FILE.exists():
        return read_json(HISTORY_FILE)
    return {"runs": []}


def combine(clean_run: dict, history: dict, force: bool) -> tuple[dict, bool]:
    existing_index = next(
        (i for i, run in enumerate(history["runs"]) if run["timestamp"] == clean_run["timestamp"]),
        None,
    )

    if existing_index is not None:
        if not force:
            return history, False
        history["runs"][existing_index] = clean_run
    else:
        history["runs"].append(clean_run)

    history["runs"].sort(key=lambda run: run["timestamp"])
    return history, True


def main(argv: Optional[list[str]] = None) -> Path:
    parser = argparse.ArgumentParser(description="Combine a cleaned benchmark run into history.json.")
    parser.add_argument(
        "--clean-file",
        type=Path,
        default=None,
        help="Path to a cleaned run JSON (default: most recent file in out/results/clean)",
    )
    parser.add_argument(
        "--force",
        action="store_true",
        help="Overwrite the history entry if this timestamp was already recorded",
    )
    args = parser.parse_args(argv)

    ensure_dirs(HISTORY_FILE.parent)

    clean_path = args.clean_file or latest_file(CLEAN_DIR, "run_*.json")
    if clean_path is None or not clean_path.exists():
        print(f"[combine_history] No cleaned run found in {CLEAN_DIR}", file=sys.stderr)
        sys.exit(1)

    clean_run = read_json(clean_path)
    history = load_history()
    history, appended = combine(clean_run, history, args.force)

    if not appended:
        print(
            f"[combine_history] Run '{clean_run['timestamp']}' already in history "
            f"(use --force to overwrite). Skipping write."
        )
        return HISTORY_FILE

    write_json(HISTORY_FILE, history)
    print(
        f"[combine_history] Added run '{clean_run['timestamp']}' "
        f"({len(clean_run['benchmarks'])} benchmarks). "
        f"History now has {len(history['runs'])} run(s) -> {HISTORY_FILE.relative_to(PROJECT_ROOT)}"
    )
    return HISTORY_FILE


if __name__ == "__main__":
    main()
