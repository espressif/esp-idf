# Openthread Extension Command

The Openthread Extension Command is a series of command lines extended from [OpenThread CLI](https://github.com/openthread/openthread/blob/main/src/cli/README.md). 
Openthread Extension Command doesn't run alone, it needs to be used in conjunction with these examples:
* [ot_cli](../ot_cli)
* [ot_br](../ot_br)

## How to use the example

Run the configuration command `idf.py menuconfig` to enable the operations you need for your example: `OpenThread Extension CLI` -> `Enable Espressif's extended features`.

## OT Extension Command Line List

* [iperf](#iperf)
* [tcpsockclient](#tcpsockclient)
* [tcpsockserver](#tcpsockserver)
* [udpsockclient](#udpsockclient)
* [udpsockserver](#udpsockserver)
* [wifi](#wifi)

### iperf

Iperf is a tool for active measurements of the maximum achievable bandwidth on Thread network, which is based TCP and UDP.

Print the iperf help:
```bash
iperf
---iperf parameter---
-s                  :     server mode, only receive
-u                  :     upd mode
-V                  :     use IPV6 address  
-c <addr>           :     client mode, only transmit
-i <interval>       :     seconds between periodic bandwidth reports
-t <time>           :     time in seconds to transmit for (default 10 secs)
-p <port>           :     server port to listen on/connect to
-l <len_send_buf>   :     the lenth of send buffer
---example---
create a tcp server :     iperf -s -i 3 -p 5001 -t 60 
create a udp client :     iperf -c <addr> -u -i 3 -t 60 -p 5001 -l 512
Done
```

Create an iperf TCP service for 20 seconds, with printing logs for each 3 seconds, at the port 5001:
```bash
> iperf -V -s -t 20 -i 3 -p 5001
Done
```

For UDP service:
```bash
> iperf -V -u -s -t 20 -i 3 -p 5001
Done
```


Create an iperf TCP client connecting to `fdde:ad00:beef:0:a7c6:6311:9c8c:271b`(the iperf server address) for 30 seconds, with printing logs for each second, at the port 5001, with 85 length of each packets:
```bash
> iperf -V -c fdde:ad00:beef:0:a7c6:6311:9c8c:271b -t 20 -i 1 -p 5001 -l 85
Done
        Interval Bandwidth
   0-   1 sec       0.05 Mbits/sec
   1-   2 sec       0.05 Mbits/sec
   2-   3 sec       0.05 Mbits/sec
   3-   4 sec       0.05 Mbits/sec
   4-   5 sec       0.05 Mbits/sec
...
   19-   20 sec       0.05 Mbits/sec
   0-   20 sec       0.05 Mbits/sec
```

For UDP client:
```bash
> iperf -V -u -c fdde:ad00:beef:0:a7c6:6311:9c8c:271b -t 20 -i 1 -p 5001 -l 85
Done
```

### tcpsockclient

Used for creating a tcp client.

```bash
> tcpsockclient fdde:ad00:beef:0:a7c6:6311:9c8c:271b
Done
ot_socket: Socket created, connecting to fdde:ad00:beef:0:a7c6:6311:9c8c:271b:12345
ot_socket: Successfully connected
...
```

### tcpsockserver

Used for creating a tcp server.

```bash
> tcpsockserver
Done
I (1310225) ot_socket: Socket created
I (1310225) ot_socket: Socket bound, port 12345
I (1310225) ot_socket: Socket listening, timeout is 30 seconds
```

### udpsockclient

Used for creating a udp client.

```bash
> udpsockclient fdde:ad00:beef:0:a7c6:6311:9c8c:271b
Done
ot_socket: Socket created, connecting to fdde:ad00:beef:0:a7c6:6311:9c8c:271b:12345
ot_socket: Successfully connected
...
```

### udpsockserver

Used for creating a udp server.

```bash
> udpsockserver
Done
I (1310225) ot_socket: Socket created
I (1310225) ot_socket: Socket bound, port 12345
I (1310225) ot_socket: Socket listening, timeout is 30 seconds
```

### wifi

Get help for wifi command:

```bash
> wifi                   
---wifi parameter---                                                                                                                      
connect                                                                                                                                                                                                                                                                             
-s                   :     wifi ssid                                                                                                                                                                                                                                                
-p                   :     wifi psk                                 
---example---                                                                                                                             
join a wifi:                           
ssid: threadcertAP                                                   
psk: threadcertAP    :     wifi connect -s threadcertAP -p threadcertAP                                                                   
state                :     get wifi state, disconnect or connect
---example---                                                                                                                             
get wifi state       :     wifi state                                                                                                     
Done                                                                                                                                      
```

Join a wifi with ssid: threadcertAP and psk: threadcertAP:

```bash
> wifi connect -s threadcertAP -p threadcertAP                                                                                            
ssid: threadcertAP                                                                                                                        
psk: threadcertAP                  
I (11331) wifi:wifi driver task: 3ffd06e4, prio:23, stack:6656, core=0                                                                    
I (11331) system_api: Base MAC address is not set                                                                                         
I (11331) system_api: read default base MAC address from EFUSE                                                                            
I (11341) wifi:wifi firmware version: 45c46a4                                                                                             
I (11341) wifi:wifi certification version: v7.0                                                                                           


..........

I (13741) esp_netif_handlers: sta ip: 192.168.3.10, mask: 255.255.255.0, gw: 192.168.3.1                                                  
W (13771) wifi:<ba-add>idx:0 (ifx:0, 02:0f:c1:32:3b:2b), tid:0, ssn:2, winSize:64
wifi sta is connected successfully                       
Done                                                       
```

Get the state of the WiFi:

```bash
> wifi state                                                
connected                                                    
Done
```

## Extension command example

### TCP and UDP Example

Before running this example, a thread network with a Leader and a router( or child) needs to be set up.

On the leader device, start a TCP or UDP server, on router device, start a TCP or UDP client.

(note: replace the parameter of the `tcpsockclient` or `udpsockclient` with the leader's IPv6 address)

### Iperf Example

Example for using iperf:

Before running iperf, a thread network with a Leader and a router( or child) needs to be set up.

On the leader device, start iperf TCP or UDP server, on the router device, start iperf TCP or UDP client.

