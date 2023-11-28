## System Notes

### Timekeeping

The following are the timekeeping mechanisms available and their differences:

1. System time (`esp_system_get_time`)

Time with the origin at `g_startup_time`. The implementation is not handled by `esp_system`,
but it does provide a default implementation using RTC timer. Currently, `esp_timer`
provides system time, since the hardware timers are under the control of that
component. However, no matter the underlying timer, the system time provider
should maintain the definition of having the origin point at `g_startup_time`.

2. `esp_timer` time (`esp_timer_get_time`)

This is the time read from an underlying hardware timer, controlled through config. Origin 
is at the point where the underlying timer starts counting.

3. `newlib` time (`gettimeofday`)

Timekeeping function in standard library. Can be set (`settimeofday`) or moved forward/backward (`adjtime`);
with the possibility of the changes being made persistent through config.
Currently implemented in terms of system time, as the point of origin is fixed.
If persistence is enabled, RTC time is also used in conjuction with system time.

4. RTC time (`esp_rtc_get_time_us`)

Time read from RTC timer.

### Brownout

on some boards, we name BOD1 as ana_bod, to unify the usage, using BOD1 in following passage.

BOD1 will be a little faster then BOD0, but BOD0 can be widely used(can reset rf, flash, or using interrupt, etc.) So, in IDF code, we use BOD1 in bootloader and BOD0 in the app.
