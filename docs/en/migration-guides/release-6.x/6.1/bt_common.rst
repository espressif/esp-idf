Bluetooth Common
================

:link_to_translation:`zh_CN:[中文]`

Example
-------

    The target-specific HCI-over-UART controller examples have been replaced by a single example, :example:`bluetooth/hci/controller_hci_uart`, which supports all targets that provide HCI over UART:

    .. list-table::
        :header-rows: 1
        :widths: 40 60

        * - Removed example
          - Targets it used to cover
        * - ``examples/bluetooth/hci/controller_hci_uart_esp32``
          - ESP32
        * - ``examples/bluetooth/hci/controller_hci_uart_esp32c3_and_esp32s3``
          - ESP32-C3, ESP32-S3
        * - ``examples/bluetooth/nimble/hci``
          - ESP32-C2, ESP32-C5, ESP32-C6, ESP32-C61, ESP32-H2, ESP32-H21, ESP32-H4, ESP32-S31

    Each removed directory keeps only a ``README.md`` that points to the new example. Select the target with ``idf.py set-target <target>``. The HCI UART pins, baud rate, and hardware flow control settings default to the values used by the removed examples, so no rewiring is required.

    The unified example relies on the Bluetooth controller to provide the HCI UART transport. Applications derived from the removed examples must therefore be updated accordingly:

    .. only:: esp32

        The HCI UART pins are now routed by the controller instead of the application. The new option :menuitem:`CONFIG_BTDM_CTRL_HCI_UART_INIT_BY_CONTROLLER` makes :cpp:func:`esp_bt_controller_init` enable the HCI UART and UHCI bus clocks and apply :menuitem:`CONFIG_BTDM_CTRL_HCI_UART_TX_PIN`, :menuitem:`CONFIG_BTDM_CTRL_HCI_UART_RX_PIN`, and, when :menuitem:`CONFIG_BTDM_CTRL_HCI_UART_FLOW_CTRL_EN` is enabled, :menuitem:`CONFIG_BTDM_CTRL_HCI_UART_RTS_PIN` and :menuitem:`CONFIG_BTDM_CTRL_HCI_UART_CTS_PIN`.

        Applications derived from ``controller_hci_uart_esp32`` previously enabled these clocks and configured the UART pins themselves using ``uart_ll_enable_bus_clock()``, ``uhci_ll_enable_bus_clock()``, and ``uart_set_pin()`` before calling :cpp:func:`esp_bt_controller_init`. Remove this code and enable :menuitem:`CONFIG_BTDM_CTRL_HCI_UART_INIT_BY_CONTROLLER` instead. If the application needs to manage the HCI UART itself, keep the option disabled. When the option is enabled, the controller overrides any HCI UART pins configured by the application.

    .. only:: esp32c3 or esp32s3

        The UHCI-based HCI UART transport is now provided by the controller instead of the application. The new option :menuitem:`CONFIG_BT_CTRL_HCI_UART_INIT_BY_CONTROLLER` makes :cpp:func:`esp_bt_controller_init` install the transport and fill in ``esp_bt_controller_config_t::hci_tl_funcs`` using the settings configured under the ``HCI UART(H4) Options`` menu, including :menuitem:`CONFIG_BT_CTRL_HCI_UART_PORT`, :menuitem:`CONFIG_BT_CTRL_HCI_UART_BAUDRATE`, :menuitem:`CONFIG_BT_CTRL_HCI_UART_FLOW_CTRL_EN`, and the TX, RX, RTS, and CTS pins.

        Applications derived from ``controller_hci_uart_esp32c3_and_esp32s3`` previously implemented this transport using LL and register access in their own source files. Remove this code and enable :menuitem:`CONFIG_BT_CTRL_HCI_UART_INIT_BY_CONTROLLER` instead.

        When :menuitem:`CONFIG_BT_CTRL_HCI_MODE_UART_H4` is selected, :cpp:func:`esp_bt_controller_init` requires either ``esp_bt_controller_config_t::hci_tl_funcs`` to be provided or :menuitem:`CONFIG_BT_CTRL_HCI_UART_INIT_BY_CONTROLLER` to be enabled. Otherwise, it reports the missing transport and returns ``ESP_ERR_INVALID_ARG``.

        For Direct Test Mode, :menuitem:`CONFIG_BT_CTRL_HCI_UART_DTM_MODE` configures the HCI UART without hardware flow control. The new function ``esp_bt_hci_uart_reconfig_pin()``, declared in :component_file:`/bt/include/esp32c3/include/esp_bt.h`, can be used to change the TX and RX pins at runtime.

    .. only:: SOC_ESP_NIMBLE_CONTROLLER

        The HCI UART transport is unchanged; only the example location has changed. The new example starts the Bluetooth controller only and does not depend on the NimBLE host. It therefore lives under ``examples/bluetooth/hci/`` instead of ``examples/bluetooth/nimble/``.

        The DTM configuration commands from the removed example are now provided by the console of the new example and are enabled with ``CONFIG_ENABLE_BLE_DTM_CONFIGURATION_COMMAND``.
