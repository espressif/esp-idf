// SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
// SPDX-License-Identifier: Apache-2.0

import { mkdir, readFile, rename, writeFile } from "node:fs/promises"
import { homedir } from "node:os"
import { dirname, join } from "node:path"

/**
 * Persistent, exclusive store for indicator channel bindings, keyed by project
 * directory.
 *
 * Each independent OpenCode instance runs in a project directory, so storing
 * `directory -> { channel, pid }` lets a manual binding survive a restart while
 * also enforcing that a channel has at most one live owner. The file is a small
 * JSON object: `{ "<directory>": { "channel": N, "pid": P }, ... }`.
 *
 * Ownership is resolved with a liveness check: a claim whose owning process is
 * gone is treated as stale and reclaimed automatically, so closing an instance
 * frees its channel. Writes go through a temp file + rename so a partial write
 * can never corrupt the shared file. Writes happen on startup auto-selection and
 * on explicit bind/unbind; cross-instance write contention is low, and a
 * concurrent claim race is possible but acceptable for this demo.
 */

type Binding = { channel: number; pid: number }

const DEFAULT_BINDING_FILE = join(homedir(), ".ble_uart_bridge", "indicator-bindings.json")

/** Location of the bindings file; override with `OPENCODE_BLE_BINDING_FILE`. */
function bindingFilePath(): string {
  return process.env.OPENCODE_BLE_BINDING_FILE ?? DEFAULT_BINDING_FILE
}

/** Whether a process id belongs to a live process on this machine. */
function isProcessAlive(pid: number): boolean {
  if (!Number.isInteger(pid) || pid <= 0) {
    return false
  }
  try {
    // Signal 0 performs existence/permission checking without sending a signal.
    process.kill(pid, 0)
    return true
  } catch (error) {
    // EPERM means the process exists but is owned by another user.
    return (error as { code?: string }).code === "EPERM"
  }
}

async function readBindings(): Promise<Record<string, Binding>> {
  try {
    const raw = await readFile(bindingFilePath(), "utf8")
    const parsed = JSON.parse(raw) as unknown
    if (parsed && typeof parsed === "object") {
      const result: Record<string, Binding> = {}
      for (const [key, value] of Object.entries(parsed as Record<string, unknown>)) {
        // Current format: { channel, pid }. Tolerate the legacy format where the
        // value was a bare channel number (treated as having no live owner).
        if (typeof value === "number" && Number.isInteger(value) && value >= 0) {
          result[key] = { channel: value, pid: 0 }
        } else if (value && typeof value === "object") {
          const channel = (value as Record<string, unknown>).channel
          const pid = (value as Record<string, unknown>).pid
          if (typeof channel === "number" && Number.isInteger(channel) && channel >= 0) {
            result[key] = { channel, pid: typeof pid === "number" ? pid : 0 }
          }
        }
      }
      return result
    }
  } catch {
    // Missing file or invalid JSON: treat as an empty store.
  }
  return {}
}

async function writeBindings(bindings: Record<string, Binding>): Promise<void> {
  const file = bindingFilePath()
  const tmp = `${file}.${process.pid}.tmp`
  await mkdir(dirname(file), { recursive: true })
  await writeFile(tmp, `${JSON.stringify(bindings, null, 2)}\n`, "utf8")
  await rename(tmp, file)
}

export type ClaimResult =
  | { ok: true; channel: number }
  | { ok: false; conflictDirectory: string; conflictPid: number }

/** Return the channel saved for a directory, or undefined if none is stored. */
export async function loadChannelForDirectory(directory: string | undefined): Promise<number | undefined> {
  if (!directory) {
    return undefined
  }
  const bindings = await readBindings()
  return bindings[directory]?.channel
}

/**
 * Claim a channel for a directory, enforcing one live owner per channel.
 *
 * Fails if another directory currently owns the channel and its owning process
 * is still alive: a channel held by a running instance cannot be taken over.
 * Stale claims (owner process gone) are reclaimed automatically. On success the
 * directory's own previous claim (if any, on a different channel) is replaced,
 * freeing that channel.
 */
export async function claimChannelForDirectory(
  directory: string | undefined,
  channel: number,
): Promise<ClaimResult> {
  if (!directory) {
    return { ok: true, channel }
  }
  const bindings = await readBindings()

  for (const [otherDir, binding] of Object.entries(bindings)) {
    if (otherDir === directory || binding.channel !== channel) {
      continue
    }
    if (isProcessAlive(binding.pid)) {
      return { ok: false, conflictDirectory: otherDir, conflictPid: binding.pid }
    }
    // Stale claim (owner process gone): release the other directory's claim.
    delete bindings[otherDir]
  }

  bindings[directory] = { channel, pid: process.pid }
  await writeBindings(bindings)
  return { ok: true, channel }
}

/**
 * Auto-select and claim the lowest-numbered free channel for a directory.
 *
 * A channel is "free" when no *other* directory with a live owning process holds
 * it (channels held by dead processes are reclaimable, hence free). Returns the
 * claimed channel, or `null` when every channel in `0..count-1` is occupied by a
 * live instance — the caller should then treat the instance as unbound
 * ("dangling"). `claimChannelForDirectory` re-reads the store before writing, so
 * a channel lost to a concurrent claim is skipped and the next one is tried.
 */
export async function pickAndClaimFreeChannel(
  directory: string | undefined,
  count: number,
): Promise<number | null> {
  if (count <= 0) {
    return null
  }
  if (!directory) {
    // No persistence without a directory key; default to the first channel.
    return 0
  }
  const bindings = await readBindings()
  const taken = new Set<number>()
  for (const [otherDir, binding] of Object.entries(bindings)) {
    if (otherDir !== directory && isProcessAlive(binding.pid)) {
      taken.add(binding.channel)
    }
  }
  for (let channel = 0; channel < count; channel++) {
    if (taken.has(channel)) {
      continue
    }
    const result = await claimChannelForDirectory(directory, channel)
    if (result.ok) {
      return channel
    }
    // Lost a concurrent race for this channel; try the next one.
    taken.add(channel)
  }
  return null
}

/** Release any channel claimed by a directory (best-effort). */
export async function releaseChannelForDirectory(directory: string | undefined): Promise<void> {
  if (!directory) {
    return
  }
  const bindings = await readBindings()
  if (bindings[directory]) {
    delete bindings[directory]
    await writeBindings(bindings)
  }
}
