# Bluetooth Examples for Host Controller Interface

Note: To use examples in this directory, you need to have Bluetooth enabled in configuration.

# Example Layout

This directory includes examples to demonstrate controller interactions by virtual HCI layer and UART.

## controller_hci_uart

Demonstrates interaction with controller through HCI over UART.

See the [README.md](./controller_hci_uart/README.md) file in the example [controller_hci_uart](./controller_hci_uart).

## controller_vhci_ble_adv

Demonstrates interaction with controller though virtual HCI layer. In this example, simple BLE advertising is done.

See the [README.md](./controller_vhci_ble_adv/README.md) file in the example [controller_vhci_ble_adv](./controller_vhci_ble_adv).

## ble_adv_scan_combined

Demonstrates interaction with controller. In this example, BLE advertising and scanning is done. Also scanned advertising reports are parsed and displayed.

See the [README.md](./ble_adv_scan_combined/README.md) file in the example [ble_adv_scan_combined](./ble_adv_scan_combined).


## hci_common_component

This is separate component adding functionalities for HCI Layer. Since this component is just used by HCI examples, it is not placed in global components.
