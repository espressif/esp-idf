// SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
// SPDX-License-Identifier: Apache-2.0

import { sendRequestToBLE } from "./ble-daemon-client"

/**
 * Signal-light control for the `ble_uart_vibe_indicator` sample device.
 *
 * Unlike the interactive companion device, the vibe_indicator is display-only:
 * its protocol (see the sample's `json_format.md`) exposes only `query` and
 * `control` commands and has no way to return a permission decision. This module
 * maps OpenCode activity onto the device's lamps.
 *
 * Every command must travel over the daemon's `/request` path
 * (`sendRequestToBLE`), never `/notify`: the notify path sends an empty `id`,
 * which the indicator firmware rejects with `id_not_specified`.
 */

/** JSONL envelope `op` the indicator firmware accepts. */
const INDICATOR_OP = "command"

/** Lamp ids within an indicator group: red / yellow / green GPIO. */
const LIGHT_RED = 0
const LIGHT_YELLOW = 1
const LIGHT_GREEN = 2

// Lamp actions defined by the sample protocol: 0 off, 1 on, 2 slow blink
// (~1 Hz), 3 fast blink (~3 Hz). Only the actions used by the state mapping
// below are bound to names.
const ACTION_OFF = 0
const ACTION_ON = 1
const ACTION_SLOW_BLINK = 2

/** Indicator control commands echo quickly; keep the request timeout short. */
const INDICATOR_REQUEST_TIMEOUT_SECONDS = 5

type LampCommand = { indicator_id: number; light_id: number; light_action: number }

/**
 * High-level indicator states mirrored on one channel's lamps:
 *
 * - `executing` → green blink (work in progress)
 * - `success`   → green solid (finished without error)
 * - `waiting`   → yellow solid (waiting for user feedback, e.g. a permission)
 * - `error`     → red solid (the session errored out)
 */
export type IndicatorState = "executing" | "success" | "waiting" | "error"

function lamp(channel: number, lightId: number, lightAction: number): LampCommand {
  return { indicator_id: channel, light_id: lightId, light_action: lightAction }
}

/**
 * Map a high-level indicator state onto the three lamps (red/yellow/green) of
 * one channel. Every state drives all three lamps so the previous one is always
 * cleared.
 */
function lampCommandsForState(channel: number, state: IndicatorState): LampCommand[] {
  switch (state) {
    case "executing":
      // Green blink — work in progress.
      return [lamp(channel, LIGHT_RED, ACTION_OFF), lamp(channel, LIGHT_YELLOW, ACTION_OFF), lamp(channel, LIGHT_GREEN, ACTION_SLOW_BLINK)]
    case "waiting":
      // Yellow solid — waiting for user feedback.
      return [lamp(channel, LIGHT_RED, ACTION_OFF), lamp(channel, LIGHT_YELLOW, ACTION_ON), lamp(channel, LIGHT_GREEN, ACTION_OFF)]
    case "error":
      // Red solid — the session errored out.
      return [lamp(channel, LIGHT_RED, ACTION_ON), lamp(channel, LIGHT_YELLOW, ACTION_OFF), lamp(channel, LIGHT_GREEN, ACTION_OFF)]
    case "success":
    default:
      // Green solid — finished without error.
      return [lamp(channel, LIGHT_RED, ACTION_OFF), lamp(channel, LIGHT_YELLOW, ACTION_OFF), lamp(channel, LIGHT_GREEN, ACTION_ON)]
  }
}

async function sendControl(payload: LampCommand[]): Promise<void> {
  // The device echoes the payload on success; the response is intentionally
  // ignored. Errors propagate so callers can keep this best-effort.
  await sendRequestToBLE(INDICATOR_OP, { cmd: "control", payload }, INDICATOR_REQUEST_TIMEOUT_SECONDS)
}

/** Mirror a high-level indicator state onto one channel's lamps. */
export async function sendIndicatorState(channel: number, state: IndicatorState): Promise<void> {
  await sendControl(lampCommandsForState(channel, state))
}

/**
 * Turn off all three lamps (red/yellow/green) of one channel.
 *
 * Used when an instance stops driving a channel — on unbind, or when rebinding
 * to a different channel — so the channel it left behind does not keep a stale
 * lamp lit (e.g. a green blink from the previous binding).
 */
export async function clearIndicatorChannel(channel: number): Promise<void> {
  await sendControl([
    lamp(channel, LIGHT_RED, ACTION_OFF),
    lamp(channel, LIGHT_YELLOW, ACTION_OFF),
    lamp(channel, LIGHT_GREEN, ACTION_OFF),
  ])
}
