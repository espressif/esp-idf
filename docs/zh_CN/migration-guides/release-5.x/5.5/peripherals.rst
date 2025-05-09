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

.. only:: SOC_TWAI_SUPPORTED

    双线汽车接口 (TWAI)
    --------------------

    TWAI 提供一套全新驱动接口以支持更多更方便的功能，请参考编程指南 :doc:`双线汽车接口 (TWAI)<../../../api-reference/peripherals/twai>`.

    相较于老驱动，新版驱动主要亮点有：

    - 事件驱动，支持注册多种回调函数
    - 支持运行时更改波特率、过滤器配置
    - 提供更友好直观的波特率配置、过滤器配置方法
    - 支持 ESP32C5 及其 FD（Flexible Datarate, 灵活数据速率）功能
    - 支持多个 TWAI 控制器同时使用

    新驱动位于 :component:`esp_driver_twai` 组件中，添加该组件依赖即可使用。包含头文件为 :component_file:`esp_driver_twai/include/esp_twai.h` 和 :component_file:`esp_driver_twai/include/esp_twai_onchip.h`。

    +---------------------------+----------------------------+---------------------------------------------------------------------+
    | 删除/弃用项目             | 替代                       | 备注                                                                |
    +===========================+============================+=====================================================================+
    | twai_read_alerts          | 无                         | 需要注册 on_state_change 和 on_error 回调来获取状态转换和在错误信息 |
    +---------------------------+----------------------------+---------------------------------------------------------------------+
    | twai_receive              | twai_node_receive_from_isr | 只能在 on_rx_done 回调中使用该函数                                  |
    +---------------------------+----------------------------+---------------------------------------------------------------------+
    | twai_clear_transmit_queue | 无                         | 暂不支持                                                            |
    +---------------------------+----------------------------+---------------------------------------------------------------------+
    | twai_clear_receive_queue  | 无                         | receive 是事件驱动的，暂不提供阻塞版本的函数                        |
    +---------------------------+----------------------------+---------------------------------------------------------------------+

    尽管我们推荐使用新的驱动 API，但旧版驱动仍然可用，其头文件引用路径为 ``driver/twai.h``。继续使用旧版驱动请注意：

    - 新旧驱动不互相兼容，不可混合使用，否则将在启动时打印警告，甚至崩溃并重启。可打开配置项 :ref:`CONFIG_TWAI_SKIP_LEGACY_CONFLICT_CHECK` 以屏蔽该检查。
    - 旧版驱动将不再添加新特性（如 TWAIFD）的支持。
