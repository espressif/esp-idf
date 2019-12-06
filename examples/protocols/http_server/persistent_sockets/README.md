# HTTPD Server Persistent Sockets Example

The Example consists of HTTPD server persistent sockets demo.
This sort of persistency enables the server to have independent sessions/contexts per client.

* Open the project configuration menu (`idf.py menuconfig`) to configure Wi-Fi or Ethernet. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.

* In order to test the HTTPD server persistent sockets demo :
    1. compile and burn the firmware `idf.py -p PORT flash`
    2. run `idf.py -p PORT monitor` and note down the IP assigned to your ESP module. The default port is 80
    3. run the test script "python2 scripts/adder.py \<IP\> \<port\> \<N\>"
        * the provided test script sends (POST) numbers from 1 to N to the server which has a URI POST handler for adding these numbers into an accumulator that is valid throughout the lifetime of the connection socket, hence persistent
        * the script does a GET before closing and displays the final value of the accumulator

See the README.md file in the upper level 'examples' directory for more information about examples.
