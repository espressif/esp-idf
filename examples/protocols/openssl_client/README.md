# Openssl Example

The Example contains of OpenSSL client demo.

First you should config the project by "make menuconfig":
  Example Configuration ->
    1. Target Domain : the domain that you want to connect to, and default is "www.baidu.com".
    2. Target port number : the port number of the target domain, and default is 443.
    3. WIFI SSID : your own WIFI, which is connected to the Internet, and default is "myssid".
    4. WIFI Password : WIFI password, and default is "mypassword"

If you want to test the OpenSSL client demo:
  1. compile the code and load the firmware
  2. open the UART TTY, then you can see it print the context of target domain

See the README.md file in the upper level 'examples' directory for more information about examples.
