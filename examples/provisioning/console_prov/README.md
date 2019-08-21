# Console based Provisioning Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

`console_prov` example demonstrates the implementation and integration of various IDF components for building a console based provisioning application.

For this example UART console is chosen as the mode of transport, over which the provisioning related communication is to take place, between the device (to be provisioned) and the client (owner of the device).

In the provisioning process the device is configured as a Wi-Fi station with specified credentials. Once configured, the device will retain the Wi-Fi configuration, until a flash erase is performed.

Right after provisioning is complete, the UART console is deactivated.

`console_prov` uses the following components :
* `wifi_provisioning` : provides data structures and protocomm endpoint handlers for Wi-Fi configuration
* `protocomm` : for protocol based communication and secure session establishment
* `protobuf` : Google's protocol buffer library for serialization of protocomm data structures

This example can be used, as it is, for adding a provisioning service to any application intended for IoT. But it is more suitable for debugging protocomm and provisioning related components and feature additions.

## How to use example

### Hardware Required

Example should be able to run on any commonly available ESP32 development board.

### Application Required

To provision the device running this example, the `esp_prov.py` script needs to be run (found under `$IDF_PATH/tools/esp_prov`). This feature of `esp_prov` should work on all platforms, given the dependencies are satisfied.

### Configure the project

```
idf.py menuconfig
```

* Set serial port under Serial Flasher Options.

* Under Example Configuration set the following :
    * Security Version (default 1)
    * Proof of Possession (default "abcd1234")

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (388) app: Starting console provisioning
I (398) app_prov: Console provisioning started
.
.
.
>>
```

In a separate terminal run the `esp_prov.py` script under `$IDP_PATH/tools/esp_prov` directory (please replace `myssid` and `mypassword` with the credentials of the AP to which the device is supposed to connect to after provisioning). Assuming default example configuration, the script should be run as follows :

```
python esp_prov.py --transport console --proto_ver "V0.1" --sec_ver 1 --pop abcd1234 --ssid myssid --passphrase mypassword
```

A console will open up and the `Client->Device` commands have to be copied manually to the serial monitor console prompt :

```
==== Verifying protocol version ====
Client->Device msg : proto-ver 0 56302e31
Enter device->client msg :
```

On pasting the command on the serial monitor console, a `Device->Client` message will appear for each command :

```
>> proto-ver 0 56302e31
53554343455353
```

Copy this message back to the `esp_prov` console for proceeding to the next command :

```
==== Verifying protocol version ====
Client->Device msg : proto-ver 0 56302e31
Enter device->client msg : 53554343455353
==== Verified protocol version successfully ====

==== Starting Session ====
Client->Device msg : prov-session 0 10015a25a201220a20677106cc2f5b2acb5d8da26f0ad443df006daa1cd5bb3d75a8324d81ec5ef970
Enter device->client msg :
```

This process keeps on till the device gets provisioned.

Note that the commands are in the following format :

```
<endpoint name> <session id> <hex message>
```

This is helpful in understanding the provisioning process and the order in which the endpoints are communicated with.

The full execution sequence of `esp_prov`, as seen on the console, is shown here :

```
==== Verifying protocol version ====
Client->Device msg : proto-ver 0 56302e31
Enter device->client msg : 53554343455353
==== Verified protocol version successfully ====

==== Starting Session ====
Client->Device msg : prov-session 0 10015a25a201220a20677106cc2f5b2acb5d8da26f0ad443df006daa1cd5bb3d75a8324d81ec5ef970
Enter device->client msg : 10015a390801aa013412207566f4de191f600ea42de5c2b1df73f1f16685c2edb43d7c3ffc83d6b81ff61b1a103db6476536a88db10b7e0a172d4adef8
Client->Device msg : prov-session 0 10015a270802b20122122084ca311e51c904a94f8a249c049f7aed33b39671cc11f0b92b15b299ef5653b7
Enter device->client msg : 10015a270803ba01221a203246230190d5c1f5d94c01b56ac8cace1086cfb2d937a4a46cb6c79db7a35a8b
==== Session Established ====

==== Sending Wifi credential to esp32 ====
Client->Device msg : prov-config 0 8f0c8cb6f2d53c4cc53b29be8ba1aac3edbb1dead39117c34687d6
Enter device->client msg : 2e1f0eb0
==== Wifi Credentials sent successfully ====

==== Applying config to esp32 ====
Client->Device msg : prov-config 0 e8df
Enter device->client msg : 245c83f0
==== Apply config sent successfully ====

==== Wifi connection state  ====
Client->Device msg : prov-config 0 2d36
Enter device->client msg : 1b38a7411b6e2608aae50a6571807e04a6e90520b3b1e3c1e5b38cea4b9022e56485b92ff84289df218311972a42eb
++++ WiFi state: connected ++++
==== Provisioning was successful ====
```

The serial monitor console, for above sequence of commands, would look like :

```
>> proto-ver 0 56302e31
53554343455353
>> prov-session 0 10015a25a201220a20677106cc2f5b2acb5d8da26f0ad443df006daa1cd5bb3d75a8324d81ec5ef970
10015a390801aa013412207566f4de191f600ea42de5c2b1df73f1f16685c2edb43d7c3ffc83d6b81ff61b1a103db6476536a88db10b7e0a172d4adef8
>> prov-session 0 10015a270802b20122122084ca311e51c904a94f8a249c049f7aed33b39671cc11f0b92b15b299ef5653b7
10015a270803ba01221a203246230190d5c1f5d94c01b56ac8cace1086cfb2d937a4a46cb6c79db7a35a8b
>> prov-config 0 8f0c8cb6f2d53c4cc53b29be8ba1aac3edbb1dead39117c34687d6
I (1073738) app_prov_handler: WiFi Credentials Received :
    ssid     : myssid
    password : mypassword

2e1f0eb0
>> prov-config 0 e8df
I (1084218) app_prov_handler: WiFi Credentials Applied

245c83f0
>> prov-config 0 2d36
I (1089728) app_prov: STA Got IP
I (1089728) app: got ip:192.168.43.220
I (1099698) app_prov_handler: Connected state

1b38a7411b6e2608aae50a6571807e04a6e90520b3b1e3c1e5b38cea4b9022e56485b92ff84289df218311972a42eb
>>
```

After sometime the provisioning app will exit and UART console will be stopped

```
I (1119728) app_prov: Stopping provisioning
I (1119728) protocomm_console: Stopping console...
I (1119728) app_prov: Provisioning stopped
I (1119748) protocomm_console: Console stopped
```

## Troubleshooting

### Provisioning failed

It is possible that the Wi-Fi credentials provided were incorrect, or the device was not able to establish connection to the network, in which the the `esp_prov` script will notify failure (with reason) and the provisioning app will continue running, allowing the user to retry the process. Serial monitor log will display the failure along with disconnect reason :

```
E (39291) app_prov: STA Disconnected
E (39291) app_prov: Disconnect reason : 201
I (39291) app_prov: STA AP Not found
I (42021) app_prov_handler: Disconnected state
```

### Provisioning does not start

If the serial monitor log is different, as shown below :

```
I (539) app_prov: Found ssid myssid
I (539) app_prov: Found password mypassword
I (549) app: Starting WiFi station
```

It means the Wi-Fi credentials were already set by some other application flashed previously to your device. To erase these credentials either do full erase and then flash the example

```
make erase_flash
idf.py -p PORT flash monitor
```

Or, enable `Reset Provisioning` option under `Example Configuration` under menuconfig. But this will erase the saved Wi-Fi credentials every time the device boots, so this is not the preferred solution.
