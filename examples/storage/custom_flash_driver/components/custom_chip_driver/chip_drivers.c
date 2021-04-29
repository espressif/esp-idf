/* Custom flash driver example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "spi_flash_chip_driver.h"
#include "spi_flash_chip_generic.h"
#include "spi_flash_chip_issi.h"
#include "spi_flash_chip_mxic.h"
#include "spi_flash_chip_gd.h"
#include "spi_flash_chip_winbond.h"
#include "spi_flash_chip_boya.h"

extern const spi_flash_chip_t esp_flash_chip_eon;

//Override the default chip driver provided by the IDF, CONFIG_SPI_FLASH_OVERRIDE_CHIP_DRIVER_LIST should be set
const spi_flash_chip_t *default_registered_chips[] = {
    &esp_flash_chip_eon,
    &esp_flash_chip_issi,
    &esp_flash_chip_gd,
    &esp_flash_chip_mxic,
    &esp_flash_chip_winbond,
    &esp_flash_chip_boya,
    // Default chip drivers that will accept all chip ID.
    // FM and XMC chips are supposed to be supported by this chip driver.
    &esp_flash_chip_generic,
    NULL,
};
