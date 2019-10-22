# ESP-NETIF architecture
     
                     |          (A) USER CODE                 |
                     |                                        |
        .............| init          settings      events     |
        .            +----------------------------------------+          
        .               .                |           *
        .               .                V           *                                   
    --------+        +===========================+   *     +-----------------------+
            |        | new/config     get/set    |   *     |                       |
            |        |                           |...*.....| init                  |
            |        |---------------------------|   *     |                       |
      init  |        |                           |****     |                       |
      start |********|  event handler            |*********|  DHCP                 |
      stop  |        |                           |         |                       |
            |        |---------------------------|         |                       | 
            |        |                           |         |    NETIF              |
      +-----|        |                           |         +-----------------+     |   
      | glue|----<---|  esp_netif_transmit       |--<------| netif_output    |     |
      |     |        |                           |         |                 |     |
      |     |---->---|  esp_netif_receive        |-->------| netif_input     |     |
      |     |        |                           |         + ----------------+     |
      |     |....<...|  esp_netif_free_rx_buffer |...<.....| packet buffer         |
      +-----|        |                           |         |                       |  
            |        |                           |         |                       |  
      (B)   |        |                           |         +-----------------------+
    --------+        +===========================+
    communication                                                NETWORK STACK
    DRIVER                   ESP-NETIF                          
    
    
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

###  C) ESP-NETIF, former tcpip_adapter
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


##  Data/event flow:

* `........`     Initialization line from user code to esp-netif and comm driver

* `--<--->--`    Data packets going from communication media to TCP/IP stack and back

* `********`     Events agregated in ESP-NETIP propagates to driver, user code and network stack

*  `|`           User settings and runtime configuration

