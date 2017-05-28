LED Control
===========

Overview
--------

The LED control module is primarily designed to control the intensity of LEDs, although it can be used to generate PWM signals for other purposes as well. 
It has 16 channels which can generate independent waveforms that can be used to drive e.g. RGB LED devices. For maximum flexibility, the high-speed as well 
as the low-speed channels can be driven from one of four high-speed/low-speed timers. The PWM controller also has the ability to automatically increase or 
decrease the duty cycle gradually, allowing for fades without any processor interference.

Application Example
-------------------

LEDC change duty cycle and fading control example: :example:`peripherals/ledc`.

API Reference
-------------

.. include:: /_build/inc/ledc.inc

