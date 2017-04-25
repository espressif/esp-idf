# Ethernet Example

Initialises the ethernet interface and enables it, then sends DHCP requests and tries to obtain a DHCP lease. If successful then you will be able to ping the device.

# PHY Configuration

Use "make menuconfig" to set the PHY model and the PHY address, and configure the SMI I/O pins (see below). These configuration items will vary depending on the hardware configuration you are using.

The default example configuration is correct for Espressif's Ethernet board with TLK110 PHY. Other hardware will require different configuration and/or changes to the example.

## PHY Address

The PHY address depends on the hardware and the PHY configuration. Consult the documentation/datasheet for the PHY hardware you have.

* Default address 31 is correct for Espressif's Ethernet board with TLK110 PHY.
* Address 1 is correct for the common Waveshare LAN8720 PHY breakout.
* Other LAN8720 breakouts may take address 0.

If the PHY address is incorrect then the EMAC will initialise but all attempts to read/write configuration registers on the PHY will fail.

## RMII PHY Wiring

The following PHY connections are required for RMII PHY data connections. These GPIO pin assignments cannot be changed.

| GPIO    | RMII Signal | ESP32 EMAC Function | Notes |
| ------- | ----------- | ------------------- | ----- |
| 0       | REF_CLK     | EMAC_TX_CLK         | Currently this must be a 50MHz reference clock input from the PHY (ext_osc configuration). |
| 21      | TX_EN       | EMAC_TX_EN          | |
| 19      | TX0         | EMAC_TXD0           | |
| 22      | TX1         | EMAC_TXD1           | |
| 25      | RX0         | EMAC_RXD0           | |
| 26      | RX1         | EMAC_RXD1           | |
| 27      | CRS_DV      | EMAC_RX_DRV         | |

## RMII PHY SMI Wiring

The following PHY connections are required for RMII PHY SMI (aka MDIO) management interface. These GPIO pin assignments can be changed to any unused GPIO pin.

For the example, these pins are configured via `make menuconfig` under the Example configuration.

| Default Example GPIO | RMII Signal | Notes         |
| -------------------- | ----------- | ------------- |
| 23                   | MDC         | Output to PHY |
| 18                   | MDIO        | Bidirectional |

The defaults in the example are correct for Espressif's Ethernet development board.

## Note about GPIO0

Because GPIO0 is a strapping pin for entering UART flashing mode on reset, care must be taken when also using this pin as EMAC_TX_CLK. If the clock output from the PHY is oscillating during reset, the ESP32 may randomly enter UART flashing mode.

One solution is to use an additional GPIO as a "power pin", which either powers the PHY on/off or enables/disables the PHY's own oscillator. This prevents the clock signal from being active during a system reset. For this configuration to work, GPIO0 also needs a pullup resistor and the "power pin" GPIO will need a pullup/pulldown resistor - as appropriate in order to keep the PHY clock disabled when the ESP32 is in reset.

See the example source code to see how the "power pin" GPIO can be managed in software.

The example defaults to using GPIO 17 for this function, but it can be overriden. On Espressif's Ethernet development board, GPIO 17 is the power pin used to enable/disable the PHY oscillator.
