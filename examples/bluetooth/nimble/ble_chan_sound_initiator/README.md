| Supported Targets | ESP32-C6 |
| ----------------- | -------- |

**The ESP32-C6 Bluetooth controller does not support Channel Sounding.** The target listed above is only the chip that runs the NimBLE host. This example is host-only and requires an external, Channel Sounding capable Bluetooth controller connected over HCI UART. No currently available ESP SoC can perform Channel Sounding on its own.

# BLE Channel Sounding Initiator Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates BLE Channel Sounding (CS) procedures in the initiator role against a peer device acting as the CS reflector.

The initiator:

* Scans for a peer advertising the Ranging Service (UUID 0x185B) and connects to it.
* Pairs using LE Secure Connections (mandatory for Channel Sounding) and requests the 2M PHY.
* Discovers the Ranging Service (RAS) on the peer and reads the RAS Features characteristic to select Real-time or On-demand ranging data mode.
* Subscribes to the RAS characteristics and enables free-running CS procedures (a single LE CS Procedure Enable with `max_procedure_count = 0`, so the controllers repeat procedures until the link drops).
* Collects the local CS step data from HCI subevent results and the peer's ranging data through the RAS RREQ (client) role, then prints a per-procedure transfer summary.

Distance estimation is intentionally out of scope for this example. Both the local step data and the peer's ranging data are made available per procedure so that an application-supplied distance-estimation module can be plugged in.

Note:

* The example sets `CONFIG_BT_CONTROLLER_DISABLED=y`, so the ESP32-C6 runs only the NimBLE host and every Channel Sounding operation is performed by the external controller. See [Hardware Required](#hardware-required) for the wiring it needs.
* To install the dependency packages needed, please refer to the top level [README file](../../../README.md#running-test-python-script-pytest).

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* A development board with an ESP SoC listed in the supported targets table above, to run the NimBLE host.
* A USB cable for power supply and programming.
* An external Bluetooth controller supporting Channel Sounding, running an HCI UART firmware.
* Four jumper wires connecting the two boards: TX, RX, RTS and CTS. Hardware flow control is mandatory; without it the HCI stream desynchronizes and the host fails during initialization.

The ESP-side HCI UART pins default to TX 4, RX 5, RTS 19 and CTS 23, and are configurable under `Component config -> Bluetooth -> NimBLE Options -> Host-controller Transport`. Connect ESP TX to controller RX, ESP RX to controller TX, ESP RTS to controller CTS and ESP CTS to controller RTS.

To run the CS procedure end to end, the peer device should be a BLE Channel Sounding reflector such as the `ble_chan_sound_reflector` example.

### Configure the project

Open the project configuration menu:

```bash
idf.py menuconfig
```

* In the `Example Configuration` menu:
  * `Enable Extended Adv` selects extended scanning/advertising support (enabled by default).

* In `Component config -> Bluetooth -> NimBLE Options -> Host-controller Transport`, the HCI UART baud rate and pin assignment must match the firmware running on the external controller. The example defaults to 230400 baud with hardware flow control enabled.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

Console output when the initiator starts and begins scanning for a reflector:

```
I (415) NimBLE_CS_INITIATOR: BLE Host Task Started
```

Once connected to a reflector with a controller that supports Channel Sounding, the initiator pairs, subscribes to the RAS characteristics, enables free-running CS procedures, and prints a transfer summary for every completed procedure (procedure counter, peer ranging data length, and local step data length).

## Troubleshooting

* If the initiator connects but CS never starts, confirm the link is encrypted (Channel Sounding requires LE Secure Connections) and that both controllers support the CS feature.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
