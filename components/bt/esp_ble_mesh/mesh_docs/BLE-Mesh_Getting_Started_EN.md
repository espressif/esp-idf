# Introduction

Bluetooth mesh networking enables many-to-many (m:m) device communications and is optimized for creating large-scale device networks.

Devices may relay data to other devices not in direct radio range of the originating device. In this way, mesh networks can span very large physical areas and contain large numbers of devices. It is ideally suited for building automation, sensor networks, and other IoT solutions where tens, hundreds, or thousands of devices need to reliably and securely communicate with one another.

Bluetooth mesh is not a wireless communications technology, but a networking technology. This technology is dependent upon Bluetooth Low Energy (BLE) - a wireless communications protocol stack.


# Specifications

The official specifications for Bluetooth mesh can be found [here](https://www.bluetooth.com/specifications/mesh-specifications)


# Getting Started with BLE Mesh on ESP32

If you are new to ESP32, you may first need to go through the [Getting Started guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html).

Built on top of Zephyr BLE Mesh stack, the ESP BLE Mesh implementation supports device provisioning and node control. It also supports such node features as Proxy, Relay, Low power and Friend.


## Access to ESP BLE Mesh
Since you are on this page, you should already have access to Espressif BLE Mesh SDK. If you do not have access, please get in touch with your point of contact.

## Documentation

The ESP BLE Mesh code in the SDK is organized as below. Each folder contains source files related to it as well as a subfolder with header files for the exposed functionality.

```
$tree components/bt/ble_mesh/

├── api           /* BLE Mesh functionality exposed through esp_ble_mesh_* APIs for the applications */
│   ├── core      /* BLE Mesh Core APIs */
│   │   └── include
│   └── models    /* Foundation Models and other Client Models APIs */
│       └── include
├── btc
│   └── include
├── mesh_core     /* BLE mesh core based on Zephyr BLE stack with miscellaneous modifications and
│   │              an adaptation layer to make it work with ESP32 */
│   └── include
├── mesh_docs     /* BLE Mesh docs */
└── mesh_models   /* Foundation Models and other Client Models implementations */
    └── include
```

To demonstrate the features supported by BLE Mesh SDK, a few sample examples have been added. Each example has a README.md file for quick start as well as a walkthrough file that explains the functionality in detail.

Below is a snapshot of the BLE Mesh examples directory

```
$ tree examples/bluetooth/ble_mesh/
├── ble_mesh_client_model
│   ├── main
│   │   ├── ble_mesh_client_model_main.c
│   │   ├── board.c
│   │   ├── board.h
│   │   ├── component.mk
│   │   └── Kconfig.projbuild
│   ├── Makefile
│   ├── README.md
│   └── sdkconfig.defaults
├── ble_mesh_node
│   ├── main
│   │   ├── ble_mesh_demo_main.c
│   │   ├── board.c
│   │   ├── board.h
│   │   ├── component.mk
│   │   └── Kconfig.projbuild
│   ├── Makefile
│   ├── README.md
│   ├── sdkconfig.defaults
│   └── tutorial
│       └── Ble_Mesh_Node_Example_Walkthrough.md
├── ble_mesh_provisioner
│   ├── main
│   │   ├── ble_mesh_demo_main.c
│   │   ├── board.c
│   │   ├── board.h
│   │   ├── component.mk
│   │   └── Kconfig.projbuild
│   ├── Makefile
│   ├── README.md
│   ├── sdkconfig.defaults
│   └── tutorial
│       └── Ble_Mesh_Provisioner_Example_Walkthrough.md
├──ble_mesh_console
│   ├── ble_mesh_node
│   └── ble_mesh_provisioner
├──ble_mesh_fast_provision
│   ├── ble_mesh_fast_prov_client
│   └── ble_mesh_fast_prov_server
├──ble_mesh_vendor_models
│   ├── fast_prov_vendor_model
└──ble_mesh_wifi_coexist
    ├── main
    ├── components
    └── tutorial
        └── ble_mesh_wifi_coexist.md
8 directories, 26 files
```


## Hardware and Setup

At present ESP32-DevKitC and ESP-WROVER-KIT are supported for BLE Mesh implementation. You can find the details about the modules [here](https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/modules-and-boards.html)

You can choose the board through menuconfig: `make menuconfig -> Example Configuration -> Board selection for BLE Mesh`

Note that if you plan to use ESP32-DevKitC, you need to connect an RGB LED to GPIO pins 25, 26 and 27.


## Sample Examples

* BLE Mesh Node

This example shows the use of BLE Mesh as a node device having a Configuration Server model and a Generic OnOff Server model. A BLE Mesh provisioner can then provision the node and control a RGB LED representing on/off state.

* BLE Mesh Client Model

This example shows how a Generic OnOff Client model within a node works. The node has a Configuration Server model, a Generic OnOff Server model and a Generic OnOff Client model.

* BLE Mesh Provisioner

This example shows how a device can act as a BLE Mesh provisioner to provision devices. The provisioner has a Configuration Server model, a Configuration Client model and a Generic OnOff Client model.


## Mobile Apps

ESP BLE Mesh implementation is compatible with a few phone apps, including Silicon Labs BLE Mesh and nRF Mesh. These apps are available on Google Play and App Store. In addition, Espressif offers its own Android app which is currently being actively developed. You can find the latest APK file [here](http://download.espressif.com/BLE_MESH/BLE_Mesh_Tools/BLE_Mesh_App/EspBleMesh-0.9.4.apk).

Note: The most recent tested version 1.1.0 of Silicon Labs App has a bug, which has been fixed by a workaround on the SDK side. The fix is implemented through a configuration option enabled by default. For other Android/iOS apps, this option needs to be disabled from menuconfig:
`make menuconfig -> Example Configuration -> This option fixes the bug of Silicon Lab Android App 1.1.0 when reconnection will cause the sequence number to recount from 0`

## Building and Flashing

If you build the application for the first time, the menuconfig screen will pop up. You can choose the board from the Example Configuration option. Additionally, you can modify the serial settings in the Serial flasher config option in accordance with your port configuration.

BLE Mesh specific configuration options can also be modified through: `make menuconfig -> Component config -> Bluetooth Mesh support`

You can still change options at any other time using `make menuconfig`.

```
$ export IDF_PATH=/path/to/esp-ble-mesh-sdk-v0.x

$ cd examples/bluetooth/ble_mesh/<example_name>

$ make -j8 flash monitor
```


# Reporting Issues

* If you find a bug or have a feature request, go to [the Issues section on GitHub](https://github.com/espressif/esp-idf/issues). Before reporting a new issue, please check the existing issues at the provided link and the FAQs document in the `mesh_docs` folder.
* When you submit an issue or a feature request on GitHub, please add the tag "BLE Mesh" in the issue title for our faster reaction.
