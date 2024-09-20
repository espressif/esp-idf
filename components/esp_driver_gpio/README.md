# GPIO API usage in Peripheral Drivers

When a peripheral driver initializes IOs, the general rule is to append necessary configurations onto the IO, instead of using any GPIO APIs that overwrite the current IO setting, such as `gpio_config`, `gpio_reset_pin`, `gpio_set_direction`, `gpio_set_pull_mode`.

Use `gpio_pullup_en` and `gpio_pulldown_en` to enable the internal pull-up/pull-down resistors if necessary.

Use `gpio_od_enable` to configure an output IO as open-drain mode if necessary.

## Configure an IO as a peripheral signal output

There is no need to enable the output for the IO explicitly, it is done internally in the following function calls.

If the signal is routed through IO MUX to the pin, then only needs to call `gpio_iomux_out` to select the IO MUX function index.

If the signal is routed through GPIO Matrix to the pin, then first call `gpio_func_sel` to let the pin use `PIN_FUNC_GPIO` function, follow by calling `esp_rom_gpio_connect_out_signal` to connect the signal.

When a peripheral driver does de-initialization, to de-configure the pin as the peripheral signal output, a call to `gpio_output_disable` is enough. It will disconnect the signal internally.

## Configure an IO as a peripheral signal input

If the signal is routed through IO MUX to the pin, then call `gpio_iomux_out` to select the IO MUX function index, and also call `gpio_iomux_in` to direct the signal to IO MUX. Input will be enabled for the IO internally.

If the signal is routed through GPIO Matrix to the pin, then first call `gpio_func_sel` to let the pin use `PIN_FUNC_GPIO` function, follow by calling `gpio_input_enable` and `esp_rom_gpio_connect_in_signal` to enable the input and connect the signal to the pin.

When a peripheral driver does de-initialization, to de-configure the pin as the peripheral signal input, use `esp_rom_gpio_connect_in_signal` to connect the signal to CONST_ONE or CONST_ZERO, so that it is disconnected from the pin. It is not desired to call `gpio_input_disable`, because there might be other drivers still using this pin as an input.
