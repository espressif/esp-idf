| Supported Targets | ESP32-C6 |
| ----------------- | -------- |

**The ESP32-C6 Bluetooth controller does not support Channel Sounding.** The target listed above is only the chip that runs the NimBLE host. This example is host-only and requires an external, Channel Sounding capable Bluetooth controller connected over HCI UART. No currently available ESP SoC can perform Channel Sounding on its own.

# BLE Channel Sounding Reflector Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates the BLE Channel Sounding (CS) reflector role, serving ranging data to a peer CS initiator through the Ranging Service (RAS).

The reflector:

* Advertises the Ranging Service (UUID 0x185B) using legacy ADV_IND PDUs so the connection is created on the 1M PHY, for the widest initiator compatibility.
* Enables the CS reflector role immediately on connect, before pairing, so that an initiator's `LL_CS_CAPABILITIES_REQ` / `LL_CS_CONFIG_REQ` is not rejected by the controller.
* Requires LE Secure Connections pairing (mandatory for Channel Sounding).
* Runs the RAS server (RRSP role from [ble_svc_ras.c](../../../../components/bt/host/nimble/nimble/nimble/host/services/ras/src/ble_svc_ras.c)), which collects CS subevent results per procedure and delivers them to the initiator in Real-time mode (unsolicited notifications) or On-demand mode (RD Ready indication, then GET_RD / ACK_RD control-point flow).

Distance estimation is intentionally out of scope for this example; the reflector's job is to serve its measured ranging data to the initiator, which combines it with its own local step data.

To test this demo, use the `ble_chan_sound_initiator` example or any BLE Channel Sounding initiator application.

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

To run the CS procedure end to end, the peer device should be a BLE Channel Sounding initiator such as the `ble_chan_sound_initiator` example.

### Configure the project

Open the project configuration menu:

```bash
idf.py menuconfig
```

* In the `Example Configuration` menu:
  * `Enable Extended Adv` selects the extended advertising API (enabled by default; the example still advertises with legacy PDUs, see above).
  * `Enable Bonding` stores keys so a bonded initiator can reconnect and re-encrypt without re-pairing (enabled by default).
  * `Enable MITM Protection` and `Enable Peer Address Resolution` configure additional security options.

* In `Component config -> Bluetooth -> NimBLE Options -> Host-controller Transport`, the HCI UART baud rate and pin assignment must match the firmware running on the external controller. The example defaults to 230400 baud with hardware flow control enabled.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

Console output when the reflector starts and begins advertising:

```
I (415) NimBLE_CS_REFLECTOR: BLE Host Task Started
```

Once an initiator with a controller that supports Channel Sounding connects, the reflector enables the CS reflector role, then pairs, exchanges CS capabilities, and serves each procedure's ranging data to the initiator over RAS.

## Troubleshooting

* If the host times out during initialization, or the controller reports unknown HCI packet headers, the HCI UART link is misconfigured. Check that the baud rate matches on both sides and that hardware flow control is enabled at both ends and physically wired. HCI over UART has no in-band resynchronization, so a single dropped byte desynchronizes the stream until the link is restarted.
* If the initiator connects but CS never starts, confirm the link is encrypted (Channel Sounding requires LE Secure Connections) and that both controllers support the CS feature.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
