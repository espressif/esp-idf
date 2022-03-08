| Supported Targets | ESP32 | ESP32-S2 |
| ----------------- | ----- | -----   |

# Async request using ASIO

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The application aims to show how to connect to a Socks4 proxy using async operations with ASIO. The SOCKS protocol is 
briefly described by the diagram below. 

                   ┌──────┐                   ┌─────┐            ┌──────┐                                        
                   │Client│                   │Proxy│            │Target│                                        
                   └──┬───┘                   └──┬──┘            └──┬───┘                                        
                      │                          │                  │                                            
                      │                        ╔═╧══════════════╗   │                                            
══════════════════════╪════════════════════════╣ Initialization ╠═══╪════════════════════════════════════════════
                      │                        ╚═╤══════════════╝   │                                            
                      │                          │                  │                                            
  ╔══════════════════╗│                          │                  │                                            
  ║We establish a   ░║│     Socket Connection    │                  │                                            
  ║TCP connection    ║│ <─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ >                  │                                            
  ║and get a socket  ║│                          │                  │                                            
  ╚══════════════════╝│                          │                  │                                            
                      │                          │                  │                                            
                      │                        ╔═╧══════════════╗   │                                            
══════════════════════╪════════════════════════╣ Socks Protocol ╠═══╪════════════════════════════════════════════
                      │                        ╚═╤══════════════╝   │                                            
                      │                          │                  │                                            
                      │ Client Connection Request│                  │                                            
                      │ ─────────────────────────>                  │                                            
                      │                          │                  │                                            
                      │                          │                  │                                            
                      │             ╔════════════╪═══════╤══════════╪════════════════════════════════╗           
                      │             ║ TARGET CONNECTION  │          │                                ║           
                      │             ╟────────────────────┘          │  ╔═══════════════════╗         ║           
                      │             ║            │ Socket Connection│  ║Proxy establishes ░║         ║           
                      │             ║            │ <─ ─ ─ ─ ─ ─ ─ ─ >  ║ TCPconnection     ║         ║           
                      │             ║            │                  │  ║ with target host  ║         ║           
                      │             ╚════════════╪══════════════════╪══╚═══════════════════╝═════════╝           
                      │                          │                  │                                            
                      │      Response packet     │                  │                                            
                      │ <─────────────────────────                  │                                            
                      │                          │                  │                                            
                      │                          │                  │                                            
                      │                          │  ╔═══════╗       │                                            
══════════════════════╪══════════════════════════╪══╣ Usage ╠═══════╪════════════════════════════════════════════
                      │                          │  ╚═══════╝       │                                            
                      │                          │                  │                                            
   ╔═════════════════╗│                          │                  │                                            
   ║Client uses the ░║│                          │                  │                                            
   ║ socket opened   ║│                          │                  │                                            
   ║ with proxy      ║│ <─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─>                                            
   ║to communicate   ║│                          │                  │                                            
   ║                 ║│                          │                  │                                            
   ╚═════════════════╝┴───┐                   ┌──┴──┐            ┌──┴───┐                                        
                   │Client│                   │Proxy│            │Target│                                        
                   └──────┘                   └─────┘            └──────┘                                        


# Configure and Building example

This example requires the proxy address to be configured. You can do this using the menuconfig option.
Proxy address and port must be configured in order for this example to work. 

If using Linux ssh can be used as a proxy for testing. 

```
ssh -N -v -D 0.0.0.0:1080 localhost
```
# Async operations composition and automatic lifetime control

For documentation about the structure of this example look into [async\_request README](../async_request/README.md).

