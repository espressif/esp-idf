// SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
// SPDX-License-Identifier: Apache-2.0

/**
 * The permission decisions this BLE demo accepts from the device.
 *
 * OpenCode may support additional replies such as "always", but this demo only
 * exposes "once" and "reject" to keep the BLE device UI simple and to avoid
 * accidentally granting broad long-lived permissions from a small remote UI.
 */
export type PermissionDecision = "once" | "reject"

/**
 * Shape of the permission event fields this plugin reads from OpenCode.
 *
 * Several ID field names are optional because OpenCode permission events have
 * changed across API versions. The bridge normalizes them before talking to the
 * BLE daemon or replying back to OpenCode.
 */
export type PermissionEventProperties = {
  id?: string
  requestID?: string
  permissionID?: string
  sessionID: string
  permission?: string
  type?: string
  title?: string
  metadata?: Record<string, unknown>
  patterns?: string[]
  tool_name?: string
  tool_input?: Record<string, unknown>
}

/**
 * Normalized response expected from the BLE daemon after a permission request.
 *
 * `decision` is optional at the type level because the daemon response is parsed
 * from JSON. Runtime code validates it before using it as a permission reply.
 */
export type BridgeResponse = {
  decision?: PermissionDecision
  message?: string
  ok?: boolean
}

/**
 * Envelope formats the BLE daemon may return.
 *
 * The daemon can wrap the actual payload in either `data` or `response`, and it
 * may encode that payload as an object or a JSON string. Keeping this type loose
 * lets the parser accept all supported daemon versions in one place.
 */
export type DaemonResponse = {
  data?: unknown
  response?: unknown
}

/** Status payload returned by the BLE UART daemon `/status` endpoint. */
export type DaemonStatus = {
  device_id?: string
  connection_state?: "DISCONNECTED" | "CONNECTING" | "CONNECTED" | string
  is_connected?: boolean
  pending_requests?: number
  reconnect_failures?: number
  max_reconnect_failures?: number
  daemon_state?: "running" | "exiting" | string
}

/**
 * Partial OpenCode client surface used by this demo.
 *
 * The plugin intentionally models only the methods it calls instead of importing
 * every generated SDK type. This makes the demo easier to read and allows it to
 * support multiple OpenCode API versions and fallback paths.
 */
export type OpenCodePermissionClient = {
  _client?: {
    post?: (input: {
      url: string
      body: { reply: PermissionDecision; message?: string }
      headers?: Record<string, string>
    }) => Promise<unknown>
  }
  app?: {
    log?: (input: {
      body: {
        service: string
        level: "debug" | "info" | "warn" | "error"
        message: string
        extra?: Record<string, unknown>
      }
    }) => Promise<unknown>
  }
  tui?: {
    showToast?: (input: {
      body: {
        title?: string
        message: string
        variant: "info" | "success" | "warning" | "error"
        duration?: number
      }
    }) => Promise<unknown>
  }
  permission?: {
    reply?: (input: {
      requestID: string
      reply?: "once" | "always" | "reject"
      message?: string
      directory?: string
      workspace?: string
    }) => Promise<unknown>
    respond?: (input: {
      sessionID: string
      permissionID: string
      response?: "once" | "always" | "reject"
      directory?: string
      workspace?: string
    }) => Promise<unknown>
  }
  postSessionIdPermissionsPermissionId?: (input: {
    path: { id: string; permissionID: string }
    body: { response: PermissionDecision }
  }) => Promise<unknown>
}

/**
 * Internal unit of work for the permission queue.
 *
 * Each item connects one OpenCode permission event to one BLE device decision,
 * plus the Promise callbacks that let the plugin's event handler wait until the
 * reply has been sent back to OpenCode.
 */
export type PermissionQueueItem = {
  client: OpenCodePermissionClient
  permission: PermissionEventProperties
  serverUrl?: URL
  directory?: string
  skipBLE?: boolean
  skipMessage?: string
  resolve: () => void
  reject: (error: unknown) => void
}
