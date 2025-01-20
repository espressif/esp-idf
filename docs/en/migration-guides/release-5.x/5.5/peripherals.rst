Peripherals
===========

:link_to_translation:`zh_CN:[中文]`

.. only:: SOC_TOUCH_SENSOR_SUPPORTED

    Touch Sensor
    ------------

    The touch sensor driver has been redesigned into :doc:`Capacitive Touch Sensor <../../../api-reference/peripherals/cap_touch_sens>`.

    - The new driver is in ``esp_driver_touch_sens`` component and the include path is ``driver/touch_sens.h``.
    - The legacy driver is still available in the previous include path ``driver/touch_sensor.h``.

    Although it is recommended to use the new driver APIs, the legacy driver is still available in the previous include path ``driver/touch_sensor.h``. However, by default, including ``driver/touch_sensor.h`` triggers the build warning below. The warning can be suppressed by the Kconfig option :ref:`CONFIG_TOUCH_SUPPRESS_DEPRECATE_WARN`.

    The major changes are listed as follows:

    Major Changes
    ^^^^^^^^^^^^^

    - Now the touch sensor APIs based on different hardware version are integrated into one API set. And each sub-feature is configurable via its own individual API.
    - Almost all hardware configurations are public to provide enough flexibility, meanwhile the driver also offers some default configurations to save your time on figuring out a basic workable configuration.
    - The new touch driver implemented Finite-State Machine (FSM) to ensure a correct touch sensor operation. Please refer to the programming guide :doc:`Capacitive Touch Sensor <../../../api-reference/peripherals/cap_touch_sens>` for more details.
    - All new APIs are guaranteed to be thread-safe.
    - Implemented a software filter for the touch sensor V1 (ESP32), and open for customization.
