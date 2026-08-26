# ESP-IDF Size Metrics Collection

Collects code size metrics for a curated set of apps and uploads them to the
esp-metrics database once per CI pipeline.

## How it works

1. **Extraction (per app, in build jobs):** `IdfCMakeApp` (`tools/ci/idf_ci_local/app.py`)
   calls `collect_build_metrics.build_raw_metrics_for_build_dir()` in
   `_post_build()`, while the map files still exist. For apps listed in
   `size_metrics_config.yml` it runs `esp-idf-size --archives --format json2`
   on the app map file and adds best-effort bootloader size JSON and `.bin`
   size for apps that explicitly opt in. The raw
   `build*/build_size_metrics.json` artifact is written in `_finalize()`, after
   build-dir cleanup, and collected via `**/build*/build_size_metrics.json` in
   `.dynamic_jobs.yml`.
2. **Aggregation + upload (single `app-size-metrics` job):** runs after all
   builds (`test_child_pipeline.yml`). `generate_metrics.py` aggregates the
   per-build fragments into one schema-shaped `metrics.json` (next to
   `schema.yaml`), then `esp-metrics-cli upload -d schema.yaml -i metrics.json`.
   One DB write per pipeline; `esp-metrics-cli` + credentials live in this job
   only.

The `size_*.json` written by idf-build-apps is left untouched — other CI
consumers (e.g. the binary-size diff in MR build reports) depend on its default
format.

## Metric types

Only apps listed in `size_metrics_config.yml` publish anything; all other built
apps are ignored.

| Family | What | When |
|---|---|---|
| `app_size` | Total flash/DRAM/IRAM usage | every listed app |
| `library_size` | Per-`.a`-archive (= per-component) breakdown | listed apps with a `libs` key |
| `bootloader_size` | `bootloader.bin` size | listed apps with `bootloader: true` |

## Config (`size_metrics_config.yml`)

```yaml
tools/test_apps/system/startup:          # bare entry -> app_size only
examples/get-started/hello_world:
  libs: all                              # every archive (empty `libs:` works too)
  bootloader: true                       # include bootloader.bin size
  configs:                               # only these build configs (omit -> all)
    - default
examples/wifi/getting_started/station:
  libs:                                  # only these archives
    - libesp_wifi.a
    - libnet80211.a
  targets:                               # only these targets (omit -> all)
    - esp32c6
```

By default every built target/config of a listed app publishes metrics. The
optional `configs` and `targets` lists restrict publishing to specific build
configs/targets; `libs`/`bootloader` apply to every matching build.

## Aggregating Fragments

```bash
cd tools/ci/metrics/size_metrics
python3 generate_metrics.py  # fragments -> aggregated metrics.json (no upload)
```

`generate_metrics.py` accepts `--config /path/to/config.yml`.

## Troubleshooting

- **No metrics:** Is the app listed in `size_metrics_config.yml`? Do the build
  dirs still contain the app `.map` files (extraction must run before cleanup)?
- **No `library_size`:** Does the app's entry have a `libs` key? Does the name
  match the archive exactly (e.g. `libesp_timer.a`)? Is PyYAML installed?
