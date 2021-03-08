# eventfd example

The example demonstrates the use of `eventfd()` to collect events from other tasks and ISRs in a
`select()` based main loop.  The example starts two tasks and installs a timer interrupt handler:
1. The first task writes to the first `eventfd` periodically.
2. The timer interrupt handler writes to the second `eventfd`.
3. The second task collects the event from two fds with a `select()` loop.

## How to use example

### Hardware Required

This example should be able to run on any commonly available ESP32, ESP32S2, ESP32S3 or ESP32C3 development board.

### Configure the project

```
idf.py menuconfig
```

The default config will work.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

The following log output should appear when the example runs (note that the bootloader log has been omitted).

```
I (4310) eventfd_example: Time: 1.99s
I (4310) eventfd_example: Select timeout
I (4310) eventfd_example: =================================
I (4310) eventfd_example: Select timeouted for 1 times
I (4320) eventfd_example: Timer triggerred for 0 times
I (4320) eventfd_example: Progress triggerred for 0 times
I (4330) eventfd_example: =================================
I (4810) eventfd_example: Time: 2.50s
I (4810) eventfd_example: TimerEvent fd event triggered
I (5810) eventfd_example: Time: 3.49s
I (5810) eventfd_example: Progress fd event triggered
I (7310) eventfd_example: Time: 5.00s
I (7310) eventfd_example: TimerEvent fd event triggered
I (9310) eventfd_example: Time: 6.99s
I (9310) eventfd_example: Select timeout
I (9310) eventfd_example: Time: 6.99s
I (9310) eventfd_example: Progress fd event triggered
I (9810) eventfd_example: Time: 7.50s
I (9810) eventfd_example: TimerEvent fd event triggered
I (11810) eventfd_example: Time: 9.49s
I (11810) eventfd_example: Select timeout
I (12310) eventfd_example: Time: 10.00s
I (12310) eventfd_example: TimerEvent fd event triggered
I (12810) eventfd_example: Time: 10.49s
I (12810) eventfd_example: Progress fd event triggered
I (14810) eventfd_example: Time: 12.49s
I (14810) eventfd_example: Select timeout
I (14810) eventfd_example: =================================
I (14810) eventfd_example: Select timeouted for 4 times
I (14820) eventfd_example: Timer triggerred for 4 times
I (14820) eventfd_example: Progress triggerred for 3 times
I (14830) eventfd_example: =================================
```
