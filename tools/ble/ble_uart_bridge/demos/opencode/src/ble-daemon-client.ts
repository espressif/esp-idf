// SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
// SPDX-License-Identifier: Apache-2.0

import { BLE_DAEMON_URL } from "./config"
import { isPermissionDecision } from "./opencode-permission-reply"
import type { BridgeResponse, DaemonResponse, DaemonStatus } from "./types"

/**
 * Check whether the local ESP-BLE-UART Daemon is reachable.
 *
 * This probe is intentionally silent: OpenCode loads plugins during startup, so
 * a missing optional daemon must not print fetch errors into the OpenCode UI.
 */
export async function isDaemonAvailable(): Promise<boolean> {
  try {
    await getDaemonStatus()
    return true
  } catch {
    return false
  }
}

/** Return the daemon status payload or throw if the daemon is unreachable. */
export async function getDaemonStatus(): Promise<DaemonStatus> {
  const response = await fetch(`${BLE_DAEMON_URL}/status`)
  if (!response.ok) {
    throw new Error(`ESP-BLE-UART Daemon status check failed: HTTP ${response.status}`)
  }
  return (await response.json()) as DaemonStatus
}

/**
 * Normalize the ESP-BLE-UART Daemon's response envelope into a permission response.
 *
 * The daemon supports both nested `data`/`response` envelopes and a direct
 * top-level decision. Keeping that tolerance here prevents transport quirks
 * from leaking into the permission queue code.
 */
function parseBridgeResponse(body: DaemonResponse): BridgeResponse {
  // Try nested data/response fields first (daemon envelope)
  const payload = body.data ?? body.response
  if (payload !== undefined) {
    if (typeof payload === "string") {
      return JSON.parse(payload) as BridgeResponse
    }
    if (payload && typeof payload === "object") {
      return payload as BridgeResponse
    }
  }

  // Fallback: daemon may have returned the decision at the top level
  if (
    body &&
    typeof body === "object" &&
    "decision" in body &&
    isPermissionDecision((body as BridgeResponse).decision)
  ) {
    return body as BridgeResponse
  }

  throw new Error(`ESP-BLE-UART Daemon returned an invalid response payload: ${JSON.stringify(body)}`)
}

/**
 * Send a one-way notification to the ESP-BLE-UART Daemon.
 *
 * Use this for events such as session status updates or permission cancellation,
 * where the BLE device should update its UI but OpenCode is not waiting for a
 * user decision.
 */
export async function notifyBLE(op: string, data: unknown): Promise<void> {
  const response = await fetch(`${BLE_DAEMON_URL}/notify`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ op, data }),
  })

  if (!response.ok) {
    throw new Error(`ESP-BLE-UART Daemon notify failed: HTTP ${response.status}`)
  }
}

/**
 * Send a request to the ESP-BLE-UART Daemon and wait for a structured response.
 *
 * Permission prompts use this path because OpenCode cannot continue until the
 * BLE device returns a decision or the request times out.
 */
export async function sendRequestToBLE(op: string, data: unknown, timeoutSeconds: number): Promise<BridgeResponse> {
  const response = await fetch(`${BLE_DAEMON_URL}/request`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({
      op,
      data,
      timeout: timeoutSeconds,
    }),
  })

  if (!response.ok) {
    throw new Error(`ESP-BLE-UART Daemon request failed: HTTP ${response.status}`)
  }

  return parseBridgeResponse((await response.json()) as DaemonResponse)
}
