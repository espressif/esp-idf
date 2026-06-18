// SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
// SPDX-License-Identifier: Apache-2.0

import { type Plugin, tool } from "@opencode-ai/plugin"

import { getDaemonStatus, notifyBLE } from "./ble-daemon-client"
import {
  claimChannelForDirectory,
  loadChannelForDirectory,
  pickAndClaimFreeChannel,
  releaseChannelForDirectory,
} from "./binding-store"
import { DEFAULT_REJECT_MESSAGE } from "./config"
import { type DeviceType, probeDeviceType } from "./device-detection"
import { clearIndicatorChannel, sendIndicatorState, type IndicatorState } from "./indicator-control"
import { appLog, appLogBestEffort, showToastBestEffort } from "./logging"
import { replyToOpenCodePermission } from "./opencode-permission-reply"
import {
  buildPermissionCancelPayload,
  buildSessionStatusPayload,
  normalizePermissionEvent,
} from "./permission-payload"
import {
  enqueuePermissionRequest,
  markActiveBLEPermissionsExternallyResolved,
  statusShouldCancelPendingPermission,
} from "./permission-queue"
import type { DaemonStatus, OpenCodePermissionClient, RawPermissionEvent } from "./types"

type BLEPluginState = "unknown" | "connected" | "degraded" | "disabled"

/**
 * Number of consecutive inconclusive ("unknown") device-type probes tolerated
 * for one connection before the device is assumed to be a generic companion
 * device. The probe classifies a device as generic only when its firmware
 * actively rejects the indicator query (daemon HTTP 502). A companion device
 * whose firmware silently ignores the query times out (HTTP 504) and would
 * otherwise stay "pending" forever — permanently suppressing session-status
 * forwarding and the BLE permission round-trip. Falling back to generic after a
 * bounded number of retries re-enables those paths.
 */
const MAX_DEVICE_TYPE_PROBE_ATTEMPTS = 3

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
    return `ESP-BLE-UART Daemon is reachable, but the device is disconnected${attempts}. The next BLE send will try to reconnect.`
  }
  if (status?.daemon_state === "exiting") {
    return "ESP-BLE-UART Daemon is exiting after repeated reconnect failures. BLE forwarding is disabled."
  }
  return "ESP-BLE-UART Daemon is unreachable. BLE forwarding is disabled until the daemon is available."
}

async function notifyStateChange(
  client: OpenCodePermissionClient,
  state: BLEPluginState,
  status?: DaemonStatus,
): Promise<void> {
  const variant = state === "connected" ? "success" : state === "degraded" ? "warning" : "error"
  const message = stateMessage(state, status)
  await showToastBestEffort(client, variant, "OpenCode ESP-BLE-UART Bridge", message)
  await appLogBestEffort(client, variant === "error" ? "error" : variant === "warning" ? "warn" : "info", message, {
    state,
    status,
  })
}

export const BLEDeviceBridgePlugin: Plugin = async ({ client, serverUrl, directory }) => {
  const openCodeClient = client as OpenCodePermissionClient
  let bleState: BLEPluginState = "unknown"
  // Device application type, detected by probing the firmware (see
  // ./device-detection). A "vibe_indicator" is a display-only device that
  // mirrors OpenCode activity on lamps and cannot return a permission decision;
  // everything else is treated as the interactive companion device that
  // understands the full permission protocol.
  let bleDeviceType: DeviceType = "unknown"
  // Number of indicator channels (groups) the device exposes, from the probe.
  let indicatorCount: number | undefined
  // Whether the device application type has been conclusively detected for the
  // current connection session, so the probe runs at most once while connected
  // (cleared on disconnect so a new or reconnected device is probed again). An
  // inconclusive "unknown" result leaves this false so a later refresh retries
  // while still connected.
  let deviceTypeProbed = false
  // Guard against overlapping refreshes probing the device at the same time.
  let deviceTypeProbing = false
  // Consecutive inconclusive probes for the current connection. Drives the
  // generic fallback after MAX_DEVICE_TYPE_PROBE_ATTEMPTS; reset on disconnect.
  let deviceTypeProbeAttempts = 0
  // Indicator channel this OpenCode instance drives, or null when no channel is
  // bound ("dangling": every channel is taken by another live instance, so lamp
  // updates are skipped until a channel is bound). Resolved once the device type
  // and channel count are known — a saved binding is re-claimed, otherwise the
  // lowest free channel is auto-selected. Each independent instance therefore
  // gets its own channel without fighting over one.
  let instanceChannel: number | null = null
  // Whether channel resolution has run for the connected indicator device, so it
  // happens at most once and an explicit bind/unbind is not overridden.
  let channelResolved = false
  // Whether the current activity errored. Set on `session.error`, so the
  // following `idle` shows red (error exit) instead of green (success); cleared
  // when new work starts (`busy`).
  let indicatorErrorActive = false
  // Latest lamp state to apply once a channel is bound (covers auto-bind I/O race).
  let pendingIndicatorState: IndicatorState | null = null
  let indicatorLampChain = Promise.resolve()
  let bleStateRefreshGeneration = 0
  const connectedSessionNotifications = new Set<string>()

  function isIndicatorDevice(): boolean {
    return bleDeviceType === "vibe_indicator"
  }

  function isDeviceTypePending(): boolean {
    return bleDeviceType === "unknown" && !deviceTypeProbed
  }

  // Run lamp work serialized on the single indicator chain and return a promise
  // that resolves when *this* work item completes. All lamp writes — event
  // driven (session.status / session.error / permission) and tool/lifecycle
  // driven (bind / unbind / dispose) — must go through here so they apply in a
  // deterministic order and never race to leave the lamp in the wrong state.
  function runIndicatorLampWork(work: () => Promise<void>): Promise<void> {
    const result = indicatorLampChain.then(work)
    // Keep the queue alive after a failed update so later work still runs.
    indicatorLampChain = result.then(
      () => {},
      () => {},
    )
    return result
  }

  function enqueueIndicatorLampWork(work: () => Promise<void>): void {
    void runIndicatorLampWork(work).catch(() => {
      // Fire-and-forget; failures are already logged by the work itself.
    })
  }

  async function driveIndicatorState(state: IndicatorState): Promise<void> {
    if (instanceChannel === null) {
      pendingIndicatorState = state
      return
    }
    try {
      await sendIndicatorState(instanceChannel, state)
    } catch (error) {
      await appLogBestEffort(openCodeClient, "warn", "Failed to update indicator lamps", {
        error: String(error),
      })
      await refreshBLEState(false)
    }
  }

  // Flush the latest pending lamp state once a channel is bound. Enqueued on the
  // shared chain so it cannot overtake or be overtaken by other lamp work.
  function replayPendingIndicatorState(): void {
    enqueueIndicatorLampWork(async () => {
      if (instanceChannel === null || pendingIndicatorState === null) {
        return
      }
      const state = pendingIndicatorState
      pendingIndicatorState = null
      try {
        await sendIndicatorState(instanceChannel, state)
      } catch (error) {
        pendingIndicatorState = state
        await appLogBestEffort(openCodeClient, "warn", "Failed to replay pending indicator state", {
          error: String(error),
        })
        await refreshBLEState(false)
      }
    })
  }

  async function refreshBLEState(notifyConnected: boolean): Promise<BLEPluginState> {
    const generation = ++bleStateRefreshGeneration
    try {
      const status = await getDaemonStatus()
      if (generation !== bleStateRefreshGeneration) {
        return bleState
      }
      const nextState = stateFromStatus(status)
      const shouldNotify = nextState !== bleState && (nextState !== "connected" || notifyConnected)
      if (bleState === "connected" && nextState !== "connected") {
        deviceTypeProbed = false
        deviceTypeProbeAttempts = 0
        bleDeviceType = "unknown"
        indicatorCount = undefined
        channelResolved = false
        indicatorErrorActive = false
        instanceChannel = null
        pendingIndicatorState = null
      }
      bleState = nextState
      // Detect the device application type the first time we observe a connected
      // device, before notifying or driving lamps, so callers that await this
      // refresh (event handlers, indicator tools) see the resolved type instead
      // of racing the probe. Awaited here is safe: the init call site wraps this
      // refresh in `void`, so plugin init stays non-blocking, and a connected
      // device answers the probe quickly. deviceTypeProbed makes a conclusive
      // result stick while an inconclusive one is retried on a later refresh.
      if (nextState === "connected" && !deviceTypeProbed) {
        await detectDeviceType()
      }
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
        await appLogBestEffort(openCodeClient, "warn", "ESP-BLE-UART Daemon status check failed", { error: String(error) })
      }
    }
    return bleState
  }

  // Probe the connected device to classify its application type (see
  // ./device-detection), then — for a vibe_indicator — resolve which channel
  // this instance drives. Detection is a demo concern, so it lives in the
  // plugin rather than the generic transport daemon. An inconclusive result
  // (write failed / timed out / daemon unreachable) leaves deviceTypeProbed
  // false so a later refresh retries while the device stays connected. A
  // concurrent in-flight guard keeps overlapping refreshes from probing twice.
  async function detectDeviceType(): Promise<void> {
    if (deviceTypeProbed || deviceTypeProbing) {
      return
    }
    // Snapshot the refresh generation so a probe result from a connection that
    // has since dropped (or been replaced by a reconnect) is discarded instead
    // of being written onto the new/disconnected state. The probe itself can
    // take seconds, during which another refresh may reset the device state.
    const generation = bleStateRefreshGeneration
    deviceTypeProbing = true
    try {
      const result = await probeDeviceType()
      if (generation !== bleStateRefreshGeneration || bleState !== "connected") {
        // The connection changed under us while probing; drop the stale result.
        return
      }
      if (result.deviceType === "unknown") {
        deviceTypeProbeAttempts += 1
        if (deviceTypeProbeAttempts < MAX_DEVICE_TYPE_PROBE_ATTEMPTS) {
          // Inconclusive; leave unprobed so a later refresh retries.
          return
        }
        // Retries exhausted: assume a generic device that does not speak the
        // indicator protocol, so the companion permission/status path is
        // re-enabled instead of staying pending forever.
        deviceTypeProbed = true
        bleDeviceType = "generic"
        indicatorCount = undefined
        await appLogBestEffort(
          openCodeClient,
          "warn",
          `Device type probe inconclusive after ${deviceTypeProbeAttempts} attempts; treating device as generic.`,
          {},
        )
        return
      }
      deviceTypeProbed = true
      bleDeviceType = result.deviceType
      indicatorCount = result.indicatorCount
      if (isIndicatorDevice() && !channelResolved && indicatorCount !== undefined) {
        await resolveInstanceChannel()
      }
    } finally {
      deviceTypeProbing = false
    }
  }

  // Resolve which indicator channel this instance drives, once the device type
  // and channel count are known (triggered from refreshBLEState). Order:
  //   1. Re-claim the saved binding for this directory if it is still free.
  //      If a live instance has taken it, report the conflict and stay unbound
  //      (no silent re-selection — the user decides via indicator_bind_channel).
  //   2. Otherwise auto-select and claim the lowest free channel.
  //   3. If every channel is taken, stay unbound ("dangling") and warn.
  // Runs at most once per connected indicator device; never blocks init.
  async function resolveInstanceChannel(): Promise<void> {
    const count = indicatorCount
    if (channelResolved || !isIndicatorDevice() || count === undefined) {
      return
    }
    channelResolved = true
    try {
      const saved = await loadChannelForDirectory(directory)
      if (saved !== undefined) {
        const result = await claimChannelForDirectory(directory, saved)
        if (result.ok) {
          instanceChannel = saved
          replayPendingIndicatorState()
          return
        }
        instanceChannel = null
        await reportDanglingChannel(
          `Saved indicator channel ${saved} is in use by another running OpenCode instance ` +
            `(pid ${result.conflictPid}). This instance is unbound; bind a free channel with the ` +
            `indicator_bind_channel tool.`,
        )
        return
      }
      const picked = await pickAndClaimFreeChannel(directory, count)
      if (picked !== null) {
        instanceChannel = picked
        await appLogBestEffort(openCodeClient, "info", `Auto-bound to indicator channel ${picked} (of ${count}).`, {})
        replayPendingIndicatorState()
        return
      }
      instanceChannel = null
      await reportDanglingChannel(
        `All ${count} indicator channel(s) are in use by other running OpenCode instances. ` +
          `This instance is unbound; free a channel or bind one explicitly with indicator_bind_channel.`,
      )
    } catch (error) {
      // File I/O failure: leave the instance unbound rather than risk a collision.
      instanceChannel = null
      await appLogBestEffort(openCodeClient, "warn", "Failed to resolve indicator channel binding", {
        error: String(error),
      })
    }
  }

  async function reportDanglingChannel(message: string): Promise<void> {
    await showToastBestEffort(openCodeClient, "warning", "OpenCode ESP-BLE-UART Bridge", message)
    await appLogBestEffort(openCodeClient, "warn", message, {})
  }

  // Fire-and-forget the initial BLE state check. Per OpenCode issue #4140,
  // awaiting client methods (app.log, tui.showToast) during plugin init can
  // hang OpenCode if the server isn't fully ready. The state will be
  // refreshed on the first incoming event anyway.
  void refreshBLEState(false).catch(() => {
    // Best-effort; state refresh happens on next event
  })

  return {
    /**
     * Turn off this instance's indicator lamps when the plugin is disposed
     * (OpenCode shutting down, or reloading the plugin), so an exited instance
     * does not leave its channel's lamp lit. Best-effort: if the daemon or device
     * is already gone the call simply fails and is ignored.
     *
     * Only this instance's own channel is cleared, not every channel, so other
     * OpenCode instances sharing the device keep their lamps. The persisted
     * channel binding is intentionally left in place: this directory re-claims the
     * same channel on the next start, and the now-dead pid lets another instance
     * reclaim it if needed.
     */
    dispose: async () => {
      if (instanceChannel === null || !isIndicatorDevice()) {
        return
      }
      const channel = instanceChannel
      // Serialized on the shared chain so the final clear is not overtaken by a
      // lamp update still queued from a last-moment event.
      await runIndicatorLampWork(async () => {
        try {
          await clearIndicatorChannel(channel)
        } catch {
          // Best-effort; nothing else to do while shutting down.
        }
      })
    },

    /**
     * Handle OpenCode events that are relevant to the BLE device.
     *
     * This demo listens for two event families:
     * - `session.status`: mirror OpenCode activity on the BLE device.
     * - `permission.asked`: ask the BLE device user to approve or reject.
     */
    event: async ({ event }) => {
      // OpenCode's SDK Event type and its internal event system use different
      // type strings for conceptually similar events. Use a string cast to
      // avoid TypeScript strict-mode compilation failures.
      const eventType = (event as { type: string }).type

      if (eventType === "session.status") {
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
        // slow or unavailable ESP-BLE-UART Daemon could block OpenCode's own event loop.
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
                "OpenCode ESP-BLE-UART Bridge",
                "BLE UART device is connected for this OpenCode session.",
              )
            }
            if (state === "disabled") {
              return
            }

            // See the permission handler: only treat a pending probe as a reason to
            // skip while the device is connected. While disconnected (degraded) the
            // type stays pending until it reconnects, and forwarding the session
            // status below is the BLE send that triggers the daemon's reconnect.
            if (state === "connected" && isDeviceTypePending()) {
              return
            }

            if (isIndicatorDevice()) {
              // Display-only device: map the session status onto the lamps.
              // - busy/retry → executing (green blink); new work clears any
              //   previous error.
              // - idle → success (green solid), unless this activity errored, in
              //   which case keep error (red solid) until the next busy.
              const statusType = properties.status.type
              enqueueIndicatorLampWork(async () => {
                let indicatorState: IndicatorState
                if (statusType === "busy" || statusType === "retry") {
                  indicatorErrorActive = false
                  indicatorState = "executing"
                } else {
                  indicatorState = indicatorErrorActive ? "error" : "success"
                }
                await driveIndicatorState(indicatorState)
              })
              return
            }

            if (
              // If the session became idle while a BLE permission prompt is
              // active, mark that prompt as externally resolved and ask the
              // ESP-BLE-UART Daemon to dismiss it. This prevents an old prompt from being
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
          }
        })()
      }

      if (eventType === "session.error") {
        // Set synchronously so a concurrent session.status idle sees the error
        // before lamp work is enqueued.
        indicatorErrorActive = true
        void (async () => {
          try {
            const state = await refreshBLEState(true)
            if (state === "disabled" || !isIndicatorDevice()) {
              return
            }
            enqueueIndicatorLampWork(async () => {
              await driveIndicatorState("error")
            })
          } catch (error) {
            await appLogBestEffort(openCodeClient, "warn", "session.error handler failed", {
              error: String(error),
            })
          }
        })()
      }

      // OpenCode's internal event system uses "permission.asked"; the SDK
      // event system uses "permission.updated". Accept both for compatibility
      // across OpenCode versions.
      if (eventType === "permission.asked" || eventType === "permission.updated") {
        const permission = normalizePermissionEvent(event.properties as RawPermissionEvent)

        try {
          const state = await refreshBLEState(true)
          if (isIndicatorDevice()) {
            // The indicator cannot return a decision. Show "waiting for user
            // feedback" (yellow solid, best-effort, non-blocking) and let the
            // user answer in the OpenCode TUI. Do not reply to OpenCode here.
            // Enqueued on the shared lamp chain so it cannot race a concurrent
            // session.status update and leave the lamp in the wrong state.
            if (state !== "disabled" && instanceChannel !== null) {
              enqueueIndicatorLampWork(async () => {
                await driveIndicatorState("waiting")
              })
            }
            return
          }
          if (state === "connected" && isDeviceTypePending()) {
            // Probe inconclusive on a *connected* device: do not route to the
            // generic BLE permission path (the device may be a vibe_indicator).
            // Answer in the OpenCode TUI. Gate this on "connected" only: when the
            // device is disconnected (degraded) the type is reset to pending and is
            // not re-probed until it reconnects, so falling through to
            // enqueuePermissionRequest below is what triggers the daemon's
            // reconnect-on-send. Blocking here would strand the user on a manual
            // reconnect.
            return
          }
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

    /**
     * User-invokable tools for binding this OpenCode instance to an indicator
     * channel. When several independent OpenCode instances share one indicator
     * device, each instance can claim its own channel so their lamps do not
     * collide. Ask the assistant, e.g. "bind the indicator to channel 1".
     */
    tool: {
      indicator_bind_channel: tool({
        description:
          "Bind this OpenCode instance's vibe_indicator lamps to a specific channel (indicator group, 0-based). " +
          "Use when multiple OpenCode instances share one indicator device and each should drive its own channel. " +
          "A channel can only be owned by one live instance; binding a channel already owned by another running " +
          "instance fails (pick a free channel instead).",
        args: {
          channel: tool.schema.number(),
        },
        async execute(args) {
          await refreshBLEState(false)
          if (!isIndicatorDevice()) {
            return "No vibe_indicator device is connected; channel binding only applies to indicator devices."
          }
          const count = indicatorCount
          const channel = Math.trunc(args.channel)
          // Validate the target channel BEFORE touching any state. If the channel
          // count is not known yet, or the channel is out of range, refuse without
          // moving the binding: otherwise an invalid channel would overwrite the
          // current (valid) binding in the store and dark its real lamp, leaving
          // this instance pointed at a non-existent channel. Refusing here keeps
          // the existing binding fully intact — i.e. it falls back to it.
          if (count === undefined) {
            return "Indicator channel count is not known yet; please try binding again in a moment."
          }
          if (channel < 0 || channel >= count) {
            const current = instanceChannel === null ? "none (unbound)" : `channel ${instanceChannel}`
            return `Invalid channel ${channel}. Valid range is 0..${count - 1}. Keeping the current binding (${current}).`
          }
          // An explicit, valid bind is the user's decision; stop auto-resolution
          // from overriding it later.
          channelResolved = true
          // Claim the channel exclusively (persisted). A channel owned by another
          // live instance cannot be taken over.
          let claimed = true
          try {
            const result = await claimChannelForDirectory(directory, channel)
            if (!result.ok) {
              return (
                `Channel ${channel} is already bound by another running OpenCode instance ` +
                `(pid ${result.conflictPid}, directory ${result.conflictDirectory}). ` +
                `Pick a free channel.`
              )
            }
          } catch {
            // Filesystem error: keep the binding in memory only (no exclusivity
            // guarantee and it will reset on restart).
            claimed = false
          }
          const previousChannel = instanceChannel
          instanceChannel = channel
          // Serialize the rebind lamp work on the shared chain so it cannot race
          // a concurrent event-driven update. Moving off a different channel:
          // turn its lamps off so the channel we left behind does not keep a
          // stale lamp lit (e.g. the previous channel's green blink). Then a
          // best-effort visual confirmation: light the chosen channel green.
          await runIndicatorLampWork(async () => {
            if (previousChannel !== null && previousChannel !== channel) {
              try {
                await clearIndicatorChannel(previousChannel)
              } catch {
                // Ignore; the new channel binding below still applies.
              }
            }
            try {
              await sendIndicatorState(channel, "success")
            } catch {
              // Ignore; the binding still takes effect for later status updates.
            }
          })
          replayPendingIndicatorState()
          const note = claimed ? "" : " (could not be saved to disk; it will reset on restart)"
          return `Bound this OpenCode instance to indicator channel ${channel} (of ${count})${note}.`
        },
      }),
      indicator_unbind_channel: tool({
        description:
          "Release this OpenCode instance's indicator channel binding so another instance can claim it. " +
          "This instance becomes unbound and stops driving any lamps until a channel is bound again.",
        args: {},
        async execute() {
          try {
            await releaseChannelForDirectory(directory)
          } catch {
            // Best-effort; the in-memory reset below still happens.
          }
          // Turn off the lamps we were driving so an unbound instance does not
          // leave a stale lamp lit on the channel it just released. Serialized
          // on the shared chain so it cannot race a concurrent lamp update.
          const releasedChannel = instanceChannel
          if (releasedChannel !== null) {
            await runIndicatorLampWork(async () => {
              try {
                await clearIndicatorChannel(releasedChannel)
              } catch {
                // Best-effort; the in-memory reset below still happens.
              }
            })
          }
          // Stay unbound (do not auto-rebind) until the user binds again.
          instanceChannel = null
          pendingIndicatorState = null
          channelResolved = true
          return "Released the channel binding; this instance is now unbound and will not drive any indicator lamps until you bind a channel."
        },
      }),
      indicator_show_binding: tool({
        description:
          "Show which vibe_indicator channel this OpenCode instance is currently bound to, and how many channels the device exposes.",
        args: {},
        async execute() {
          await refreshBLEState(false)
          const bound = instanceChannel === null ? "none (unbound)" : String(instanceChannel)
          if (!isIndicatorDevice()) {
            return `Bound channel: ${bound}. No vibe_indicator device is currently connected.`
          }
          const range = indicatorCount !== undefined ? `. Device channels: 0..${indicatorCount - 1}` : ""
          if (instanceChannel === null) {
            return `Bound channel: none (unbound/dangling)${range}.`
          }
          return `Bound channel: ${instanceChannel}${range}.`
        },
      }),
    },
  }
}
