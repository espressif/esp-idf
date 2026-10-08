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
