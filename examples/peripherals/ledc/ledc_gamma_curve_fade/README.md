| Supported Targets | ESP32-C6 | ESP32-H2 | ESP32-P4 |
| ----------------- | -------- | -------- | -------- |

# _LEDC Gamma Curve Fade Example_

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to control intensity of LEDs with [gamma correction](https://en.wikipedia.org/wiki/Gamma_correction) involved using selected SoC's on-board hardware LED PWM Controller module.

## How to use example

### Hardware Required

* A development board with an Espressif SoC that has Gamma Curve Fading feature (e.g., ESP32C6, ESP32H2)
* A USB cable for power supply and programming

Connect two RGB LEDs to the ESP Board as illustrated in the following diagram:

```
              ESP Board                                                                                   
     +-----------------------+                    RGB_LED_1                                                      
     |                       |                +---------------+                                           
     |           LEDC_RED_IO +----------------+ R             |                                           
     |                       |                |               |                                           
     |         LEDC_GREEN_IO +----------------+ G             |                                           
     |                       |                |               |                                           
     |          LEDC_BLUE_IO +----------------+ B     GND     |                                           
     |                       |                +-------+-------+                                           
     |                       |                        |                                                   
     |                   GND +------------------------|                                                   
     |                       |                        |                                                   
     |                       |                +-------+-------+                                           
     |     LEDC_GAMMA_RED_IO +----------------+ R     GND     |                                           
     |                       |                |               |                                           
     |   LEDC_GAMMA_GREEN_IO +----------------+ G             |                                           
     |                       |                |               |                                           
     |    LEDC_GAMMA_BLUE_IO +----------------+ B             |                                           
     |                       |                +---------------+                                           
     +-----------------------+                    RGB_LED_2                  
```

The GPIO number used by this example can be changed in [ledc_gamma_curve_fade_example_main.c](main/ledc_gamma_curve_fade_example_main.c).

### Configure the project

The example provides two ways to calculate gamma corrected duty cycles. One is through math power operation, the other is through look up table (LUT). By default, the example uses the former method, where the gamma factor value can be adjusted in `GAMMA_FACTOR` macro in [ledc_gamma_curve_fade_example_main.c](main/ledc_gamma_curve_fade_example_main.c). If runtime computation wants to be reduced, LUT method can be enabled by running `idf.py menuconfig` and selecting the `Use look up table to do gamma correction` option at `Example Configuration`. Note that the LUT provided in the example is only for a Gamma factor of 2.6. Any LUT for a different Gamma factor needs to be provided by the users.

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

Running this example, you will see color and brightness difference between two RGB LEDs during fading. This is because on RGB_LED_1, no gamma correction is applied; while on RGB_LED_2, gamma correction is applied, and it is fading fitting to a gamma curve. Overall, the color change on RGB_LED_2 should look more gradual and smoother to human eyes.

The program first lights up and dims down some certain colors to show how Gamma corrects the RGB color on RGB LEDs by applying Gamma correction to the PWM duty cycle values and how Gamma curve fading helps to maintain color hue when lighting up and dimming down through the change of duty cycle.

Then both the RGB LEDs start to cycle through the color spectrum.

## Troubleshooting

* Programming fail

    * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there are any output logs.
    * The baud rate for downloading is too high: lower your baud rate in the `menuconfig` menu, and try again.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
