Peripherals
===========

:link_to_translation:`zh_CN:[中文]`

.. only:: SOC_TOUCH_SENSOR_SUPPORTED

    Touch Sensor
    ------------

    电容式触摸传感器的驱动已更新为 :doc:`Capacitive Touch Sensor <../../../api-reference/peripherals/cap_touch_sens>`.

    - The new driver is in ``esp_driver_touch_sens`` component and the include path is ``driver/touch_sens.h``.
    - The legacy driver is still available in the previous include path ``driver/touch_sensor.h``.

    尽管我们推荐使用新的驱动 API， 旧版驱动仍然可用，其头文件引用路径为 ``driver/touch_sensor.h``。但是，引用 ``driver/touch_sensor.h`` 会默认触发如下编译警告，可通过配置 Kconfig 选项 :ref:`CONFIG_TOUCH_SUPPRESS_DEPRECATE_WARN` 关闭该警告。

    使用方法上的主要更新如下所示：

    主要使用方法更新
    ^^^^^^^^^^^^^^^^^^^^^^^^^

    - 基于不同硬件版本的触摸传感器 API 现已经整合为一套 API，每个子功能都有独立的 API 可配置。
    - 所有的硬件配置都可配置，以提供足够的灵活性，同时驱动程序还提供了一些默认配置，以便您不必花费时间去确定可用的基本配置。
    - 新的触摸驱动实现了有限状态机（FSM）以确保正确的触摸传感器操作。请参阅编程指南 :doc:`电容式触摸传感器 <../../../api-reference/peripherals/cap_touch_sens>` 了解更多详情。
    - 所有新的API都保证是线程安全的。
    - 为触摸传感器 V1（ESP32）实现了软件滤波器，并开放自定义滤波器接口。
