| Supported Targets | ESP32-S2 | ESP32-C3 |
| ----------------- | -------- | -------- |

# RMT Transmit Loop Example -- Musical Buzzer

(See the README.md file in the upper level 'examples' directory for more information about examples.)

RMT peripheral can send customized RMT items in a loop, which means we can use it to generate a configurable length of periodic signal.

This example will show how to drive a passive buzzer to play a simple music, based on the RMT loop feature.

## How to Use Example

### Hardware Required

* A development board with ESP32-S2 SoC
* A USB cable for Power supply and programming
* A passive buzzer

Connection :

```
VCC  +--------------+
                    | /+
                   +++ |
                   | | | Passive Buzzer
                   +++ |
                    | \+
                    |
              +     |
              +<----+
GPIO +--------+
              +-----+
              +     |
                    |
GND  +--------------+
```

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.


## Example Output

```
I (325) example: Playing Beethoven's Ode to joy
```

After you seeing this log, you should hear the music from your buzzer. You can also play other music by updating the `notation` array in the `musical_buzzer_example_main.c`.

## Troubleshooting

For any technical queries, please open an [issue] (https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
