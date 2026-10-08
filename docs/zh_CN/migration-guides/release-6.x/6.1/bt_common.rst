蓝牙通用
================

:link_to_translation:`en:[English]`

示例
----

    原先按目标芯片划分的 HCI over UART 控制器示例已被统一为一个示例 :example:`bluetooth/hci/controller_hci_uart`，该示例支持所有提供 HCI over UART 的目标芯片：

    .. list-table::
        :header-rows: 1
        :widths: 40 60

        * - 已移除的示例
          - 原先覆盖的目标芯片
        * - ``examples/bluetooth/hci/controller_hci_uart_esp32``
          - ESP32
        * - ``examples/bluetooth/hci/controller_hci_uart_esp32c3_and_esp32s3``
          - ESP32-C3、ESP32-S3
        * - ``examples/bluetooth/nimble/hci``
          - ESP32-C2、ESP32-C5、ESP32-C6、ESP32-C61、ESP32-H2、ESP32-H21、ESP32-H4、ESP32-S31

    上述被移除的目录仅保留一个指向新示例的 ``README.md``。请使用 ``idf.py set-target <target>`` 选择目标芯片。HCI UART 引脚、波特率和硬件流控的默认值与被移除示例所使用的一致，因此无需重新接线。

    统一后的示例依赖蓝牙控制器提供 HCI UART 传输层。因此，基于被移除示例开发的应用程序需要相应调整：

    .. only:: esp32

        HCI UART 引脚现在由控制器而非应用程序配置。启用新增选项 :menuitem:`CONFIG_BTDM_CTRL_HCI_UART_INIT_BY_CONTROLLER` 后，:cpp:func:`esp_bt_controller_init` 会使能 HCI UART 与 UHCI 总线时钟，并应用 :menuitem:`CONFIG_BTDM_CTRL_HCI_UART_TX_PIN`、:menuitem:`CONFIG_BTDM_CTRL_HCI_UART_RX_PIN`，以及在启用 :menuitem:`CONFIG_BTDM_CTRL_HCI_UART_FLOW_CTRL_EN` 时应用 :menuitem:`CONFIG_BTDM_CTRL_HCI_UART_RTS_PIN` 和 :menuitem:`CONFIG_BTDM_CTRL_HCI_UART_CTS_PIN`。

        基于 ``controller_hci_uart_esp32`` 开发的应用程序原先需要在调用 :cpp:func:`esp_bt_controller_init` 之前，自行通过 ``uart_ll_enable_bus_clock()``、``uhci_ll_enable_bus_clock()`` 和 ``uart_set_pin()`` 使能这些时钟并配置 UART 引脚。请删除这部分代码，改为启用 :menuitem:`CONFIG_BTDM_CTRL_HCI_UART_INIT_BY_CONTROLLER`。如果应用程序需要自行管理 HCI UART，请保持该选项为关闭状态。启用该选项后，控制器会覆盖应用程序配置的 HCI UART 引脚。

    .. only:: esp32c3 or esp32s3

        基于 UHCI 的 HCI UART 传输层现在由控制器提供，不再由应用程序实现。启用新增选项 :menuitem:`CONFIG_BT_CTRL_HCI_UART_INIT_BY_CONTROLLER` 后，:cpp:func:`esp_bt_controller_init` 会使用 ``HCI UART(H4) Options`` 菜单下的配置安装该传输层并填充 ``esp_bt_controller_config_t::hci_tl_funcs``，相关配置包括 :menuitem:`CONFIG_BT_CTRL_HCI_UART_PORT`、:menuitem:`CONFIG_BT_CTRL_HCI_UART_BAUDRATE`、:menuitem:`CONFIG_BT_CTRL_HCI_UART_FLOW_CTRL_EN` 以及 TX、RX、RTS 和 CTS 引脚。

        基于 ``controller_hci_uart_esp32c3_and_esp32s3`` 开发的应用程序原先通过 LL 和寄存器访问在自己的源文件中实现该传输层。请删除这部分代码，改为启用 :menuitem:`CONFIG_BT_CTRL_HCI_UART_INIT_BY_CONTROLLER`。

        选择 :menuitem:`CONFIG_BT_CTRL_HCI_MODE_UART_H4` 时，:cpp:func:`esp_bt_controller_init` 要求提供 ``esp_bt_controller_config_t::hci_tl_funcs``，或启用 :menuitem:`CONFIG_BT_CTRL_HCI_UART_INIT_BY_CONTROLLER`，否则会报告传输层缺失并返回 ``ESP_ERR_INVALID_ARG``。

        针对直接测试模式，:menuitem:`CONFIG_BT_CTRL_HCI_UART_DTM_MODE` 会将 HCI UART 配置为不使用硬件流控。:component_file:`/bt/include/esp32c3/include/esp_bt.h` 中新增的 ``esp_bt_hci_uart_reconfig_pin()`` 函数可在运行时修改 TX 和 RX 引脚。

    .. only:: SOC_ESP_NIMBLE_CONTROLLER

        HCI UART 传输层没有变化，仅示例位置发生了改变。新示例仅启动蓝牙控制器，不依赖 NimBLE 主机，因此放在 ``examples/bluetooth/hci/`` 而不是 ``examples/bluetooth/nimble/`` 目录下。

        被移除示例中的 DTM 配置命令现由新示例的控制台提供，通过 ``CONFIG_ENABLE_BLE_DTM_CONFIGURATION_COMMAND`` 启用。
