/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_flash.h"
#include "memspi_host_driver.h"
#include "esp_flash_spi_init.h"
#include "driver/gpio.h"
#include "esp_rom_gpio.h"
#include "esp_rom_efuse.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "hal/spi_types.h"
#include "driver/spi_common_internal.h"
#include "hal/spi_flash_hal.h"
#include "hal/gpio_hal.h"
#include "esp_flash_internal.h"
#include "esp_rom_gpio.h"
#include "esp_private/spi_flash_os.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/spi_flash.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/spi_flash.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/spi_flash.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/spi_flash.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/spi_flash.h"
#endif

__attribute__((unused)) static const char TAG[] = "spi_flash";

/* This pointer is defined in ROM and extern-ed on targets where CONFIG_SPI_FLASH_ROM_IMPL = y*/
#if !CONFIG_SPI_FLASH_ROM_IMPL
esp_flash_t *esp_flash_default_chip = NULL;
#endif

#ifndef CONFIG_SPI_FLASH_USE_LEGACY_IMPL

#ifdef CONFIG_ESPTOOLPY_FLASHFREQ_80M
#define DEFAULT_FLASH_SPEED ESP_FLASH_80MHZ
#elif defined CONFIG_ESPTOOLPY_FLASHFREQ_40M
#define DEFAULT_FLASH_SPEED ESP_FLASH_40MHZ
#elif defined CONFIG_ESPTOOLPY_FLASHFREQ_26M
#define DEFAULT_FLASH_SPEED ESP_FLASH_26MHZ
#elif defined CONFIG_ESPTOOLPY_FLASHFREQ_20M
#define DEFAULT_FLASH_SPEED ESP_FLASH_20MHZ
#elif defined CONFIG_ESPTOOLPY_FLASHFREQ_120M
#define DEFAULT_FLASH_SPEED ESP_FLASH_120MHZ
#else
#error Flash frequency not defined! Check the ``CONFIG_ESPTOOLPY_FLASHFREQ_*`` options.
#endif

#if defined(CONFIG_ESPTOOLPY_FLASHMODE_QIO)
#define DEFAULT_FLASH_MODE  SPI_FLASH_QIO
#elif defined(CONFIG_ESPTOOLPY_FLASHMODE_QOUT)
#define DEFAULT_FLASH_MODE  SPI_FLASH_QOUT
#elif defined(CONFIG_ESPTOOLPY_FLASHMODE_DIO)
#define DEFAULT_FLASH_MODE  SPI_FLASH_DIO
#elif defined(CONFIG_ESPTOOLPY_FLASHMODE_DOUT)
#define DEFAULT_FLASH_MODE  SPI_FLASH_DOUT
#elif defined(CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_STR)
#define DEFAULT_FLASH_MODE SPI_FLASH_OPI_STR
#elif defined(CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_DTR)
#define DEFAULT_FLASH_MODE SPI_FLASH_OPI_DTR
#else
#define DEFAULT_FLASH_MODE SPI_FLASH_FASTRD
#endif

//TODO: modify cs hold to meet requirements of all chips!!!
#if CONFIG_IDF_TARGET_ESP32
#define ESP_FLASH_HOST_CONFIG_DEFAULT()  (memspi_host_config_t){ \
    .host_id = SPI1_HOST,\
    .speed = DEFAULT_FLASH_SPEED, \
    .cs_num = 0, \
    .iomux = false, \
    .input_delay_ns = 0,\
    .cs_setup = 1,\
}
#elif CONFIG_IDF_TARGET_ESP32S2
#define ESP_FLASH_HOST_CONFIG_DEFAULT()  (memspi_host_config_t){ \
    .host_id = SPI1_HOST,\
    .speed = DEFAULT_FLASH_SPEED, \
    .cs_num = 0, \
    .iomux = true, \
    .input_delay_ns = 0,\
    .cs_setup = 1,\
}
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/efuse.h"
#define ESP_FLASH_HOST_CONFIG_DEFAULT()  (memspi_host_config_t){ \
    .host_id = SPI1_HOST,\
    .speed = DEFAULT_FLASH_SPEED, \
    .cs_num = 0, \
    .iomux = true, \
    .input_delay_ns = 0,\
    .cs_setup = 1,\
}
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/efuse.h"
#if !CONFIG_SPI_FLASH_AUTO_SUSPEND
#define ESP_FLASH_HOST_CONFIG_DEFAULT()  (memspi_host_config_t){ \
    .host_id = SPI1_HOST,\
    .speed = DEFAULT_FLASH_SPEED, \
    .cs_num = 0, \
    .iomux = true, \
    .input_delay_ns = 0,\
    .cs_setup = 1,\
}
#else
#define ESP_FLASH_HOST_CONFIG_DEFAULT()  (memspi_host_config_t){ \
    .host_id = SPI1_HOST,\
    .speed = DEFAULT_FLASH_SPEED, \
    .cs_num = 0, \
    .iomux = true, \
    .input_delay_ns = 0,\
    .auto_sus_en = true,\
    .cs_setup = 1,\
}
#endif //!CONFIG_SPI_FLASH_AUTO_SUSPEND
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/efuse.h"
#if !CONFIG_SPI_FLASH_AUTO_SUSPEND
#define ESP_FLASH_HOST_CONFIG_DEFAULT()  (memspi_host_config_t){ \
    .host_id = SPI1_HOST,\
    .speed = DEFAULT_FLASH_SPEED, \
    .cs_num = 0, \
    .iomux = true, \
    .input_delay_ns = 0,\
}
#else
#define ESP_FLASH_HOST_CONFIG_DEFAULT()  (memspi_host_config_t){ \
    .host_id = SPI1_HOST,\
    .speed = DEFAULT_FLASH_SPEED, \
    .cs_num = 0, \
    .iomux = true, \
    .input_delay_ns = 0,\
    .auto_sus_en = true,\
}
#endif //!CONFIG_SPI_FLASH_AUTO_SUSPEND
#endif


static IRAM_ATTR NOINLINE_ATTR void cs_initialize(esp_flash_t *chip, const esp_flash_spi_device_config_t *config, bool use_iomux, int cs_id)
{
    //Not using spicommon_cs_initialize since we don't want to put the whole
    //spi_periph_signal into the DRAM. Copy these data from flash before the
    //cache disabling
    int cs_io_num = config->cs_io_num;
    int spics_in = spi_periph_signal[config->host_id].spics_in;
    int spics_out = spi_periph_signal[config->host_id].spics_out[cs_id];
    int spics_func = spi_periph_signal[config->host_id].func;
    uint32_t iomux_reg = GPIO_PIN_MUX_REG[cs_io_num];

    //To avoid the panic caused by flash data line conflicts during cs line
    //initialization, disable the cache temporarily
    chip->os_func->start(chip->os_func_data);
    PIN_INPUT_ENABLE(iomux_reg);
    if (use_iomux) {
        gpio_hal_iomux_func_sel(iomux_reg, spics_func);
    } else {
#if SOC_GPIO_PIN_COUNT <= 32
        GPIO.enable_w1ts.val = (0x1 << cs_io_num);
#else
        if (cs_io_num < 32) {
            GPIO.enable_w1ts = (0x1 << cs_io_num);
        } else {
            GPIO.enable1_w1ts.data = (0x1 << (cs_io_num - 32));
        }
#endif
        GPIO.pin[cs_io_num].pad_driver = 0;
        esp_rom_gpio_connect_out_signal(cs_io_num, spics_out, false, false);
        if (cs_id == 0) {
            esp_rom_gpio_connect_in_signal(cs_io_num, spics_in, false);
        }
        gpio_hal_iomux_func_sel(iomux_reg, PIN_FUNC_GPIO);
    }
    chip->os_func->end(chip->os_func_data);
}

esp_err_t spi_bus_add_flash_device(esp_flash_t **out_chip, const esp_flash_spi_device_config_t *config)
{
    if (out_chip == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!GPIO_IS_VALID_OUTPUT_GPIO(config->cs_io_num)) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_flash_t *chip = NULL;
    memspi_host_inst_t *host = NULL;
    esp_err_t ret = ESP_OK;

    uint32_t caps = MALLOC_CAP_DEFAULT;
    if (config->host_id == SPI1_HOST) caps = MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT;

    chip = (esp_flash_t*)heap_caps_malloc(sizeof(esp_flash_t), caps);
    if (!chip) {
        ret = ESP_ERR_NO_MEM;
        goto fail;
    }

    host = (memspi_host_inst_t*)heap_caps_malloc(sizeof(memspi_host_inst_t), caps);
    *chip = (esp_flash_t) {
        .read_mode = config->io_mode,
        .host = (spi_flash_host_inst_t*)host,
    };
    if (!host) {
        ret = ESP_ERR_NO_MEM;
        goto fail;
    }

    int dev_id = -1;
    esp_err_t err = esp_flash_init_os_functions(chip, config->host_id, &dev_id);
    if (err == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGE(TAG, "Init os functions failed! No free CS.");
    } else if (err == ESP_ERR_INVALID_ARG) {
        ESP_LOGE(TAG, "Init os functions failed! Bus lock not initialized (check CONFIG_SPI_FLASH_SHARE_SPI1_BUS).");
    }
    if (err != ESP_OK) {
        ret = err;
        goto fail;
    }
    // When `CONFIG_SPI_FLASH_SHARE_SPI1_BUS` is not enabled on SPI1 bus, the
    // `esp_flash_init_os_functions` will not be able to assign a new device ID. In this case, we
    // use the `cs_id` in the config structure.
    if (dev_id == -1 && config->host_id == SPI1_HOST) {
        dev_id = config->cs_id;
    }
    assert(dev_id < SOC_SPI_PERIPH_CS_NUM(config->host_id) && dev_id >= 0);

    bool use_iomux = spicommon_bus_using_iomux(config->host_id);
    memspi_host_config_t host_cfg = {
        .host_id = config->host_id,
        .cs_num = dev_id,
        .iomux = use_iomux,
        .input_delay_ns = config->input_delay_ns,
        .speed = config->speed,
    };
    err = memspi_host_init_pointers(host, &host_cfg);
    if (err != ESP_OK) {
        ret = err;
        goto fail;
    }

    // The cs_id inside `config` is deprecated, use the `dev_id` provided by the bus lock instead.
    cs_initialize(chip, config, use_iomux, dev_id);
    *out_chip = chip;
    return ret;
fail:
    // The memory allocated are free'd in the `spi_bus_remove_flash_device`.
    spi_bus_remove_flash_device(chip);
    return ret;
}

esp_err_t spi_bus_remove_flash_device(esp_flash_t *chip)
{
    if (chip==NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_flash_deinit_os_functions(chip);
    free(chip->host);
    free(chip);
    return ESP_OK;
}

/* The default (ie initial boot) no-OS ROM esp_flash_os_functions_t */
extern const esp_flash_os_functions_t esp_flash_noos_functions;

static DRAM_ATTR memspi_host_inst_t esp_flash_default_host;

static DRAM_ATTR esp_flash_t default_chip = {
    .read_mode = DEFAULT_FLASH_MODE,
    .host = (spi_flash_host_inst_t*)&esp_flash_default_host,
    .os_func = &esp_flash_noos_functions,
};

extern esp_err_t esp_flash_suspend_cmd_init(esp_flash_t* chip);
esp_err_t esp_flash_init_default_chip(void)
{
    const esp_rom_spiflash_chip_t *legacy_chip = &g_rom_flashchip;
    memspi_host_config_t cfg = ESP_FLASH_HOST_CONFIG_DEFAULT();

    #if !CONFIG_IDF_TARGET_ESP32
    // For esp32s2 spi IOs are configured as from IO MUX by default
    cfg.iomux = esp_rom_efuse_get_flash_gpio_info() == 0 ?  true : false;
    #endif

    #if CONFIG_ESPTOOLPY_OCT_FLASH
    cfg.octal_mode_en = 1;
    cfg.default_io_mode = DEFAULT_FLASH_MODE;
    #endif

    // For chips need time tuning, get value directely from system here.
    #if SOC_SPI_MEM_SUPPORT_TIME_TUNING
    if (spi_timing_is_tuned()) {
        cfg.using_timing_tuning = 1;
        spi_timing_get_flash_timing_param(&cfg.timing_reg);
    }
    #endif // SOC_SPI_MEM_SUPPORT_TIME_TUNING

    //the host is already initialized, only do init for the data and load it to the host
    esp_err_t err = memspi_host_init_pointers(&esp_flash_default_host, &cfg);
    if (err != ESP_OK) {
        return err;
    }

    // ROM TODO: account for non-standard default pins in efuse
    // ROM TODO: to account for chips which are slow to power on, maybe keep probing in a loop here
    err = esp_flash_init_main(&default_chip);
    if (err != ESP_OK) {
        return err;
    }
    if (default_chip.size < legacy_chip->chip_size) {
        ESP_EARLY_LOGE(TAG, "Detected size(%dk) smaller than the size in the binary image header(%dk). Probe failed.", default_chip.size/1024, legacy_chip->chip_size/1024);
        return ESP_ERR_FLASH_SIZE_NOT_MATCH;
    }

    if (default_chip.size > legacy_chip->chip_size) {
        ESP_EARLY_LOGW(TAG, "Detected size(%dk) larger than the size in the binary image header(%dk). Using the size in the binary image header.", default_chip.size/1024, legacy_chip->chip_size/1024);
    }
    default_chip.size = legacy_chip->chip_size;

    esp_flash_default_chip = &default_chip;
#ifdef CONFIG_SPI_FLASH_AUTO_SUSPEND
    err = esp_flash_suspend_cmd_init(&default_chip);
    if (err != ESP_OK) {
        return err;
    }
#endif
    return ESP_OK;
}

esp_err_t esp_flash_app_init(void)
{
    esp_err_t err = ESP_OK;
#if CONFIG_SPI_FLASH_SHARE_SPI1_BUS
    err = esp_flash_init_main_bus_lock();
    if (err != ESP_OK) return err;
#endif
    err = esp_flash_app_enable_os_functions(&default_chip);
    return err;
}

#endif //!CONFIG_SPI_FLASH_USE_LEGACY_IMPL
