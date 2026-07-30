"""
bench_common.py

Shared paths, constants, and small helpers used across the benchmark
reporter pipeline:

    run_benchmarks.py  -> clean_results.py -> combine_history.py
        -> compute_stats.py -> generate_report.py

All scripts can be run standalone, but benchmark_reporter.py wires them
together end to end.
"""

from __future__ import annotations

import json
import re
import subprocess
from dataclasses import dataclass, field
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Optional

# ---------------------------------------------------------------------------
# Paths (mirrors cmake/output.cmake and cmake/targets.cmake conventions)
# ---------------------------------------------------------------------------

PROJECT_ROOT = Path(__file__).resolve().parent.parent

OUT_DIR = PROJECT_ROOT / "out"
BIN_DIR = OUT_DIR / "bin"

RESULTS_DIR = OUT_DIR / "results"
RAW_DIR = RESULTS_DIR / "raw"
CLEAN_DIR = RESULTS_DIR / "clean"
COMPARISON_DIR = RESULTS_DIR / "comparison"

HISTORY_DIR = OUT_DIR / "history"
HISTORY_FILE = HISTORY_DIR / "history.json"

REPORTS_DIR = OUT_DIR / "reports"
CHARTS_DIR = REPORTS_DIR / "charts"

# suite name -> executable name, matches add_benchmark_target() calls in
# cmake/targets.cmake (executable is always zd_json_{suite}_benchmarks)
BENCHMARK_SUITES = [
    "tokenizer",
    "lexer",
    "parser",
    "models",
    "serializer",
    "real_datasets",
]

TIMESTAMP_FORMAT = "%Y%m%d%H%M%S"

# Google Benchmark time_unit -> nanoseconds multiplier
_TIME_UNIT_TO_NS = {
    "ns": 1.0,
    "us": 1_000.0,
    "ms": 1_000_000.0,
    "s": 1_000_000_000.0,
}


def suite_executable(suite: str) -> str:
    return f"zd_json_{suite}_benchmarks"


def executable_to_suite(exe_name: str) -> Optional[str]:
    match = re.fullmatch(r"zd_json_(.+)_benchmarks", exe_name)
    return match.group(1) if match else None


def now_timestamp() -> str:
    return datetime.now(timezone.utc).strftime(TIMESTAMP_FORMAT)


def timestamp_to_iso(ts: str) -> str:
    try:
        return (
            datetime.strptime(ts, TIMESTAMP_FORMAT)
            .replace(tzinfo=timezone.utc)
            .isoformat()
        )
    except ValueError:
        return ts


def to_nanoseconds(value: float, time_unit: str) -> float:
    factor = _TIME_UNIT_TO_NS.get(time_unit, 1.0)
    return value * factor


def git_commit(short: bool = True) -> Optional[str]:
    """Best-effort short git commit hash. Returns None outside a git repo."""
    try:
        args = ["git", "rev-parse", "--short", "HEAD"] if short else ["git", "rev-parse", "HEAD"]
        result = subprocess.run(
            args,
            cwd=PROJECT_ROOT,
            capture_output=True,
            text=True,
            check=True,
            timeout=5,
        )
        return result.stdout.strip() or None
    except Exception:
        return None


def ensure_dirs(*dirs: Path) -> None:
    for directory in dirs:
        directory.mkdir(parents=True, exist_ok=True)


def read_json(path: Path) -> Any:
    with path.open("r", encoding="utf-8") as handle:
        return json.load(handle)


def write_json(path: Path, data: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8") as handle:
        json.dump(data, handle, indent=2, ensure_ascii=False)
        handle.write("\n")


def latest_file(directory: Path, pattern: str) -> Optional[Path]:
    candidates = sorted(directory.glob(pattern))
    return candidates[-1] if candidates else None


@dataclass
class BenchmarkEntry:
    """A single Google Benchmark result, tagged with its source suite."""

    suite: str
    name: str
    real_time: float
    cpu_time: float
    time_unit: str
    iterations: Optional[int] = None
    extra: dict[str, Any] = field(default_factory=dict)

    @property
    def real_time_ns(self) -> float:
        return to_nanoseconds(self.real_time, self.time_unit)

    @property
    def cpu_time_ns(self) -> float:
        return to_nanoseconds(self.cpu_time, self.time_unit)

    def to_dict(self) -> dict[str, Any]:
        payload = {
            "suite": self.suite,
            "name": self.name,
            "real_time": self.real_time,
            "cpu_time": self.cpu_time,
            "time_unit": self.time_unit,
            "iterations": self.iterations,
        }
        payload.update(self.extra)
        return payload

    @staticmethod
    def from_dict(data: dict[str, Any]) -> "BenchmarkEntry":
        known = {"suite", "name", "real_time", "cpu_time", "time_unit", "iterations"}
        return BenchmarkEntry(
            suite=data.get("suite", "unknown"),
            name=data["name"],
            real_time=data["real_time"],
            cpu_time=data["cpu_time"],
            time_unit=data.get("time_unit", "ns"),
            iterations=data.get("iterations"),
            extra={k: v for k, v in data.items() if k not in known},
        )
