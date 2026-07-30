# Benchmark Reporter (v0.9.0)

Implements the roadmap items under **v0.9.0 Benchmark Reporter**:
save history, compare results, detect regressions, generate a Markdown
report, and generate charts.

## Pipeline

```
run benchmark
    -> generate benchmark result (raw json)         [run_benchmarks.py]
    -> cleaning benchmark result (strip metadata)    [clean_results.py]
    -> combine with previous benchmark results       [combine_history.py]
    -> compute benchmark result                      [compute_stats.py]
    -> generate chart & summary (based on baseline)  [generate_report.py]
```

**Baseline** = the oldest run ever recorded in history (`history["runs"][0]`).
Every new run is compared against that first measurement, not the
previous run.

## Setup

```bash
pip install -r scripts/requirements.txt
```

## Quick start

```bash
# Build + run all suites + full pipeline in one go
python scripts/benchmark_reporter.py --build

# Already built? Just run the pipeline
python scripts/benchmark_reporter.py

# Only some suites, custom regression threshold, fail CI on regression
python scripts/benchmark_reporter.py --suites parser lexer --threshold 3 --fail-on-regression
```

Output layout (all under `out/`, matching the existing CMake `out/bin`,
`out/lib` convention):

```
out/results/raw/<suite>_<timestamp>.json         # raw Google Benchmark JSON, one per suite
out/results/clean/run_<timestamp>.json           # one run, metadata stripped, suites combined
out/history/history.json                         # every run ever recorded, oldest first
out/results/comparison/comparison_<timestamp>.json
out/reports/report_<timestamp>.md
out/reports/report_latest.md                     # always the most recent report
out/reports/charts/<suite>_<timestamp>.png
```

## Running stages individually

Each stage is also a standalone script, useful for debugging or a
custom CI flow:

```bash
python scripts/run_benchmarks.py --build --suites parser
python scripts/clean_results.py
python scripts/combine_history.py
python scripts/compute_stats.py --threshold 5
python scripts/generate_report.py
```

The first-ever run only establishes the baseline: `compute_stats.py`
prints a message and exits 0 without producing a comparison/report
once history has fewer than 2 runs.

## Notes

- `git_commit` in each history entry is best-effort (`None` outside a
  git repository).
- Regression/improvement classification compares `real_time`
  normalized to nanoseconds, so runs mixing `ns`/`us`/`ms` benchmarks
  stay comparable.
- `--fail-on-regression` (on `benchmark_reporter.py` or
  `compute_stats.py`) is meant for CI: the report is still generated
  before the process exits non-zero.
