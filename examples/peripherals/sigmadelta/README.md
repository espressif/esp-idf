# Example: sigma_delta modulation

This example uses the sigma-delta driver to generate modulated output on a GPIO.

By default the GPIO output is 4, however you can edit this in the `sigmadelta_init()` function inside `main/sigmadelta_test.c`.

If you connect a LED to the output GPIO, you will see it blinking slowly.
