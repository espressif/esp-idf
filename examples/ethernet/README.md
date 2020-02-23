# Ethernet Examples

See the [README.md](../README.md) file in the upper level [examples](../) directory for more information about examples.

## Common Pin Assignments

### Using ESP32 internal MAC

* RMII PHY wiring is fixed and can not be changed through either IOMUX or GPIO Matrix. By default, they're connected as follows:

| GPIO   | RMII Signal | Notes        |
| ------ | ----------- | ------------ |
| GPIO21 | TX_EN       | EMAC_TX_EN   |
| GPIO19 | TX0         | EMAC_TXD0    |
| GPIO22 | TX1         | EMAC_TXD1    |
| GPIO25 | RX0         | EMAC_RXD0    |
| GPIO26 | RX1         | EMAC_RXD1    |
| GPIO27 | CRS_DV      | EMAC_RX_DRV  |

* SMI (Serial Management Interface) wiring is not fixed. You may need to changed it according to your board schematic. By default they're connected as follows:

| GPIO   | SMI Signal  | Notes         |
| ------ | ----------- | ------------- |
| GPIO23 | MDC         | Output to PHY |
| GPIO18 | MDIO        | Bidirectional |

* PHY chip has a reset pin, if want to do a hardware reset during initialization, then you have to connect it with one GPIO on ESP32. See more information from [here](#configure-the-project). The default GPIO used for resetting PHY chip is GPIO5.

### Using DM9051

* DM9051 Ethernet module consumes one SPI interface plus an interrupt and reset GPIO. By default they're connected as follows:

| GPIO   | DM9051      |
| ------ | ----------- |
| GPIO19 | SPI_CLK     |
| GPIO23 | SPI_MOSI    |
| GPIO25 | SPI_MISO    |
| GPIO22 | SPI_CS      |
| GPIO4  | Interrupt   |
| GPIO5  | Reset       |

## Common Configurations

1. In the `Example Configuration` menu:
    * Choose the kind of Ethernet under `Ethernet Type`.
    * If `Internal EMAC` is selected:
        * Choose PHY device under `Ethernet PHY Device`, by default, the **ESP32-Ethernet-Kit** has an `IP101` on board.
        * Set GPIO number used by SMI signal under `SMI MDC GPIO number` and `SMI MDIO GPIO number` respectively.
    * If `DM9051 Module` is selected:
        * Set SPI specific configuration, including SPI host number, GPIO number and clock rate.
    * Set GPIO number used by PHY chip reset under `PHY Reset GPIO number`, you may have to change the default value according to your board schematic. **PHY hardware reset can be disabled by set this value to -1**.
    * Set PHY address under `PHY Address`, you may have to change the default value according to your board schematic.

2. In the `Component config > Ethernet` menu:
    * Under `Support ESP32 internal EMAC controller` sub-menu:
        * In the `PHY interface`, select `Reduced Media Independent Interface (RMII)`, ESP-IDF currently only support RMII mode.
        * In the `RMII clock mode`, select one of the source that RMII clock (50MHz) comes from: `Input RMII clock from external` or `Output RMII clock from internal`.
        * If `Output RMII clock from internal` is enabled, you also have to set the GPIO number that used to output the RMII clock, under `RMII clock GPIO number`. In this case, you can set the GPIO number to 16 or 17.
        * If `Output RMII clock from GPIO0 (Experimental!)` is also enabled, then you have no choice but GPIO0 to output the RMII clock.
        * In `Amount of Ethernet DMA Rx buffers` and `Amount of Ethernet DMA Tx buffers`, you can set the amount of DMA buffers used for Tx and Rx.
    * Under `Support SPI to Ethernet Module` sub-menu, select the SPI module that you used for this example. Currently ESP-IDF only supports `DM9051`.

## Common Troubleshooting

* The data panel between ESP32's MAC and PHY needs a fixed 50MHz clock to do synchronization, which also called RMII clock. It can either be provided by an external oscillator or generated from internal APLL. The signal integrity of RMII clock is strict, so keep the trace as short as possible!
* If the RMII clock is generated from internal APLL, then APLL can't be used for other purpose (e.g. I2S).
