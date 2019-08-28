# Example: deep sleep

This example illustrates usage of deep sleep mode and various wakeup sources.

The following wake up sources are configured:

- Timer: wake up the chip in 20 seconds
- EXT1:  wake up the chip if any of the two buttons are pressed (GPIO25, GPIO26)
- Touch: wake up the chip if any of the touch pads are pressed (GPIO32, GPIO33)
- ULP: wake up when the chip temperature changes by more than ~5 degrees Celsius (this value hasn't been characterized exactly yet).

In this example, the `CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP` Kconfig option is used, which allows you to reduce the boot time of the bootloader during waking up from deep sleep. The bootloader stores in rtc memory the address of a running partition and uses it when it wakes up. This example allows you to skip all image checks and speed up the boot.