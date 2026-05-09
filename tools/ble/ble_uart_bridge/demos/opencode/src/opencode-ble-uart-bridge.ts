// SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
// SPDX-License-Identifier: Apache-2.0

import type { Plugin } from "@opencode-ai/plugin"

import { isDaemonAvailable, notifyBLE } from "./ble-daemon-client"
import { appLog, appLogBestEffort, debugLog } from "./logging"
import { buildPermissionCancelPayload, buildSessionStatusPayload } from "./permission-payload"
import {
  enqueuePermissionRequest,
  markActiveBLEPermissionsExternallyResolved,
  statusShouldCancelPendingPermission,
} from "./permission-queue"
import type { OpenCodePermissionClient, PermissionEventProperties } from "./types"

export { notifyBLE, sendRequestToBLE } from "./ble-daemon-client"
export { buildPermissionPayload } from "./permission-payload"

/**
 * OpenCode plugin entry point for the BLE device bridge demo.
 *
 * OpenCode calls this exported function once when loading the plugin. The
 * returned object registers an `event` callback, and OpenCode calls that
 * callback for session and permission events.
 */
export const BLEDeviceBridgePlugin: Plugin = async ({ client, serverUrl, directory }) => {
  if (!(await isDaemonAvailable())) {
    return {}
  }

  return {
    /**
     * Handle OpenCode events that are relevant to the BLE device.
     *
     * This demo listens for two event families:
     * - `session.status`: mirror OpenCode activity on the BLE device.
     * - `permission.asked`: ask the BLE device user to approve or reject.
     */
    event: async ({ event }) => {
      if (event.type === "session.status") {
        // OpenCode emits this event whenever a session changes state, such as
        // becoming busy, retrying, or going idle. The plugin forwards this
        // state to the BLE device so the device can mirror OpenCode's current
        // activity.
        const properties = event.properties as {
          sessionID: string
          status: { type: "idle" | "busy" | "retry"; attempt?: number; message?: string; next?: number }
        }

        // Forwarding session status is best-effort background work. Do not
        // await this async IIFE from the OpenCode event callback; otherwise a
        // slow or unavailable BLE daemon could block OpenCode's own event loop.
        void (async () => {
          if (
            // If the session became idle while a BLE permission prompt is
            // active, mark that prompt as externally resolved and ask the BLE
            // daemon to dismiss it. This prevents an old prompt from being
            // answered after OpenCode no longer needs the decision. Only idle
            // triggers this cancellation: busy/retry are normal activity
            // transitions and should not dismiss an actively displayed prompt.
            statusShouldCancelPendingPermission(properties.status) &&
            markActiveBLEPermissionsExternallyResolved(properties.sessionID)
          ) {
            try {
              // OpenCode does not emit a permission-specific cancellation event
              // when a user answers the same prompt in the TUI. This best-effort
              // notification tells the daemon the BLE prompt is stale before the
              // following idle status also clears the device UI.
              await notifyBLE("permission.cancel", buildPermissionCancelPayload(properties.sessionID))
            } catch (error) {
              console.warn("Failed to cancel stale BLE permission on device", error)
            }
          }

          try {
            // Always forward the latest session status, even if there was no
            // stale permission prompt to cancel. This keeps the BLE device's
            // display synchronized with OpenCode.
            await notifyBLE("session.status", buildSessionStatusPayload(properties.sessionID, properties.status))
          } catch (error) {
            console.warn("Failed to forward session status to BLE device", error)
          }
        })()
      }

      if (event.type === "permission.asked") {
        const permission = event.properties as PermissionEventProperties
        const permissionSummary = {
          keys: Object.keys(permission),
          id: permission.id,
          requestID: permission.requestID,
          permissionID: permission.permissionID,
          sessionID: permission.sessionID,
          permission: permission.permission,
          type: permission.type,
          title: permission.title,
        }
        debugLog("received permission.asked", permissionSummary)
        await appLogBestEffort(client as OpenCodePermissionClient, "info", "received permission.asked", permissionSummary)

        try {
          // Permission handling is awaited because OpenCode needs an explicit
          // reply before it can continue the tool or command that requested
          // permission. The queue itself serializes BLE prompts.
          await enqueuePermissionRequest(client as OpenCodePermissionClient, permission, serverUrl, directory)
        } catch (error) {
          await appLog(client as OpenCodePermissionClient, "error", "Failed to reply to OpenCode permission request", {
            error: String(error),
          })
          throw error
        }
      }
    },
  }
}
