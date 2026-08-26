// SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
// SPDX-License-Identifier: Apache-2.0

import { BLE_DAEMON_URL } from "./config"
import type { DaemonResponse } from "./types"

/**
 * Application-type detection for the connected ESP-BLE-UART device.
 *
 * The transport layer (NUS-over-BLE JSONL) is identical for every ESP-BLE-UART
 * device, so neither the daemon nor this plugin can tell a `vibe_indicator`
 * apart from any other device by connection alone. This detection is a demo
 * concern — the daemon is a generic transport — so the probe lives here in the
 * plugin: we send the `vibe_indicator` capability query and classify the device
 * from its reply.
 *
 * - A well-formed `{"count": N}` reply with `N >= 1` identifies a `vibe_indicator`.
 * - A protocol or application rejection (daemon HTTP 502) identifies a generic
 *   ESP-BLE-UART device that does not understand the indicator protocol.
 * - A write failure, timeout, or unreachable daemon leaves the type `unknown`,
 *   so the caller can retry on a later refresh.
 */

export type DeviceType = "vibe_indicator" | "generic" | "unknown"

export type DeviceProbeResult = {
  deviceType: DeviceType
  /** Number of indicator channels, only present for a `vibe_indicator`. */
  indicatorCount?: number
}

/** JSONL envelope `op` and capability query the indicator firmware answers. */
const PROBE_OP = "command"
const PROBE_DATA = { cmd: "query", type: "indicator_count" }
const PROBE_TIMEOUT_SECONDS = 5

/**
 * Probe the connected device and classify its application type.
 *
 * This goes straight to the daemon's `/request` path (rather than
 * `sendRequestToBLE`) because it needs the raw device payload (`{count}`) and
 * the daemon's HTTP status to distinguish a rejection (generic device) from a
 * transport failure (unknown), neither of which the permission-oriented response
 * parser preserves.
 */
export async function probeDeviceType(): Promise<DeviceProbeResult> {
  let response: Response
  try {
    response = await fetch(`${BLE_DAEMON_URL}/request`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ op: PROBE_OP, data: PROBE_DATA, timeout: PROBE_TIMEOUT_SECONDS }),
    })
  } catch {
    // Daemon unreachable: cannot determine the type.
    return { deviceType: "unknown" }
  }

  if (!response.ok) {
    // 502: the device rejected the indicator probe (protocol/application error)
    //   → a generic ESP-BLE-UART device.
    // 503 (write failed) / 504 (timeout) / anything else: transport could not
    //   deliver the probe, so the type stays unknown and is retried later.
    return { deviceType: response.status === 502 ? "generic" : "unknown" }
  }

  try {
    const body = (await response.json()) as DaemonResponse
    const payload = body.data
    const count =
      payload && typeof payload === "object" ? (payload as Record<string, unknown>).count : undefined
    // Reject booleans (typeof boolean !== "number") and non-integers, matching the
    // firmware's integer indicator count. Require at least one channel.
    if (typeof count === "number" && Number.isInteger(count) && count >= 1) {
      return { deviceType: "vibe_indicator", indicatorCount: count }
    }
    // Answered the probe without a usable indicator count: treat as a generic device.
    return { deviceType: "generic" }
  } catch {
    return { deviceType: "unknown" }
  }
}
