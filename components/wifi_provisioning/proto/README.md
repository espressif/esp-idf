# Protobuf files for defining Wi-Fi provisioning packet structures

`wifi_provisioning` uses Google Protobuf for language, transport and architecture agnostic protocol communication. These proto files define the protocomm packet structure, separated across multiple files:
* wifi_contants.proto - Defines the various enums for indicating state of Wi-Fi (connected / disconnect / connecting), diconnect reasons, auth modes, etc.
* wifi_config.proto - Defines Wi-Fi configuration structures and commands for setting credentials (SSID, passphrase, BSSID), applying credentials and getting connection state.
* wifi_scan.proto - Defines Wi-Fi scan commands and result structures

Note : These proto files are not automatically compiled during the build process.

# Compilation

Compilation requires protoc (Protobuf Compiler) and protoc-c (Protobuf C Compiler) installed. Since the generated files are to remain the same, as long as the proto files are not modified, therefore the generated files are already available under `components/wifi_provisioning/proto-c` and `components/wifi_provisioning/python` directories, and thus running cmake / make (and installing the Protobuf compilers) is optional.

If using `cmake` follow the below steps. If using `make`, jump to Step 2 directly.

## Step 1 (Only for cmake)

When using cmake, first create a build directory and call cmake from inside:

```
mkdir build
cd build
cmake ..
```

## Step 2

Simply run `make` to generate the respective C and Python files. The newly created files will overwrite those under `components/wifi_provisioning/proto-c` and `components/wifi_provisioning/python`
