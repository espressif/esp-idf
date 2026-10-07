| Supported Targets | ESP32-C3 | Linux |
| ----------------- | -------- | ----- |

# Console I/O Multiplexer Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example illustrates the runtime API of the ESP-IDF console I/O multiplexer (`esp_stdio`). The multiplexer, exposed on `/dev/console`, lets an application register one or more custom VFS backends and switch between them at runtime, without reassigning the `stdin`/`stdout`/`stderr` streams itself.

The example demonstrates the three runtime calls:

* `esp_stdio_register_driver()` — add a custom VFS backend as a write-only **auxiliary**. Every byte written to `stdout`/`stderr` is then fanned out to it in addition to the current primary.
* `esp_stdio_set_primary_driver()` — promote a backend to the active read+write **primary**, saving the handle of the previous primary so it can be restored later. The previous primary is not removed; it stays registered as an auxiliary and keeps receiving the fan-out.
* `esp_stdio_unregister_driver()` — remove a backend. The backend currently acting as the primary cannot be removed, so a different one must be promoted first.

The custom backend used here is a small "capture" sink: while it is registered, every byte written to `/dev/console` is copied into a RAM buffer. At the end the example prints back what the backend recorded, so the effect of the multiplexer's fan-out is visible.

The example writes to `/dev/console` directly (rather than via `printf`) so the behavior is identical on chip targets and on the Linux host. On chip targets newlib also wires `stdout`/`printf` to `/dev/console`, so the same fan-out applies to plain `printf`; on the Linux host `printf` goes straight to the host's stdout and bypasses the multiplexer, which is why the demonstration writes to `/dev/console` explicitly.

## How to use example

The example demonstrates portable, host-independent behavior (a custom VFS backend and the multiplexer runtime API), so it can run both on the `linux` host and on a real chip target.

### Run on the Linux host

```
idf.py --preview set-target linux
idf.py build
build/esp_stdio_runtime_registration.elf
```

See the [Linux target guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/linux-host-testing.html) for details on running ESP-IDF applications on the Linux host.

### Run on a chip target

```
idf.py set-target esp32c3
idf.py -p PORT flash monitor
```

(Replace `esp32c3` with any supported target and `PORT` with the serial port name.)


## Example Output

```
----- capture backend recorded 156 bytes -----
[1] Written while the capture backend is an auxiliary.
[2] Written while the capture backend is the primary.
[3] Written after restoring the original primary.
----- end of captured data -----
```

On the Linux host the console primary is a bit-bucket, so the three lines written to `/dev/console` are not printed live; the final RAM dump is what makes the multiplexer's fan-out observable. On a chip target the same three lines would also appear live on the physical console.
