// SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
// SPDX-License-Identifier: Apache-2.0

import type { OpenCodePermissionClient } from "./types"

/**
 * Prefer OpenCode's application log API. Silently no-op when the API is
 * unavailable, avoiding fallback to console.* calls that would corrupt
 * the OpenCode TUI display.
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
  } catch {
    // Best-effort; toast is not critical
  }
}

/**
 * Best-effort logging wrapper used on error paths.
 *
 * Permission handling should not fail just because the logging endpoint is
 * unavailable, so this helper catches log write errors silently.
 */
export async function appLogBestEffort(
  client: OpenCodePermissionClient,
  level: "debug" | "info" | "warn" | "error",
  message: string,
  extra?: Record<string, unknown>,
) {
  try {
    await appLog(client, level, message, extra)
  } catch {
    // Best-effort; silent no-op
  }
}
