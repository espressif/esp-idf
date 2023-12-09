| Supported Targets | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- | -------- | -------- |

# RMT Transmit Loop Count Example -- Musical Buzzer

(See the README.md file in the upper level 'examples' directory for more information about examples.)

RMT tx channel can send symbols in a loop by hardware, which is useful to generate a variable length of periodic signal.

This example shows how to drive a passive buzzer to play a simple music. Each musical score is represented by a constant frequency of PWM with a constant duration. To play a music is to encoding the musical score continuously. An encoder called `score_encoder` is implemented in the example, it works as a simple wrapper of the `copy_encoder`. See [musical_score_encoder](main/musical_score_encoder.c) for details.

## How to Use Example

### Hardware Required

* A development board with any supported Espressif SOC chip (see `Supported Targets` table above)
* A USB cable for Power supply and programming
* A **passive** buzzer

Connection :

```
            VCC+--------------+
                              | /+
                             +++ |
                             | | | Passive Buzzer
                             +++ |
                              | \+
                              |
                        +     |
                        +<----+
RMT_BUZZER_GPIO+--------+
                        +-----+
                        +     |
                              |
            GND+--------------+
```

The GPIO number used in this example can be changed according to your board, by the macro `RMT_BUZZER_GPIO_NUM` defined in the [source file](main/musical_buzzer_example_main.c).

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.


## Console Output

```
...
I (0) cpu_start: Starting scheduler on APP CPU.
I (318) example: Create RMT TX channel
I (338) example: Install musical score encoder
I (348) example: Playing Beethoven's Ode to joy...
...
```

After you seeing this log, you should hear the music from the buzzer. To play other music, you need to change the musical score array `score` defined in the [source file](main/musical_buzzer_example_main.c). The first member declares the frequency of one musical note, and the second member declares the duration that the note should last.

## Troubleshooting

For any technical queries, please open an [issue] (https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
