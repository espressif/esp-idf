// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <string.h>
#include "driver/spi_master.h"
#include "soc/dport_reg.h"
#include "soc/spi_periph.h"
#include "rom/ets_sys.h"
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_intr.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_err.h"
#include "soc/soc.h"
#include "soc/dport_reg.h"
#include "rom/lldesc.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include "esp_heap_caps.h"
#include "driver/spi_common.h"
#include "stdatomic.h"

static const char *SPI_TAG = "spi";

#define SPI_CHECK(a, str, ret_val) \
    if (!(a)) { \
        ESP_LOGE(SPI_TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val); \
    }


typedef struct spi_device_t spi_device_t;

#define FUNC_SPI    1   //all pins of HSPI and VSPI shares this function number
#define FUNC_GPIO   PIN_FUNC_GPIO


#define DMA_CHANNEL_ENABLED(dma_chan)    (BIT(dma_chan-1))

//Periph 1 is 'claimed' by SPI flash code.
static atomic_bool spi_periph_claimed[3] = { ATOMIC_VAR_INIT(true), ATOMIC_VAR_INIT(false), ATOMIC_VAR_INIT(false)};
static uint8_t spi_dma_chan_enabled = 0;
static portMUX_TYPE spi_dma_spinlock = portMUX_INITIALIZER_UNLOCKED;


//Returns true if this peripheral is successfully claimed, false if otherwise.
bool spicommon_periph_claim(spi_host_device_t host)
{
    bool false_var = false;
    bool ret = atomic_compare_exchange_strong(&spi_periph_claimed[host], &false_var, true);
    if (ret) periph_module_enable(spi_periph_signal[host].module);
    return ret;
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

spi_dev_t *spicommon_hw_for_host(spi_host_device_t host)
{
    return spi_periph_signal[host].hw;
}

bool spicommon_dma_chan_claim (int dma_chan)
{
    bool ret = false;
    assert( dma_chan == 1 || dma_chan == 2 );

    portENTER_CRITICAL(&spi_dma_spinlock);
    if ( !(spi_dma_chan_enabled & DMA_CHANNEL_ENABLED(dma_chan)) ) {
        // get the channel only when it's not claimed yet.
        spi_dma_chan_enabled |= DMA_CHANNEL_ENABLED(dma_chan);
        ret = true;
    }
    periph_module_enable( PERIPH_SPI_DMA_MODULE );
    portEXIT_CRITICAL(&spi_dma_spinlock);

    return ret;
}

bool spicommon_dma_chan_free(int dma_chan)
{
    assert( dma_chan == 1 || dma_chan == 2 );
    assert( spi_dma_chan_enabled & DMA_CHANNEL_ENABLED(dma_chan) );

    portENTER_CRITICAL(&spi_dma_spinlock);
    spi_dma_chan_enabled &= ~DMA_CHANNEL_ENABLED(dma_chan);
    if ( spi_dma_chan_enabled == 0 ) {
        //disable the DMA only when all the channels are freed.
        periph_module_disable( PERIPH_SPI_DMA_MODULE );
    }
    portEXIT_CRITICAL(&spi_dma_spinlock);

    return true;
}

/*
Do the common stuff to hook up a SPI host to a bus defined by a bunch of GPIO pins. Feed it a host number and a
bus config struct and it'll set up the GPIO matrix and enable the device. If a pin is set to non-negative value,
it should be able to be initialized.
*/
esp_err_t spicommon_bus_initialize_io(spi_host_device_t host, const spi_bus_config_t *bus_config, int dma_chan, uint32_t flags, uint32_t* flags_o)
{
    bool use_iomux = true;
    uint32_t temp_flag=0;
    bool quad_pins_exist = true;
    //the MISO should be output capable in slave mode, or in DIO/QIO mode.
    bool miso_output = !(flags&SPICOMMON_BUSFLAG_MASTER) || flags&SPICOMMON_BUSFLAG_DUAL;
    //the MOSI should be output capble in master mode, or in DIO/QIO mode.
    bool mosi_output = (flags&SPICOMMON_BUSFLAG_MASTER)!=0 || flags&SPICOMMON_BUSFLAG_DUAL;

    //check pins existence and if the selected pins correspond to the iomux pins of the peripheral
    if (bus_config->sclk_io_num>=0) {
        temp_flag |= SPICOMMON_BUSFLAG_SCLK;
        SPI_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(bus_config->sclk_io_num), "sclk not valid", ESP_ERR_INVALID_ARG);
        if (bus_config->sclk_io_num != spi_periph_signal[host].spiclk_iomux_pin) use_iomux = false;
    } else {
        SPI_CHECK((flags&SPICOMMON_BUSFLAG_SCLK)==0, "sclk pin required.", ESP_ERR_INVALID_ARG);
    }
    if (bus_config->quadwp_io_num>=0) {
        SPI_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(bus_config->quadwp_io_num), "spiwp not valid", ESP_ERR_INVALID_ARG);
        if (bus_config->quadwp_io_num != spi_periph_signal[host].spiwp_iomux_pin) use_iomux = false;
    } else {
        quad_pins_exist = false;
        SPI_CHECK((flags&SPICOMMON_BUSFLAG_WPHD)==0, "spiwp pin required.", ESP_ERR_INVALID_ARG);
    }
    if (bus_config->quadhd_io_num>=0) {
        SPI_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(bus_config->quadhd_io_num), "spihd not valid", ESP_ERR_INVALID_ARG);
        if (bus_config->quadhd_io_num != spi_periph_signal[host].spihd_iomux_pin) use_iomux = false;
    } else {
        quad_pins_exist = false;
        SPI_CHECK((flags&SPICOMMON_BUSFLAG_WPHD)==0, "spihd pin required.", ESP_ERR_INVALID_ARG);
    }
    if (bus_config->mosi_io_num >= 0) {
        temp_flag |= SPICOMMON_BUSFLAG_MOSI;
        if (mosi_output) {
            SPI_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(bus_config->mosi_io_num), "mosi not valid", ESP_ERR_INVALID_ARG);
        } else {
            SPI_CHECK(GPIO_IS_VALID_GPIO(bus_config->mosi_io_num), "mosi not valid", ESP_ERR_INVALID_ARG);
        }
        if (bus_config->mosi_io_num != spi_periph_signal[host].spid_iomux_pin) use_iomux = false;
    } else {
        SPI_CHECK((flags&SPICOMMON_BUSFLAG_MOSI)==0, "mosi pin required.", ESP_ERR_INVALID_ARG);
    }
    if (bus_config->miso_io_num>=0) {
        temp_flag |= SPICOMMON_BUSFLAG_MISO;
        if (miso_output) {
            SPI_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(bus_config->miso_io_num), "miso not valid", ESP_ERR_INVALID_ARG);
        } else {
            SPI_CHECK(GPIO_IS_VALID_GPIO(bus_config->miso_io_num), "miso not valid", ESP_ERR_INVALID_ARG);
        }
        if (bus_config->miso_io_num != spi_periph_signal[host].spiq_iomux_pin) use_iomux = false;
    } else {
        SPI_CHECK((flags&SPICOMMON_BUSFLAG_MISO)==0, "miso pin required.", ESP_ERR_INVALID_ARG);
    }
    //set flags for DUAL mode according to output-capability of MOSI and MISO pins.
    if ( (bus_config->mosi_io_num < 0 || GPIO_IS_VALID_OUTPUT_GPIO(bus_config->mosi_io_num)) &&
        (bus_config->miso_io_num < 0 || GPIO_IS_VALID_OUTPUT_GPIO(bus_config->miso_io_num)) ) {
        temp_flag |= SPICOMMON_BUSFLAG_DUAL;
    }
    //set flags for QUAD mode according to the existence of wp and hd
    if (quad_pins_exist) temp_flag |= SPICOMMON_BUSFLAG_WPHD;
    //check iomux pins if required.
    SPI_CHECK((flags&SPICOMMON_BUSFLAG_NATIVE_PINS)==0 || use_iomux, "not using iomux pins", ESP_ERR_INVALID_ARG);

    if (use_iomux) {
        //All SPI iomux pin selections resolve to 1, so we put that here instead of trying to figure
        //out which FUNC_GPIOx_xSPIxx to grab; they all are defined to 1 anyway.
        ESP_LOGD(SPI_TAG, "SPI%d use iomux pins.", host );
        if (bus_config->mosi_io_num >= 0) {
            gpio_iomux_in(bus_config->mosi_io_num, spi_periph_signal[host].spid_in);
            gpio_iomux_out(bus_config->mosi_io_num, FUNC_SPI, false);
        }
        if (bus_config->miso_io_num >= 0) {
            gpio_iomux_in(bus_config->miso_io_num, spi_periph_signal[host].spiq_in);
            gpio_iomux_out(bus_config->miso_io_num, FUNC_SPI, false);
        }
        if (bus_config->quadwp_io_num >= 0) {
            gpio_iomux_in(bus_config->quadwp_io_num, spi_periph_signal[host].spiwp_in);
            gpio_iomux_out(bus_config->quadwp_io_num, FUNC_SPI, false);
        }
        if (bus_config->quadhd_io_num >= 0) {
            gpio_iomux_in(bus_config->quadhd_io_num, spi_periph_signal[host].spihd_in);
            gpio_iomux_out(bus_config->quadhd_io_num, FUNC_SPI, false);
        }
        if (bus_config->sclk_io_num >= 0) {
            gpio_iomux_in(bus_config->sclk_io_num, spi_periph_signal[host].spiclk_in);
            gpio_iomux_out(bus_config->sclk_io_num, FUNC_SPI, false);
        }
        temp_flag |= SPICOMMON_BUSFLAG_NATIVE_PINS;
    } else {
        //Use GPIO matrix
        ESP_LOGD(SPI_TAG, "SPI%d use gpio matrix.", host );
        if (bus_config->mosi_io_num >= 0) {
            if (mosi_output || (temp_flag&SPICOMMON_BUSFLAG_DUAL)) {
                gpio_set_direction(bus_config->mosi_io_num, GPIO_MODE_INPUT_OUTPUT);
                gpio_matrix_out(bus_config->mosi_io_num, spi_periph_signal[host].spid_out, false, false);
            } else {
                gpio_set_direction(bus_config->mosi_io_num, GPIO_MODE_INPUT);
            }
            gpio_matrix_in(bus_config->mosi_io_num, spi_periph_signal[host].spid_in, false);
            PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->mosi_io_num], FUNC_GPIO);
        }
        if (bus_config->miso_io_num >= 0) {
            if (miso_output || (temp_flag&SPICOMMON_BUSFLAG_DUAL)) {
                gpio_set_direction(bus_config->miso_io_num, GPIO_MODE_INPUT_OUTPUT);
                gpio_matrix_out(bus_config->miso_io_num, spi_periph_signal[host].spiq_out, false, false);
            } else {
                gpio_set_direction(bus_config->miso_io_num, GPIO_MODE_INPUT);
            }
            gpio_matrix_in(bus_config->miso_io_num, spi_periph_signal[host].spiq_in, false);
            PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->miso_io_num], FUNC_GPIO);
        }
        if (bus_config->quadwp_io_num >= 0) {
            gpio_set_direction(bus_config->quadwp_io_num, GPIO_MODE_INPUT_OUTPUT);
            gpio_matrix_out(bus_config->quadwp_io_num, spi_periph_signal[host].spiwp_out, false, false);
            gpio_matrix_in(bus_config->quadwp_io_num, spi_periph_signal[host].spiwp_in, false);
            PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->quadwp_io_num], FUNC_GPIO);
        }
        if (bus_config->quadhd_io_num >= 0) {
            gpio_set_direction(bus_config->quadhd_io_num, GPIO_MODE_INPUT_OUTPUT);
            gpio_matrix_out(bus_config->quadhd_io_num, spi_periph_signal[host].spihd_out, false, false);
            gpio_matrix_in(bus_config->quadhd_io_num, spi_periph_signal[host].spihd_in, false);
            PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->quadhd_io_num], FUNC_GPIO);
        }
        if (bus_config->sclk_io_num >= 0) {
            gpio_set_direction(bus_config->sclk_io_num, GPIO_MODE_INPUT_OUTPUT);
            gpio_matrix_out(bus_config->sclk_io_num, spi_periph_signal[host].spiclk_out, false, false);
            gpio_matrix_in(bus_config->sclk_io_num, spi_periph_signal[host].spiclk_in, false);
            PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->sclk_io_num], FUNC_GPIO);
        }
    }

    //Select DMA channel.
    DPORT_SET_PERI_REG_BITS(DPORT_SPI_DMA_CHAN_SEL_REG, 3, dma_chan, (host * 2));

    if (flags_o) *flags_o = temp_flag;
    return ESP_OK;
}


//Find any pin with output muxed to ``func`` and reset it to GPIO
static void reset_func_to_gpio(int func)
{
    for (int x = 0; x < GPIO_PIN_COUNT; x++) {
        if (GPIO_IS_VALID_GPIO(x) && (READ_PERI_REG(GPIO_FUNC0_OUT_SEL_CFG_REG + (x * 4))&GPIO_FUNC0_OUT_SEL_M) == func)  {
            gpio_matrix_out(x, SIG_GPIO_OUT_IDX, false, false);
        }
    }
}

esp_err_t spicommon_bus_free_io(spi_host_device_t host)
{
    if (REG_GET_FIELD(GPIO_PIN_MUX_REG[spi_periph_signal[host].spid_iomux_pin], MCU_SEL) == 1) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[spi_periph_signal[host].spid_iomux_pin], PIN_FUNC_GPIO);
    if (REG_GET_FIELD(GPIO_PIN_MUX_REG[spi_periph_signal[host].spiq_iomux_pin], MCU_SEL) == 1) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[spi_periph_signal[host].spiq_iomux_pin], PIN_FUNC_GPIO);
    if (REG_GET_FIELD(GPIO_PIN_MUX_REG[spi_periph_signal[host].spiclk_iomux_pin], MCU_SEL) == 1) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[spi_periph_signal[host].spiclk_iomux_pin], PIN_FUNC_GPIO);
    if (REG_GET_FIELD(GPIO_PIN_MUX_REG[spi_periph_signal[host].spiwp_iomux_pin], MCU_SEL) == 1) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[spi_periph_signal[host].spiwp_iomux_pin], PIN_FUNC_GPIO);
    if (REG_GET_FIELD(GPIO_PIN_MUX_REG[spi_periph_signal[host].spihd_iomux_pin], MCU_SEL) == 1) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[spi_periph_signal[host].spihd_iomux_pin], PIN_FUNC_GPIO);
    reset_func_to_gpio(spi_periph_signal[host].spid_out);
    reset_func_to_gpio(spi_periph_signal[host].spiq_out);
    reset_func_to_gpio(spi_periph_signal[host].spiclk_out);
    reset_func_to_gpio(spi_periph_signal[host].spiwp_out);
    reset_func_to_gpio(spi_periph_signal[host].spihd_out);
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
        gpio_iomux_out(cs_io_num, FUNC_SPI, false);
    } else {
        //Use GPIO matrix
        gpio_matrix_out(cs_io_num, spi_periph_signal[host].spics_out[cs_num], false, false);
        if (cs_num == 0) gpio_matrix_in(cs_io_num, spi_periph_signal[host].spics_in, false);
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[cs_io_num], FUNC_GPIO);
    }
}

void spicommon_cs_free(spi_host_device_t host, int cs_io_num)
{
    if (cs_io_num == 0 && REG_GET_FIELD(GPIO_PIN_MUX_REG[spi_periph_signal[host].spics0_iomux_pin], MCU_SEL) == 1) {
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[spi_periph_signal[host].spics0_iomux_pin], PIN_FUNC_GPIO);
    }
    reset_func_to_gpio(spi_periph_signal[host].spics_out[cs_io_num]);
}

void spicommon_cs_free_io(int cs_gpio_num)
{
    assert(cs_gpio_num>=0 && GPIO_IS_VALID_GPIO(cs_gpio_num));
    gpio_reset_pin(cs_gpio_num);
}

//Set up a list of dma descriptors. dmadesc is an array of descriptors. Data is the buffer to point to.
void IRAM_ATTR spicommon_setup_dma_desc_links(lldesc_t *dmadesc, int len, const uint8_t *data, bool isrx)
{
    int n = 0;
    while (len) {
        int dmachunklen = len;
        if (dmachunklen > SPI_MAX_DMA_LEN) dmachunklen = SPI_MAX_DMA_LEN;
        if (isrx) {
            //Receive needs DMA length rounded to next 32-bit boundary
            dmadesc[n].size = (dmachunklen + 3) & (~3);
            dmadesc[n].length = (dmachunklen + 3) & (~3);
        } else {
            dmadesc[n].size = dmachunklen;
            dmadesc[n].length = dmachunklen;
        }
        dmadesc[n].buf = (uint8_t *)data;
        dmadesc[n].eof = 0;
        dmadesc[n].sosf = 0;
        dmadesc[n].owner = 1;
        dmadesc[n].qe.stqe_next = &dmadesc[n + 1];
        len -= dmachunklen;
        data += dmachunklen;
        n++;
    }
    dmadesc[n - 1].eof = 1; //Mark last DMA desc as end of stream.
    dmadesc[n - 1].qe.stqe_next = NULL;
}


/*
Code for workaround for DMA issue in ESP32 v0/v1 silicon
*/


static volatile int dmaworkaround_channels_busy[2] = {0, 0};
static dmaworkaround_cb_t dmaworkaround_cb;
static void *dmaworkaround_cb_arg;
static portMUX_TYPE dmaworkaround_mux = portMUX_INITIALIZER_UNLOCKED;
static int dmaworkaround_waiting_for_chan = 0;

bool IRAM_ATTR spicommon_dmaworkaround_req_reset(int dmachan, dmaworkaround_cb_t cb, void *arg)
{
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
}

bool IRAM_ATTR spicommon_dmaworkaround_reset_in_progress()
{
    return (dmaworkaround_waiting_for_chan != 0);
}

void IRAM_ATTR spicommon_dmaworkaround_idle(int dmachan)
{
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
}

void IRAM_ATTR spicommon_dmaworkaround_transfer_active(int dmachan)
{
    portENTER_CRITICAL_ISR(&dmaworkaround_mux);
    dmaworkaround_channels_busy[dmachan-1] = 1;
    portEXIT_CRITICAL_ISR(&dmaworkaround_mux);
}


