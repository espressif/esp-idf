| Supported Targets | ESP32 |
| ----------------- | ----- |

# ESP32-WROOM-32SE ECDSA sign/verify example
## Description
This example requires [ESP32-WROOM-32SE](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32se_datasheet_en.pdf) which has Microchip's [ATECC608A](https://www.microchip.com/wwwproducts/en/ATECC608A) (Secure Element) integrated on the module. The example performs `ECDSA sign/verify` functions on sample data using hardware private key stored in ATECC608A chip.If you want to use bare `ATECC608A` chip (Secure Element) with `ESP32-WROOM-32` module by making external connections, please refer [this](https://github.com/espressif/esp-cryptoauthlib/blob/master/esp_cryptoauth_utility/README.md#using-atecc608a-with-esp32-wroom-32) for details.
See the README.md file in the upper level examples directory for more information about examples.
## Hardware
To get started you will need a  `ESP32-WROOM-32SE` development board which integrates Microchip’s ATECC608A CryptoAuth chip in the module.

## Configuration
ATECC608A chip on ESP32-WROOM-32SE should be configured to run the example, for details on configuration of ATECC608A chip, please refer [esp_cryptoauth_utility](https://github.com/espressif/esp-cryptoauthlib/blob/master/esp_cryptoauth_utility/README.md#esp_cryptoauth_utility)

1) Set type of `ATECC608A` chip in menuconfig.

*   `menuconfig->Component config->esp-cryptoauthlib->Choose Type of ATECC608A chip`.
for more details refer [Find ATECC608A chip type](https://github.com/espressif/esp-cryptoauthlib/blob/master/esp_cryptoauth_utility/README.md#find-type-of-atecc608a-chip-connected-to-esp32-wroom32-se).

2) Enable `Hardware ECDSA sign/verify` in menuconfig as by enabling following. (Enabled by default for this example)

*   `menuconfig->Component config->esp-cryptoauthlib->Enable Hardware ECDSA keys for mbedTLS`
*   `menuconfig->Component config->esp-cryptoauthlib->Enable ATECC608A sign operations in mbedTLS`
*   `menuconfig->Component config->esp-cryptoauthlib->Enable ATECC608A verify operations in mbedTLS`

## Build and Flash

Run following command to build and flash the project.
```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.
