# ESP-NETIF architecture
     
                           |          (A) USER CODE                 |
                           |                                        |
          .................| init          settings      events     |
          .                +----------------------------------------+
          .                   .                |           *
          .                   .                |           *
      --------+            +===========================+   *     +-----------------------+
              |            | new/config     get/set    |   *     |                       |
              |            |                           |...*.....| init                  |
              |            |---------------------------|   *     |                       |
        init  |            |                           |****     |                       |
        start |************|  event handler            |*********|  DHCP                 |
        stop  |            |                           |         |                       |
              |            |---------------------------|         |                       |
              |            |                           |         |    NETIF              |
        +-----|            |                           |         +-----------------+     |
        | glue|---<----|---|  esp_netif_transmit       |--<------| netif_output    |     |
        |     |        |   |                           |         |                 |     |
        |     |--->----|---|  esp_netif_receive        |-->------| netif_input     |     |
        |     |        |   |                           |         + ----------------+     |
        |     |...<....|...|  esp_netif_free_rx_buffer |...<.....| packet buffer         |
        +-----|     |  |   |                           |         |                       |
              |     |  |   |                           |         |         (D)           |
        (B)   |     |  |   |          (C)              |         +-----------------------+
      --------+     |  |   +===========================+
    communication   |  |                                               NETWORK STACK
    DRIVER          |  |           ESP-NETIF
                    |  |                                         +------------------+
                    |  |   +---------------------------+.........| open/close       |
                    |  |   |                           |         |                  |
                    |  -<--|  l2tap_write              |-----<---|  write           |
                    |      |                           |         |                  |
                    ---->--|  esp_vfs_l2tap_eth_filter |----->---|  read            |
                           |                           |         |                  |
                           |            (E)            |         +------------------+
                           +---------------------------+
                                                                      USER CODE
                                ESP-NETIF L2 TAP

##  Data/event flow:

* `........`     Initialization line from user code to esp-netif and comm driver

* `--<--->--`    Data packets going from communication media to TCP/IP stack and back

* `********`     Events agregated in ESP-NETIP propagates to driver, user code and network stack

*  `|`           User settings and runtime configuration

##  Components:

###  A) User code, boiler plate
Overall application interaction with communication media and network stack
  
  * initialization code
      - create a new instance of ESP-NETIF
      - configure the object with
          1) netif specific options (flags, behaviour, name)
          2) network stack options (netif init and input functions, not publicly available)
          3) IO driver specific options (transmit, tx_free functions, IO driver handle)
    - setup event handlers
    - use default handlers for common interfaces defined in IO drivers; or define a specific handlers
                        for customised behaviour/new interfaces
    - register handlers for app related events (such as IP lost/acquired)
  - interact with network interfaces using ESP-NETIF API

###  B) Communication driver, IO driver, media driver
  * event handler
      - define behaviour patterns of interaction with ESP-NETIF (example: ehternet link-up -> turn netif on)
  * glue IO layer: adapt the input/output functions to use esp-netif transmit/input/free_rx
      - install driver_transmit to appropriate ESP-NETIF object, so that outgoing packets from
                network stack are passed to the IO driver
      - calls esp_netif_receive to pass incoming data to network stack

###  C) ESP-NETIF
* init API (new, configure)
* IO API: for passing data between IO driver and network stack
* event/action API (esp-netif lifecycle management)
  - building blocks for designing event handlers
* setters, getters
* network stack abstraction: enabling user interaction with TCP/IP stack
  - netif up/down
  - DHCP server, client
  - DNS API
* driver conversion utilities

###  D) Network stack: no public interaction with user code (wrtt interfaces)

###  E) ESP-NETIF L2 TAP Interface
The ESP-NETIF L2 TAP interface is ESP-IDF mechanism utilized to access Data Link Layer (L2 per OSI/ISO) for frame reception and
transmission from user application. Its typical usage in embedded world might be implementation of non-IP related protocols
such as PTP, Wake on LAN and others. Note that only Ethernet (IEEE 802.3)
is currently supported. 

From user perspective, the ESP-NETIF L2 TAP interface is accessed using file descriptors of VFS which provides a file-like interfacing
(using functions like ``open()``, ``read()``, ``write()``, etc).
 
There is only one ESP-NETIF L2 TAP interface device (path name) available. However multiple file descriptors with different configuration
can be opened at a time since the ESP-NETIF L2 TAP interface can be understood as generic entry point to the NETIF internal structure.
Important is then specific configuration of particular file descriptor. It can be configured to give an access to specific Network Interface
identified by ``if_key`` (e.g. `ETH_DEF`) and to filter only specific frames based on their type (e.g. Ethernet type in case of IEEE 802.3).
Filtering only specific frames is crucial since the ESP-NETIF L2 TAP needs to work along with IP stack and so the IP related traffic
(IP, ARP, etc.) should not be passed directly to the user application. Even though such option is still configurable, it is not recommended in
standard use cases. Filtering is also advantageous from a perspective the user’s application gets access only to frame types it is interested
in and the remaining traffic is either passed to other L2 TAP file descriptors or to IP stack.
