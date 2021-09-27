# Setup dummy network interfaces
```
sudo ip link add eth2 type dummy
sudo ip addr add 192.168.1.200/24 dev eth2
sudo ip link set eth2 up
sudo ifconfig eth2 multicast
```

# Dig on a specified interface

```
dig +short -b 192.168.1.200 -p 5353 @224.0.0.251 myesp.local
```

# Run avahi to browse services

Avahi needs the netif to have the "multicast" flag set

```bash
david@david-comp:~/esp/idf (feature/mdns_networking_socket)$ avahi-browse -a -r -p
+;eth2;IPv6;myesp-service2;Web Site;local
+;eth2;IPv4;myesp-service2;Web Site;local
=;eth2;IPv6;myesp-service2;Web Site;local;myesp.local;192.168.1.200;80;"board=esp32" "u=user" "p=password"
=;eth2;IPv4;myesp-service2;Web Site;local;myesp.local;192.168.1.200;80;"board=esp32" "u=user" "p=password"
```
