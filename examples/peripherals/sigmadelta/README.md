# Sigma Delta Modulation Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example uses the sigma-delta driver to generate modulated output on a GPIO. If you connect a LED to the output GPIO, you will see it slowly brightening and dimming.


## How to use example

### Hardware Required

Besides the [ESP32 development board](https://www.espressif.com/en/products/hardware/development-boards) you need a LED and a resistor to limit the LED current. Connect them as below:

```
             330R            LED     
GPIO4 +----/\/\/\----+------|>|-----+ GND
```

A resistor in range from 100 Ohm to 1 kOhm should usually be fine. You may use ESP32 development board by other vendors as well, provided they have at least one GPIO output pin exposed.

By default the GPIO output is 4. To change it, edit the line with `GPIO_NUM_4` in `sigmadelta_init()` function inside `main/sigmadelta_test.c`. For example to use GPIO 25, modify the line to contain `GPIO_NUM_25` instead.


### Configure the project

```
idf.py menuconfig
```

Set serial port under Serial Flasher Options and save the configuration.


### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

Once the upload is complete and the board is reset, the program should start running. This is reported on the monitor as below:

```
...
I (275) cpu_start: Pro cpu start user code
I (293) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
```

Immediately after that the LED should start brightening and dimming.


## Troubleshooting

If you are using [ESP-WROVER-KIT](https://www.espressif.com/en/products/hardware/esp-wrover-kit/overview) then this board has an RGB LED already installed. GPIO4 is driving blue color of the LED. The brightening and dimming effect of the blue LED may not be distinctly visible because red and green LEDs are not actively driven by this example and will slightly lit. To resolve this issue you can switch both diodes off by adding the following code at the end of `sigmadelta_example_init()` function:

```c
gpio_pad_select_gpio(GPIO_NUM_0);
gpio_set_direction(GPIO_NUM_0, GPIO_MODE_OUTPUT);
gpio_set_level(GPIO_NUM_0, 0);

gpio_pad_select_gpio(GPIO_NUM_2);
gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
gpio_set_level(GPIO_NUM_2, 0);
```
