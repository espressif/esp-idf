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

.. only:: SOC_TWAI_SUPPORTED

    Two-Wire Automotive Interface (TWAI)
    ------------------------------------

    TWAI provides a brand-new driver interface that supports a wider range of convenient features. Please refer to the programming guide: :doc:`Two-Wire Automotive Interface (TWAI)<../../../api-reference/peripherals/twai>`.

    Compared to the legacy driver, the new driver offers several major improvements:

    - Event-driven architecture, supporting registration of multiple callback functions
    - Runtime configuration changes for bitrate and filter settings
    - More intuitive and user-friendly APIs for bitrate and filter configuration
    - Support for ESP32-C5 and its FD (Flexible Datarate) capabilities
    - Support using multiple TWAI controllers simultaneously

    The new driver is located in the :component:`esp_driver_twai` component. To use it, simply add the component dependency. The relevant header files are :component_file:`esp_driver_twai/include/esp_twai.h` and :component_file:`esp_driver_twai/include/esp_twai_onchip.h`.

    +---------------------------+----------------------------+-----------------------------------------------------------------------------------------------+
    | Deprecated/Removed        | Replacement                | Notes                                                                                         |
    +===========================+============================+===============================================================================================+
    | twai_read_alerts          | None                       | Use ``on_state_change`` and ``on_error`` callbacks to receive status and error notifications. |
    +---------------------------+----------------------------+-----------------------------------------------------------------------------------------------+
    | twai_receive              | twai_node_receive_from_isr | Can only be called inside the on_rx_done callback.                                            |
    +---------------------------+----------------------------+-----------------------------------------------------------------------------------------------+
    | twai_clear_transmit_queue | None                       | Not supported at the moment.                                                                  |
    +---------------------------+----------------------------+-----------------------------------------------------------------------------------------------+
    | twai_clear_receive_queue  | None                       | The new driver uses event-driven receive; blocking receive APIs are not currently provided.   |
    +---------------------------+----------------------------+-----------------------------------------------------------------------------------------------+

    Although we recommend using the new TWAI driver APIs, the legacy driver is still available. To use the legacy driver, include the header file ``driver/twai.h``. When using the legacy driver, please note the following:

    - The new and legacy drivers are not compatible and must not be used together. Mixing them will trigger warnings during startup, and may even cause crashes and system reboots. To suppress this compatibility check, you may enable the configuration option :ref:`CONFIG_TWAI_SKIP_LEGACY_CONFLICT_CHECK`.
    - The legacy driver will no longer receive new features, such as TWAI FD (Flexible Data-rate) support.
