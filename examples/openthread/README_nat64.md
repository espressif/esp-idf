# Visiting the Internet on the Thread network with NAT64

The Espressif Thread border router supports NAT64 which allows Thread devices to visit the IPv4 Internet.

## Building the Thread border router with NAT64

The Thread border router supports NAT64 by default. After the Wi-Fi and the Thread network is launched, we can fetch the NAT64 prefix:

```
> br nat64prefix
fd97:83ad:debc:2:0:0::/96
Done
```

This prefix will be used for translating the IPv4 addresses.

## Visiting IPv4 UDP servers

Build and run the OpenThread cli example with the default configuration and join the Thread network formed by the border router.
On the local machine, setup a basic UDP echo server. The destination UDP address will be the hexadecimal IPv4 address added to the NAT64 prefix.
Say the host has address 192.168.1.2 and the NAT64 prefix is fd97:83ad:debc:2:0:0::/96, the destination address will be fd97:83ad:debc:2:0:0:c0a8:0102.

```
> udp send fd97:83ad:debc:2:0:0:c0a8:0102 4388 hello

Done
> 5 bytes from fd97:83ad:debc:2:0:0:c0a8:102 4388 hello
```

## Visiting IPv4 HTTP servers

For visiting HTTP servers with domain names, we need to first set the DNS server:

```
> dns64server 8.8.8.8
Done
```

Now we can curl the HTTP server

```
> curl http://www.google.com

Done
> I (33809) HTTP_CLIENT: Body received in fetch header state, 0x3fca4025, 111
<!doctype html><html itemscope="" itemtype="http://schema.org/WebPage" lang="zh-HK"><head><meta content="text/html; charset=UTF-8" http-equiv="Content-Type"> ...
// Other HTTP response to follow
```
