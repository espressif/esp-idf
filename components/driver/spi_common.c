// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#include "soc/gpio_sig_map.h"
#include "soc/spi_reg.h"
#include "soc/dport_reg.h"
#include "soc/spi_struct.h"
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
#include "esp_heap_alloc_caps.h"

#include "driver/spi_common.h"

static const char *SPI_TAG = "spi";

#define SPI_CHECK(a, str, ret_val) \
    if (!(a)) { \
        ESP_LOGE(SPI_TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val); \
    }


typedef struct spi_device_t spi_device_t;

/*
 Stores a bunch of per-spi-peripheral data.
*/
typedef struct {
    const uint8_t spiclk_out;       //GPIO mux output signals
    const uint8_t spiclk_in;
    const uint8_t spid_out;
    const uint8_t spiq_out;
    const uint8_t spiwp_out;
    const uint8_t spihd_out;
    const uint8_t spid_in;          //GPIO mux input signals
    const uint8_t spiq_in;
    const uint8_t spiwp_in;
    const uint8_t spihd_in;
    const uint8_t spics_out[3];     // /CS GPIO output mux signals
    const uint8_t spics_in;
    const uint8_t spiclk_native;    //IO pins of IO_MUX muxed signals
    const uint8_t spid_native;
    const uint8_t spiq_native;
    const uint8_t spiwp_native;
    const uint8_t spihd_native;
    const uint8_t spics0_native;
    const uint8_t irq;              //irq source for interrupt mux
    const uint8_t irq_dma;          //dma irq source for interrupt mux
    const periph_module_t module;   //peripheral module, for enabling clock etc
    spi_dev_t *hw;              //Pointer to the hardware registers
} spi_signal_conn_t;

/*
 Bunch of constants for every SPI peripheral: GPIO signals, irqs, hw addr of registers etc
*/
static const spi_signal_conn_t io_signal[3] = {
    {
        .spiclk_out = SPICLK_OUT_IDX,
        .spiclk_in = SPICLK_IN_IDX,
        .spid_out = SPID_OUT_IDX,
        .spiq_out = SPIQ_OUT_IDX,
        .spiwp_out = SPIWP_OUT_IDX,
        .spihd_out = SPIHD_OUT_IDX,
        .spid_in = SPID_IN_IDX,
        .spiq_in = SPIQ_IN_IDX,
        .spiwp_in = SPIWP_IN_IDX,
        .spihd_in = SPIHD_IN_IDX,
        .spics_out = {SPICS0_OUT_IDX, SPICS1_OUT_IDX, SPICS2_OUT_IDX},
        .spics_in = SPICS0_IN_IDX,
        .spiclk_native = 6,
        .spid_native = 8,
        .spiq_native = 7,
        .spiwp_native = 10,
        .spihd_native = 9,
        .spics0_native = 11,
        .irq = ETS_SPI1_INTR_SOURCE,
        .irq_dma = ETS_SPI1_DMA_INTR_SOURCE,
        .module = PERIPH_SPI_MODULE,
        .hw = &SPI1
    }, {
        .spiclk_out = HSPICLK_OUT_IDX,
        .spiclk_in = HSPICLK_IN_IDX,
        .spid_out = HSPID_OUT_IDX,
        .spiq_out = HSPIQ_OUT_IDX,
        .spiwp_out = HSPIWP_OUT_IDX,
        .spihd_out = HSPIHD_OUT_IDX,
        .spid_in = HSPID_IN_IDX,
        .spiq_in = HSPIQ_IN_IDX,
        .spiwp_in = HSPIWP_IN_IDX,
        .spihd_in = HSPIHD_IN_IDX,
        .spics_out = {HSPICS0_OUT_IDX, HSPICS1_OUT_IDX, HSPICS2_OUT_IDX},
        .spics_in = HSPICS0_IN_IDX,
        .spiclk_native = 14,
        .spid_native = 13,
        .spiq_native = 12,
        .spiwp_native = 2,
        .spihd_native = 4,
        .spics0_native = 15,
        .irq = ETS_SPI2_INTR_SOURCE,
        .irq_dma = ETS_SPI2_DMA_INTR_SOURCE,
        .module = PERIPH_HSPI_MODULE,
        .hw = &SPI2
    }, {
        .spiclk_out = VSPICLK_OUT_IDX,
        .spiclk_in = VSPICLK_IN_IDX,
        .spid_out = VSPID_OUT_IDX,
        .spiq_out = VSPIQ_OUT_IDX,
        .spiwp_out = VSPIWP_OUT_IDX,
        .spihd_out = VSPIHD_OUT_IDX,
        .spid_in = VSPID_IN_IDX,
        .spiq_in = VSPIQ_IN_IDX,
        .spiwp_in = VSPIWP_IN_IDX,
        .spihd_in = VSPIHD_IN_IDX,
        .spics_out = {VSPICS0_OUT_IDX, VSPICS1_OUT_IDX, VSPICS2_OUT_IDX},
        .spics_in = VSPICS0_IN_IDX,
        .spiclk_native = 18,
        .spid_native = 23,
        .spiq_native = 19,
        .spiwp_native = 22,
        .spihd_native = 21,
        .spics0_native = 5,
        .irq = ETS_SPI3_INTR_SOURCE,
        .irq_dma = ETS_SPI3_DMA_INTR_SOURCE,
        .module = PERIPH_VSPI_MODULE,
        .hw = &SPI3
    }
};

//Periph 1 is 'claimed' by SPI flash code.
static bool spi_periph_claimed[3] = {true, false, false};

//Returns true if this peripheral is successfully claimed, false if otherwise.
bool spicommon_periph_claim(spi_host_device_t host)
{
    bool ret = __sync_bool_compare_and_swap(&spi_periph_claimed[host], false, true);
    if (ret) periph_module_enable(io_signal[host].module);
    return ret;
}

//Returns true if this peripheral is successfully freed, false if otherwise.
bool spicommon_periph_free(spi_host_device_t host)
{
    bool ret = __sync_bool_compare_and_swap(&spi_periph_claimed[host], true, false);
    if (ret) periph_module_disable(io_signal[host].module);
    return ret;
}


int spicommon_irqsource_for_host(spi_host_device_t host)
{
    return io_signal[host].irq;
}

spi_dev_t *spicommon_hw_for_host(spi_host_device_t host)
{
    return io_signal[host].hw;
}

/*
Do the common stuff to hook up a SPI host to a bus defined by a bunch of GPIO pins. Feed it a host number and a
bus config struct and it'll set up the GPIO matrix and enable the device. It will set is_native to 1 if the bus
config can be done using the IOMUX instead of using the GPIO matrix.
*/
esp_err_t spicommon_bus_initialize_io(spi_host_device_t host, const spi_bus_config_t *bus_config, int dma_chan, int flags, bool *is_native)
{
    bool native = true;
    bool use_quad = (flags & SPICOMMON_BUSFLAG_QUAD) != 0;

    SPI_CHECK(bus_config->mosi_io_num < 0 || GPIO_IS_VALID_OUTPUT_GPIO(bus_config->mosi_io_num), "spid pin invalid", ESP_ERR_INVALID_ARG);
    SPI_CHECK(bus_config->sclk_io_num < 0 || GPIO_IS_VALID_OUTPUT_GPIO(bus_config->sclk_io_num), "spiclk pin invalid", ESP_ERR_INVALID_ARG);
    SPI_CHECK(bus_config->miso_io_num < 0 || GPIO_IS_VALID_GPIO(bus_config->miso_io_num), "spiq pin invalid", ESP_ERR_INVALID_ARG);
    if (use_quad) {
        SPI_CHECK(bus_config->quadwp_io_num < 0 || GPIO_IS_VALID_OUTPUT_GPIO(bus_config->quadwp_io_num), "spiwp pin invalid", ESP_ERR_INVALID_ARG);
        SPI_CHECK(bus_config->quadhd_io_num < 0 || GPIO_IS_VALID_OUTPUT_GPIO(bus_config->quadhd_io_num), "spihd pin invalid", ESP_ERR_INVALID_ARG);
    }

    //Check if the selected pins correspond to the native pins of the peripheral
    if (bus_config->mosi_io_num >= 0 && bus_config->mosi_io_num != io_signal[host].spid_native) native = false;
    if (bus_config->miso_io_num >= 0 && bus_config->miso_io_num != io_signal[host].spiq_native) native = false;
    if (bus_config->sclk_io_num >= 0 && bus_config->sclk_io_num != io_signal[host].spiclk_native) native = false;
    if (use_quad) {
        if (bus_config->quadwp_io_num >= 0 && bus_config->quadwp_io_num != io_signal[host].spiwp_native) native = false;
        if (bus_config->quadhd_io_num >= 0 && bus_config->quadhd_io_num != io_signal[host].spihd_native) native = false;
    }

    *is_native = native;

    if (native) {
        //All SPI native pin selections resolve to 1, so we put that here instead of trying to figure
        //out which FUNC_GPIOx_xSPIxx to grab; they all are defined to 1 anyway.
        if (bus_config->mosi_io_num > 0) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->mosi_io_num], 1);
        if (bus_config->miso_io_num > 0) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->miso_io_num], 1);
        if (use_quad && bus_config->quadwp_io_num > 0) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->quadwp_io_num], 1);
        if (use_quad && bus_config->quadhd_io_num > 0) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->quadhd_io_num], 1);
        if (bus_config->sclk_io_num > 0) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->sclk_io_num], 1);
    } else {
        //Use GPIO
        if (bus_config->mosi_io_num > 0) {
            PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->mosi_io_num], PIN_FUNC_GPIO);
            gpio_set_direction(bus_config->mosi_io_num, GPIO_MODE_INPUT_OUTPUT);
            gpio_matrix_out(bus_config->mosi_io_num, io_signal[host].spid_out, false, false);
            gpio_matrix_in(bus_config->mosi_io_num, io_signal[host].spid_in, false);
        }
        if (bus_config->miso_io_num > 0) {
            PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->miso_io_num], PIN_FUNC_GPIO);
            gpio_set_direction(bus_config->miso_io_num, GPIO_MODE_INPUT_OUTPUT);
            gpio_matrix_out(bus_config->miso_io_num, io_signal[host].spiq_out, false, false);
            gpio_matrix_in(bus_config->miso_io_num, io_signal[host].spiq_in, false);
        }
        if (use_quad && bus_config->quadwp_io_num > 0) {
            PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->quadwp_io_num], PIN_FUNC_GPIO);
            gpio_set_direction(bus_config->quadwp_io_num, GPIO_MODE_INPUT_OUTPUT);
            gpio_matrix_out(bus_config->quadwp_io_num, io_signal[host].spiwp_out, false, false);
            gpio_matrix_in(bus_config->quadwp_io_num, io_signal[host].spiwp_in, false);
        }
        if (use_quad && bus_config->quadhd_io_num > 0) {
            PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->quadhd_io_num], PIN_FUNC_GPIO);
            gpio_set_direction(bus_config->quadhd_io_num, GPIO_MODE_INPUT_OUTPUT);
            gpio_matrix_out(bus_config->quadhd_io_num, io_signal[host].spihd_out, false, false);
            gpio_matrix_in(bus_config->quadhd_io_num, io_signal[host].spihd_in, false);
        }
        if (bus_config->sclk_io_num > 0) {
            PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->sclk_io_num], PIN_FUNC_GPIO);
            gpio_set_direction(bus_config->sclk_io_num, GPIO_MODE_INPUT_OUTPUT);
            gpio_matrix_out(bus_config->sclk_io_num, io_signal[host].spiclk_out, false, false);
            gpio_matrix_in(bus_config->sclk_io_num, io_signal[host].spiclk_in, false);
        }
    }

    //Select DMA channel.
    DPORT_SET_PERI_REG_BITS(DPORT_SPI_DMA_CHAN_SEL_REG, 3, dma_chan, (host * 2));

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
    if (REG_GET_FIELD(GPIO_PIN_MUX_REG[io_signal[host].spid_native], MCU_SEL) == 1) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[io_signal[host].spid_native], PIN_FUNC_GPIO);
    if (REG_GET_FIELD(GPIO_PIN_MUX_REG[io_signal[host].spiq_native], MCU_SEL) == 1) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[io_signal[host].spiq_native], PIN_FUNC_GPIO);
    if (REG_GET_FIELD(GPIO_PIN_MUX_REG[io_signal[host].spiclk_native], MCU_SEL) == 1) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[io_signal[host].spiclk_native], PIN_FUNC_GPIO);
    if (REG_GET_FIELD(GPIO_PIN_MUX_REG[io_signal[host].spiwp_native], MCU_SEL) == 1) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[io_signal[host].spiwp_native], PIN_FUNC_GPIO);
    if (REG_GET_FIELD(GPIO_PIN_MUX_REG[io_signal[host].spihd_native], MCU_SEL) == 1) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[io_signal[host].spihd_native], PIN_FUNC_GPIO);
    reset_func_to_gpio(io_signal[host].spid_out);
    reset_func_to_gpio(io_signal[host].spiq_out);
    reset_func_to_gpio(io_signal[host].spiclk_out);
    reset_func_to_gpio(io_signal[host].spiwp_out);
    reset_func_to_gpio(io_signal[host].spihd_out);
    return ESP_OK;
}

void spicommon_cs_initialize(spi_host_device_t host, int cs_io_num, int cs_num, int force_gpio_matrix)
{
    if (!force_gpio_matrix && cs_io_num == io_signal[host].spics0_native && cs_num == 0) {
        //The cs0s for all SPI peripherals map to pin mux source 1, so we use that instead of a define.
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[cs_io_num], 1);
    } else {
        //Use GPIO matrix
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[cs_io_num], PIN_FUNC_GPIO);
        gpio_matrix_out(cs_io_num, io_signal[host].spics_out[cs_num], false, false);
        if (cs_num == 0) gpio_matrix_in(cs_io_num, io_signal[host].spics_in, false);
    }
}

void spicommon_cs_free(spi_host_device_t host, int cs_io_num)
{
    if (cs_io_num == 0 && REG_GET_FIELD(GPIO_PIN_MUX_REG[io_signal[host].spics0_native], MCU_SEL) == 1) {
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[io_signal[host].spics0_native], PIN_FUNC_GPIO);
    }
    reset_func_to_gpio(io_signal[host].spics_out[cs_io_num]);
}

//Set up a list of dma descriptors. dmadesc is an array of descriptors. Data is the buffer to point to.
void spicommon_setup_dma_desc_links(lldesc_t *dmadesc, int len, const uint8_t *data, bool isrx)
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
    portENTER_CRITICAL(&dmaworkaround_mux);
    if (dmaworkaround_channels_busy[otherchan-1]) {
        //Other channel is busy. Call back when it's done.
        dmaworkaround_cb = cb;
        dmaworkaround_cb_arg = arg;
        dmaworkaround_waiting_for_chan = otherchan;
        ret = false;
    } else {
        //Reset DMA
        DPORT_SET_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI_DMA_RST);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI_DMA_RST);
        ret = true;
    }
    portEXIT_CRITICAL(&dmaworkaround_mux);
    return ret;
}

bool IRAM_ATTR spicommon_dmaworkaround_reset_in_progress()
{
    return (dmaworkaround_waiting_for_chan != 0);
}

void IRAM_ATTR spicommon_dmaworkaround_idle(int dmachan)
{
    portENTER_CRITICAL(&dmaworkaround_mux);
    dmaworkaround_channels_busy[dmachan-1] = 0;
    if (dmaworkaround_waiting_for_chan == dmachan) {
        //Reset DMA
        DPORT_SET_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI_DMA_RST);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI_DMA_RST);
        dmaworkaround_waiting_for_chan = 0;
        //Call callback
        dmaworkaround_cb(dmaworkaround_cb_arg);

    }
    portEXIT_CRITICAL(&dmaworkaround_mux);
}

void IRAM_ATTR spicommon_dmaworkaround_transfer_active(int dmachan)
{
    portENTER_CRITICAL(&dmaworkaround_mux);
    dmaworkaround_channels_busy[dmachan-1] = 1;
    portEXIT_CRITICAL(&dmaworkaround_mux);
}


