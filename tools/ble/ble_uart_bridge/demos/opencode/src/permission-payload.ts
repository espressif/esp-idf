// SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
// SPDX-License-Identifier: Apache-2.0

import {
  DEFAULT_PERMISSION_TITLE,
  DEFAULT_PERMISSION_TYPE,
  MAX_METADATA_VALUE_CHARS,
  METADATA_DISPLAY_KEYS,
} from "./config"
import type { PermissionEventProperties } from "./types"

/** Create a unique event ID for messages sent to the BLE daemon. */
function eventID() {
  return crypto.randomUUID()
}

/** Keep one displayed metadata value small enough for constrained BLE device UIs. */
function truncateForBLE(value: string): string {
  return value.length > MAX_METADATA_VALUE_CHARS ? value.slice(0, MAX_METADATA_VALUE_CHARS) : value
}

/** Return the first non-empty string from several OpenCode event fields. */
function firstNonEmptyString(...values: unknown[]): string | undefined {
  for (const value of values) {
    if (typeof value === "string" && value.length > 0) {
      return value
    }
  }
  return undefined
}

/** Check whether compacted display metadata has at least one useful entry. */
function hasMetadata(metadata: Record<string, string>): boolean {
  return Object.keys(metadata).length > 0
}

/**
 * Pick one BLE-friendly metadata entry from a larger OpenCode metadata object.
 *
 * A permission event can include many nested fields, but a BLE device usually
 * has limited display space. This helper prefers human-relevant keys and avoids
 * sending large or non-string values to the daemon.
 */
function compactMetadataForBLE(metadata: Record<string, unknown> | undefined): Record<string, string> {
  if (!metadata) {
    return {}
  }

  for (const key of METADATA_DISPLAY_KEYS) {
    const value = metadata[key]
    if (typeof value === "string" && value.length > 0) {
      return { [key]: truncateForBLE(value) }
    }
  }

  for (const [key, value] of Object.entries(metadata)) {
    if (typeof value === "string" && value.length > 0) {
      return { [key]: truncateForBLE(value) }
    }
  }

  return {}
}

/** Convert OpenCode permission fields into a short type label for the BLE UI. */
function displayTypeForBLE(permission: PermissionEventProperties): string {
  return firstNonEmptyString(permission.type, permission.permission, permission.tool_name) ?? DEFAULT_PERMISSION_TYPE
}

/** Convert OpenCode permission fields into the main prompt title shown on BLE. */
function displayTitleForBLE(permission: PermissionEventProperties): string {
  return (
    firstNonEmptyString(permission.title, permission.tool_input?.description, permission.permission, permission.tool_name) ??
    DEFAULT_PERMISSION_TITLE
  )
}

/**
 * Choose the single most useful metadata field for the BLE permission prompt.
 *
 * The order is: explicit permission metadata, tool input metadata, then the
 * first command pattern. This keeps the device prompt concise while still
 * showing the user why OpenCode is asking for permission.
 */
function displayMetadataForBLE(permission: PermissionEventProperties): Record<string, string> {
  const metadata = compactMetadataForBLE(permission.metadata)
  if (hasMetadata(metadata)) {
    return metadata
  }

  const toolInput = compactMetadataForBLE(permission.tool_input)
  if (hasMetadata(toolInput)) {
    return toolInput
  }

  const pattern = permission.patterns?.find((value) => typeof value === "string" && value.length > 0)
  if (pattern) {
    return { command: truncateForBLE(pattern) }
  }

  return {}
}

/**
 * Normalize the permission ID across OpenCode API versions.
 *
 * Different OpenCode events may call the same concept `id`, `requestID`, or
 * `permissionID`. Reply code and BLE payloads should use one normalized value.
 */
export function permissionRequestID(permission: PermissionEventProperties): string {
  const id = permission.id ?? permission.requestID ?? permission.permissionID
  if (typeof id === "string" && id.length > 0) {
    return id
  }
  throw new Error("OpenCode permission event is missing id/requestID/permissionID")
}

/**
 * Build the protocol message sent to the BLE daemon whenever OpenCode's session
 * state changes. This is a one-way notification, so the BLE device can update
 * its UI but is not expected to send a reply.
 */
export function buildSessionStatusPayload(
  sessionID: string,
  status: { type: "idle" | "busy" | "retry"; attempt?: number; message?: string; next?: number },
) {
  return {
    v: 1,
    kind: "session.status",
    event_id: eventID(),
    session_id: sessionID,
    requires_reply: false,
    payload: status,
  }
}

/**
 * Tell the BLE daemon to dismiss any permission prompt for this session.
 *
 * This is used when OpenCode has already moved on, for example after the
 * session becomes idle before the BLE device returns a decision.
 */
export function buildPermissionCancelPayload(sessionID: string) {
  return {
    v: 1,
    kind: "permission.cancel",
    event_id: eventID(),
    session_id: sessionID,
    requires_reply: false,
    payload: {
      reason: "opencode_state_changed",
    },
  }
}

/**
 * Build the BLE permission prompt payload from an OpenCode permission event.
 *
 * This is the main protocol boundary between OpenCode and the BLE daemon. The
 * outer fields describe routing and reply behavior; the nested `payload` fields
 * are intentionally small and display-oriented for the device UI.
 */
export function buildPermissionPayload(permission: PermissionEventProperties, id = eventID()) {
  const requestID = permissionRequestID(permission)
  return {
    v: 1,
    kind: "permission.request",
    event_id: id,
    session_id: permission.sessionID,
    permission_id: requestID,
    requires_reply: true,
    payload: {
      id: requestID,
      sessionID: permission.sessionID,
      type: displayTypeForBLE(permission),
      title: displayTitleForBLE(permission),
      metadata: displayMetadataForBLE(permission),
    },
  }
}
