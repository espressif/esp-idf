# Non-Mesh IP Internal Communication Example

This example demonstrates how to use mesh to forward non-mesh IP traffic to internet edge and back. 
It's basically a fork of the `ip_internal_network` but where a node can be converted to an entrypoint node 
for non-mesh devices to reach internet via the mesh network. It can be seen as a WiFi extender/repeater. 

![nonmesh architecture](docs/esp_mesh_architecture.png)

## Functionality

This example uses experimental NAT feature to translate addresses/ports from an internal subnet (mesh-network), that is created 
by the root node running a DHCP server. 
At the same time, a node can be converted in an entrypoint node for non-mesh device.
To do so you need to send `ap` command to the esp console of the desired node.
This command reconfigure the soft-AP and creates a new WiFI network (with DHCP server support). 
Traffic coming from that network (nonmesh-network) will be NATed and forwarded to the root node and ultimately reach the Internet.

### Hardware Required

This example can be executed on any platform board, the only required interface is WiFi and connection to internet.

### Configure the project

Open the project configuration menu (`idf.py menuconfig`) to configure the mesh network channel, router SSID, router password and mesh softAP settings.

### Build and Flash

Build the project and flash it to multiple boards forming a mesh network, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

The following logs shows how an ICMP echo goes from the entrypoint to internet and back.
* 172.16.1.0/24 is the mesh network where the IPs are given by the root node
* 172.16.0.0/24 is the non-mesh network exposed by the entrypoint node

### Output sample from mesh entrypoint node
```
I (73949) mesh_main: [NODE] LAYER: 2 | TYPE: 2 | MESH_SSID: ESPM_702FC0 | IP:172.16.0.2 | AP: 40:f5:20:70:2f:c1 | STA: 40:f5:20:70:2f:c0
I (73949) mesh_main: Interface ap, NAPT: 1, IPv4: 172.16.1.1
I (73959) mesh_main: Interface st, NAPT: 0, IPv4: 172.16.0.2
I (73959) mesh_main: Interface lo, NAPT: 0, IPv4: 127.0.0.1
Processing packet coming from 'ap' src 172.16.1.2 dest 1.1.1.1
Checking NAPT table for dest: 1.1.1.1
W (74889) wifi:NAPT enabled, skip check
Destination address doesnt match NAPT interface address 172.16.1.1
<ba-add>idx:2 (ifx:1, 8c:aa:b5:a7:a2:98), tid:0, ssn:0, winSize:64
ip_input(ap): iphdr->dest 0x1010101 iphdr->src 0x20110ac netif->ip_addr 0x10110ac (0x10101, 0x110ac, 0x1000000)
ip_input(st): iphdr->dest 0x1010101 iphdr->src 0x20110ac netif->ip_addr 0x20010ac (0x10101, 0x10ac, 0x1000000)
ip_input(lo): iphdr->dest 0x1010101 iphdr->src 0x20110ac netif->ip_addr 0x100007f (0x1, 0x7f, 0x1010100)
ip4_input: packet from ap not for us.
ip4_route_src_hook 172.16.1.2 -> 1.1.1.1
 ip4_route_src_hook ap
 ip4_route_src_hook ap UP with IP  172.16.1.1
 ip4_route_src_hook st
 ip4_route_src_hook st UP with IP  172.16.0.2
 ip4_route_src_hook lo
 ip4_route_src_hook lo UP with IP  127.0.0.1
I (74969) mesh_main: [NODE] LAYER: 2 | TYPE: 2 | MESH_SSID: ESPM_702FC0 | IP:172.16.0.2 | AP: 40:f5:20:70:2f:c1 | STA: 40:f5:20:70:2f:c0
ip4_route matching for dest IP 1.1.1.1
 ip4_route ap
I (74979) mesh_main: Interface ap, NAPT: 1, IPv4: 172.16.1.1
 ip4_route matching on if ap with IP  172.16.1.1
 ip4_route st
 ip4_route matching on if st with IP  172.16.0.2
 ip4_route lo
 ip4_route matching on if lo with IP  127.0.0.1
I (74989) mesh_main: Interface st, NAPT: 0, IPv4: 172.16.0.2
ip4_route_src_hook -1.-1.-1.-1 -> 1.1.1.1
I (75009) mesh_main: Interface lo, NAPT: 0, IPv4: 127.0.0.1
ip4_route returning default interface st
ip_napt_find
looking up in table src: 172.16.1.2, port: 44975, mport: 0
NAPT table:
 src                     dest                    sport   dport   mport   
+-----------------------+-----------------------+-------+-------+-------+
| 172 |  16 |   1 |   2 |   1 |   1 |   1 |   1 | 44975 | 44975 | 44975 |
ip_napt_free
NAPT table:
 src                     dest                    sport   dport   mport   
+-----------------------+-----------------------+-------+-------+-------+
not found
ip_napt_insert(): TCP=0, UDP=0, ICMP=1
ip_napt_add
NAPT table:
 src                     dest                    sport   dport   mport   
+-----------------------+-----------------------+-------+-------+-------+
| 172 |  16 |   1 |   2 |   1 |   1 |   1 |   1 | 44975 | 44975 | 44975 |
[ICMP] ip_napt_forward src: 0x20010ac, dest: 0x20010ac, 
ip4_forward(ap->st): forwarding packet from 172.16.1.2 to 1.1.1.1
I (75099) mesh_netif: Sending to root, dest addr: 40:f5:20:71:fa:e5, size: 74
I (75459) mesh_netif: Node received: from: 40:f5:20:70:2f:c0 to 40:f5:20:71:fa:e5 size: 74
I (75469) mesh_netif: Forwarding response to TCP/IP stack of STA interface!
Processing packet coming from 'st' src 1.1.1.1 dest 172.16.0.2
Checking NAPT table for dest: 172.16.0.2
ip_napt_find
looking up in table dest: 1.1.1.1, port: 44975, mport: 44975
NAPT table:
 src                     dest                    sport   dport   mport   
+-----------------------+-----------------------+-------+-------+-------+
| 172 |  16 |   1 |   2 |   1 |   1 |   1 |   1 | 44975 | 44975 | 44975 |
found
[ICMP] ip_napt_recv dst: 0x20010ac, dest: 0x20110ac, 
ip_input(st): iphdr->dest 0x20110ac iphdr->src 0x1010101 netif->ip_addr 0x20010ac (0x110ac, 0x10ac, 0x2000000)
ip_input(ap): iphdr->dest 0x20110ac iphdr->src 0x1010101 netif->ip_addr 0x10110ac (0x110ac, 0x110ac, 0x2000000)
ip_input(lo): iphdr->dest 0x20110ac iphdr->src 0x1010101 netif->ip_addr 0x100007f (0xac, 0x7f, 0x2011000)
ip4_input: packet from st not for us.
ip4_route_src_hook 1.1.1.1 -> 172.16.1.2
 ip4_route_src_hook ap
 ip4_route_src_hook ap UP with IP  172.16.1.1
 ip4_route_src_hook st
 ip4_route_src_hook st UP with IP  172.16.0.2
 ip4_route_src_hook lo
 ip4_route_src_hook lo UP with IP  127.0.0.1
ip4_route matching for dest IP 172.16.1.2
 ip4_route ap
 ip4_route matching on if ap with IP  172.16.1.1
 ip4_route match on if ap with IP  172.16.1.1 netmask 255.255.255.0
[NAPT] ip_napt_forward NAPT not enabled for interface st
ip4_forward(st->ap): forwarding packet from 1.1.1.1 to 172.16.1.2
```

### Output sample from the root node
```
I (74162) mesh_main: [ROOT] LAYER: 1 | TYPE: 1 | MESH_SSID: ESPM_71FAE4 | IP:10.1.50.150 | AP: 40:f5:20:71:fa:e5 | STA: 40:f5:20:71:fa:e4
I (74172) mesh_main: Interface ap, NAPT: 1, IPv4: 172.16.0.1
I (74192) mesh_main: Interface st, NAPT: 0, IPv4: 10.1.50.150
I (73932) mesh_netif: Root received: from: 40:f5:20:71:fa:e5 to 40:f5:20:70:2f:c0 size: 74
Processing packet coming from 'ap' src 172.16.0.2 dest 1.1.1.1
Checking NAPT table for dest: 1.1.1.1
NAPT enabled, skip check
Destination address doesnt match NAPT interface address 172.16.0.1
ip_input(ap): iphdr->dest 0x1010101 iphdr->src 0x20010ac netif->ip_addr 0x10010ac (0x10101, 0x10ac, 0x1000000)
ip_input(st): iphdr->dest 0x1010101 iphdr->src 0x20010ac netif->ip_addr 0x9632010a (0x101, 0x10a, 0x1010000)
ip_input(lo): iphdr->dest 0x1010101 iphdr->src 0x20010ac netif->ip_addr 0x100007f (0x1, 0x7f, 0x1010100)
ip4_input: packet from ap not for us.
ip4_route_src_hook 172.16.0.2 -> 1.1.1.1
 ip4_route_src_hook ap
 ip4_route_src_hook ap UP with IP  172.16.0.1
 ip4_route_src_hook st
 ip4_route_src_hook st UP with IP  10.1.50.150
 ip4_route_src_hook lo
 ip4_route_src_hook lo UP with IP  127.0.0.1
ip4_route matching for dest IP 1.1.1.1
 ip4_route ap
 ip4_route matching on if ap with IP  172.16.0.1
 ip4_route st
 ip4_route matching on if st with IP  10.1.50.150
 ip4_route lo
 ip4_route matching on if lo with IP  127.0.0.1
ip4_route_src_hook -1.-1.-1.-1 -> 1.1.1.1
ip4_route returning default interface st
ip_napt_find
looking up in table src: 172.16.0.2, port: 44975, mport: 0
NAPT table:
 src                     dest                    sport   dport   mport   
+-----------------------+-----------------------+-------+-------+-------+
| 172 |  16 |   0 |   2 |   1 |   1 |   1 |   1 | 44975 | 44975 | 44975 |
ip_napt_free
NAPT table:
 src                     dest                    sport   dport   mport   
+-----------------------+-----------------------+-------+-------+-------+
not found
ip_napt_insert(): TCP=0, UDP=0, ICMP=1
ip_napt_add
NAPT table:
 src                     dest                    sport   dport   mport   
+-----------------------+-----------------------+-------+-------+-------+
| 172 |  16 |   0 |   2 |   1 |   1 |   1 |   1 | 44975 | 44975 | 44975 |
[ICMP] ip_napt_forward src: 0x9632010a, dest: 0x9632010a, 
ip4_forward(ap->st): forwarding packet from 172.16.0.2 to 1.1.1.1
Processing packet coming from 'st' src 1.1.1.1 dest 10.1.50.150
Checking NAPT table for dest: 10.1.50.150
ip_napt_find
looking up in table dest: 1.1.1.1, port: 44975, mport: 44975
NAPT table:
 src                     dest                    sport   dport   mport   
+-----------------------+-----------------------+-------+-------+-------+
| 172 |  16 |   0 |   2 |   1 |   1 |   1 |   1 | 44975 | 44975 | 44975 |
found
[ICMP] ip_napt_recv dst: 0x9632010a, dest: 0x20010ac, 
ip_input(st): iphdr->dest 0x20010ac iphdr->src 0x1010101 netif->ip_addr 0x9632010a (0x10ac, 0x10a, 0x2000000)
ip_input(ap): iphdr->dest 0x20010ac iphdr->src 0x1010101 netif->ip_addr 0x10010ac (0x10ac, 0x10ac, 0x2000000)
ip_input(lo): iphdr->dest 0x20010ac iphdr->src 0x1010101 netif->ip_addr 0x100007f (0xac, 0x7f, 0x2001000)
ip4_input: packet from st not for us.
ip4_route_src_hook 1.1.1.1 -> 172.16.0.2
 ip4_route_src_hook ap
 ip4_route_src_hook ap UP with IP  172.16.0.1
 ip4_route_src_hook st
 ip4_route_src_hook st UP with IP  10.1.50.150
I (74212) mesh_main: Interface lo, NAPT: 0, IPv4: 127.0.0.1
 ip4_route_src_hook lo
 ip4_route_src_hook lo UP with IP  127.0.0.1
ip4_route matching for dest IP 172.16.0.2
 ip4_route ap
 ip4_route matching on if ap with IP  172.16.0.1
 ip4_route match on if ap with IP  172.16.0.1 netmask 255.255.255.0
[NAPT] ip_napt_forward NAPT not enabled for interface st
ip4_forward(st->ap): forwarding packet from 1.1.1.1 to 172.16.0.2
```
