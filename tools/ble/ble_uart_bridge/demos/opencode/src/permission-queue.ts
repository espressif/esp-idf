// SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
// SPDX-License-Identifier: Apache-2.0

import { CONCURRENT_REJECT_MESSAGE, DECISION_TIMEOUT_SECONDS, DEFAULT_REJECT_MESSAGE } from "./config"
import { sendRequestToBLE } from "./ble-daemon-client"
import { appLogBestEffort, debugLog, showToastBestEffort } from "./logging"
import { isPermissionDecision, replyToOpenCodePermission } from "./opencode-permission-reply"
import { buildPermissionPayload, permissionRequestID } from "./permission-payload"
import type { OpenCodePermissionClient, PermissionDecision, PermissionEventProperties, PermissionQueueItem } from "./types"

/**
 * Queue permission prompts so the BLE device only asks one question at a time.
 *
 * This keeps the device UI simple and avoids racing multiple decisions for the
 * same OpenCode session. It also mirrors OpenCode's behavior of cancelling
 * other same-session pending permissions after one pending request is rejected.
 */
const permissionQueue: PermissionQueueItem[] = []

/** Prevents multiple asynchronous queue drainers from processing the same queue. */
let permissionQueueRunning = false

/**
 * Tracks permission IDs currently waiting for a BLE device decision per session.
 *
 * The session.status handler uses this map to detect stale prompts when OpenCode
 * becomes idle before the BLE device replies.
 */
const activeBLEPermissionsBySession = new Map<string, Set<string>>()

/**
 * Permission IDs that OpenCode resolved outside the BLE decision path.
 *
 * This can happen when a user answers the same permission directly in the TUI
 * while the BLE device is still showing the prompt. If the BLE device replies
 * later, the queue ignores that reply instead of double-answering an already
 * finished OpenCode request.
 */
const externallyResolvedPermissionIDs = new Set<string>()

/** Return a non-empty string or a fallback message for OpenCode replies. */
function nonEmptyString(value: unknown, fallback: string): string {
  return typeof value === "string" && value.length > 0 ? value : fallback
}

/** Mark a permission as actively displayed or pending on the BLE device. */
function beginActiveBLEPermission(permission: PermissionEventProperties) {
  const requestID = permissionRequestID(permission)
  const ids = activeBLEPermissionsBySession.get(permission.sessionID) ?? new Set<string>()
  ids.add(requestID)
  activeBLEPermissionsBySession.set(permission.sessionID, ids)
}

/** Remove a permission from the active BLE prompt tracking map. */
function endActiveBLEPermission(permission: PermissionEventProperties) {
  const requestID = permissionRequestID(permission)
  const ids = activeBLEPermissionsBySession.get(permission.sessionID)
  if (ids === undefined) {
    return
  }
  ids.delete(requestID)
  if (ids.size === 0) {
    activeBLEPermissionsBySession.delete(permission.sessionID)
  }
}

/**
 * Mark active prompts for a session as resolved by OpenCode state changes.
 *
 * The session.status handler calls this when a session becomes idle. Returning
 * `true` tells the caller there was a prompt worth cancelling on the BLE device.
 */
export function markActiveBLEPermissionsExternallyResolved(sessionID: string): boolean {
  const ids = activeBLEPermissionsBySession.get(sessionID)
  if (ids === undefined || ids.size === 0) {
    return false
  }
  for (const id of ids) {
    externallyResolvedPermissionIDs.add(id)
  }
  return true
}

/**
 * Skip queued prompts for a session after one prompt is rejected.
 *
 * OpenCode cancels other pending permission requests in the same session when
 * one pending request is rejected. Because this demo serializes prompts before
 * they reach BLE, later same-session items may still be waiting in this FIFO.
 * Marking them as skipped rejects them back to OpenCode without showing stale
 * prompts on the device during the next OpenCode turn.
 */
function markQueuedPermissionsSkipped(sessionID: string) {
  for (const item of permissionQueue) {
    if (item.permission.sessionID === sessionID && item.skipBLE !== true) {
      item.skipBLE = true
      item.skipMessage = CONCURRENT_REJECT_MESSAGE
    }
  }
}

/**
 * Process one queued permission request from BLE prompt through OpenCode reply.
 *
 * The function defaults to rejection on timeout or daemon failure. That fail-safe
 * behavior is important for a permission bridge: losing contact with the remote
 * device should not grant access.
 */
async function handlePermissionQueueItem(item: PermissionQueueItem): Promise<PermissionDecision | undefined> {
  const requestID = permissionRequestID(item.permission)
  let decision: PermissionDecision = "reject"
  let decisionMessage: string | undefined

  if (item.skipBLE === true) {
    decisionMessage = nonEmptyString(item.skipMessage, DEFAULT_REJECT_MESSAGE)
    debugLog("skipping BLE permission request after concurrent reject", {
      requestID,
      sessionID: item.permission.sessionID,
      message: decisionMessage,
    })
    await appLogBestEffort(item.client, "warn", "skipping BLE permission request after concurrent reject", {
      requestID,
      sessionID: item.permission.sessionID,
      message: decisionMessage,
    })
  } else {
    try {
      beginActiveBLEPermission(item.permission)
      const result = await sendRequestToBLE(
        "permission.request",
        buildPermissionPayload(item.permission),
        DECISION_TIMEOUT_SECONDS,
      )

      if (externallyResolvedPermissionIDs.has(requestID)) {
        markQueuedPermissionsSkipped(item.permission.sessionID)
        debugLog("BLE permission response ignored after OpenCode state changed", {
          requestID,
          sessionID: item.permission.sessionID,
          result,
        })
        return undefined
      }

      if (!isPermissionDecision(result.decision)) {
        throw new Error(`Invalid BLE permission decision: ${String(result.decision)}`)
      }
      decision = result.decision
      if (decision === "reject") {
        // Always include a reject message. A bare OpenCode reject is treated as
        // PermissionRejectedError and can block the current turn; feedback makes
        // it a corrective denial so later serial tool calls can continue.
        decisionMessage = nonEmptyString(result.message, DEFAULT_REJECT_MESSAGE)
      }
      debugLog("BLE permission decision received", result)
      await appLogBestEffort(item.client, "info", "BLE permission decision received", result as Record<string, unknown>)
    } catch (error) {
      if (externallyResolvedPermissionIDs.has(requestID)) {
        markQueuedPermissionsSkipped(item.permission.sessionID)
        debugLog("BLE permission failure ignored after OpenCode state changed", {
          requestID,
          sessionID: item.permission.sessionID,
          error: String(error),
        })
        return undefined
      }
      // Daemon failures are fail-closed, but still include feedback for the same
      // reason as device-side rejects: avoid turning a denial into a hard turn
      // blocker when OpenCode can continue with later serial requests.
      decisionMessage = DEFAULT_REJECT_MESSAGE
      await appLogBestEffort(item.client, "warn", "Failed to get BLE permission decision, rejecting request", {
        error: String(error),
      })
      await showToastBestEffort(
        item.client,
        "error",
        "OpenCode ESP-BLE-UART Bridge",
        "Failed to get a BLE permission decision. The request was rejected.",
      )
    } finally {
      endActiveBLEPermission(item.permission)
      externallyResolvedPermissionIDs.delete(requestID)
    }
  }

  if (decision === "reject") {
    // After one reject, keep queued same-session prompts away from BLE. This
    // matches OpenCode's concurrent-cancel semantics and prevents the device
    // from showing permission requests that OpenCode has already invalidated.
    markQueuedPermissionsSkipped(item.permission.sessionID)
  }

  try {
    await replyToOpenCodePermission(
      item.client,
      item.permission,
      decision,
      decisionMessage,
      item.serverUrl,
      item.directory,
    )
  } catch (error) {
    if (item.skipBLE === true) {
      await appLogBestEffort(item.client, "warn", "Skipped queued permission was already resolved by OpenCode", {
        requestID,
        sessionID: item.permission.sessionID,
        error: String(error),
      })
      return decision
    }
    throw error
  }

  return decision
}

/**
 * Drain queued permission requests sequentially.
 *
 * A new drainer starts only when one is not already running. If new items arrive
 * just as the loop exits, the `finally` block starts another pass.
 */
async function drainPermissionQueue() {
  if (permissionQueueRunning) {
    return
  }
  permissionQueueRunning = true
  try {
    while (permissionQueue.length > 0) {
      const item = permissionQueue.shift()
      if (item === undefined) {
        continue
      }
      try {
        await handlePermissionQueueItem(item)
        item.resolve()
      } catch (error) {
        item.reject(error)
      }
    }
  } finally {
    permissionQueueRunning = false
    if (permissionQueue.length > 0) {
      void drainPermissionQueue()
    }
  }
}

/**
 * Add a permission request to the serialized BLE decision queue.
 *
 * The returned Promise resolves only after the queue item has either replied to
 * OpenCode or decided that OpenCode already resolved the request elsewhere.
 */
export function enqueuePermissionRequest(
  client: OpenCodePermissionClient,
  permission: PermissionEventProperties,
  serverUrl?: URL,
  directory?: string,
): Promise<void> {
  return new Promise((resolve, reject) => {
    permissionQueue.push({ client, permission, serverUrl, directory, resolve, reject })
    void drainPermissionQueue()
  })
}

/**
 * Decide whether a session status update should cancel pending BLE prompts.
 *
 * An idle session means OpenCode is no longer actively waiting for the operation
 * that originally caused the permission prompt. Any BLE prompt still shown for
 * that session is therefore considered stale.
 */
export function statusShouldCancelPendingPermission(status: { type: "idle" | "busy" | "retry" }): boolean {
  return status.type === "idle"
}
