# Example: deep sleep

This example illustrates usage of deep sleep mode and various wakeup sources.

The following wake up sources are configured:

- Timer: wake up the chip in 20 seconds
- EXT1:  wake up the chip if any of the two buttons are pressed (GPIO25, GPIO26)
- Touch: wake up the chip if any of the touch pads are pressed (GPIO32, GPIO33)
- ULP: wake up when the chip temperature changes by more than ~5 degrees Celsius (this value hasn't been characterized exactly yet).

