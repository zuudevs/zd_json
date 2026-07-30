"""
generate_report.py

Stage 5 (final) of the benchmark reporter pipeline.

Reads a comparison JSON (produced by compute_stats.py) and renders:

  * A Markdown report at assets/benchmark_results/reports/report_<timestamp>.md
    (also copied to assets/benchmark_results/reports/report_latest.md for convenience),
    with a summary table and a detailed per-benchmark table.

  * One PNG bar chart per suite at assets/benchmark_results/reports/charts/<suite>_<timestamp>.png,
    comparing baseline vs. current real_time per benchmark, using
    matplotlib.

Usage:
    python scripts/reporter/generate_report.py
    python scripts/reporter/generate_report.py --comparison-file assets/benchmark_results/results/comparison/comparison_20260730120000.json
"""

from __future__ import annotations

import argparse
import sys
from collections import defaultdict
from pathlib import Path
from typing import Optional

# Ensure project root is in sys.path when script is executed directly
_PROJECT_ROOT = Path(__file__).resolve().parents[2]
if str(_PROJECT_ROOT) not in sys.path:
    sys.path.insert(0, str(_PROJECT_ROOT))

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402

from scripts.reporter.bench_common import (
    CHARTS_DIR,
    COMPARISON_DIR,
    PROJECT_ROOT,
    REPORTS_DIR,
    ensure_dirs,
    latest_file,
    read_json,
)

STATUS_EMOJI = {"regressed": "🔴", "improved": "🟢", "stable": "⚪"}
STATUS_COLOR = {"regressed": "#d9534f", "improved": "#5cb85c", "stable": "#999999"}


def format_ns(value_ns: float) -> str:
    if value_ns >= 1_000_000:
        return f"{value_ns / 1_000_000:.3f} ms"
    if value_ns >= 1_000:
        return f"{value_ns / 1_000:.3f} us"
    return f"{value_ns:.1f} ns"


def render_markdown(comparison: dict) -> str:
    summary = comparison["summary"]
    threshold = comparison["threshold_percent"]
    lines: list[str] = []

    lines.append("# Benchmark Report")
    lines.append("")
    lines.append(f"- **Baseline run:** `{comparison['baseline_timestamp']}` (commit `{comparison['baseline_git_commit'] or 'n/a'}`)")
    lines.append(f"- **Current run:** `{comparison['current_timestamp']}` (commit `{comparison['current_git_commit'] or 'n/a'}`)")
    lines.append(f"- **Regression threshold:** ±{threshold}%")
    lines.append("")

    lines.append("## Summary")
    lines.append("")
    lines.append("| Regressed 🔴 | Improved 🟢 | Stable ⚪ | New | Removed |")
    lines.append("|---|---|---|---|---|")
    lines.append(
        f"| {summary['regressed']} | {summary['improved']} | {summary['stable']} "
        f"| {len(summary['new'])} | {len(summary['removed'])} |"
    )
    lines.append("")

    if summary["regressed"] > 0:
        lines.append(f"> ⚠️ **{summary['regressed']} benchmark(s) regressed beyond the ±{threshold}% threshold.**")
        lines.append("")

    records_by_suite: dict[str, list[dict]] = defaultdict(list)
    for record in comparison["records"]:
        records_by_suite[record["suite"]].append(record)

    for suite in sorted(records_by_suite):
        lines.append(f"## Suite: `{suite}`")
        lines.append("")
        lines.append(f"![{suite} chart](charts/{suite}_{comparison['current_timestamp']}.png)")
        lines.append("")
        lines.append("| Benchmark | Baseline | Current | Change | Status |")
        lines.append("|---|---|---|---|---|")
        for record in sorted(records_by_suite[suite], key=lambda r: r["name"]):
            lines.append(
                f"| {record['name']} "
                f"| {format_ns(record['baseline_real_time_ns'])} "
                f"| {format_ns(record['current_real_time_ns'])} "
                f"| {record['percent_change']:+.2f}% "
                f"| {STATUS_EMOJI[record['status']]} {record['status']} |"
            )
        lines.append("")

    if summary["new"]:
        lines.append("## New benchmarks (no baseline data)")
        lines.append("")
        for name in summary["new"]:
            lines.append(f"- {name}")
        lines.append("")

    if summary["removed"]:
        lines.append("## Removed benchmarks (present in baseline only)")
        lines.append("")
        for name in summary["removed"]:
            lines.append(f"- {name}")
        lines.append("")

    return "\n".join(lines)


def render_charts(comparison: dict) -> list[Path]:
    ensure_dirs(CHARTS_DIR)
    current_ts = comparison["current_timestamp"]

    records_by_suite: dict[str, list[dict]] = defaultdict(list)
    for record in comparison["records"]:
        records_by_suite[record["suite"]].append(record)

    chart_paths: list[Path] = []
    for suite, records in records_by_suite.items():
        records = sorted(records, key=lambda r: r["name"])
        names = [r["name"] for r in records]
        baseline_ns = [r["baseline_real_time_ns"] for r in records]
        current_ns = [r["current_real_time_ns"] for r in records]
        colors = [STATUS_COLOR[r["status"]] for r in records]

        fig_height = max(2.5, 0.4 * len(names))
        fig, ax = plt.subplots(figsize=(9, fig_height))

        y_positions = range(len(names))
        bar_height = 0.35

        ax.barh(
            [y + bar_height / 2 for y in y_positions],
            baseline_ns,
            height=bar_height,
            color="#bbbbbb",
            label="Baseline",
        )
        ax.barh(
            [y - bar_height / 2 for y in y_positions],
            current_ns,
            height=bar_height,
            color=colors,
            label="Current",
        )

        ax.set_yticks(list(y_positions))
        ax.set_yticklabels(names, fontsize=8)
        ax.set_xlabel("real_time (ns)")
        ax.set_title(f"{suite} — baseline vs current")
        ax.legend(loc="lower right")
        ax.invert_yaxis()
        fig.tight_layout()

        out_path = CHARTS_DIR / f"{suite}_{current_ts}.png"
        fig.savefig(out_path, dpi=150)
        plt.close(fig)
        chart_paths.append(out_path)

    return chart_paths


def main(argv: Optional[list[str]] = None) -> Path:
    parser = argparse.ArgumentParser(description="Generate Markdown report + charts from a benchmark comparison.")
    parser.add_argument(
        "--comparison-file",
        type=Path,
        default=None,
        help="Path to a comparison JSON (default: most recent file in assets/benchmark_results/results/comparison)",
    )
    args = parser.parse_args(argv)

    ensure_dirs(REPORTS_DIR, CHARTS_DIR)

    comparison_path = args.comparison_file or latest_file(COMPARISON_DIR, "comparison_*.json")
    if comparison_path is None or not comparison_path.exists():
        print(f"[generate_report] No comparison file found in {COMPARISON_DIR}", file=sys.stderr)
        sys.exit(1)

    comparison = read_json(comparison_path)

    render_charts(comparison)
    markdown = render_markdown(comparison)

    report_path = REPORTS_DIR / f"report_{comparison['current_timestamp']}.md"
    report_path.write_text(markdown, encoding="utf-8")

    latest_path = REPORTS_DIR / "report_latest.md"
    latest_path.write_text(markdown, encoding="utf-8")

    print(f"[generate_report] Report written -> {report_path.relative_to(PROJECT_ROOT)}")
    print(f"[generate_report] Also updated  -> {latest_path.relative_to(PROJECT_ROOT)}")
    return report_path


if __name__ == "__main__":
    main()
