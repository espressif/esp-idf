# ESP-IDF Changes

This document is used to track all changes made to the FreeRTOS-Plus-POSIX V1.0.0 source code used in ESP-IDF.

## License Headers

- Added `SPDX-FileCopyrightText` and `SPDX-FileContributor` tags to all files to pass ESP-IDF pre-commit checks.

## POSIX errno

Instead of relying on the FreeRTOS `errno` scheme by enabling `configUSE_POSIX_ERRNO`, the `errno` provided by newlib is used. `configUSE_POSIX_ERRNO` should stay disabled.

## API Changes

`mq_open()` has been changed to make the arguments `mode` and `attr` optional, as specified by [POSIX](https://pubs.opengroup.org/onlinepubs/9699919799/functions/mq_open.html).

The following functions are stubs and always return `ENOSYS`:
* `mq_notify()`
* `mq_setattr()`

## Header File Organization

In the original FreeRTOS-Plus-POSIX project, the POSIX header files are provided in the sub directory `FreeRTOS-Plus-POSIX`. In ESP-IDF, however, the POSIX header files do not need any prefix. This increases compatibility with applications originally written for other platforms. All files originating from the FreeRTOS-Plus-POSIX project have been modified to reflect the different include path.

Wherever possible, ESP-IDF is using header files from newlib instead of the header files from FreeRTOS-Plus-POSIX.

In some cases, additional includes have been added to files from FreeRTOS-Plus-POSIX to simplify building.

## Critical Sections

The critical sections have been changed according to the ESP-IDF critical section API.

## Definitions

The following definitions have been added to the private include `utils.h`:
```
MICROSECONDS_PER_SECOND
NANOSECONDS_PER_SECOND 
NANOSECONDS_PER_TICK   
```

In FreeRTOS-Plus-POSIX, they are located in the custom `time.h`, but ESP-IDF uses newlib's `time.h`, where these definitions are not present.

## Code Format

- Files have been formatted by [`uncrustify`](https://github.com/uncrustify/uncrustify), using [FreeRTOS' `uncrustify.cfc`](../freertos/FreeRTOS-Kernel/uncrustify.cfg).
