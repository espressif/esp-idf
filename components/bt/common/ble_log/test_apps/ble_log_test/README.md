# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0

# BLE Log Functional Test

| Supported Targets |
| ----------------- |

This app uses the in-memory test peripheral (`CONFIG_BLE_LOG_PRPH_TEST=y`) to
validate the BLE Log transport on target.

It covers:

- literal protocol-v7 framing and fixed Internal Snapshot ABI;
- build, library, chip, and protocol versions inside the snapshot;
- task and `NON_YIELD` source metadata plus HCI direction encoding;
- direct compression claim/commit, stale handles, and per-source serialization;
- oversized-record rejection, flush sequence continuity, pool exhaustion, and non-yield reserve use;
- periodic snapshot busy/loss behavior;
- enable, disable, parked-writer, and deinit races.
