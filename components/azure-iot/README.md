
# Get Started with Microsoft Azure IoT Starter Kit - ESP32-DevKitC ("Core Board")

This sample was tested with Espressif's **ESP32-DevKitC ("Core Board")**, but many other kits would work as well.

Don't have a kit yet? Click [here](http://esp32.net/)

This sample was modified from [this one](https://github.com/ustccw/AzureESP32.git).

## Step 1 - Download the ESP32 SDK

Clone the [Espressif IoT Development Framework](https://github.com/espressif/esp-idf) repository with the following command:

`git clone https://github.com/espressif/esp-idf.git --recursive`

## Step 2 - Set up the ESP32 toolchain

Follow the instructions for setting up the ESP32 toolchain found [here](http://esp-idf.readthedocs.io/en/latest/#setup-toolchain).

## Step 3 - Set IDF_PATH

Set the IDF_PATH environment variable to point to the location of the **esp-idf** directory that you 
cloned in Step 1.

If you're using MSYS on Windows, a good place to set the IDF_PATH variable is in the 
`~\msys32\home\user\.bashrc` file that gets created the first time you run `msys2_shell.cmd`.

## Step 4 - Assemble the sample project

Your `$IDF_PATH/components/azure-iot/sample` directory contains a sample project skeleton. You may 
compile it from that location, or you may copy it to a new location, whichever you prefer. 
To flesh out the skeleton,
copy the sample files from the Azure IoT SDK into `main` directory of the copy of the sample that 
you're using:

`sloc=$IDF_PATH/components/azure-iot/sdk/iothub_client/samples/iothub_client_sample_mqtt`<br/>
`cp $sloc/iothub_client_sample_mqtt.h <your_sample_location>/main`<br/>
`cp $sloc/iothub_client_sample_mqtt.c <your_sample_location>/main`

## Step 5 - Set your device's connection string

Create an IoT Hub and an associated device identity 
[as shown here](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-csharp-csharp-getstarted).
Then open the `main/iothub_client_sample_mqtt.c` file in the `main` directory of your 
project and find the line near the top that reads 

```c
static const char* connectionString = "";

```

and set the value of the connectionString variable to the be the connection string of the device 
identity that you created.

## Step 6- Configure the make process

Using the toolchain you installed in Step 2 (MSYS, for example), navigate to the location of the 
sample you assembled in Step 4 and run the following command:

`make menuconfig`

This command will bring up a configuration dialog.

1. Under "Serial flasher config --->Default serial port" set the serial port ID to that of your ESP32 device. (On Windows you can find the serial port ID under Computer Management.)

1. Under "Example Configuration --->" enter your WiFi router SSID and password.

1. Save the configuration and exit the dialog.

## Step 7 - Run the make process

Build the sample with the simple command:

`make`

This will produce a iothub_client_sample_mqtt.bin file, a partitions_singleapp.bin file, a bootloader/bootloader.bin file, plus associated maps.

## Step 8 - Flash the ESP32 device

Make sure the ESP32 device is plugged in and run the command:

`make flash`

This will flash the project onto the ESP32 device. Alternate methods of flashing the device can be found [here](https://espressif.com/en/support/download/other-tools)

## Step 9 - Monitor the device output

The sample program sends status output to the device's serial port at a default 115200 baud. You monitor this output by connecting to the serial port with any terminal program such as [Putty](http://www.putty.org/).

