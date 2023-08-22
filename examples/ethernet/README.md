| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

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

* One of the following GPIO pins can be used as RMII REF_CLK input/output:

| GPIO   | Function             | Notes        |
| ------ | -------------------- | ------------ |
| GPIO0  | EMAC_TX_CLK/CLK_OUT1 | input/output |
| GPIO16 | EMAC_CLK_OUT         | output       |
| GPIO17 | EMAC_CLK_180         | output       |

* SMI (Serial Management Interface) wiring is not fixed. You may need to changed it according to your board schematic. By default they're connected as follows:

| GPIO   | SMI Signal  | Notes         |
| ------ | ----------- | ------------- |
| GPIO23 | MDC         | Output to PHY |
| GPIO18 | MDIO        | Bidirectional |

* PHY chip has a reset pin, if want to do a hardware reset during initialization, then you have to connect it with one GPIO on ESP32. See more information from [here](#configure-the-project). The default GPIO used for resetting PHY chip is GPIO5.

### Using SPI ethernet modules

* SPI Ethernet modules (DM9051, W5500, ...) typically consume one SPI interface plus an interrupt and reset GPIO. They can be connected as follows for ESP32 as an example. However, they can be remapped to any pin using the GPIO Matrix.

| GPIO   | DM9051      |
| ------ | ----------- |
| GPIO14 | SPI_CLK     |
| GPIO13 | SPI_MOSI    |
| GPIO12 | SPI_MISO    |
| GPIO15 | SPI_CS      |
| GPIO4  | Interrupt   |
| NC     | Reset       |

---

**Warning:**
Please consult Espressif Technical reference manual along with datasheet for specific ESP Module you use when assigning any other pins, especially when choosing from system configuration menu for the ethernet examples, some pins cannot be used (they may already be utilized for different purpose like SPI Flash/RAM, some pins might be inputs only, etc.).

---

## Common Configurations

1. In the `Example Ethernet Configuration` menu:
    * Choose the kind of Ethernet.
    * If `Internal EMAC` is selected:
        * Choose PHY device under `Ethernet PHY Device`, by default, the **ESP32-Ethernet-Kit** has an `IP101` on board.
        * Set GPIO number used by SMI signal under `SMI MDC GPIO number` and `SMI MDIO GPIO number` respectively.
    * If `SPI Ethernet` is selected:
        * Set SPI specific configuration, including SPI host number, GPIO numbers and clock rate.
        * Multiple Ethernet SPI modules of the same type can be connected to single SPI interface at a time. The modules then share data and CLK signals. The CS, interrupt and reset pins need to be specifically configured for each module separately.
    * Set GPIO number used by PHY chip reset under `PHY Reset GPIO number`, you may have to change the default value according to your board schematic. **PHY hardware reset can be disabled by set this value to -1**.
    * Set PHY address under `PHY Address`, you may have to change the default value according to your board schematic.

2. In the `Component config > Ethernet` menu:
    * Under `Support ESP32 internal EMAC controller` sub-menu:
        * In the `PHY interface`, select `Reduced Media Independent Interface (RMII)`, ESP-IDF currently only support RMII mode.
        * In the `RMII clock mode`, select one of the source that RMII clock (50MHz) comes from: `Input RMII clock from external` or `Output RMII clock from internal`.
        * If `Output RMII clock from internal` is enabled, you also have to set the GPIO number that used to output the RMII clock, under `RMII clock GPIO number`. In this case, you can set the GPIO number to 16 or 17.
        * If `Output RMII clock from GPIO0 (Experimental!)` is also enabled, then you have no choice but GPIO0 to output the RMII clock.
        * In `Amount of Ethernet DMA Rx buffers` and `Amount of Ethernet DMA Tx buffers`, you can set the amount of DMA buffers used for Tx and Rx.
    * Under `Support SPI to Ethernet Module` sub-menu, select the SPI module that you used for this example. Currently ESP-IDF only supports `DM9051`, `W5500` and `KSZ8851SNL`.

## Common Troubleshooting

* The data panel between ESP32's MAC and PHY needs a fixed 50MHz clock to do synchronization, which also called RMII clock. It can either be provided by an external oscillator or generated from internal APLL. The signal integrity of RMII clock is strict, so keep the trace as short as possible!
* If the RMII clock is generated from internal APLL, then APLL can't be used for other purpose (e.g. I2S).
* If you observe undefined behavior (e.g. LCD glitches) of any **SPI device** which works normally when Ethernet is not connected over internal EMAC, you need to adjust EMAC DMA burst length (the DMA is shared resource between EMAC and the SPI). The same applies when you observe Ethernet frames corruption at the output of SPI Ethernet module and you use combination of internal EMAC and SPI Ethernet module as network interfaces. To configure the EMAC DMA burst length, modify internal Ethernet initialization as follows:

```c
esp32_emac_config.dma_burst_len = ETH_DMA_BURST_LEN_4; // or other appropriate value
```
