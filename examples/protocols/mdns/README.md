# mDNS example

Shows how to use mDNS to advertise lookup services and hosts

## Example workflow

- mDNS is initialized with host name and instance name defined through `make menuconfig` and `_http._tcp` service is added to be advertised
- WiFi STA is started and trying to connect to the access point defined through `make menuconfig`
- The system event handler is used to pass the network events to mDNS so the service is aware when the interface comes up or down
- GPIO0 (BOOT Button) is initialized as pulled-up input that can be monitored for button press
- Example task is started to check if the button is pressed so it can execute the mDNS queries defined

## Running the example

- Run `make menuconfig` to configure the access point's SSID and Password and the default device mDNS host name and instance name
- Run `make flash monitor` to build and upload the example to your board and connect to it's serial terminal
- Wait for WiFi to connec to your access point
- You can now ping the device at `[hostname].local` and browse for `_http._tcp` on the same network to find the advertised service
- Pressing the BOOT button will start quring the local network for the predefined in `check_button` hosts and services

See the README.md file in the upper level 'examples' directory for more information about examples.
