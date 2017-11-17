# Ethernet Example
Initialises the Ethernet interface and enables it, then sends DHCP requests and tries to obtain a DHCP lease. If successful then you will be able to ping the device.

# PHY Configuration
Use `make menuconfig` to set the PHY model and the PHY address, and configure the SMI I/O pins (see below). These configuration items will vary depending on the hardware configuration you are using.

The default example configuration is correct for Espressif's Ethernet board with TLK110 PHY. Other hardware will require different configuration and/or changes to the example.

## PHY Address
The PHY address depends on the hardware and the PHY configuration. Consult the documentation/datasheet for the PHY hardware you have.

* Address 31 (default) for Espressif's Ethernet board with TLK110 PHY
* Address 1 for the common Waveshare LAN8720 PHY breakout
* Address 0 for other LAN8720 breakouts

If the PHY address is incorrect then the EMAC will initialise, but all attempts to read/write configuration registers on the PHY will fail.

## PHY Clock Wiring
The ESP32 and the Ethernet PHY need a common 50MHz reference clock. This clock can either be be provided externally by a crystal oscillator (e.g. crystal connected to the PHY or a seperate crystal oscillator) or internally by using the EPS32's APLL.

Because of its freqency the signal integrity has to be observed (ringing, capacitive load, resisitive load, skew, length of PCB trace). It is recommended to add a 33Ω resistor in series to reduce ringing.

Possible configurations of the 50MHz clock signal:

| Mode     | GPIO Pin | Signal name    | Notes                                                                                              |
| -------- | -------- | -------------- | -------------------------------------------------------------------------------------------------- |
| external | `GPIO0`  | `EMAC_TX_CLK`  | Input of 50MHz PHY clock                                                                           |
| internal | `GPIO0`  | `CLK_OUT1`     | Output of 50MHz APLL clock. Signal quality might be an issue.                                      |
| internal | `GPIO16` | `EMAC_CLK_OUT` | Output of 50MHz APLL clock.                                                                        |
| internal | `GPIO17` | `EMAC_CLK_180` | Inverted output of 50MHz APLL clock. Found to be best suitable for LAN8720 with long signal lines. |


#### External PHY Clock
The external reference clock of 50MHz must be supplied on `GPIO0`. See note about `GPIO0` below.

#### Internal PHY Clock
The ESP32 can generate a 50MHz clock using its APLL. When the APLL is already used as clock source for other purposes (most likely I²S) external PHY has to be used.

On different test setups clock output on `GPIO0` was found unstable because in most designs the signal path is not ideal for this high frequency (the PCB trace has several devices added to it and therefore the capacitive load is relatively high)

The inverted clock signal `EMAC_CLK_180` was found working best with a LAN8720 PHY.

## RMII PHY Wiring
The following PHY connections are required for RMII PHY data connections. These `GPIO` pin assignments cannot be changed.

| GPIO     | RMII Signal | ESP32 EMAC Function | Notes |
| -------- | ----------- | ------------------- | ----- |
| `GPIO21` | `TX_EN`     | `EMAC_TX_EN`        |       |
| `GPIO19` | `TX0`       | `EMAC_TXD0`         |       |
| `GPIO22` | `TX1`       | `EMAC_TXD1`         |       |
| `GPIO25` | `RX0`       | `EMAC_RXD0`         |       |
| `GPIO26` | `RX1`       | `EMAC_RXD1`         |       |
| `GPIO27` | `CRS_DV`    | `EMAC_RX_DRV`       |       |

## RMII PHY SMI Wiring

The following PHY connections are required for RMII PHY SMI (aka MDIO) management interface. These GPIO pin assignments can be changed to any unused GPIO pin.

For the example, these pins are configured via `make menuconfig` under the Example configuration.

| Default Example GPIO | RMII Signal | Notes         |
| -------------------- | ----------- | ------------- |
| `GPIO23`             | `MDC`       | Output to PHY |
| `GPIO18`             | `MDIO`      | Bidirectional |

The defaults in the example are correct for Espressif's Ethernet development board.

## Note about `GPIO0`

Because `GPIO0` is a strapping pin for entering UART flashing mode on reset, care must be taken when also using this pin as `EMAC_TX_CLK`. If the clock output from the PHY is oscillating during reset, the ESP32 may randomly enter UART flashing mode.

One solution is to use an additional GPIO as a "power pin", which either powers the PHY on/off or enables/disables the PHY's own oscillator. This prevents the clock signal from being active during a system reset. For this configuration to work, `GPIO0` also needs a pullup resistor and the "power pin" GPIO will need a pullup/pulldown resistor - as appropriate in order to keep the PHY clock disabled when the ESP32 is in reset.

See the example source code to see how the "power pin" GPIO can be managed in software.

The example defaults to using `GPIO17` for this function, but it can be overriden. On Espressif's Ethernet development board, `GPIO17` is the power pin used to enable/disable the PHY oscillator.
