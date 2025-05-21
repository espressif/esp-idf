/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
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
#include "esp_private/spi_share_hw_ctrl.h"
#include "esp_ldo_regulator.h"
#include "hal/spi_flash_hal.h"
#include "spi_flash_chip_driver.h"
#include "hal/gpio_hal.h"
#include "esp_flash_internal.h"
#include "esp_rom_gpio.h"
#include "esp_private/spi_flash_os.h"
#include "esp_private/cache_utils.h"
#include "esp_spi_flash_counters.h"
#include "esp_rom_spiflash.h"
#include "bootloader_flash.h"
#include "esp_check.h"

__attribute__((unused)) static const char TAG[] = "spi_flash";

#if !CONFIG_SPI_FLASH_AUTO_SUSPEND && !CONFIG_SPI_FLASH_PLACE_FUNCTIONS_IN_IRAM
#error "CONFIG_SPI_FLASH_PLACE_FUNCTIONS_IN_IRAM cannot be disabled when CONFIG_SPI_FLASH_AUTO_SUSPEND is disabled."
#endif

#if CONFIG_SPI_FLASH_ROM_IMPL && (CONFIG_ESPTOOLPY_FLASHSIZE_32MB || CONFIG_ESPTOOLPY_FLASHSIZE_64MB || CONFIG_ESPTOOLPY_FLASHSIZE_128MB)
#error "Flash chip size equal or over 32MB memory cannot use driver in ROM"
#endif

/* This pointer is defined in ROM and extern-ed on targets where CONFIG_SPI_FLASH_ROM_IMPL = y*/
#if !CONFIG_SPI_FLASH_ROM_IMPL
esp_flash_t *esp_flash_default_chip = NULL;
#endif

#if defined CONFIG_ESPTOOLPY_FLASHFREQ_120M
#define DEFAULT_FLASH_SPEED 120
#elif defined CONFIG_ESPTOOLPY_FLASHFREQ_80M
#define DEFAULT_FLASH_SPEED 80
#elif defined CONFIG_ESPTOOLPY_FLASHFREQ_64M
#define DEFAULT_FLASH_SPEED 64
#elif defined CONFIG_ESPTOOLPY_FLASHFREQ_60M
#define DEFAULT_FLASH_SPEED 60
#elif defined CONFIG_ESPTOOLPY_FLASHFREQ_48M
#define DEFAULT_FLASH_SPEED 48
#elif defined CONFIG_ESPTOOLPY_FLASHFREQ_40M
#define DEFAULT_FLASH_SPEED 40
#elif defined CONFIG_ESPTOOLPY_FLASHFREQ_32M
#define DEFAULT_FLASH_SPEED 32
#elif defined CONFIG_ESPTOOLPY_FLASHFREQ_30M
#define DEFAULT_FLASH_SPEED 30
#elif defined CONFIG_ESPTOOLPY_FLASHFREQ_26M
#define DEFAULT_FLASH_SPEED 26
#elif defined CONFIG_ESPTOOLPY_FLASHFREQ_24M
#define DEFAULT_FLASH_SPEED 24
#elif defined CONFIG_ESPTOOLPY_FLASHFREQ_20M
#define DEFAULT_FLASH_SPEED 20
#elif defined CONFIG_ESPTOOLPY_FLASHFREQ_16M
#define DEFAULT_FLASH_SPEED 16
#elif defined CONFIG_ESPTOOLPY_FLASHFREQ_15M
#define DEFAULT_FLASH_SPEED 15
#elif defined CONFIG_ESPTOOLPY_FLASHFREQ_12M
#define DEFAULT_FLASH_SPEED 12
#else
#error Flash frequency not defined! Check the ``CONFIG_ESPTOOLPY_FLASHFREQ_*`` options.
#endif

#if defined(CONFIG_ESPTOOLPY_FLASHMODE_QIO)
#define DEFAULT_FLASH_MODE  SPI_FLASH_QIO
#define FLASH_MODE_STRING   "qio"
#elif defined(CONFIG_ESPTOOLPY_FLASHMODE_QOUT)
#define DEFAULT_FLASH_MODE  SPI_FLASH_QOUT
#define FLASH_MODE_STRING   "qout"
#elif defined(CONFIG_ESPTOOLPY_FLASHMODE_DIO)
#define DEFAULT_FLASH_MODE  SPI_FLASH_DIO
#define FLASH_MODE_STRING   "dio"
#elif defined(CONFIG_ESPTOOLPY_FLASHMODE_DOUT)
#define DEFAULT_FLASH_MODE  SPI_FLASH_DOUT
#define FLASH_MODE_STRING   "dout"
#elif defined(CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_STR)
#define DEFAULT_FLASH_MODE SPI_FLASH_OPI_STR
#define FLASH_MODE_STRING   "opi_str"
#elif defined(CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_DTR)
#define DEFAULT_FLASH_MODE SPI_FLASH_OPI_DTR
#define FLASH_MODE_STRING   "opi_dtr"
#else
#define DEFAULT_FLASH_MODE SPI_FLASH_FASTRD
#define FLASH_MODE_STRING   "fast_rd"
#endif

//TODO: modify cs hold to meet requirements of all chips!!!
#if CONFIG_IDF_TARGET_ESP32
#define ESP_FLASH_HOST_CONFIG_DEFAULT()  (memspi_host_config_t){ \
    .host_id = SPI1_HOST,\
    .freq_mhz = DEFAULT_FLASH_SPEED, \
    .cs_num = 0, \
    .iomux = false, \
    .input_delay_ns = 0,\
    .cs_setup = 1,\
}
#else // Other target
#if !CONFIG_SPI_FLASH_AUTO_SUSPEND
#define ESP_FLASH_HOST_CONFIG_DEFAULT()  (memspi_host_config_t){ \
    .host_id = SPI1_HOST,\
    .freq_mhz = DEFAULT_FLASH_SPEED, \
    .cs_num = 0, \
    .iomux = true, \
    .input_delay_ns = 0,\
    .cs_setup = 1,\
}
#else
#define ESP_FLASH_HOST_CONFIG_DEFAULT()  (memspi_host_config_t){ \
    .host_id = SPI1_HOST,\
    .freq_mhz = DEFAULT_FLASH_SPEED, \
    .cs_num = 0, \
    .iomux = true, \
    .input_delay_ns = 0,\
    .auto_sus_en = true,\
    .cs_setup = 1,\
}
#define TSUS_VAL_SUSPEND CONFIG_SPI_FLASH_SUSPEND_TSUS_VAL_US
#endif //!CONFIG_SPI_FLASH_AUTO_SUSPEND
#endif // Other target

static IRAM_ATTR NOINLINE_ATTR void cs_initialize(esp_flash_t *chip, const esp_flash_spi_device_config_t *config, bool cs_use_iomux, int cs_id)
{
    //Not using spicommon_cs_initialize since we don't want to put the whole
    //spi_periph_signal into the DRAM. Copy these data from flash before the
    //cache disabling
    int cs_io_num = config->cs_io_num;
    int spics_in = spi_periph_signal[config->host_id].spics_in;
    int spics_out = spi_periph_signal[config->host_id].spics_out[cs_id];
    int spics_func = spi_periph_signal[config->host_id].func;
    gpio_hal_context_t gpio_hal = {
        .dev = GPIO_HAL_GET_HW(GPIO_PORT_0)
    };

    //To avoid the panic caused by flash data line conflicts during cs line
    //initialization, disable the cache temporarily
    chip->os_func->start(chip->os_func_data);
    gpio_hal_input_enable(&gpio_hal, cs_io_num);
    if (cs_use_iomux) {
        gpio_hal_func_sel(&gpio_hal, cs_io_num, spics_func);
    } else {
        gpio_hal_output_enable(&gpio_hal, cs_io_num);
        gpio_hal_od_disable(&gpio_hal, cs_io_num);
        esp_rom_gpio_connect_out_signal(cs_io_num, spics_out, false, false);
        if (cs_id == 0) {
            esp_rom_gpio_connect_in_signal(cs_io_num, spics_in, false);
        }
        gpio_hal_func_sel(&gpio_hal, cs_io_num, PIN_FUNC_GPIO);
    }
    chip->os_func->end(chip->os_func_data);
}

static bool use_bus_lock(int host_id)
{
    if (host_id != SPI1_HOST) {
        return true;
    }
#if CONFIG_SPI_FLASH_SHARE_SPI1_BUS
    return true;
#else
    return false;
#endif
}

static bool bus_using_iomux(spi_host_device_t host)
{
    CHECK_IOMUX_PIN(host, spid);
    CHECK_IOMUX_PIN(host, spiq);
    CHECK_IOMUX_PIN(host, spiwp);
    CHECK_IOMUX_PIN(host, spihd);
    return true;
}

static bool cs_using_iomux(const esp_flash_spi_device_config_t *config)
{
    bool use_iomux = true;
    CHECK_IOMUX_PIN(config->host_id, spics);
    if (config->cs_io_num != spi_periph_signal[config->host_id].spics_in) {
        use_iomux = false;
    }
    return use_iomux;
}

static esp_err_t acquire_spi_device(const esp_flash_spi_device_config_t *config, int* out_dev_id, spi_bus_lock_dev_handle_t* out_dev_handle)
{
    esp_err_t ret = ESP_OK;
    int dev_id = -1;
    spi_bus_lock_dev_handle_t dev_handle = NULL;

    if (use_bus_lock(config->host_id)) {
        spi_bus_lock_handle_t lock = spi_bus_lock_get_by_id(config->host_id);
        spi_bus_lock_dev_config_t config = {.flags = SPI_BUS_LOCK_DEV_FLAG_CS_REQUIRED};

        ret = spi_bus_lock_register_dev(lock, &config, &dev_handle);
        if (ret == ESP_OK) {
            dev_id = spi_bus_lock_get_dev_id(dev_handle);
        } else if (ret == ESP_ERR_NOT_SUPPORTED) {
            ESP_LOGE(TAG, "No free CS.");
        } else if (ret == ESP_ERR_INVALID_ARG) {
            ESP_LOGE(TAG, "Bus lock not initialized (check CONFIG_SPI_FLASH_SHARE_SPI1_BUS).");
        }
    } else {
        const bool is_main_flash = (config->host_id == SPI1_HOST && config->cs_id == 0);
        if (config->cs_id >= SOC_SPI_PERIPH_CS_NUM(config->host_id) || config->cs_id < 0 || is_main_flash) {
            ESP_LOGE(TAG, "Not valid CS.");
            ret = ESP_ERR_INVALID_ARG;
        } else {
            dev_id = config->cs_id;
            assert(dev_handle == NULL);
        }
    }

    *out_dev_handle = dev_handle;
    *out_dev_id = dev_id;
    return ret;
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

    int dev_id;
    spi_bus_lock_dev_handle_t dev_handle;
    esp_err_t err = acquire_spi_device(config, &dev_id, &dev_handle);
    if (err != ESP_OK) {
        ret = err;
        goto fail;
    }

    err = esp_flash_init_os_functions(chip, config->host_id, dev_handle);
    if (err != ESP_OK) {
        ret = err;
        goto fail;
    }

    //avoid conflicts with main flash
    assert(config->host_id != SPI1_HOST || dev_id != 0);
    bool use_iomux = bus_using_iomux(config->host_id);
    memspi_host_config_t host_cfg = {
        .host_id = config->host_id,
        .cs_num = dev_id,
        .iomux = use_iomux,
        .input_delay_ns = config->input_delay_ns,
        .freq_mhz = config->freq_mhz,
    };

    host_cfg.clock_src_freq = spi_flash_ll_get_source_clock_freq_mhz(host_cfg.host_id);

    err = memspi_host_init_pointers(host, &host_cfg);
    if (err != ESP_OK) {
        ret = err;
        goto fail;
    }

    // The cs_id inside `config` is deprecated, use the `dev_id` provided by the bus lock instead.
    cs_initialize(chip, config, cs_using_iomux(config), dev_id);
    *out_chip = chip;
    return ret;
fail:
    // The memory allocated are free'd in the `spi_bus_remove_flash_device`.
    spi_bus_remove_flash_device(chip);
    return ret;
}

esp_err_t spi_bus_remove_flash_device(esp_flash_t *chip)
{
    if (chip == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    spi_bus_lock_dev_handle_t dev_handle = NULL;
    esp_flash_deinit_os_functions(chip, &dev_handle);
    if (dev_handle) {
        spi_bus_lock_unregister_dev(dev_handle);
    }
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

#if CONFIG_ESPTOOLPY_FLASH_MODE_AUTO_DETECT
/* This function is used to correct flash mode if config option is not consistent with efuse information */
static void s_esp_flash_choose_correct_mode(memspi_host_config_t *cfg)
{
    static const char *mode = FLASH_MODE_STRING;
    if (bootloader_flash_is_octal_mode_enabled()) {
    #if !CONFIG_ESPTOOLPY_FLASHMODE_OPI
        ESP_EARLY_LOGW(TAG, "Octal flash chip is using but %s mode is selected, will automatically switch to Octal mode", mode);
        cfg->octal_mode_en = 1;
        cfg->default_io_mode = SPI_FLASH_OPI_STR;
        default_chip.read_mode = SPI_FLASH_OPI_STR;
    #endif
    } else {
    #if CONFIG_ESPTOOLPY_FLASHMODE_OPI
        ESP_EARLY_LOGW(TAG, "Quad flash chip is using but %s flash mode is selected, will automatically switch to DIO mode", mode);
        cfg->octal_mode_en = 0;
        cfg->default_io_mode = SPI_FLASH_DIO;
        default_chip.read_mode = SPI_FLASH_DIO;
    #endif
    }
}
#endif // CONFIG_ESPTOOLPY_FLASH_MODE_AUTO_DETECT

extern esp_err_t esp_flash_suspend_cmd_init(esp_flash_t* chip);
esp_err_t esp_flash_init_default_chip(void)
{
    const esp_rom_spiflash_chip_t *legacy_chip = &g_rom_flashchip;
    memspi_host_config_t cfg = ESP_FLASH_HOST_CONFIG_DEFAULT();

    #if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3
    // Only these chips have efuses for spi pin configuration.
    cfg.iomux = esp_rom_efuse_get_flash_gpio_info() == 0 ?  true : false;
    #endif

    #if CONFIG_ESPTOOLPY_OCT_FLASH
    // Default value. When `CONFIG_ESPTOOLPY_FLASH_MODE_AUTO_DETECT` selected, if the selected mode not consistent with
    // hardware, will be overwritten in s_esp_flash_choose_correct_mode.
    cfg.octal_mode_en = 1;
    cfg.default_io_mode = DEFAULT_FLASH_MODE;
    #endif

    #if CONFIG_ESPTOOLPY_FLASH_MODE_AUTO_DETECT
    // Automatically detect flash mode in run time
    s_esp_flash_choose_correct_mode(&cfg);
    #endif

    #if !CONFIG_SPI_FLASH_OVERRIDE_CHIP_DRIVER_LIST
    spi_flash_chip_list_check(&default_chip, legacy_chip->device_id);
    #endif

    // For chips need time tuning, get value directly from system here.
    #if SOC_SPI_MEM_SUPPORT_TIMING_TUNING
    if (spi_flash_timing_is_tuned()) {
        cfg.using_timing_tuning = 1;
        spi_timing_get_flash_timing_param(&cfg.timing_reg);
    }
    #endif // SOC_SPI_MEM_SUPPORT_TIMING_TUNING

    cfg.clock_src_freq = spi_flash_ll_get_source_clock_freq_mhz(cfg.host_id);

    #if CONFIG_SPI_FLASH_AUTO_SUSPEND
    if (TSUS_VAL_SUSPEND > 400 || TSUS_VAL_SUSPEND < 20) {
        // Assume that the tsus value cannot larger than 400 (because the performance might be really bad)
        // And value cannot smaller than 20 (never see that small tsus value, might be wrong)
        return ESP_ERR_INVALID_ARG;
    }
    cfg.tsus_val = TSUS_VAL_SUSPEND;
    #endif // CONFIG_SPI_FLASH_AUTO_SUSPEND

    #if CONFIG_SPI_FLASH_AUTO_CHECK_SUSPEND_STATUS
    cfg.auto_waiti_pes = true;
    #endif

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
#if !CONFIG_IDF_TARGET_ESP32P4 || !CONFIG_APP_BUILD_TYPE_RAM // IDF-10019
    if (legacy_chip->chip_size > 16 * 1024 * 1024) {
        ESP_RETURN_ON_ERROR_ISR(esp_mspi_32bit_address_flash_feature_check(), TAG, "32bit address feature check failed");
    }
#endif // !CONFIG_IDF_TARGET_ESP32P4 || !CONFIG_APP_BUILD_TYPE_RAM
    // Set chip->size equal to ROM flash size(also equal to the size in binary image header), which means the available size that can be used
    default_chip.size = legacy_chip->chip_size;

    esp_flash_default_chip = &default_chip;
#ifdef CONFIG_SPI_FLASH_AUTO_SUSPEND
    err = esp_flash_suspend_cmd_init(&default_chip);
    if (err != ESP_OK) {
        return err;
    }
#endif

#if CONFIG_SPI_FLASH_HPM_DC_ON
    if (spi_flash_hpm_dummy_adjust()) {
        default_chip.hpm_dummy_ena = 1;
    }
#endif

    return ESP_OK;
}

esp_err_t esp_flash_app_init(void)
{
    esp_err_t err = ESP_OK;

    // Acquire the LDO channel used by the SPI NOR flash
    // in case the LDO voltage is changed by other users
#if CONFIG_ESP_LDO_RESERVE_SPI_NOR_FLASH
    static esp_ldo_channel_handle_t s_ldo_chan = NULL;
    esp_ldo_channel_config_t ldo_config = {
        .chan_id = CONFIG_ESP_LDO_CHAN_SPI_NOR_FLASH_DOMAIN,
        .voltage_mv = CONFIG_ESP_LDO_VOLTAGE_SPI_NOR_FLASH_DOMAIN,
        .flags = {
            .owned_by_hw = true,     // LDO output is totally controlled by hardware
        },
    };
    err = esp_ldo_acquire_channel(&ldo_config, &s_ldo_chan);
    if (err != ESP_OK) return err;
#endif // CONFIG_ESP_LDO_RESERVE_SPI_NOR_FLASH

    spi_flash_init_lock();
    spi_flash_guard_set(&g_flash_guard_default_ops);
#if CONFIG_SPI_FLASH_ENABLE_COUNTERS
    esp_flash_reset_counters();
#endif
#if CONFIG_SPI_FLASH_SHARE_SPI1_BUS
    err = esp_flash_init_main_bus_lock();
    if (err != ESP_OK) return err;
#endif
    err = esp_flash_app_enable_os_functions(&default_chip);
    return err;
}
