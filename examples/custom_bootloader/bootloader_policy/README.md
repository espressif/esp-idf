# Bootloader decision policy

| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

This example uses the bootloader hooks to make a decision rather than to log one. A small
journal of boot decisions is kept in a flash partition, consecutive unconfirmed boots are
counted, and when the budget is spent the bootloader can move to another bootable partition
and flag the boot as degraded through a safe mode flag that the application reads.

Boot policies differ between products. Some want to tolerate a few failed boots before
trying something else, some want to know that the last boots failed, and some boot a single
application partition. This example is a starting point that can be tuned per product.

## What it does

1. The bootloader, in `bootloader_after_init()`, reads the newest journal record. It carries
   the attempt count and what the application reported last time.
2. It decides what to do: keep booting the current target, or move to the fallback target
   and set safe mode.
3. It appends the decision to the journal, in a ring that is safe across power loss.
4. The application reads the journal back and prints what happened, and optionally reports
   that this image is good.

The example does not confirm itself by default, so every reset is an unconfirmed boot and the
budget is spent after three of them:

```
bootloader_policy: bootable=1 attempts=2->3 target=factory reason=1 fallback=0 safe=0 journal=written
bootloader_policy: records=135 latest_seq=135 attempts=3 reason=1 safe_mode=0 confirmed=0
bootloader_policy: bootable=1 attempts=3->0 target=factory reason=3 fallback=0 safe=1 journal=written
bootloader_policy: records=136 latest_seq=136 attempts=0 reason=3 safe_mode=1 confirmed=0
```

`reason=3` means the attempts were spent while the previous boot never got far, and `safe=1`
tells the application to come up in a reduced state. Enable `CONFIG_EXAMPLE_CONFIRM_IMAGE`
to see the other path, where the application reports a good boot and the attempt budget
starts again.

## Relationship to the rollback options

Application rollback is enabled by default and has two confirmation checkpoints. With
`CONFIG_BOOTLOADER_APP_ROLLBACK_CONFIRM_ON_STARTUP`, an image is marked valid near the end of
system startup, before `app_main`, so reaching startup is treated as a good boot. With
`CONFIG_BOOTLOADER_APP_ROLLBACK_CONFIRM_BY_APP`, the application makes that decision.

This example builds on that mechanism rather than replacing it:

- On a layout with an `otadata` partition, a slot that is marked valid there counts as a
  confirmation, so the policy agrees with the rollback state instead of treating a confirmed
  image as a failed boot.
- On a layout with a single application partition there is no otadata, and the journal record
  is the confirmation. That is the layout this example ships with.
- Counting consecutive unconfirmed boots and reporting safe mode adds what the checkpoint
  options do not express: more than one tolerated failure, and a signal the application can
  read when it comes up in a reduced state.

## How to use it

```
idf.py build
idf.py -p PORT flash monitor
```

Reset the board a few times (`Ctrl-T Ctrl-R` in the monitor, or the reset button) and watch
the attempt counter, the reason and the safe mode flag change.

## Configuration

| Option | Where | Purpose |
|---|---|---|
| `CONFIG_PARTITION_TABLE_OFFSET` | `sdkconfig.defaults` | the bootloader carries policy code, so the table moves to `0xa000` |
| `CONFIG_EXAMPLE_CONFIRM_IMAGE` | `main/Kconfig.projbuild` | makes the application report a good boot |
| attempt limit and window | `bootloader_components/boot_policy/boot_policy.c` | see the note below |

The bootloader subproject builds from its own component list and reads the project
configuration for the components it contains. The limits are therefore constants in the
bootloader component; a project that needs them configurable can generate a header for the
component at configure time.

## The journal record

40 bytes, little endian, CRC-32/ISO-HDLC without the final XOR over the first 36 bytes.

| offset | size | field |
|---|---|---|
| 0 | 4 | magic `B P O L` |
| 4 | 2 | version, 1 |
| 6 | 2 | size, 40 |
| 8 | 4 | sequence |
| 12 | 4 | attempts the next boot starts from |
| 16 | 4 | timestamp at the decision, unused here |
| 20 | 4 | uptime the application reported, 0 when it never did |
| 24 | 4 | reset reason, unused here |
| 28 | 1 | reason: 0 first boot, 1 normal, 2 attempts exhausted, 3 crash loop, 4 no fallback |
| 29 | 1 | flags: bit0 safe mode, bit1 fallback used, bit2 confirmed, bit3 no fallback |
| 30 | 6 | reserved |
| 36 | 4 | CRC |

The journal partition is a ring of 4 KB sectors holding 102 records each. The first record of
a sector is its head marker, and the sector is erased when that slot is about to be written,
which bounds recovery to one read per sector plus one sector scan. Scanning the sector after
the newest head as well means recovery also survives a head write that was cut short by a
reset.

## How the hooks are wired here

- `bootloader_after_init()` is used because it runs after initialization and before the
  partition is chosen, which is the last point at which the boot path can be influenced.
- The hooks are declared `weak`, so this component defines `bootloader_hooks_include()` as an
  anchor in the same file. The bootloader links with `-u bootloader_hooks_include`, which is
  what keeps the hooks in the link.

## Extending the pattern

- On a layout with OTA slots, the decision can be applied through the otadata entry, which is
  what the second stage uses to pick the slot.
- The attempt count can also be kept in retained RTC memory as a fast path next to the
  journal, which is read after a reset that did not lose power.
- A hold after the decision, before handing over to the application, gives a service tool a
  window to read the state of a device that will not boot.
