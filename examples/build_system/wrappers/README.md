| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Using wrapper to redefine IDF functions

This examples shows a linker feature that will let anyone redefine or override any public function included in both ESP-IDF and the bootloader.

Thanks to this, it is possible to modify the default behavior of a function or extend it.

## Building this example

To build this example, set the target first:
```
# Build for ESP32-C3 for example 
idf.py set-target esp32c3
```

Launch the build:
```
idf.py build
```

Finally, flash it and check the monitor:
```
idf.py flash monitor
```

## Expected output

This example will redefine the bootloader's `bootloader_print_banner` function and IDF's `esp_restart` function.

Thus in the monitor, the following messages should appear:
```
I (30) boot-wrapper: message from a bootloader wrapper
[...]
Restarting in 5 seconds...
Restarting is progress...
[...]
```

This shows that functions have been redefined successfully.

## How does it work?

When redefining a function, the option `-Wl,--wrap=a_common_idf_function` will tell the linker to replace all the occurrences of `a_common_idf_function` function calls in the code and precompiled libraries to `__wrap_a_common_idf_function`. Thus, the application should now provide such symbol.

Moreover, the linker will also provide a new symbol, `__real_a_common_idf_function`, which points to the former function implementation. This is very handy if the new redefinition needs to call the former implementation at some point.

## Limitations

Because the wrapping system happens at link time, the function to redefine must be global. Indeed, functions marked as `static` won't be visible by the linker and thus, cannot be replaced.

Moreover, even though it is not recommended, it is also possible to wrap IDF internal functions that are not marked as `static`. However, keep in mind that such function may be renamed or removed from one IDF version to another. Therefore, after upgrading IDF, make sure the functions you redefine have not been renamed or removed.

Finally, wrapping certain functions may lead to bugs or undefined behavior, for example, redefining a function in IRAM by a function in flash may lead to exceptions at runtime.

Overall, this wrapping method should be used at your own risk.
