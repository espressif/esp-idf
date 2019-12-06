# Openssl Example

The Example contains of OpenSSL client demo.

Open the project configuration menu (`idf.py menuconfig`):

* Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../README.md) for more details.

* When using Make build system, set `Default serial port` under `Serial flasher config`.

* Configure target domain and port number under "Example Configuration"

If you want to test the OpenSSL client demo:
  1. compile the code and load the firmware
  2. open the UART TTY, then you can see it print the context of target domain

See the README.md file in the upper level 'examples' directory for more information about examples.
