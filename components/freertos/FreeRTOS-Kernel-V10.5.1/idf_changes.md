# ESP-IDF Changes

This document is used to track all changes made the to FreeRTOS V10.5.1 source code when adding dual core SMP support or IDF additional features.

## License Headers

- Added `SPDX-FileCopyrightText` and `SPDX-FileContributor` tags to all files to pass ESP-IDF pre-commit checks.
- Changed kernel version tag to `FreeRTOS Kernel V10.5.1 (ESP-IDF SMP modified)` in all files to indicate files are different from upstream V10.5.1.

## Omitted Features

- Removed `croutine.c` and `croutine.h`

## Changes from Upstream Main Branch not Included in v10.5.1

- Added ...GetStaticBuffers functions that were upstreamed but not included in v10.5.1