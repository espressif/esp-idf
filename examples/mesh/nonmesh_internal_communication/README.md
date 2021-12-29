# Non-Mesh Communication Example

This example demonstrates how to use mesh to forward non-mesh traffic to internet and back.
It's basically a fork of the `internal_communication` but with some additional (IP) configurations.
Basically it provides internet access via every node hidden SSID (`ESPM_XXYYZZ` where `XXYYZZ` are the last 3 bytes of the STA MAC address).

![nonmesh architecture](docs/esp_mesh_architecture_v2.png)

## Limitations
* No internet connectivity from the hidden SSID of the ROOT node (only from forward/leaf nodes)
* DNS server is not correctly forwarded which seems to be caused by the unset `ESP_NETIF_DHCP_SERVER` on the ap in `esp_netif_create_default_wifi_mesh_netifs`

## Perf

### WiFi iPerf client (directly connected to router SSID)
```
Accepted connection from 192.168.1.101, port 58107
[  5] local 192.168.1.107 port 5201 connected to 192.168.1.101 port 58108
[ ID] Interval           Transfer     Bitrate
[  5]   0.00-1.00   sec  9.44 MBytes  79.2 Mbits/sec                  
[  5]   1.00-2.00   sec  10.4 MBytes  87.5 Mbits/sec                  
[  5]   2.00-3.00   sec  10.6 MBytes  89.2 Mbits/sec                  
[  5]   3.00-4.00   sec  10.7 MBytes  89.9 Mbits/sec                  
[  5]   4.00-5.00   sec  10.2 MBytes  85.1 Mbits/sec                  
[  5]   5.00-6.00   sec  10.8 MBytes  90.8 Mbits/sec                  
[  5]   6.00-7.00   sec  10.9 MBytes  91.1 Mbits/sec                  
[  5]   7.00-8.00   sec  11.0 MBytes  92.5 Mbits/sec                  
[  5]   8.00-9.00   sec  10.7 MBytes  89.5 Mbits/sec                  
[  5]   9.00-10.00  sec  10.7 MBytes  89.6 Mbits/sec                  
[  5]  10.00-10.04  sec   433 KBytes  93.8 Mbits/sec                  
- - - - - - - - - - - - - - - - - - - - - - - - -
[ ID] Interval           Transfer     Bitrate
[  5]   0.00-10.04  sec   106 MBytes  88.5 Mbits/sec                  receiver
```

### WiFi iPerf client (via 2-layer mesh network)
```
Accepted connection from 192.168.1.101, port 58087
[  5] local 192.168.1.107 port 5201 connected to 192.168.1.101 port 58088
[ ID] Interval           Transfer     Bitrate
[  5]   0.00-1.00   sec   669 KBytes  5.48 Mbits/sec                  
[  5]   1.00-2.00   sec   778 KBytes  6.37 Mbits/sec                  
[  5]   2.00-3.00   sec   950 KBytes  7.78 Mbits/sec                  
[  5]   3.00-4.00   sec   874 KBytes  7.16 Mbits/sec                  
[  5]   4.00-5.00   sec   868 KBytes  7.11 Mbits/sec                  
[  5]   5.00-6.00   sec   894 KBytes  7.32 Mbits/sec                  
[  5]   6.00-7.00   sec   796 KBytes  6.52 Mbits/sec                  
[  5]   7.00-8.00   sec   861 KBytes  7.05 Mbits/sec                  
[  5]   8.00-9.00   sec   874 KBytes  7.16 Mbits/sec                  
[  5]   9.00-10.00  sec   853 KBytes  6.99 Mbits/sec                  
[  5]  10.00-10.05  sec  48.1 KBytes  8.05 Mbits/sec                  
- - - - - - - - - - - - - - - - - - - - - - - - -
[ ID] Interval           Transfer     Bitrate
[  5]   0.00-10.05  sec  8.27 MBytes  6.90 Mbits/sec                  receiver
```