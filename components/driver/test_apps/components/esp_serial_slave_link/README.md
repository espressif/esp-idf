# Espressif Serial Slave Link (ESSL) component

This component used to reside in [esp-idf](https://github.com/espressif/esp-idf) project as its component.

It's used on the HOST, to communicate with ESP chips as SLAVE via SDIO/SPI slave HD mode.

The port layer (`essl_sdio.c/essl_spi.c`) are currently only written to run on ESP chips in master mode, but you may also modify them to work on more platforms.

See more documentation: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/esp_serial_slave_link.html

But it's going to be an idf-extra-component, and should be pulled-in by an idf_component.yml (See `examples/peripherals/sdio/host/main/idf_component.yml`).

This folder is here for two purposes:

1. Fix a version for tests, to make them stable and not affected by external factors such as network, changes in idf-extra-components, etc.
2. Build Programming Guide from ESP-IDF before ESSL actually have a place to put the documents.
