# LEDC (LED PWM Controller) Example

This example shows how to control intensity of LEDs using ESP32's on-board hardware LED PWM Controller module.

## Functionality

Operations performed by example:

* Configuration of two timers (one high speed and the other low speed) that will be clocking four LEDC channels.

* Configuration of four channels of LEDC module, two channels will operate in high speed mode and two in low speed mode. Each channel will drive one GPIO / LED.

* Initialization of fade service to fade / gradually change intensity of LEDs.

* Operation of channels in a loop by cycling through four steps that will drive LEDs as follows:

  1. Fade up / increase intensity
  2. Fade down / decrease intensity (down to zero)
  3. Set steady intensity
  4. Set intensity to zero

## Hardware Setup

Connect four LEDs to the following LEDC channels / individual GPIOs:

  * Channel 0 - GPIO18
  * Channel 1 - GPIO19
  * Channel 2 - GPIO4
  * Channel 3 - GPIO5

