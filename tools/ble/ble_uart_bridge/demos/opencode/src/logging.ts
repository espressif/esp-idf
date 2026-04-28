// SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
// SPDX-License-Identifier: Apache-2.0

import { DEBUG } from "./config"
import type { OpenCodePermissionClient } from "./types"

/**
 * Write local debug logs only when explicitly enabled.
 *
 * Demo plugins often need extra observability while being developed, but they
 * should stay quiet by default when users copy them into normal OpenCode use.
 */
export function debugLog(message: string, data?: unknown) {
  if (!DEBUG) {
    return
  }
  if (data === undefined) {
    console.info(`[opencode-ble] ${message}`)
    return
  }
  console.info(`[opencode-ble] ${message}`, data)
}

/**
 * Prefer OpenCode's application log API, with console fallback for older clients.
 *
 * The partial client type means a copied demo can still run against OpenCode
 * versions that do not expose every helper method used by newer SDKs.
 */
export async function appLog(
  client: OpenCodePermissionClient,
  level: "debug" | "info" | "warn" | "error",
  message: string,
  extra?: Record<string, unknown>,
) {
  if (typeof client.app?.log === "function") {
    await client.app.log({
      body: {
        service: "opencode-ble",
        level,
        message,
        extra,
      },
    })
    return
  }
  if (level === "error") {
    console.error(`[opencode-ble] ${message}`, extra)
  } else if (level === "warn") {
    console.warn(`[opencode-ble] ${message}`, extra)
  } else {
    debugLog(message, extra)
  }
}

/**
 * Best-effort logging wrapper used on error paths.
 *
 * Permission handling should not fail just because the logging endpoint is
 * unavailable, so this helper catches log write errors and reports them locally.
 */
export async function appLogBestEffort(
  client: OpenCodePermissionClient,
  level: "debug" | "info" | "warn" | "error",
  message: string,
  extra?: Record<string, unknown>,
) {
  try {
    await appLog(client, level, message, extra)
  } catch (error) {
    console.warn(`[opencode-ble] failed to write app log: ${message}`, error)
  }
}
