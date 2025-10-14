控制器 （Controller）& HCI
==================================================================

:link_to_translation:`en:[English]`

.. only:: esp32 or esp32c3 or esp32s3

    应用例程
    ------------

    .. only:: esp32

        - :example:`bluetooth/hci/ble_adv_scan_combined` 演示了如何使用乐鑫自定义 HCI 命令进行蓝牙广播和扫描。本示例展示了如何在没有主机的情况下实现部分主机功能，并显示其他设备的扫描广播报告。

        - :example:`bluetooth/hci/controller_hci_uart_esp32` 演示了如何配置 {IDF_TARGET_NAME} 上的低功耗蓝牙控制器的 HCI 通过 UART 通信，从而与外部蓝牙主机协议栈进行通信。

        - :example:`bluetooth/hci/controller_vhci_ble_adv` 演示了如何使用 ESP-IDF 的 `ble_advertising` 应用在无主机的情况下进行广播，并显示从控制器接收到的HCI事件。

    .. only:: esp32c3 or esp32s3

        - :example:`bluetooth/hci/controller_hci_uart_esp32c3_and_esp32s3` 演示了如何配置{IDF_TARGET_NAME}上的低功耗蓝牙控制器的HCI通过UART通信，从而与外部蓝牙主机协议栈进行通信。

API 参考
-------------

.. include-build-file:: inc/esp_bt.inc


乐鑫自定义 HCI 命令
----------------------

乐鑫自定义 HCI 命令专为 Espressif 的蓝牙主机协议栈或内部调试用途设计。应用程序开发者 **不应** 在其应用程序中初始化或调用这些命令。有关这些命令的详细信息，请参阅 :doc:`bt_vhci`。
