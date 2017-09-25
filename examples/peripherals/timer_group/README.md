# Example: timer_group

This example uses the timer group driver to generate timer interrupts at two specified alarm intervals.

## Functionality Overview

* Two timers are configured
* Each timer is set with some sample alarm interval
* On reaching the interval value each timer will generate an alarm
* One of the timers is configured to automatically reload it's counter value on the alarm
* The other timer is configured to keep incrementing and is reloaded by the application each time the alarm happens
* Alarms trigger subsequent interrupts, that is tracked with messages printed on the terminal:

```
    Example timer with auto reload
Group[0], timer[1] alarm event
------- EVENT TIME --------
Counter: 0x000000000000000a
Time   : 0.00000200 s
-------- TASK TIME --------
Counter: 0x00000000000107ff
Time   : 0.01351660 s

    Example timer without reload
Group[0], timer[0] alarm event
------- EVENT TIME --------
Counter: 0x00000000092ae316
Time   : 30.76111800 s
-------- TASK TIME --------
Counter: 0x00000000092bd535
Time   : 30.77351460 s

```
