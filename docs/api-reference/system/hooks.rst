.. _hooks_api_reference:

ESP-IDF FreeRTOS Hooks
======================

Overview
--------

FreeRTOS consists of Idle Hooks and Tick Hooks which allow for application 
specific funtiionality to be added to the Idle Task and Tick Interrupt. The
ESP32 is dual core in nature, hence the ESP-IDF provides its own Idle and Tick
Hooks that are dual core compatible in addition to the hooks provided by Vanilla
FreeRTOS. 

Vanilla FreeRTOS Hooks
----------------------

Idle and Tick Hooks in vanilla FreeRTOS are implemented by defining
implementations for the functions ``vApplicationIdleHook`` and 
``vApplicationTickHook`` respectively somewhere in the application. Vanilla 
FreeRTOS will run the user defined Idle Hook every iteration of the Idle Task, 
whereas the user defined Tick Hook will run once per tick interrupt (given that 
there are no pended ticks).

Due to vanilla FreeRTOS being designed for single core, ``vApplicationIdleHook``
and ``vApplicationTickHook`` will be run in both cores on the ESP32. In 
other words, the same Idle Hook and Tick Hook are used for both cores.

To enable the vanilla FreeRTOS hooks in ESP-IDF, :ref:`CONFIG_FREERTOS_LEGACY_HOOKS` 
must be enabled in ``make menuconfig``. :ref:`CONFIG_FREERTOS_LEGACY_IDLE_HOOK`
and :ref:`CONFIG_FREERTOS_LEGACY_TICK_HOOK` should also be enabled.

ESP-IDF Idle and Tick Hooks
---------------------------

Due to the dual core nature of the ESP32, it may be necessary for some
applications to have seperate Idle Hooks for each core. Furthermore, it may
be necessary for Idle and Tick Hooks to have execute multiple functionalities
that are configurable at run time. Therefore the ESP-IDF provides it's own Idle 
and Tick Hooks in addition to the hooks provided by Vanilla FreeRTOS. 

The ESP-IDF Hooks do not operate in the same way as Vanilla FreeRTOS Hooks
where users provide a definition for each of the hooks. Instead, the ESP-IDF
Hooks are predefined to call a list of user registered callbacks specific to 
each core. Users can register and deregister callbacks which are run on the 
Idle or Tick Hook of a specific core. 

API Reference
-------------

.. include:: /_build/inc/esp_freertos_hooks.inc