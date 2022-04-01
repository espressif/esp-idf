// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <string.h>
#include "sdkconfig.h"
#include "driver/spi_master.h"
#include "soc/spi_periph.h"
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_err.h"
#include "soc/soc.h"
#include "soc/dport_reg.h"
#include "soc/lldesc.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include "esp_heap_caps.h"
#include "driver/spi_common_internal.h"
#include "stdatomic.h"
#include "hal/spi_hal.h"


static const char *SPI_TAG = "spi";

#define SPI_CHECK(a, str, ret_val) do { \
    if (!(a)) { \
        ESP_LOGE(SPI_TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val); \
    } \
    } while(0)

#define SPI_CHECK_PIN(pin_num, pin_name, check_output) if (check_output) { \
            SPI_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(pin_num), pin_name" not valid", ESP_ERR_INVALID_ARG); \
        } else { \
            SPI_CHECK(GPIO_IS_VALID_GPIO(pin_num), pin_name" not valid", ESP_ERR_INVALID_ARG); \
        }


typedef struct spi_device_t spi_device_t;

#define FUNC_GPIO   PIN_FUNC_GPIO

#define DMA_CHANNEL_ENABLED(dma_chan)    (BIT(dma_chan-1))


typedef struct {
    int host_id;
    spi_destroy_func_t destroy_func;
    void* destroy_arg;
    spi_bus_attr_t bus_attr;
} spicommon_bus_context_t;

#define MAIN_BUS_DEFAULT() { \
        .host_id = 0, \
        .bus_attr = { \
            .dma_chan = 0, \
            .max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE, \
            .dma_desc_num= 0, \
        }, \
    }

//Periph 1 is 'claimed' by SPI flash code.
static atomic_bool spi_periph_claimed[SOC_SPI_PERIPH_NUM] = { ATOMIC_VAR_INIT(true), ATOMIC_VAR_INIT(false), ATOMIC_VAR_INIT(false),
#if SOC_SPI_PERIPH_NUM >= 4
        ATOMIC_VAR_INIT(false),
#endif
};
static const char* spi_claiming_func[3] = {NULL, NULL, NULL};
static uint8_t spi_dma_chan_enabled = 0;
static portMUX_TYPE spi_dma_spinlock = portMUX_INITIALIZER_UNLOCKED;

static spicommon_bus_context_t s_mainbus = MAIN_BUS_DEFAULT();
static spicommon_bus_context_t* bus_ctx[SOC_SPI_PERIPH_NUM] = {&s_mainbus};


//Returns true if this peripheral is successfully claimed, false if otherwise.
bool spicommon_periph_claim(spi_host_device_t host, const char* source)
{
    bool false_var = false;
    bool ret = atomic_compare_exchange_strong(&spi_periph_claimed[host], &false_var, true);
    if (ret) {
        spi_claiming_func[host] = source;
        periph_module_enable(spi_periph_signal[host].module);
    } else {
        ESP_EARLY_LOGE(SPI_TAG, "SPI%d already claimed by %s.", host+1, spi_claiming_func[host]);
    }
    return ret;
}

bool spicommon_periph_in_use(spi_host_device_t host)
{
    return atomic_load(&spi_periph_claimed[host]);
}

//Returns true if this peripheral is successfully freed, false if otherwise.
bool spicommon_periph_free(spi_host_device_t host)
{
    bool true_var = true;
    bool ret = atomic_compare_exchange_strong(&spi_periph_claimed[host], &true_var, false);
    if (ret) periph_module_disable(spi_periph_signal[host].module);
    return ret;
}


int spicommon_irqsource_for_host(spi_host_device_t host)
{
    return spi_periph_signal[host].irq;
}

int spicommon_irqdma_source_for_host(spi_host_device_t host)
{
    return spi_periph_signal[host].irq_dma;
}

static inline uint32_t get_dma_periph(int dma_chan)
{
#if CONFIG_IDF_TARGET_ESP32S2
    if (dma_chan == 1) {
        return PERIPH_SPI2_DMA_MODULE;
    } else if (dma_chan==2) {
        return PERIPH_SPI3_DMA_MODULE;
    } else {
        abort();
        return -1;
    }
#elif CONFIG_IDF_TARGET_ESP32
    return PERIPH_SPI_DMA_MODULE;
#endif
}

bool spicommon_dma_chan_claim (int dma_chan)
{
    bool ret = false;
    assert(dma_chan >= 1 && dma_chan <= SOC_SPI_DMA_CHAN_NUM);

    portENTER_CRITICAL(&spi_dma_spinlock);
    if ( !(spi_dma_chan_enabled & DMA_CHANNEL_ENABLED(dma_chan)) ) {
        // get the channel only when it's not claimed yet.
        spi_dma_chan_enabled |= DMA_CHANNEL_ENABLED(dma_chan);
        ret = true;
    }

#if CONFIG_IDF_TARGET_ESP32
    periph_module_enable(get_dma_periph(dma_chan));
#elif CONFIG_IDF_TARGET_ESP32S2
    if (dma_chan==1) {
        periph_module_enable(PERIPH_SPI2_DMA_MODULE);
    } else if (dma_chan==2) {
        periph_module_enable(PERIPH_SPI3_DMA_MODULE);
    }
#endif
    portEXIT_CRITICAL(&spi_dma_spinlock);

    return ret;
}

bool spicommon_dma_chan_in_use(int dma_chan)
{
    assert(dma_chan==1 || dma_chan == 2);
    return spi_dma_chan_enabled & DMA_CHANNEL_ENABLED(dma_chan);
}

bool spicommon_dma_chan_free(int dma_chan)
{
    assert( dma_chan == 1 || dma_chan == 2 );
    assert( spi_dma_chan_enabled & DMA_CHANNEL_ENABLED(dma_chan) );

    portENTER_CRITICAL(&spi_dma_spinlock);
    spi_dma_chan_enabled &= ~DMA_CHANNEL_ENABLED(dma_chan);
#if CONFIG_IDF_TARGET_ESP32
    if ( spi_dma_chan_enabled == 0 ) {
        //disable the DMA only when all the channels are freed.
        periph_module_disable(get_dma_periph(dma_chan));
    }
#elif CONFIG_IDF_TARGET_ESP32S2
    if (dma_chan==1) {
        periph_module_disable(PERIPH_SPI2_DMA_MODULE);
    } else if (dma_chan==2) {
        periph_module_disable(PERIPH_SPI3_DMA_MODULE);
    }
#endif
    portEXIT_CRITICAL(&spi_dma_spinlock);

    return true;
}

static bool bus_uses_iomux_pins(spi_host_device_t host, const spi_bus_config_t* bus_config)
{
    if (bus_config->sclk_io_num>=0 &&
        bus_config->sclk_io_num != spi_periph_signal[host].spiclk_iomux_pin) return false;
    if (bus_config->quadwp_io_num>=0 &&
        bus_config->quadwp_io_num != spi_periph_signal[host].spiwp_iomux_pin) return false;
    if (bus_config->quadhd_io_num>=0 &&
        bus_config->quadhd_io_num != spi_periph_signal[host].spihd_iomux_pin) return false;
    if (bus_config->mosi_io_num >= 0 &&
        bus_config->mosi_io_num != spi_periph_signal[host].spid_iomux_pin) return false;
    if (bus_config->miso_io_num>=0 &&
        bus_config->miso_io_num != spi_periph_signal[host].spiq_iomux_pin) return false;

    return true;
}

/*
Do the common stuff to hook up a SPI host to a bus defined by a bunch of GPIO pins. Feed it a host number and a
bus config struct and it'll set up the GPIO matrix and enable the device. If a pin is set to non-negative value,
it should be able to be initialized.
*/
esp_err_t spicommon_bus_initialize_io(spi_host_device_t host, const spi_bus_config_t *bus_config, int dma_chan, uint32_t flags, uint32_t* flags_o)
{
    uint32_t temp_flag=0;

    bool miso_need_output;
    bool mosi_need_output;
    bool sclk_need_output;
    if ((flags&SPICOMMON_BUSFLAG_MASTER) != 0) {
        //initial for master
        miso_need_output = ((flags&SPICOMMON_BUSFLAG_DUAL) != 0) ? true : false;
        mosi_need_output = true;
        sclk_need_output = true;
    } else {
        //initial for slave
        miso_need_output = true;
        mosi_need_output = ((flags&SPICOMMON_BUSFLAG_DUAL) != 0) ? true : false;
        sclk_need_output = false;
    }

    const bool wp_need_output = true;
    const bool hd_need_output = true;

    //check pin capabilities
    if (bus_config->sclk_io_num>=0) {
        temp_flag |= SPICOMMON_BUSFLAG_SCLK;
        SPI_CHECK_PIN(bus_config->sclk_io_num, "sclk", sclk_need_output);
    }
    if (bus_config->quadwp_io_num>=0) {
        SPI_CHECK_PIN(bus_config->quadwp_io_num, "wp", wp_need_output);
    }
    if (bus_config->quadhd_io_num>=0) {
        SPI_CHECK_PIN(bus_config->quadhd_io_num, "hd", hd_need_output);
    }
    //set flags for QUAD mode according to the existence of wp and hd
    if (bus_config->quadhd_io_num >= 0 && bus_config->quadwp_io_num >= 0) temp_flag |= SPICOMMON_BUSFLAG_WPHD;
    if (bus_config->mosi_io_num >= 0) {
        temp_flag |= SPICOMMON_BUSFLAG_MOSI;
        SPI_CHECK_PIN(bus_config->mosi_io_num, "mosi", mosi_need_output);
    }
    if (bus_config->miso_io_num>=0) {
        temp_flag |= SPICOMMON_BUSFLAG_MISO;
        SPI_CHECK_PIN(bus_config->miso_io_num, "miso", miso_need_output);
    }
    //set flags for DUAL mode according to output-capability of MOSI and MISO pins.
    if ( (bus_config->mosi_io_num < 0 || GPIO_IS_VALID_OUTPUT_GPIO(bus_config->mosi_io_num)) &&
        (bus_config->miso_io_num < 0 || GPIO_IS_VALID_OUTPUT_GPIO(bus_config->miso_io_num)) ) {
        temp_flag |= SPICOMMON_BUSFLAG_DUAL;
    }

    //check if the selected pins correspond to the iomux pins of the peripheral
    bool use_iomux = bus_uses_iomux_pins(host, bus_config);
    if (use_iomux) temp_flag |= SPICOMMON_BUSFLAG_IOMUX_PINS;

    uint32_t missing_flag = flags & ~temp_flag;
    missing_flag &= ~SPICOMMON_BUSFLAG_MASTER;//don't check this flag

    if (missing_flag != 0) {
    //check pins existence
        if (missing_flag & SPICOMMON_BUSFLAG_SCLK) ESP_LOGE(SPI_TAG, "sclk pin required.");
        if (missing_flag & SPICOMMON_BUSFLAG_MOSI) ESP_LOGE(SPI_TAG, "mosi pin required.");
        if (missing_flag & SPICOMMON_BUSFLAG_MISO) ESP_LOGE(SPI_TAG, "miso pin required.");
        if (missing_flag & SPICOMMON_BUSFLAG_DUAL) ESP_LOGE(SPI_TAG, "not both mosi and miso output capable");
        if (missing_flag & SPICOMMON_BUSFLAG_WPHD) ESP_LOGE(SPI_TAG, "both wp and hd required.");
        if (missing_flag & SPICOMMON_BUSFLAG_IOMUX_PINS) ESP_LOGE(SPI_TAG, "not using iomux pins");
        SPI_CHECK(missing_flag == 0, "not all required capabilities satisfied.", ESP_ERR_INVALID_ARG);
    }

    if (use_iomux) {
        //All SPI iomux pin selections resolve to 1, so we put that here instead of trying to figure
        //out which FUNC_GPIOx_xSPIxx to grab; they all are defined to 1 anyway.
        ESP_LOGD(SPI_TAG, "SPI%d use iomux pins.", host+1);
        if (bus_config->mosi_io_num >= 0) {
            gpio_iomux_in(bus_config->mosi_io_num, spi_periph_signal[host].spid_in);
            gpio_iomux_out(bus_config->mosi_io_num, spi_periph_signal[host].func, false);
        }
        if (bus_config->miso_io_num >= 0) {
            gpio_iomux_in(bus_config->miso_io_num, spi_periph_signal[host].spiq_in);
            gpio_iomux_out(bus_config->miso_io_num, spi_periph_signal[host].func, false);
        }
        if (bus_config->quadwp_io_num >= 0) {
            gpio_iomux_in(bus_config->quadwp_io_num, spi_periph_signal[host].spiwp_in);
            gpio_iomux_out(bus_config->quadwp_io_num, spi_periph_signal[host].func, false);
        }
        if (bus_config->quadhd_io_num >= 0) {
            gpio_iomux_in(bus_config->quadhd_io_num, spi_periph_signal[host].spihd_in);
            gpio_iomux_out(bus_config->quadhd_io_num, spi_periph_signal[host].func, false);
        }
        if (bus_config->sclk_io_num >= 0) {
            gpio_iomux_in(bus_config->sclk_io_num, spi_periph_signal[host].spiclk_in);
            gpio_iomux_out(bus_config->sclk_io_num, spi_periph_signal[host].func, false);
        }
        temp_flag |= SPICOMMON_BUSFLAG_IOMUX_PINS;
    } else {
        //Use GPIO matrix
        ESP_LOGD(SPI_TAG, "SPI%d use gpio matrix.", host+1);
        if (bus_config->mosi_io_num >= 0) {
            if (mosi_need_output || (temp_flag&SPICOMMON_BUSFLAG_DUAL)) {
                gpio_set_direction(bus_config->mosi_io_num, GPIO_MODE_INPUT_OUTPUT);
                gpio_matrix_out(bus_config->mosi_io_num, spi_periph_signal[host].spid_out, false, false);
            } else {
                gpio_set_direction(bus_config->mosi_io_num, GPIO_MODE_INPUT);
            }
            gpio_matrix_in(bus_config->mosi_io_num, spi_periph_signal[host].spid_in, false);
#if CONFIG_IDF_TARGET_ESP32S2
            PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[bus_config->mosi_io_num]);
#endif
            PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->mosi_io_num], FUNC_GPIO);
        }
        if (bus_config->miso_io_num >= 0) {
            if (miso_need_output || (temp_flag&SPICOMMON_BUSFLAG_DUAL)) {
                gpio_set_direction(bus_config->miso_io_num, GPIO_MODE_INPUT_OUTPUT);
                gpio_matrix_out(bus_config->miso_io_num, spi_periph_signal[host].spiq_out, false, false);
            } else {
                gpio_set_direction(bus_config->miso_io_num, GPIO_MODE_INPUT);
            }
            gpio_matrix_in(bus_config->miso_io_num, spi_periph_signal[host].spiq_in, false);
#if CONFIG_IDF_TARGET_ESP32S2
            PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[bus_config->miso_io_num]);
#endif
            PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->miso_io_num], FUNC_GPIO);
        }
        if (bus_config->quadwp_io_num >= 0) {
            gpio_set_direction(bus_config->quadwp_io_num, GPIO_MODE_INPUT_OUTPUT);
            gpio_matrix_out(bus_config->quadwp_io_num, spi_periph_signal[host].spiwp_out, false, false);
            gpio_matrix_in(bus_config->quadwp_io_num, spi_periph_signal[host].spiwp_in, false);
#if CONFIG_IDF_TARGET_ESP32S2
            PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[bus_config->quadwp_io_num]);
#endif
            PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->quadwp_io_num], FUNC_GPIO);
        }
        if (bus_config->quadhd_io_num >= 0) {
            gpio_set_direction(bus_config->quadhd_io_num, GPIO_MODE_INPUT_OUTPUT);
            gpio_matrix_out(bus_config->quadhd_io_num, spi_periph_signal[host].spihd_out, false, false);
            gpio_matrix_in(bus_config->quadhd_io_num, spi_periph_signal[host].spihd_in, false);
#if CONFIG_IDF_TARGET_ESP32S2
            PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[bus_config->quadhd_io_num]);
#endif
            PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->quadhd_io_num], FUNC_GPIO);
        }
        if (bus_config->sclk_io_num >= 0) {
            if (sclk_need_output) {
                gpio_set_direction(bus_config->sclk_io_num, GPIO_MODE_INPUT_OUTPUT);
                gpio_matrix_out(bus_config->sclk_io_num, spi_periph_signal[host].spiclk_out, false, false);
            } else {
                gpio_set_direction(bus_config->sclk_io_num, GPIO_MODE_INPUT);
            }
            gpio_matrix_in(bus_config->sclk_io_num, spi_periph_signal[host].spiclk_in, false);
#if CONFIG_IDF_TARGET_ESP32S2
            PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[bus_config->sclk_io_num]);
#endif
            PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->sclk_io_num], FUNC_GPIO);
        }
    }

    //Select DMA channel.
#if CONFIG_IDF_TARGET_ESP32
    DPORT_SET_PERI_REG_BITS(DPORT_SPI_DMA_CHAN_SEL_REG, 3, dma_chan, (host * 2));
#endif

    if (flags_o) *flags_o = temp_flag;
    return ESP_OK;
}

esp_err_t spicommon_bus_free_io_cfg(const spi_bus_config_t *bus_cfg)
{
    int pin_array[] = {
        bus_cfg->mosi_io_num,
        bus_cfg->miso_io_num,
        bus_cfg->sclk_io_num,
        bus_cfg->quadwp_io_num,
        bus_cfg->quadhd_io_num,
    };
    for (int i = 0; i < sizeof(pin_array)/sizeof(int); i ++) {
        const int io = pin_array[i];
        if (io >= 0 && GPIO_IS_VALID_GPIO(io)) gpio_reset_pin(io);
    }
    return ESP_OK;
}

void spicommon_cs_initialize(spi_host_device_t host, int cs_io_num, int cs_num, int force_gpio_matrix)
{
    if (!force_gpio_matrix && cs_io_num == spi_periph_signal[host].spics0_iomux_pin && cs_num == 0) {
        //The cs0s for all SPI peripherals map to pin mux source 1, so we use that instead of a define.
        gpio_iomux_in(cs_io_num, spi_periph_signal[host].spics_in);
#if CONFIG_IDF_TARGET_ESP32
        gpio_iomux_out(cs_io_num, spi_periph_signal[host].func, false);
#elif CONFIG_IDF_TARGET_ESP32S2
        gpio_iomux_out(cs_io_num, spi_periph_signal[host].func, false);
#endif
    } else {
        //Use GPIO matrix
        if (GPIO_IS_VALID_OUTPUT_GPIO(cs_io_num)) {
            gpio_set_direction(cs_io_num, GPIO_MODE_INPUT_OUTPUT);
            gpio_matrix_out(cs_io_num, spi_periph_signal[host].spics_out[cs_num], false, false);
        } else {
            gpio_set_direction(cs_io_num, GPIO_MODE_INPUT);
        }
        if (cs_num == 0) gpio_matrix_in(cs_io_num, spi_periph_signal[host].spics_in, false);
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[cs_io_num]);
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[cs_io_num], FUNC_GPIO);
    }
}

void spicommon_cs_free_io(int cs_gpio_num)
{
    assert(cs_gpio_num>=0 && GPIO_IS_VALID_GPIO(cs_gpio_num));
    gpio_reset_pin(cs_gpio_num);
}

bool spicommon_bus_using_iomux(spi_host_device_t host)
{
#define CHECK_IOMUX_PIN(HOST, PIN_NAME) if (GPIO.func_in_sel_cfg[spi_periph_signal[(HOST)].PIN_NAME##_in].sig_in_sel) return false

    CHECK_IOMUX_PIN(host, spid);
    CHECK_IOMUX_PIN(host, spiq);
    CHECK_IOMUX_PIN(host, spiwp);
    CHECK_IOMUX_PIN(host, spihd);
    return true;
}


void spi_bus_main_set_lock(spi_bus_lock_handle_t lock)
{
    bus_ctx[0]->bus_attr.lock = lock;
}

spi_bus_lock_handle_t spi_bus_lock_get_by_id(spi_host_device_t host_id)
{
    return bus_ctx[host_id]->bus_attr.lock;
}

static inline bool is_valid_host(spi_host_device_t host)
{
    return host >= SPI1_HOST && host <= SPI3_HOST;
}

esp_err_t spi_bus_initialize(spi_host_device_t host_id, const spi_bus_config_t *bus_config, int dma_chan)
{
    esp_err_t err = ESP_OK;
    spicommon_bus_context_t *ctx = NULL;
    spi_bus_attr_t *bus_attr = NULL;
    SPI_CHECK(is_valid_host(host_id), "invalid host_id", ESP_ERR_INVALID_ARG);
    SPI_CHECK(bus_ctx[host_id] == NULL, "SPI bus already initialized.", ESP_ERR_INVALID_STATE);
#ifdef CONFIG_IDF_TARGET_ESP32
    SPI_CHECK( dma_chan >= 0 && dma_chan <= 2, "invalid dma channel", ESP_ERR_INVALID_ARG );
#elif CONFIG_IDF_TARGET_ESP32S2
    SPI_CHECK( dma_chan == 0 || dma_chan == host_id, "invalid dma channel", ESP_ERR_INVALID_ARG );
#endif
    SPI_CHECK((bus_config->intr_flags & (ESP_INTR_FLAG_HIGH|ESP_INTR_FLAG_EDGE|ESP_INTR_FLAG_INTRDISABLED))==0, "intr flag not allowed", ESP_ERR_INVALID_ARG);
#ifndef CONFIG_SPI_MASTER_ISR_IN_IRAM
    SPI_CHECK((bus_config->intr_flags & ESP_INTR_FLAG_IRAM)==0, "ESP_INTR_FLAG_IRAM should be disabled when CONFIG_SPI_MASTER_ISR_IN_IRAM is not set.", ESP_ERR_INVALID_ARG);
#endif

    bool spi_chan_claimed = spicommon_periph_claim(host_id, "spi master");
    SPI_CHECK(spi_chan_claimed, "host_id already in use", ESP_ERR_INVALID_STATE);

    if (dma_chan != 0) {
        bool dma_chan_claimed=spicommon_dma_chan_claim(dma_chan);
        if (!dma_chan_claimed) {
            spicommon_periph_free(host_id);
            SPI_CHECK(false, "dma channel already in use", ESP_ERR_INVALID_STATE);
        }
    }

    //clean and initialize the context
    ctx = (spicommon_bus_context_t*)malloc(sizeof(spicommon_bus_context_t));
    if (!ctx) {
        err = ESP_ERR_NO_MEM;
        goto cleanup;
    }
    *ctx = (spicommon_bus_context_t) {
        .host_id = host_id,
        .bus_attr = {
            .bus_cfg = *bus_config,
            .dma_chan = dma_chan,
        },
    };

    bus_attr = &ctx->bus_attr;
    if (dma_chan == 0) {
        bus_attr->max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE;
        bus_attr->dma_desc_num = 0;
    } else {
        //See how many dma descriptors we need and allocate them
        int dma_desc_ct = lldesc_get_required_num(bus_config->max_transfer_sz);
        if (dma_desc_ct == 0) dma_desc_ct = 1; //default to 4k when max is not given

        bus_attr->max_transfer_sz = dma_desc_ct * LLDESC_MAX_NUM_PER_DESC;
        bus_attr->dmadesc_tx = heap_caps_malloc(sizeof(lldesc_t) * dma_desc_ct, MALLOC_CAP_DMA);
        bus_attr->dmadesc_rx = heap_caps_malloc(sizeof(lldesc_t) * dma_desc_ct, MALLOC_CAP_DMA);
        if (bus_attr->dmadesc_tx == NULL || bus_attr->dmadesc_rx == NULL) {
            err = ESP_ERR_NO_MEM;
            goto cleanup;
        }
        bus_attr->dma_desc_num = dma_desc_ct;
    }

    spi_bus_lock_config_t lock_config = {
        .host_id = host_id,
        .cs_num = SOC_SPI_PERIPH_CS_NUM(host_id),
    };
    err = spi_bus_init_lock(&bus_attr->lock, &lock_config);
    if (err != ESP_OK) {
        goto cleanup;
    }

#ifdef CONFIG_PM_ENABLE
    err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "spi_master",
            &bus_attr->pm_lock);
    if (err != ESP_OK) {
        goto cleanup;
    }
#endif //CONFIG_PM_ENABLE

    err = spicommon_bus_initialize_io(host_id, bus_config, dma_chan, SPICOMMON_BUSFLAG_MASTER | bus_config->flags, &bus_attr->flags);
    if (err != ESP_OK) {
        goto cleanup;
    }

    bus_ctx[host_id] = ctx;
    return ESP_OK;

cleanup:
    if (bus_attr) {
#ifdef CONFIG_PM_ENABLE
        esp_pm_lock_delete(bus_attr->pm_lock);
#endif
        if (bus_attr->lock) {
            spi_bus_deinit_lock(bus_attr->lock);
        }
        free(bus_attr->dmadesc_tx);
        free(bus_attr->dmadesc_rx);
    }
    free(ctx);
    if (dma_chan) {
        spicommon_dma_chan_free(dma_chan);
    }
    spicommon_periph_free(host_id);
    return err;
}

const spi_bus_attr_t* spi_bus_get_attr(spi_host_device_t host_id)
{
    if (bus_ctx[host_id] == NULL) return NULL;

    return &bus_ctx[host_id]->bus_attr;
}

esp_err_t spi_bus_free(spi_host_device_t host_id)
{
    if (bus_ctx[host_id] == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = ESP_OK;
    spicommon_bus_context_t* ctx = bus_ctx[host_id];
    spi_bus_attr_t* bus_attr = &ctx->bus_attr;

    if (ctx->destroy_func) {
        err = ctx->destroy_func(ctx->destroy_arg);
    }

    spicommon_bus_free_io_cfg(&bus_attr->bus_cfg);

#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_delete(bus_attr->pm_lock);
#endif
    spi_bus_deinit_lock(bus_attr->lock);

    free(bus_attr->dmadesc_rx);
    free(bus_attr->dmadesc_tx);

    if (bus_attr->dma_chan > 0) {
        spicommon_dma_chan_free (bus_attr->dma_chan);
    }
    spicommon_periph_free(host_id);

    free(ctx);
    bus_ctx[host_id] = NULL;
    return err;
}

esp_err_t spi_bus_register_destroy_func(spi_host_device_t host_id,
                                        spi_destroy_func_t f, void *arg)
{
    bus_ctx[host_id]->destroy_func = f;
    bus_ctx[host_id]->destroy_arg = arg;
    return ESP_OK;
}


/*
Code for workaround for DMA issue in ESP32 v0/v1 silicon
*/
#if CONFIG_IDF_TARGET_ESP32
static volatile int dmaworkaround_channels_busy[2] = {0, 0};
static dmaworkaround_cb_t dmaworkaround_cb;
static void *dmaworkaround_cb_arg;
static portMUX_TYPE dmaworkaround_mux = portMUX_INITIALIZER_UNLOCKED;
static int dmaworkaround_waiting_for_chan = 0;
#endif

bool IRAM_ATTR spicommon_dmaworkaround_req_reset(int dmachan, dmaworkaround_cb_t cb, void *arg)
{
#if CONFIG_IDF_TARGET_ESP32
    int otherchan = (dmachan == 1) ? 2 : 1;
    bool ret;
    portENTER_CRITICAL_ISR(&dmaworkaround_mux);
    if (dmaworkaround_channels_busy[otherchan-1]) {
        //Other channel is busy. Call back when it's done.
        dmaworkaround_cb = cb;
        dmaworkaround_cb_arg = arg;
        dmaworkaround_waiting_for_chan = otherchan;
        ret = false;
    } else {
        //Reset DMA
        periph_module_reset( PERIPH_SPI_DMA_MODULE );
        ret = true;
    }
    portEXIT_CRITICAL_ISR(&dmaworkaround_mux);
    return ret;
#else
    //no need to reset
    return true;
#endif
}

bool IRAM_ATTR spicommon_dmaworkaround_reset_in_progress(void)
{
#if CONFIG_IDF_TARGET_ESP32
    return (dmaworkaround_waiting_for_chan != 0);
#else
    return false;
#endif
}

void IRAM_ATTR spicommon_dmaworkaround_idle(int dmachan)
{
#if CONFIG_IDF_TARGET_ESP32
    portENTER_CRITICAL_ISR(&dmaworkaround_mux);
    dmaworkaround_channels_busy[dmachan-1] = 0;
    if (dmaworkaround_waiting_for_chan == dmachan) {
        //Reset DMA
        periph_module_reset( PERIPH_SPI_DMA_MODULE );
        dmaworkaround_waiting_for_chan = 0;
        //Call callback
        dmaworkaround_cb(dmaworkaround_cb_arg);

    }
    portEXIT_CRITICAL_ISR(&dmaworkaround_mux);
#endif
}

void IRAM_ATTR spicommon_dmaworkaround_transfer_active(int dmachan)
{
#if CONFIG_IDF_TARGET_ESP32
    portENTER_CRITICAL_ISR(&dmaworkaround_mux);
    dmaworkaround_channels_busy[dmachan-1] = 1;
    portEXIT_CRITICAL_ISR(&dmaworkaround_mux);
#endif
}
