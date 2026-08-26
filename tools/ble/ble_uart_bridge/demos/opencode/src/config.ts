// SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
// SPDX-License-Identifier: Apache-2.0

/** Maximum number of characters sent for one metadata value shown on the BLE device. */
export const MAX_METADATA_VALUE_CHARS = 512

/** Fallback display type when OpenCode does not provide a specific permission type. */
export const DEFAULT_PERMISSION_TYPE = "unknown"

/** Fallback title shown on the BLE device when the permission event has no title. */
export const DEFAULT_PERMISSION_TITLE = "Permission request"

/**
 * Default rejection message sent back to OpenCode when the BLE path rejects or fails.
 *
 * Keep this message non-empty: OpenCode treats a bare reject as a hard
 * PermissionRejectedError that can block the current assistant turn. A reject
 * with feedback behaves like a corrective denial and lets later serial tool
 * requests continue.
 */
export const DEFAULT_REJECT_MESSAGE = "Rejected from BLE device"

/**
 * Metadata keys that are most useful on a small BLE screen.
 *
 * The bridge shows at most one metadata entry, so these keys are prioritized
 * before falling back to the first available string metadata value.
 */
export const METADATA_DISPLAY_KEYS = ["command", "path", "url"] as const

/** HTTP base URL of the local ESP-BLE-UART Daemon that bridges OpenCode to the BLE device. */
export const BLE_DAEMON_URL = process.env.OPENCODE_BLE_DAEMON_URL ?? "http://127.0.0.1:8888"

const DEFAULT_DECISION_TIMEOUT_SECONDS = 60

function parseDecisionTimeoutSeconds(value: string | undefined): number {
  const parsed = Number(value ?? String(DEFAULT_DECISION_TIMEOUT_SECONDS))
  return Number.isFinite(parsed) && parsed > 0 ? parsed : DEFAULT_DECISION_TIMEOUT_SECONDS
}

/** Maximum time to wait for a user decision from the BLE device. */
export const DECISION_TIMEOUT_SECONDS = parseDecisionTimeoutSeconds(process.env.OPENCODE_BLE_DECISION_TIMEOUT_SECONDS)

/** Message used when later prompts are skipped after an earlier same-session reject. */
export const CONCURRENT_REJECT_MESSAGE = "Another concurrent permission request was rejected"
