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
 * Prefer OpenCode's application log API. Fall back to debug-only local logs so
 * daemon connection failures do not pollute the OpenCode TUI.
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
    debugLog(`error: ${message}`, extra)
  } else if (level === "warn") {
    debugLog(`warn: ${message}`, extra)
  } else {
    debugLog(message, extra)
  }
}

/** Show an OpenCode TUI toast when that API is available. */
export async function showToastBestEffort(
  client: OpenCodePermissionClient,
  variant: "info" | "success" | "warning" | "error",
  title: string,
  message: string,
) {
  try {
    await client.tui?.showToast?.({
      body: {
        title,
        message,
        variant,
        duration: 5000,
      },
    })
  } catch (error) {
    debugLog("failed to show TUI toast", { error: String(error), title, message, variant })
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
    debugLog(`failed to write app log: ${message}`, { error: String(error) })
  }
}
