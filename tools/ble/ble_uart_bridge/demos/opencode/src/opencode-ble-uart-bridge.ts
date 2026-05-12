// SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
// SPDX-License-Identifier: Apache-2.0

import type { Plugin } from "@opencode-ai/plugin"

import { getDaemonStatus, notifyBLE } from "./ble-daemon-client"
import { DEFAULT_REJECT_MESSAGE } from "./config"
import { appLog, appLogBestEffort, debugLog, showToastBestEffort } from "./logging"
import { replyToOpenCodePermission } from "./opencode-permission-reply"
import { buildPermissionCancelPayload, buildSessionStatusPayload } from "./permission-payload"
import {
  enqueuePermissionRequest,
  markActiveBLEPermissionsExternallyResolved,
  statusShouldCancelPendingPermission,
} from "./permission-queue"
import type { DaemonStatus, OpenCodePermissionClient, PermissionEventProperties } from "./types"

export { notifyBLE, sendRequestToBLE } from "./ble-daemon-client"
export { buildPermissionPayload } from "./permission-payload"

type BLEPluginState = "unknown" | "connected" | "degraded" | "disabled"

function stateFromStatus(status: DaemonStatus): BLEPluginState {
  if (status.daemon_state === "exiting") {
    return "disabled"
  }
  if (status.is_connected === true) {
    return "connected"
  }
  return "degraded"
}

function stateMessage(state: BLEPluginState, status?: DaemonStatus): string {
  if (state === "connected") {
    return `BLE UART device connected${status?.device_id ? `: ${status.device_id}` : ""}`
  }
  if (state === "degraded") {
    const attempts =
      status?.reconnect_failures !== undefined && status.max_reconnect_failures !== undefined
        ? ` (${status.reconnect_failures}/${status.max_reconnect_failures} reconnect failures)`
        : ""
    return `BLE UART daemon is reachable, but the device is disconnected${attempts}. The next BLE send will try to reconnect.`
  }
  if (status?.daemon_state === "exiting") {
    return "BLE UART daemon is exiting after repeated reconnect failures. BLE forwarding is disabled."
  }
  return "BLE UART daemon is unreachable. BLE forwarding is disabled until the daemon is available."
}

async function notifyStateChange(
  client: OpenCodePermissionClient,
  state: BLEPluginState,
  status?: DaemonStatus,
): Promise<void> {
  const variant = state === "connected" ? "success" : state === "degraded" ? "warning" : "error"
  const message = stateMessage(state, status)
  await showToastBestEffort(client, variant, "OpenCode BLE UART Bridge", message)
  await appLogBestEffort(client, variant === "error" ? "error" : variant === "warning" ? "warn" : "info", message, {
    state,
    status,
  })
}

/**
 * OpenCode plugin entry point for the BLE device bridge demo.
 *
 * OpenCode calls this exported function once when loading the plugin. The
 * returned object registers an `event` callback, and OpenCode calls that
 * callback for session and permission events.
 */
export const BLEDeviceBridgePlugin: Plugin = async ({ client, serverUrl, directory }) => {
  const openCodeClient = client as OpenCodePermissionClient
  let bleState: BLEPluginState = "unknown"
  let bleStateRefreshGeneration = 0
  const connectedSessionNotifications = new Set<string>()

  async function refreshBLEState(notifyConnected: boolean): Promise<BLEPluginState> {
    const generation = ++bleStateRefreshGeneration
    try {
      const status = await getDaemonStatus()
      if (generation !== bleStateRefreshGeneration) {
        return bleState
      }
      const nextState = stateFromStatus(status)
      const shouldNotify = nextState !== bleState && (nextState !== "connected" || notifyConnected)
      bleState = nextState
      if (shouldNotify) {
        await notifyStateChange(openCodeClient, nextState, status)
      }
    } catch (error) {
      if (generation !== bleStateRefreshGeneration) {
        return bleState
      }
      const shouldNotify = bleState !== "disabled"
      bleState = "disabled"
      if (shouldNotify) {
        await notifyStateChange(openCodeClient, "disabled")
        await appLogBestEffort(openCodeClient, "warn", "BLE UART daemon status check failed", { error: String(error) })
      }
    }
    return bleState
  }

  await refreshBLEState(false)

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
          try {
            const previousState = bleState
            const state = await refreshBLEState(true)
            if (
              properties.status.type === "busy" &&
              state === "connected" &&
              previousState === "connected" &&
              !connectedSessionNotifications.has(properties.sessionID)
            ) {
              connectedSessionNotifications.add(properties.sessionID)
              await showToastBestEffort(
                openCodeClient,
                "success",
                "OpenCode BLE UART Bridge",
                "BLE UART device is connected for this OpenCode session.",
              )
            }
            if (state === "disabled") {
              return
            }

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
                await appLogBestEffort(openCodeClient, "warn", "Failed to cancel stale BLE permission on device", {
                  error: String(error),
                })
                await refreshBLEState(false)
              }
            }

            try {
              // Always forward the latest session status, even if there was no
              // stale permission prompt to cancel. This keeps the BLE device's
              // display synchronized with OpenCode.
              await notifyBLE("session.status", buildSessionStatusPayload(properties.sessionID, properties.status))
            } catch (error) {
              await appLogBestEffort(openCodeClient, "warn", "Failed to forward session status to BLE device", {
                error: String(error),
              })
              await refreshBLEState(false)
            }
          } catch (error) {
            await appLogBestEffort(openCodeClient, "warn", "session.status handler failed", {
              error: String(error),
            })
            debugLog("session.status handler failed", { error: String(error) })
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
        await appLogBestEffort(openCodeClient, "info", "received permission.asked", permissionSummary)

        try {
          const state = await refreshBLEState(true)
          if (state === "disabled") {
            await replyToOpenCodePermission(
              openCodeClient,
              permission,
              "reject",
              DEFAULT_REJECT_MESSAGE,
              serverUrl,
              directory,
            )
            return
          }
          // Permission handling is awaited because OpenCode needs an explicit
          // reply before it can continue the tool or command that requested
          // permission. The queue itself serializes BLE prompts.
          await enqueuePermissionRequest(openCodeClient, permission, serverUrl, directory)
        } catch (error) {
          await appLog(openCodeClient, "error", "Failed to reply to OpenCode permission request", {
            error: String(error),
          })
          throw error
        }
      }
    },
  }
}
