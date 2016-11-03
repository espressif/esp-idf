# Example: using LwIP SNTP module and time functions

This example demonstrates the use of LwIP SNTP module to obtain time from Internet servers. See the README.md file in the upper level 'examples' directory for more information about examples.

## Obtaining time using LwIP SNTP module

When this example boots first time after ESP32 is reset, it connects to WiFi and obtains time using SNTP.
See `initialize_sntp` function for details.

## Timekeeping

Once time is synchronized, ESP32 will perform timekeeping using built-in timers.

- RTC clock is used to maintain accurate time when chip is in deep sleep mode

- FRC1 timer is used to provide time at microsecond accuracy when ESP32 is running.

Timekeeping using RTC timer is demonstrated in this example by going into deep sleep mode. After wake up, ESP32 will print current time without connecting to WiFi.

To use this functionality, make sure "Timers used for gettimeofday function" option in "ESP32-specific config" menu of menuconfig is set to "RTC and FRC1" or "RTC".

## Working with time

To get current time, [`gettimeofday`](http://man7.org/linux/man-pages/man2/gettimeofday.2.html) function may be used. Additionally the following [standard C library functions](http://en.cppreference.com/w/cpp/header/ctime) can be used to obtain time and manipulate it:

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
