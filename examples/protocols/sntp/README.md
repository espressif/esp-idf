| Supported Targets | ESP32 |
| ----------------- | ----- |

# Example: using LwIP SNTP module and time functions

This example demonstrates the use of LwIP SNTP module to obtain time from Internet servers. See the README.md file in the upper level 'examples' directory for more information about examples.

## Configuring the Example

Open the project configuration menu (`idf.py menuconfig`):

* Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../README.md) for more details.

* Select one method to synchronize time out of the three available in `CONFIG_SNTP_TIME_SYNC_METHOD` (default `update time immediately when received`).

* The time synchronization period is defined by `CONFIG_LWIP_SNTP_UPDATE_DELAY`. This option allows you to set the time synchronization period (default is one hour). This option does not affect this example because after synchronization the program goes into deep sleep for 10 seconds. If you modify this example or use the code from this example, keep in mind that this option will trigger time synchronization. You can change it in `Component config-->LWIP-->SNTP-->Request interval to update time (ms)` menu.

## Obtaining time using LwIP SNTP module

When this example boots first time after ESP32 is reset, it connects to WiFi and obtains time using SNTP.
See `initialize_sntp` function for details.

## Obtaining time using LwIP SNTP-over-DHCP module

NTP server addresses could be automatically acquired via DHCP server option 42. This could be useful on closed environments where public NTPs are not accessible
or to prefer local servers and reduce traffic to the outer world.
See following menuconfig options:
 * `Component config-->LWIP-->SNTP-->Maximum number of NTP servers`
 * `Component config-->LWIP-->SNTP-->Request NTP servers from DHCP`
 * `Component config-->LWIP-->SNTP-->Maximum number of NTP servers aquired via DHCP`
 * `Component config-->LWIP-->Enable LWIP Debug-->Enable SNTP debug messages`

Please note, that `dhcp_set_ntp_servers()` does not only set NTP servers provided by DHCP, but also resets all other NTP server configured before. If you want to keep both manually configured and DHCP obtained NTP servers, please use the API in this order:
* Enable SNTP-over-DHCP before getting the IP using `sntp_servermode_dhcp()`
* Set the static NTP servers after receiving the DHCP lease using `sntp_setserver()`

## Timekeeping

Once time is synchronized, ESP32 will perform timekeeping using built-in timers.

- RTC clock is used to maintain accurate time when chip is in deep sleep mode

- High-resolution timer is used to provide time at microsecond accuracy when ESP32 is running.

Timekeeping using RTC timer is demonstrated in this example by going into deep sleep mode. After wake up, ESP32 will print current time without connecting to WiFi.

To use this functionality, make sure "Timers used for gettimeofday function" option in "ESP32-specific config" menu of menuconfig is set to "RTC and high-resolution timer" or "RTC".

## Working with time

To get current time, [`gettimeofday`](http://man7.org/linux/man-pages/man2/gettimeofday.2.html) function may be used. Additionally the following [standard C library functions](https://en.cppreference.com/w/cpp/header/ctime) can be used to obtain time and manipulate it:

	gettimeofday
	time
	asctime
	clock
	ctime
	difftime
	gmtime
	localtime
	mktime
	strftime

To set time, [`settimeofday`](http://man7.org/linux/man-pages/man2/settimeofday.2.html) POSIX function can be used. It is used internally in LwIP SNTP library to set current time when response from NTP server is received.

## Timezones

To set local timezone, use [`setenv`](http://man7.org/linux/man-pages/man3/setenv.3.html) and [`tzset`](http://man7.org/linux/man-pages/man3/tzset.3.html) POSIX functions. First, call `setenv` to set `TZ` environment variable to the correct value depending on device location. Format of the time string is described in [libc documentation](https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html). Next, call `tzset` to update C library runtime data for the new time zone. Once these steps are done, `localtime` function will return correct local time, taking time zone offset and daylight saving time into account.

## Additional options

This example can use 3 time synchronization method:

- `update time immediately when received` - update time immediately as received an answer from SNTP server. Allows syncing the time without any additional code from the user side, and use a callback function or getting status for notification of the process of sync.

- `update time with smooth method (adjtime)` - time synchronization will use the adjtime function to smoothly update the time. Allows syncing the time without any additional code from the user side, and use a callback function or getting status for notification of the process of sync.

- `custom implementation` - allows replacing the built-in time synchronization functionality. This option redefines `sntp_sync_time()` function.

## Useful API function:

- `sntp_set_time_sync_notification_cb()` - use this function to set a callback function to notify about the time synchronization process.
- `sntp_get_sync_status()` and `sntp_set_sync_status()` - get/set time synchronization status.
- `sntp_get_sync_mode()` and `sntp_set_sync_mode()` - get/set the sync mode. Allowable two mode: `SNTP_SYNC_MODE_IMMED` and `SNTP_SYNC_MODE_SMOOTH`.

## Mode of update time

`sntp_set_sync_mode()` - Set the sync mode of the system time. It has two mode:

* `SNTP_SYNC_MODE_IMMED` - Update system time immediately when receiving a response from the SNTP server.
* `SNTP_SYNC_MODE_SMOOTH` - Smooth time updating. Time error is gradually reduced using adjtime function. If the difference between SNTP response time and system time is large (more than 35 minutes) then update immediately. This mode uses `adjtime()` function.

## Adjtime()
`int adjtime(const struct timeval *delta, struct timeval *outdelta)`

`adjtime()` is a libc function that is called automatically in "smooth" time update mode, but can also be called from custom time synchronization code.
If the time error is less than 35 minutes then `adjtime` function will start smooth adjusting otherwise the return value is -1.

This function speeds up or slows down the system clock in order to make a gradual adjustment. This ensures that the calendar time reported by the system clock is always monotonically increasing, which might not happen if you simply set the clock. If adjusting the system clock by `adjtime()` is already done during the second call `adjtime()`, and the delta of the second call is not NULL, the earlier tuning is stopped, but the already completed part of the adjustment is not canceled.

The delta argument specifies a relative adjustment to be made to the clock time. If negative, the system clock is slowed down for a while until it has lost this much elapsed time. If positive, the system clock is speeded up for a while.

If the olddelta argument is not a null pointer, the adjtime function returns information about any previous time adjustment that has not yet completed.

The return value is 0 on success and -1 on failure.

To stop the smooth time adjustment, you need to record the current time using the function `settimeofday()`.
