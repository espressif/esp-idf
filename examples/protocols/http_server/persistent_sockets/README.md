# HTTPD Server Persistant Sockets Example

The Example consists of HTTPD server persistent sockets demo.
This sort of persistancy enables the server to have independent sessions/contexts per client.

* Configure the project using "make menuconfig" and goto :
    * Example Configuration ->
        1. WIFI SSID: WIFI network to which your PC is also connected to.
        2. WIFI Password: WIFI password

* In order to test the HTTPD server persistent sockets demo :
    1. compile and burn the firmware "make flash"
    2. run "make monitor" and note down the IP assigned to your ESP module. The default port is 80
    3. run the test script "python2 scripts/adder.py \<IP\> \<port\> \<N\>"
        * the provided test script sends (POST) numbers from 1 to N to the server which has a URI POST handler for adding these numbers into an accumulator that is valid throughout the lifetime of the connection socket, hence persistent
        * the script does a GET before closing and displays the final value of the accumulator

See the README.md file in the upper level 'examples' directory for more information about examples.
