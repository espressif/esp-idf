| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# Minimal Baseline Size Metrics App

This is a **build-only** integration app for size metrics tracking. It exercises the absolute minimum SDK stack:
- `app_main()` entry point
- `esp_system` and its core dependencies

## Purpose

Establish a floor measurement for the core ESP-IDF SDK and detect regressions in:
- Toolchain changes (compiler, linker)
- newlib updates
- Linker script changes
- Core system component dependencies

## Source Frozen

This app is **source-frozen**. The application code should never change unless forced by SDK API changes (e.g., breaking API changes at major version boundaries). Any size change detected in CI is attributable to SDK changes, not application code evolution.

## Configuration

- Built with `-Os` (size optimization)
- Disables WDT, assertions, and other noise-inducing features
- Trimmed build: only includes `main` component and its dependencies
- Targets: ESP32 (Xtensa) and ESP32-C6 (RISC-V)

## Metrics Tracked

- Total flash usage
- Total DRAM usage
- Total IRAM usage

Metrics are collected from the `.map` file using `esp_idf_size` and uploaded via `esp-metrics-cli`.
