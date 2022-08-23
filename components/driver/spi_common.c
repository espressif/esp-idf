/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <string.h>
#include "sdkconfig.h"
#include "driver/spi_master.h"
#include "soc/spi_periph.h"
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_err.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/soc_pins.h"
#include "soc/lldesc.h"
#include "driver/gpio.h"
#include "esp_private/periph_ctrl.h"
#include "esp_heap_caps.h"
#include "esp_private/spi_common_internal.h"
#include "stdatomic.h"
#include "hal/spi_hal.h"
#include "hal/gpio_hal.h"
#include "esp_rom_gpio.h"
#if CONFIG_IDF_TARGET_ESP32
#include "soc/dport_reg.h"
#endif
#if SOC_GDMA_SUPPORTED
#include "esp_private/gdma.h"
#endif

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

#define SPI_MAIN_BUS_DEFAULT() { \
        .host_id = 0, \
        .bus_attr = { \
            .tx_dma_chan = 0, \
            .rx_dma_chan = 0, \
            .max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE, \
            .dma_desc_num= 0, \
        }, \
    }

#define FUNC_GPIO   PIN_FUNC_GPIO


typedef struct {
    int host_id;
    spi_destroy_func_t destroy_func;
    void* destroy_arg;
    spi_bus_attr_t bus_attr;
#if SOC_GDMA_SUPPORTED
    gdma_channel_handle_t tx_channel;
    gdma_channel_handle_t rx_channel;
#endif
} spicommon_bus_context_t;

//Periph 1 is 'claimed' by SPI flash code.
static atomic_bool spi_periph_claimed[SOC_SPI_PERIPH_NUM] = { ATOMIC_VAR_INIT(true), ATOMIC_VAR_INIT(false),
#if (SOC_SPI_PERIPH_NUM >= 3)
ATOMIC_VAR_INIT(false),
#endif
#if (SOC_SPI_PERIPH_NUM >= 4)
ATOMIC_VAR_INIT(false),
#endif
};

static const char* spi_claiming_func[3] = {NULL, NULL, NULL};
static spicommon_bus_context_t s_mainbus = SPI_MAIN_BUS_DEFAULT();
static spicommon_bus_context_t* bus_ctx[SOC_SPI_PERIPH_NUM] = {&s_mainbus};

#if !SOC_GDMA_SUPPORTED
//Each bit stands for 1 dma channel, BIT(0) should be used for SPI1
static uint8_t spi_dma_chan_enabled = 0;
static portMUX_TYPE spi_dma_spinlock = portMUX_INITIALIZER_UNLOCKED;
#endif  //#if !SOC_GDMA_SUPPORTED


static inline bool is_valid_host(spi_host_device_t host)
{
#if (SOC_SPI_PERIPH_NUM == 2)
    return host >= SPI1_HOST && host <= SPI2_HOST;
#elif (SOC_SPI_PERIPH_NUM == 3)
    return host >= SPI1_HOST && host <= SPI3_HOST;
#endif
}

//----------------------------------------------------------alloc spi periph-------------------------------------------------------//
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

//----------------------------------------------------------alloc dma periph-------------------------------------------------------//
#if !SOC_GDMA_SUPPORTED
static inline periph_module_t get_dma_periph(int dma_chan)
{
    assert(dma_chan >= 1 && dma_chan <= SOC_SPI_DMA_CHAN_NUM);
#if CONFIG_IDF_TARGET_ESP32S2
    if (dma_chan == 1) {
        return PERIPH_SPI2_DMA_MODULE;
    } else if (dma_chan == 2) {
        return PERIPH_SPI3_DMA_MODULE;
    } else {
        abort();
    }
#elif CONFIG_IDF_TARGET_ESP32
    return PERIPH_SPI_DMA_MODULE;
#endif
}

static bool claim_dma_chan(int dma_chan, uint32_t *out_actual_dma_chan)
{
    bool ret = false;

    portENTER_CRITICAL(&spi_dma_spinlock);
    bool is_used = (BIT(dma_chan) & spi_dma_chan_enabled);
    if (!is_used) {
        spi_dma_chan_enabled |= BIT(dma_chan);
        periph_module_enable(get_dma_periph(dma_chan));
        *out_actual_dma_chan = dma_chan;
        ret = true;
    }
    portEXIT_CRITICAL(&spi_dma_spinlock);

    return ret;
}

static void connect_spi_and_dma(spi_host_device_t host, int dma_chan)
{
#if CONFIG_IDF_TARGET_ESP32
    DPORT_SET_PERI_REG_BITS(DPORT_SPI_DMA_CHAN_SEL_REG, 3, dma_chan, (host * 2));
#elif CONFIG_IDF_TARGET_ESP32S2
    //On ESP32S2, each SPI controller has its own DMA channel. So there is no need to connect them.
#endif
}

static esp_err_t alloc_dma_chan(spi_host_device_t host_id, spi_dma_chan_t dma_chan, uint32_t *out_actual_tx_dma_chan, uint32_t *out_actual_rx_dma_chan)
{
    assert(is_valid_host(host_id));
#if CONFIG_IDF_TARGET_ESP32
    assert(dma_chan > SPI_DMA_DISABLED && dma_chan <= SPI_DMA_CH_AUTO);
#elif CONFIG_IDF_TARGET_ESP32S2
    assert(dma_chan == (int)host_id || dma_chan == SPI_DMA_CH_AUTO);
#endif

    esp_err_t ret = ESP_OK;
    bool success = false;
    uint32_t actual_dma_chan = 0;

    if (dma_chan == SPI_DMA_CH_AUTO) {
#if CONFIG_IDF_TARGET_ESP32
        for (int i = 1; i < SOC_SPI_DMA_CHAN_NUM+1; i++) {
            success = claim_dma_chan(i, &actual_dma_chan);
            if (success) {
                break;
            }
        }
#elif CONFIG_IDF_TARGET_ESP32S2
        //On ESP32S2, each SPI controller has its own DMA channel
        success = claim_dma_chan(host_id, &actual_dma_chan);
#endif  //#if CONFIG_IDF_TARGET_XXX
    } else {
        success = claim_dma_chan((int)dma_chan, &actual_dma_chan);
    }

    //On ESP32 and ESP32S2, actual_tx_dma_chan and actual_rx_dma_chan are always same
    *out_actual_tx_dma_chan = actual_dma_chan;
    *out_actual_rx_dma_chan = actual_dma_chan;

    if (!success) {
        SPI_CHECK(false, "no available dma channel", ESP_ERR_NOT_FOUND);
    }

    connect_spi_and_dma(host_id, *out_actual_tx_dma_chan);

    return ret;
}

#else //SOC_GDMA_SUPPORTED
static esp_err_t alloc_dma_chan(spi_host_device_t host_id, spi_dma_chan_t dma_chan, uint32_t *out_actual_tx_dma_chan, uint32_t *out_actual_rx_dma_chan)
{
    assert(is_valid_host(host_id));
    assert(dma_chan == SPI_DMA_CH_AUTO);

    esp_err_t ret = ESP_OK;
    spicommon_bus_context_t *ctx = bus_ctx[host_id];

    if (dma_chan == SPI_DMA_CH_AUTO) {
        gdma_channel_alloc_config_t tx_alloc_config = {
            .flags.reserve_sibling = 1,
            .direction = GDMA_CHANNEL_DIRECTION_TX,
        };
        ret = gdma_new_channel(&tx_alloc_config, &ctx->tx_channel);
        if (ret != ESP_OK) {
            return ret;
        }

        gdma_channel_alloc_config_t rx_alloc_config = {
            .direction = GDMA_CHANNEL_DIRECTION_RX,
            .sibling_chan = ctx->tx_channel,
        };
        ret = gdma_new_channel(&rx_alloc_config, &ctx->rx_channel);
        if (ret != ESP_OK) {
            return ret;
        }

        if (host_id == SPI2_HOST) {
            gdma_connect(ctx->rx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_SPI, 2));
            gdma_connect(ctx->tx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_SPI, 2));
        }
#if (SOC_SPI_PERIPH_NUM >= 3)
        else if (host_id == SPI3_HOST) {
            gdma_connect(ctx->rx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_SPI, 3));
            gdma_connect(ctx->tx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_SPI, 3));
        }
#endif
        gdma_get_channel_id(ctx->tx_channel, (int *)out_actual_tx_dma_chan);
        gdma_get_channel_id(ctx->rx_channel, (int *)out_actual_rx_dma_chan);
    }

    return ret;
}
#endif  //#if !SOC_GDMA_SUPPORTED

esp_err_t spicommon_dma_chan_alloc(spi_host_device_t host_id, spi_dma_chan_t dma_chan, uint32_t *out_actual_tx_dma_chan, uint32_t *out_actual_rx_dma_chan)
{
    assert(is_valid_host(host_id));
#if CONFIG_IDF_TARGET_ESP32
    assert(dma_chan > SPI_DMA_DISABLED && dma_chan <= SPI_DMA_CH_AUTO);
#elif CONFIG_IDF_TARGET_ESP32S2
    assert(dma_chan == (int)host_id || dma_chan == SPI_DMA_CH_AUTO);
#endif

    esp_err_t ret = ESP_OK;
    uint32_t actual_tx_dma_chan = 0;
    uint32_t actual_rx_dma_chan = 0;
    spicommon_bus_context_t *ctx = (spicommon_bus_context_t *)calloc(1, sizeof(spicommon_bus_context_t));
    if (!ctx) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }
    bus_ctx[host_id] = ctx;
    ctx->host_id = host_id;

    ret = alloc_dma_chan(host_id, dma_chan, &actual_tx_dma_chan, &actual_rx_dma_chan);
    if (ret != ESP_OK) {
        goto cleanup;
    }
    ctx->bus_attr.tx_dma_chan = actual_tx_dma_chan;
    ctx->bus_attr.rx_dma_chan = actual_rx_dma_chan;
    *out_actual_tx_dma_chan = actual_tx_dma_chan;
    *out_actual_rx_dma_chan = actual_rx_dma_chan;

    return ret;

cleanup:
    free(ctx);
    ctx = NULL;
    return ret;
}

//----------------------------------------------------------free dma periph-------------------------------------------------------//
static esp_err_t dma_chan_free(spi_host_device_t host_id)
{
    assert(is_valid_host(host_id));

    spicommon_bus_context_t *ctx = bus_ctx[host_id];
#if !SOC_GDMA_SUPPORTED
    //On ESP32S2, each SPI controller has its own DMA channel
    int dma_chan = ctx->bus_attr.tx_dma_chan;
    assert(spi_dma_chan_enabled & BIT(dma_chan));

    portENTER_CRITICAL(&spi_dma_spinlock);
    spi_dma_chan_enabled &= ~BIT(dma_chan);
    periph_module_disable(get_dma_periph(dma_chan));
    portEXIT_CRITICAL(&spi_dma_spinlock);

#else //SOC_GDMA_SUPPORTED
    if (ctx->rx_channel) {
        gdma_disconnect(ctx->rx_channel);
        gdma_del_channel(ctx->rx_channel);
    }
    if (ctx->tx_channel) {
        gdma_disconnect(ctx->tx_channel);
        gdma_del_channel(ctx->tx_channel);
    }
#endif

    return ESP_OK;
}

esp_err_t spicommon_dma_chan_free(spi_host_device_t host_id)
{
    assert(is_valid_host(host_id));

    esp_err_t ret = dma_chan_free(host_id);
    free(bus_ctx[host_id]);
    bus_ctx[host_id] = NULL;

    return ret;
}

//----------------------------------------------------------IO general-------------------------------------------------------//
#if SOC_SPI_SUPPORT_OCT
static bool check_iomux_pins_oct(spi_host_device_t host, const spi_bus_config_t* bus_config)
{
    if (host != SPI2_HOST) {
        return false;
    }
    int io_nums[] = {bus_config->data0_io_num, bus_config->data1_io_num, bus_config->data2_io_num, bus_config->data3_io_num,
        bus_config->sclk_io_num, bus_config->data4_io_num, bus_config->data5_io_num, bus_config->data6_io_num, bus_config->data7_io_num};
    int io_mux_nums[] = {SPI2_IOMUX_PIN_NUM_MOSI_OCT, SPI2_IOMUX_PIN_NUM_MISO_OCT, SPI2_IOMUX_PIN_NUM_WP_OCT, SPI2_IOMUX_PIN_NUM_HD_OCT,
        SPI2_IOMUX_PIN_NUM_CLK_OCT, SPI2_IOMUX_PIN_NUM_IO4_OCT, SPI2_IOMUX_PIN_NUM_IO5_OCT, SPI2_IOMUX_PIN_NUM_IO6_OCT, SPI2_IOMUX_PIN_NUM_IO7_OCT};
    for (size_t i = 0; i < sizeof(io_nums)/sizeof(io_nums[0]); i++) {
        if (io_nums[i] >= 0 && io_nums[i] != io_mux_nums[i]) {
            return false;
        }
    }
    return true;
}
#endif

static bool check_iomux_pins_quad(spi_host_device_t host, const spi_bus_config_t* bus_config)
{
    if (bus_config->sclk_io_num>=0 &&
        bus_config->sclk_io_num != spi_periph_signal[host].spiclk_iomux_pin) {
        return false;
    }
    if (bus_config->quadwp_io_num>=0 &&
        bus_config->quadwp_io_num != spi_periph_signal[host].spiwp_iomux_pin) {
        return false;
    }
    if (bus_config->quadhd_io_num>=0 &&
        bus_config->quadhd_io_num != spi_periph_signal[host].spihd_iomux_pin) {
        return false;
    }
    if (bus_config->mosi_io_num >= 0 &&
        bus_config->mosi_io_num != spi_periph_signal[host].spid_iomux_pin) {
        return false;
    }
    if (bus_config->miso_io_num>=0 &&
        bus_config->miso_io_num != spi_periph_signal[host].spiq_iomux_pin) {
        return false;
    }
    return true;
}

static bool bus_uses_iomux_pins(spi_host_device_t host, const spi_bus_config_t* bus_config)
{
//Check if SPI pins could be routed to iomux.
#if SOC_SPI_SUPPORT_OCT
    //The io mux pins available for Octal mode is not the same as the ones we use for non-Octal mode.
    if ((bus_config->flags & SPICOMMON_BUSFLAG_OCTAL) == SPICOMMON_BUSFLAG_OCTAL) {
        return check_iomux_pins_oct(host, bus_config);
    }
#endif
    return check_iomux_pins_quad(host, bus_config);
}

#if SOC_SPI_SUPPORT_OCT
static void bus_iomux_pins_set_oct(spi_host_device_t host, const spi_bus_config_t* bus_config)
{
    assert(host == SPI2_HOST);
    int io_nums[] = {bus_config->data0_io_num, bus_config->data1_io_num, bus_config->data2_io_num, bus_config->data3_io_num,
        bus_config->sclk_io_num, bus_config->data4_io_num, bus_config->data5_io_num, bus_config->data6_io_num, bus_config->data7_io_num};
    int io_signals[] = {spi_periph_signal[host].spid_in, spi_periph_signal[host].spiq_in, spi_periph_signal[host].spiwp_in,
        spi_periph_signal[host].spihd_in,spi_periph_signal[host].spiclk_in, spi_periph_signal[host].spid4_out,
        spi_periph_signal[host].spid5_out, spi_periph_signal[host].spid6_out, spi_periph_signal[host].spid7_out};
    for (size_t i = 0; i < sizeof(io_nums)/sizeof(io_nums[0]); i++) {
        if (io_nums[i] > 0) {
            gpio_iomux_in(io_nums[i], io_signals[i]);
            // In Octal mode use function channel 2
            gpio_iomux_out(io_nums[i], SPI2_FUNC_NUM_OCT, false);
        }
    }
}
#endif //SOC_SPI_SUPPORT_OCT

static void bus_iomux_pins_set_quad(spi_host_device_t host, const spi_bus_config_t* bus_config)
{
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
}

static void bus_iomux_pins_set(spi_host_device_t host, const spi_bus_config_t* bus_config)
{
#if SOC_SPI_SUPPORT_OCT
    if ((bus_config->flags & SPICOMMON_BUSFLAG_OCTAL) == SPICOMMON_BUSFLAG_OCTAL) {
        bus_iomux_pins_set_oct(host, bus_config);
        return;
    }
#endif
    bus_iomux_pins_set_quad(host, bus_config);
}

/*
Do the common stuff to hook up a SPI host to a bus defined by a bunch of GPIO pins. Feed it a host number and a
bus config struct and it'll set up the GPIO matrix and enable the device. If a pin is set to non-negative value,
it should be able to be initialized.
*/
esp_err_t spicommon_bus_initialize_io(spi_host_device_t host, const spi_bus_config_t *bus_config, uint32_t flags, uint32_t* flags_o)
{
#if SOC_SPI_SUPPORT_OCT
    // In the driver of previous version, spi data4 ~ spi data7 are not in spi_bus_config_t struct. So the new-added pins come as 0
    // if they are not really set. Add this boolean variable to check if the user has set spi data4 ~spi data7 pins .
    bool io4_7_is_blank = !bus_config->data4_io_num && !bus_config->data5_io_num && !bus_config->data6_io_num && !bus_config->data7_io_num;
    // This boolean variable specifies if user sets pins used for octal mode (users can set spi data4 ~ spi data7 to -1).
    bool io4_7_enabled = !io4_7_is_blank && bus_config->data4_io_num >= 0 && bus_config->data5_io_num >= 0 &&
                         bus_config->data6_io_num >= 0 && bus_config->data7_io_num >= 0;
    SPI_CHECK((flags & SPICOMMON_BUSFLAG_MASTER) || !((flags & SPICOMMON_BUSFLAG_OCTAL) == SPICOMMON_BUSFLAG_OCTAL), "Octal SPI mode / OPI mode only works when SPI is used as Master", ESP_ERR_INVALID_ARG);
    SPI_CHECK(host == SPI2_HOST || !((flags & SPICOMMON_BUSFLAG_OCTAL) == SPICOMMON_BUSFLAG_OCTAL), "Only SPI2 supports Octal SPI mode / OPI mode", ESP_ERR_INVALID_ARG);
#endif //SOC_SPI_SUPPORT_OCT

    uint32_t temp_flag = 0;

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
#if SOC_SPI_SUPPORT_OCT
    const bool io4_need_output = true;
    const bool io5_need_output = true;
    const bool io6_need_output = true;
    const bool io7_need_output = true;
    // set flags for OCTAL mode according to the existence of spi data4 ~ spi data7
    if (io4_7_enabled) {
        temp_flag |= SPICOMMON_BUSFLAG_IO4_IO7;
        if (bus_config->data4_io_num >= 0) {
            SPI_CHECK_PIN(bus_config->data4_io_num, "spi data4", io4_need_output);
        }
        if (bus_config->data5_io_num >= 0) {
            SPI_CHECK_PIN(bus_config->data5_io_num, "spi data5", io5_need_output);
        }
        if (bus_config->data6_io_num >= 0) {
            SPI_CHECK_PIN(bus_config->data6_io_num, "spi data6", io6_need_output);
        }
        if (bus_config->data7_io_num >= 0) {
            SPI_CHECK_PIN(bus_config->data7_io_num, "spi data7", io7_need_output);
        }
    }
#endif //SOC_SPI_SUPPORT_OCT

    //set flags for QUAD mode according to the existence of wp and hd
    if (bus_config->quadhd_io_num >= 0 && bus_config->quadwp_io_num >= 0) temp_flag |= SPICOMMON_BUSFLAG_WPHD;
    if (bus_config->mosi_io_num >= 0) {
        temp_flag |= SPICOMMON_BUSFLAG_MOSI;
        SPI_CHECK_PIN(bus_config->mosi_io_num, "mosi", mosi_need_output);
    }
    if (bus_config->miso_io_num >= 0) {
        temp_flag |= SPICOMMON_BUSFLAG_MISO;
        SPI_CHECK_PIN(bus_config->miso_io_num, "miso", miso_need_output);
    }
    //set flags for DUAL mode according to output-capability of MOSI and MISO pins.
    if ( (bus_config->mosi_io_num < 0 || GPIO_IS_VALID_OUTPUT_GPIO(bus_config->mosi_io_num)) &&
        (bus_config->miso_io_num < 0 || GPIO_IS_VALID_OUTPUT_GPIO(bus_config->miso_io_num)) ) {
        temp_flag |= SPICOMMON_BUSFLAG_DUAL;
    }

    //check if the selected pins correspond to the iomux pins of the peripheral
    bool use_iomux = !(flags & SPICOMMON_BUSFLAG_GPIO_PINS) && bus_uses_iomux_pins(host, bus_config);
    if (use_iomux) {
        temp_flag |= SPICOMMON_BUSFLAG_IOMUX_PINS;
    } else {
        temp_flag |= SPICOMMON_BUSFLAG_GPIO_PINS;
    }

    uint32_t missing_flag = flags & ~temp_flag;
    missing_flag &= ~SPICOMMON_BUSFLAG_MASTER;//don't check this flag

    if (missing_flag != 0) {
    //check pins existence
        if (missing_flag & SPICOMMON_BUSFLAG_SCLK) {
            ESP_LOGE(SPI_TAG, "sclk pin required.");
        }
        if (missing_flag & SPICOMMON_BUSFLAG_MOSI) {
            ESP_LOGE(SPI_TAG, "mosi pin required.");
        }
        if (missing_flag & SPICOMMON_BUSFLAG_MISO) {
            ESP_LOGE(SPI_TAG, "miso pin required.");
        }
        if (missing_flag & SPICOMMON_BUSFLAG_DUAL) {
            ESP_LOGE(SPI_TAG, "not both mosi and miso output capable");
        }
        if (missing_flag & SPICOMMON_BUSFLAG_WPHD) {
            ESP_LOGE(SPI_TAG, "both wp and hd required.");
        }
        if (missing_flag & SPICOMMON_BUSFLAG_IOMUX_PINS) {
            ESP_LOGE(SPI_TAG, "not using iomux pins");
        }
#if SOC_SPI_SUPPORT_OCT
        if (missing_flag & SPICOMMON_BUSFLAG_IO4_IO7) {
            ESP_LOGE(SPI_TAG, "spi data4 ~ spi data7 are required.");
        }
#endif
        SPI_CHECK(missing_flag == 0, "not all required capabilities satisfied.", ESP_ERR_INVALID_ARG);
    }

    if (use_iomux) {
        //All SPI iomux pin selections resolve to 1, so we put that here instead of trying to figure
        //out which FUNC_GPIOx_xSPIxx to grab; they all are defined to 1 anyway.
        ESP_LOGD(SPI_TAG, "SPI%d use iomux pins.", host+1);
        bus_iomux_pins_set(host, bus_config);
    } else {
        //Use GPIO matrix
        ESP_LOGD(SPI_TAG, "SPI%d use gpio matrix.", host+1);
        if (bus_config->mosi_io_num >= 0) {
            if (mosi_need_output || (temp_flag&SPICOMMON_BUSFLAG_DUAL)) {
                gpio_set_direction(bus_config->mosi_io_num, GPIO_MODE_INPUT_OUTPUT);
                esp_rom_gpio_connect_out_signal(bus_config->mosi_io_num, spi_periph_signal[host].spid_out, false, false);
            } else {
                gpio_set_direction(bus_config->mosi_io_num, GPIO_MODE_INPUT);
            }
            esp_rom_gpio_connect_in_signal(bus_config->mosi_io_num, spi_periph_signal[host].spid_in, false);
#if CONFIG_IDF_TARGET_ESP32S2
            PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[bus_config->mosi_io_num]);
#endif
            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[bus_config->mosi_io_num], FUNC_GPIO);
        }
        if (bus_config->miso_io_num >= 0) {
            if (miso_need_output || (temp_flag&SPICOMMON_BUSFLAG_DUAL)) {
                gpio_set_direction(bus_config->miso_io_num, GPIO_MODE_INPUT_OUTPUT);
                esp_rom_gpio_connect_out_signal(bus_config->miso_io_num, spi_periph_signal[host].spiq_out, false, false);
            } else {
                gpio_set_direction(bus_config->miso_io_num, GPIO_MODE_INPUT);
            }
            esp_rom_gpio_connect_in_signal(bus_config->miso_io_num, spi_periph_signal[host].spiq_in, false);
#if CONFIG_IDF_TARGET_ESP32S2
            PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[bus_config->miso_io_num]);
#endif
            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[bus_config->miso_io_num], FUNC_GPIO);
        }
        if (bus_config->quadwp_io_num >= 0) {
            gpio_set_direction(bus_config->quadwp_io_num, GPIO_MODE_INPUT_OUTPUT);
            esp_rom_gpio_connect_out_signal(bus_config->quadwp_io_num, spi_periph_signal[host].spiwp_out, false, false);
            esp_rom_gpio_connect_in_signal(bus_config->quadwp_io_num, spi_periph_signal[host].spiwp_in, false);
#if CONFIG_IDF_TARGET_ESP32S2
            PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[bus_config->quadwp_io_num]);
#endif
            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[bus_config->quadwp_io_num], FUNC_GPIO);
        }
        if (bus_config->quadhd_io_num >= 0) {
            gpio_set_direction(bus_config->quadhd_io_num, GPIO_MODE_INPUT_OUTPUT);
            esp_rom_gpio_connect_out_signal(bus_config->quadhd_io_num, spi_periph_signal[host].spihd_out, false, false);
            esp_rom_gpio_connect_in_signal(bus_config->quadhd_io_num, spi_periph_signal[host].spihd_in, false);
#if CONFIG_IDF_TARGET_ESP32S2
            PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[bus_config->quadhd_io_num]);
#endif
            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[bus_config->quadhd_io_num], FUNC_GPIO);
        }
        if (bus_config->sclk_io_num >= 0) {
            if (sclk_need_output) {
                gpio_set_direction(bus_config->sclk_io_num, GPIO_MODE_INPUT_OUTPUT);
                esp_rom_gpio_connect_out_signal(bus_config->sclk_io_num, spi_periph_signal[host].spiclk_out, false, false);
            } else {
                gpio_set_direction(bus_config->sclk_io_num, GPIO_MODE_INPUT);
            }
            esp_rom_gpio_connect_in_signal(bus_config->sclk_io_num, spi_periph_signal[host].spiclk_in, false);
#if CONFIG_IDF_TARGET_ESP32S2
            PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[bus_config->sclk_io_num]);
#endif
            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[bus_config->sclk_io_num], FUNC_GPIO);
        }
#if SOC_SPI_SUPPORT_OCT
        if ((flags & SPICOMMON_BUSFLAG_OCTAL) == SPICOMMON_BUSFLAG_OCTAL) {
            int io_nums[] = {bus_config->data4_io_num, bus_config->data5_io_num, bus_config->data6_io_num, bus_config->data7_io_num};
            uint8_t io_signals[4][2] = {{spi_periph_signal[host].spid4_out, spi_periph_signal[host].spid4_in},
                                        {spi_periph_signal[host].spid5_out, spi_periph_signal[host].spid5_in},
                                        {spi_periph_signal[host].spid6_out, spi_periph_signal[host].spid6_in},
                                        {spi_periph_signal[host].spid7_out, spi_periph_signal[host].spid7_in}};
            for (size_t i = 0; i < sizeof(io_nums) / sizeof(io_nums[0]); i++) {
                if (io_nums[i] >= 0) {
                    gpio_set_direction(io_nums[i], GPIO_MODE_INPUT_OUTPUT);
                    esp_rom_gpio_connect_out_signal(io_nums[i], io_signals[i][0], false, false);
                    esp_rom_gpio_connect_in_signal(io_nums[i], io_signals[i][1], false);
#if CONFIG_IDF_TARGET_ESP32S2
                    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[io_nums[i]]);
#endif
                    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[io_nums[i]], FUNC_GPIO);
                }
            }
        }
#endif //SOC_SPI_SUPPORT_OCT
    }

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
        if (GPIO_IS_VALID_GPIO(io)) {
            gpio_reset_pin(io);
        }
    }
    return ESP_OK;
}

void spicommon_cs_initialize(spi_host_device_t host, int cs_io_num, int cs_num, int force_gpio_matrix)
{
    if (!force_gpio_matrix && cs_io_num == spi_periph_signal[host].spics0_iomux_pin && cs_num == 0) {
        //The cs0s for all SPI peripherals map to pin mux source 1, so we use that instead of a define.
        gpio_iomux_in(cs_io_num, spi_periph_signal[host].spics_in);
        gpio_iomux_out(cs_io_num, spi_periph_signal[host].func, false);
    } else {
        //Use GPIO matrix
        if (GPIO_IS_VALID_OUTPUT_GPIO(cs_io_num)) {
            gpio_set_direction(cs_io_num, GPIO_MODE_INPUT_OUTPUT);
            esp_rom_gpio_connect_out_signal(cs_io_num, spi_periph_signal[host].spics_out[cs_num], false, false);
        } else {
            gpio_set_direction(cs_io_num, GPIO_MODE_INPUT);
        }
        if (cs_num == 0) esp_rom_gpio_connect_in_signal(cs_io_num, spi_periph_signal[host].spics_in, false);
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[cs_io_num]);
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[cs_io_num], FUNC_GPIO);
    }
}

void spicommon_cs_free_io(int cs_gpio_num)
{
    assert(GPIO_IS_VALID_GPIO(cs_gpio_num));
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

//----------------------------------------------------------master bus init-------------------------------------------------------//
esp_err_t spi_bus_initialize(spi_host_device_t host_id, const spi_bus_config_t *bus_config, spi_dma_chan_t dma_chan)
{
    esp_err_t err = ESP_OK;
    spicommon_bus_context_t *ctx = NULL;
    spi_bus_attr_t *bus_attr = NULL;
    uint32_t actual_tx_dma_chan = 0;
    uint32_t actual_rx_dma_chan = 0;

    SPI_CHECK(is_valid_host(host_id), "invalid host_id", ESP_ERR_INVALID_ARG);
    SPI_CHECK(bus_ctx[host_id] == NULL, "SPI bus already initialized.", ESP_ERR_INVALID_STATE);
#ifdef CONFIG_IDF_TARGET_ESP32
    SPI_CHECK(dma_chan >= SPI_DMA_DISABLED && dma_chan <= SPI_DMA_CH_AUTO, "invalid dma channel", ESP_ERR_INVALID_ARG );
#elif CONFIG_IDF_TARGET_ESP32S2
    SPI_CHECK( dma_chan == SPI_DMA_DISABLED || dma_chan == (int)host_id || dma_chan == SPI_DMA_CH_AUTO, "invalid dma channel", ESP_ERR_INVALID_ARG );
#elif SOC_GDMA_SUPPORTED
    SPI_CHECK( dma_chan == SPI_DMA_DISABLED || dma_chan == SPI_DMA_CH_AUTO, "invalid dma channel, chip only support spi dma channel auto-alloc", ESP_ERR_INVALID_ARG );
#endif
    SPI_CHECK((bus_config->intr_flags & (ESP_INTR_FLAG_HIGH|ESP_INTR_FLAG_EDGE|ESP_INTR_FLAG_INTRDISABLED))==0, "intr flag not allowed", ESP_ERR_INVALID_ARG);
#ifndef CONFIG_SPI_MASTER_ISR_IN_IRAM
    SPI_CHECK((bus_config->intr_flags & ESP_INTR_FLAG_IRAM)==0, "ESP_INTR_FLAG_IRAM should be disabled when CONFIG_SPI_MASTER_ISR_IN_IRAM is not set.", ESP_ERR_INVALID_ARG);
#endif

    bool spi_chan_claimed = spicommon_periph_claim(host_id, "spi master");
    SPI_CHECK(spi_chan_claimed, "host_id already in use", ESP_ERR_INVALID_STATE);

    //clean and initialize the context
    ctx = (spicommon_bus_context_t *)calloc(1, sizeof(spicommon_bus_context_t));
    if (!ctx) {
        err = ESP_ERR_NO_MEM;
        goto cleanup;
    }
    bus_ctx[host_id] = ctx;
    ctx->host_id = host_id;
    bus_attr = &ctx->bus_attr;
    bus_attr->bus_cfg = *bus_config;

    if (dma_chan != SPI_DMA_DISABLED) {
        bus_attr->dma_enabled = 1;

        err = alloc_dma_chan(host_id, dma_chan, &actual_tx_dma_chan, &actual_rx_dma_chan);
        if (err != ESP_OK) {
            goto cleanup;
        }
        bus_attr->tx_dma_chan = actual_tx_dma_chan;
        bus_attr->rx_dma_chan = actual_rx_dma_chan;

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
    } else {
        bus_attr->dma_enabled = 0;
        bus_attr->max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE;
        bus_attr->dma_desc_num = 0;
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

    err = spicommon_bus_initialize_io(host_id, bus_config, SPICOMMON_BUSFLAG_MASTER | bus_config->flags, &bus_attr->flags);
    if (err != ESP_OK) {
        goto cleanup;
    }

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
        bus_attr->dmadesc_tx = NULL;
        bus_attr->dmadesc_rx = NULL;
        if (bus_attr->dma_enabled) {
            dma_chan_free(host_id);
        }
    }
    spicommon_periph_free(host_id);
    free(bus_ctx[host_id]);
    bus_ctx[host_id] = NULL;
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
    bus_attr->dmadesc_tx = NULL;
    bus_attr->dmadesc_rx = NULL;
    if (bus_attr->dma_enabled > 0) {
        dma_chan_free(host_id);
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
