# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0

# BLE Log Functional Test

| Supported Targets |
| ----------------- |

This app uses the in-memory test peripheral (`CONFIG_BLE_LOG_PRPH_TEST=y`) to
validate the BLE Log transport on target.

It covers:

- literal protocol-v8 framing and fixed Internal Snapshot ABI;
- build, library, chip, and protocol versions inside the snapshot;
- task and critical-section writes plus HCI direction encoding;
- direct compression claim/commit, stale handles, and per-source serialization;
- oversized-record rejection, flush sequence continuity, pool exhaustion, and non-yield reserve use;
- shared log/snapshot Global SN, independent 24-bit anchor counts, and snapshot busy/loss behavior;
- successful logical-byte counts for public, claim/commit, and LL writes, plus FLUSH reset;
- enable, disable, parked-writer, and deinit races.

Run `python3 host/test_dump.py` on the host to check that console dumps include
the dedicated snapshot buffer, including after its transport has been recycled.

Variant build for the bounded pool-acquire wait
(`sdkconfig.ci.bounded_wait`, requires `CONFIG_BLE_LOG_PRPH_TEST=y`
from the base defaults):

```
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.bounded_wait" build
```

It compiles the bounded-wait tests (recycle-within-budget and
fail-after-budget) on top of the default suite; the default build keeps
the historical infinite wait (`CONFIG_BLE_LOG_POOL_WAIT_TIMEOUT_MS=-1`)
and compiles none of them.

The same suite at 1000 Hz (`sdkconfig.ci.bounded_tick_1000`, which carries
the same finite budget so the variant also works on its own in CI) checks
the tick rounding of the wait:

```
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.bounded_tick_1000" build
```

## Lifecycle regression

Run `python3 host/test_log_stop.py` from this app directory for the shared BLE Log
tests. They extract production log functions without reading controller or BTDM
sources. Coverage includes blocking/recycle, cancellation, ref/waiter handoff,
admission locking, pause/deinit draining, and shutdown flushing of queued frames.
The production timeout is checked at 100 Hz and 1000 Hz, including continued
waiting when assertions are disabled and flush timeout/state restoration.

Controller test entry points are included with their corresponding controller
fixes. Run the entries present in the branch:

```sh
for test in host/test_*.py; do
    python3 "$test" || exit 1
done
```

For backports, take the shared implementation/tests and the required controller
commits. Each controller entry reads only its group's production sources; a
missing required source or function fails the test instead of silently skipping
coverage. The shared tests run even when no controller integration is present.
The optional first argument selects another IDF source root for isolated checks.

These host models do not simulate actual multicore scheduling or DMA. The target
suite also checks that pausing a full pool releases a parked LL writer while
leaving queued logs available to the peripheral. The separate lifecycle app is a
manual integration test and requires the selected target's controller fix.
