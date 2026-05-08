// SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
// SPDX-License-Identifier: Apache-2.0

import { debugLog } from "./logging"
import { permissionRequestID } from "./permission-payload"
import type { OpenCodePermissionClient, PermissionDecision, PermissionEventProperties } from "./types"

/** Extract SDK-style errors from methods that return error objects instead of throwing. */
function sdkResultError(result: unknown): unknown {
  if (result && typeof result === "object" && "error" in result) {
    return (result as { error?: unknown }).error
  }
  return undefined
}

/** Build Basic Auth headers for raw OpenCode server fallback calls when configured. */
function serverAuthHeaders(): Record<string, string> {
  const password = process.env.OPENCODE_SERVER_PASSWORD
  if (!password) {
    return {}
  }

  const username = process.env.OPENCODE_SERVER_USERNAME ?? "opencode"
  return {
    Authorization: `Basic ${Buffer.from(`${username}:${password}`).toString("base64")}`,
  }
}

/**
 * Headers required by raw OpenCode HTTP calls.
 *
 * The `x-opencode-directory` header tells OpenCode which workspace directory the
 * request belongs to, matching how the SDK normally scopes permission replies.
 */
function opencodeRequestHeaders(directory: string): Record<string, string> {
  return {
    "Content-Type": "application/json",
    "x-opencode-directory": encodeURIComponent(directory),
    ...serverAuthHeaders(),
  }
}

/**
 * Reply to an OpenCode permission request using the best API available.
 *
 * The preferred path is the modern `permission.reply` API. The remaining paths
 * keep this demo useful across OpenCode versions and environments where only a
 * raw client, raw server URL, older v1 helper, or deprecated v2 helper exists.
 */
export async function replyToOpenCodePermission(
  client: OpenCodePermissionClient,
  permission: PermissionEventProperties,
  decision: PermissionDecision,
  message?: string,
  serverUrl?: URL,
  directory?: string,
) {
  const requestID = permissionRequestID(permission)
  // Probe reply APIs in compatibility order:
  // - modern v2 SDK: client.permission.reply()
  // - plugin-internal client: client._client.post()
  // - raw HTTP fallback: fetch(serverUrl /permission/:id/reply)
  // - v1 SDK helper: postSessionIdPermissionsPermissionId()
  // - deprecated v2 SDK: client.permission.respond()
  debugLog("replying to OpenCode permission", {
    requestID,
    sessionID: permission.sessionID,
    decision,
    message,
    hasServerUrl: serverUrl !== undefined,
    hasDirectory: directory !== undefined,
    hasInternalPost: typeof client._client?.post === "function",
    hasV2Reply: typeof client.permission?.reply === "function",
    hasV2Respond: typeof client.permission?.respond === "function",
    hasV1Respond: typeof client.postSessionIdPermissionsPermissionId === "function",
  })

  if (typeof client.permission?.reply === "function") {
    debugLog("using OpenCode v2 permission.reply API")
    const result = await client.permission.reply({
      requestID,
      reply: decision,
      message,
    })
    const error = sdkResultError(result)
    if (error !== undefined) {
      throw new Error(`OpenCode permission response failed: ${String(error)}`)
    }
    debugLog("OpenCode permission response completed", result)
    return
  }

  if (typeof client._client?.post === "function") {
    // In TUI/plugin runtime, serverUrl can be a phantom URL: a plain
    // fetch(serverUrl) may fail or hit the wrong OpenCode instance. The
    // injected HeyAPI client uses OpenCode's in-process app.fetch() binding,
    // so this is the reliable fallback when the public reply helper is absent.
    debugLog("using OpenCode internal raw v2 permission.reply endpoint")
    const result = await client._client.post({
      url: `/permission/${encodeURIComponent(requestID)}/reply`,
      body: {
        reply: decision,
        message,
      },
      headers: { "Content-Type": "application/json" },
    })
    const error = sdkResultError(result)
    if (error !== undefined) {
      throw new Error(`OpenCode permission response failed: ${String(error)}`)
    }
    debugLog("OpenCode permission response completed", result)
    return
  }

  if (serverUrl !== undefined && directory !== undefined) {
    debugLog("using OpenCode raw v2 permission.reply endpoint")
    try {
      const response = await fetch(new URL(`/permission/${encodeURIComponent(requestID)}/reply`, serverUrl), {
        method: "POST",
        headers: opencodeRequestHeaders(directory),
        body: JSON.stringify({
          reply: decision,
          message,
        }),
      })
      if (!response.ok) {
        throw new Error(`HTTP ${response.status} ${await response.text()}`)
      }
      try {
        debugLog("OpenCode permission response completed", await response.json())
      } catch {
        debugLog("OpenCode permission response completed (non-JSON body)")
      }
      return
    } catch (error) {
      debugLog("OpenCode raw v2 permission.reply failed, trying fallback", { error: String(error) })
    }
  }

  if (typeof client.postSessionIdPermissionsPermissionId === "function") {
    debugLog("using OpenCode v1 permission respond API")
    const result = await client.postSessionIdPermissionsPermissionId({
      path: {
        id: permission.sessionID,
        permissionID: requestID,
      },
      body: {
        response: decision,
      },
    })
    const error = sdkResultError(result)
    if (error !== undefined) {
      throw new Error(`OpenCode permission response failed: ${String(error)}`)
    }
    debugLog("OpenCode permission response completed", result)
    return
  }

  if (typeof client.permission?.respond === "function") {
    debugLog("using OpenCode v2 deprecated permission.respond API")
    const result = await client.permission.respond({
      sessionID: permission.sessionID,
      permissionID: requestID,
      response: decision,
    })
    const error = sdkResultError(result)
    if (error !== undefined) {
      throw new Error(`OpenCode permission response failed: ${String(error)}`)
    }
    debugLog("OpenCode permission response completed", result)
    return
  }

  throw new Error("OpenCode client does not expose a permission reply API")
}

/** Runtime guard for permission decisions parsed from BLE daemon JSON. */
export function isPermissionDecision(value: unknown): value is PermissionDecision {
  return value === "once" || value === "reject"
}
