# RMT Transmit Example

This example shows how to configure and operate the remote control (RMT) peripheral to transmit a sample message in the [Morse code](https://en.wikipedia.org/wiki/Morse_code).

Configuration (pin number, etc.) can be modified in the top of the `main/rmt_tx_main.c` file.

To be able to see and hear the message output by the RMT, connect a LED and a speaker or an earphone (be careful - it may be loud) to the GPIO configured under `RMT_TX_GPIO` define (default is GPIO 18).

Example connections:

```
             330R            LED     
GPIO18 +----/\/\/\----+------|>|-----+ GND
                      |    
                      | /|
                     +-+ |   Speaker
                     | | |     or
                     +-+ |  earphone
                      | \|
                      |
                      +--------------+ GND
```


See the README.md file in the upper level 'examples' directory for more information about examples.
