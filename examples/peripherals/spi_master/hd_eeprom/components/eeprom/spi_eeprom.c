/*
    This code demonstrates how to use the SPI master half duplex mode to read/write a AT932C46D
    EEPROM (8-bit mode).

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "spi_eeprom.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include <unistd.h>
#include "esp_log.h"
#include <sys/param.h>
#include "sdkconfig.h"

#define EEPROM_BUSY_TIMEOUT_MS  5

#define EEPROM_CLK_FREQ         (1*1000*1000)   //When powered by 3.3V, EEPROM max freq is 1MHz
#define EEPROM_INPUT_DELAY_NS   ((1000*1000*1000/EEPROM_CLK_FREQ)/2+20)

#define ADDR_MASK   0x7f

#define CMD_EWDS    0x200
#define CMD_WRAL    0x200
#define CMD_ERAL    0x200
#define CMD_EWEN    0x200
#define CMD_CKBS    0x000
#define CMD_READ    0x300
#define CMD_ERASE   0x380
#define CMD_WRITE   0x280

#define ADD_EWDS    0x00
#define ADD_WRAL    0x20
#define ADD_ERAL    0x40
#define ADD_EWEN    0x60

/// Context (config and data) of the spi_eeprom
struct eeprom_context_t {
    eeprom_config_t cfg;        ///< Configuration by the caller.
    spi_device_handle_t spi;    ///< SPI device handle
    SemaphoreHandle_t ready_sem; ///< Semaphore for ready signal
};

typedef struct eeprom_context_t eeprom_context_t;

static const char TAG[] = "eeprom";

static esp_err_t eeprom_simple_cmd(eeprom_context_t *ctx, uint16_t cmd)
{
    spi_transaction_t t = {
        .cmd = cmd,
        .user = ctx
    };
    return spi_device_polling_transmit(ctx->spi, &t);
}

static esp_err_t eeprom_wait_done_by_intr(eeprom_context_t* ctx)
{
    xSemaphoreTake(ctx->ready_sem, 0);
    gpio_set_level(ctx->cfg.cs_io, 1);
    gpio_intr_enable(ctx->cfg.miso_io);

    //Max processing time is 5ms, tick=1 may happen very soon, set to 2 at least
    uint32_t tick_to_wait = MAX(EEPROM_BUSY_TIMEOUT_MS / portTICK_PERIOD_MS, 2);
    BaseType_t ret = xSemaphoreTake(ctx->ready_sem, tick_to_wait);
    gpio_intr_disable(ctx->cfg.miso_io);
    gpio_set_level(ctx->cfg.cs_io, 0);

    if (ret != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }
    return ESP_OK;
}

static esp_err_t eeprom_wait_done_by_polling(eeprom_context_t* ctx)
{
    bool timeout = true;
    gpio_set_level(ctx->cfg.cs_io, 1);
    for (int i = 0; i < EEPROM_BUSY_TIMEOUT_MS * 1000; i ++) {
        if (gpio_get_level(ctx->cfg.miso_io)) {
            timeout = false;
            break;
        }
        usleep(1);
    }
    gpio_set_level(ctx->cfg.cs_io, 0);
    if (timeout) {
        return ESP_ERR_TIMEOUT;
    }
    return ESP_OK;
}

static esp_err_t eeprom_wait_done(eeprom_context_t* ctx)
{
    //have to keep cs low for 250ns
    usleep(1);
    esp_err_t ret = ESP_FAIL;
    if (ctx->cfg.intr_used) {
        ret = eeprom_wait_done_by_intr(ctx);
    } else {
        ret = eeprom_wait_done_by_polling(ctx);
    }
    return ret;
}

static void cs_high(spi_transaction_t* t)
{
    ESP_EARLY_LOGV(TAG, "cs high %d.", ((eeprom_context_t*)t->user)->cfg.cs_io);
    gpio_set_level(((eeprom_context_t*)t->user)->cfg.cs_io, 1);
}

static void cs_low(spi_transaction_t* t)
{
    gpio_set_level(((eeprom_context_t*)t->user)->cfg.cs_io, 0);
    ESP_EARLY_LOGV(TAG, "cs low %d.", ((eeprom_context_t*)t->user)->cfg.cs_io);
}

void ready_rising_isr(void* arg)
{
    eeprom_context_t* ctx = (eeprom_context_t*)arg;
    xSemaphoreGive(ctx->ready_sem);
    ESP_EARLY_LOGV(TAG, "ready detected.");
}

esp_err_t spi_eeprom_deinit(eeprom_context_t* ctx)
{
    spi_bus_remove_device(ctx->spi);
    if (ctx->cfg.intr_used) {
        vSemaphoreDelete(ctx->ready_sem);
    }
    free(ctx);
    return ESP_OK;
}

esp_err_t spi_eeprom_init(const eeprom_config_t *cfg, eeprom_context_t** out_ctx)
{
    esp_err_t err = ESP_OK;
    if (cfg->intr_used && cfg->host == SPI1_HOST) {
        ESP_LOGE(TAG, "interrupt cannot be used on SPI1 host.");
        return ESP_ERR_INVALID_ARG;
    }

    eeprom_context_t* ctx = (eeprom_context_t*)malloc(sizeof(eeprom_context_t));
    if (!ctx) {
        return ESP_ERR_NO_MEM;
    }

    *ctx = (eeprom_context_t) {
        .cfg = *cfg,
    };

    spi_device_interface_config_t devcfg = {
        .command_bits = 10,
        .clock_speed_hz = EEPROM_CLK_FREQ,
        .mode = 0,          //SPI mode 0
        /*
         * The timing requirements to read the busy signal from the EEPROM cannot be easily emulated
         * by SPI transactions. We need to control CS pin by SW to check the busy signal manually.
         */
        .spics_io_num = -1,
        .queue_size = 1,
        .flags = SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_POSITIVE_CS,
        .pre_cb = cs_high,
        .post_cb = cs_low,
        .input_delay_ns = EEPROM_INPUT_DELAY_NS,  //the EEPROM output the data half a SPI clock behind.
    };
    //Attach the EEPROM to the SPI bus
    err = spi_bus_add_device(ctx->cfg.host, &devcfg, &ctx->spi);
    if (err != ESP_OK) {
        goto cleanup;
    }

    gpio_set_level(ctx->cfg.cs_io, 0);
    gpio_config_t cs_cfg = {
        .pin_bit_mask = BIT64(ctx->cfg.cs_io),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&cs_cfg);

    if (ctx->cfg.intr_used) {
        ctx->ready_sem = xSemaphoreCreateBinary();
        if (ctx->ready_sem == NULL) {
            err = ESP_ERR_NO_MEM;
            goto cleanup;
        }

        gpio_set_intr_type(ctx->cfg.miso_io, GPIO_INTR_POSEDGE);
        err = gpio_isr_handler_add(ctx->cfg.miso_io, ready_rising_isr, ctx);
        if (err != ESP_OK) {
            goto cleanup;
        }
        gpio_intr_disable(ctx->cfg.miso_io);
    }
    *out_ctx = ctx;
    return ESP_OK;

cleanup:
    if (ctx->spi) {
        spi_bus_remove_device(ctx->spi);
        ctx->spi = NULL;
    }
    if (ctx->ready_sem) {
        vSemaphoreDelete(ctx->ready_sem);
        ctx->ready_sem = NULL;
    }
    free(ctx);
    return err;
}

esp_err_t spi_eeprom_read(eeprom_context_t* ctx, uint8_t addr, uint8_t* out_data)
{
    spi_transaction_t t = {
        .cmd = CMD_READ | (addr & ADDR_MASK),
        .rxlength = 8,
        .flags = SPI_TRANS_USE_RXDATA,
        .user = ctx,
    };
    esp_err_t err = spi_device_polling_transmit(ctx->spi, &t);
    if (err != ESP_OK) {
        return err;
    }

    *out_data = t.rx_data[0];
    return ESP_OK;
}

esp_err_t spi_eeprom_erase(eeprom_context_t* ctx, uint8_t addr)
{
    esp_err_t err;
    err = spi_device_acquire_bus(ctx->spi, portMAX_DELAY);
    if (err != ESP_OK) {
        return err;
    }

    err = eeprom_simple_cmd(ctx, CMD_ERASE | (addr & ADDR_MASK));

    if (err == ESP_OK) {
        err = eeprom_wait_done(ctx);
    }

    spi_device_release_bus(ctx->spi);
    return err;
}

esp_err_t spi_eeprom_write(eeprom_context_t* ctx, uint8_t addr, uint8_t data)
{
    esp_err_t err;
    err = spi_device_acquire_bus(ctx->spi, portMAX_DELAY);
    if (err != ESP_OK) {
        return err;
    }

    spi_transaction_t t = {
        .cmd = CMD_WRITE | (addr & ADDR_MASK),
        .length = 8,
        .flags = SPI_TRANS_USE_TXDATA,
        .tx_data = {data},
        .user = ctx,
    };
    err = spi_device_polling_transmit(ctx->spi, &t);

    if (err == ESP_OK) {
        err = eeprom_wait_done(ctx);
    }

    spi_device_release_bus(ctx->spi);
    return err;
}

esp_err_t spi_eeprom_write_enable(eeprom_context_t* ctx)
{
    return eeprom_simple_cmd(ctx, CMD_EWEN | ADD_EWEN);
}

esp_err_t spi_eeprom_write_disable(eeprom_context_t* ctx)
{
    return eeprom_simple_cmd(ctx, CMD_EWDS | ADD_EWDS);
}

esp_err_t spi_eeprom_erase_all(eeprom_context_t* ctx)
{
#if !CONFIG_EXAMPLE_5V_COMMANDS
    //not supported in 3.3V VCC
    ESP_LOGE(TAG, "erase all not supported by EEPROM under 3.3V VCC");
    return ESP_ERR_NOT_SUPPORTED;
#endif

    esp_err_t err;
    err = spi_device_acquire_bus(ctx->spi, portMAX_DELAY);
    if (err != ESP_OK) {
        return err;
    }

    err = eeprom_simple_cmd(ctx, CMD_ERAL | ADD_ERAL);

    if (err == ESP_OK) {
        err = eeprom_wait_done(ctx);
    }

    spi_device_release_bus(ctx->spi);
    return err;
}

esp_err_t spi_eeprom_write_all(eeprom_context_t* ctx, uint8_t data)
{
#if !CONFIG_EXAMPLE_5V_COMMANDS
    //not supported in 3.3V VCC
    ESP_LOGE(TAG, "write all not supported by EEPROM under 3.3V VCC");
    return ESP_ERR_NOT_SUPPORTED;
#endif

    esp_err_t err;
    err = spi_device_acquire_bus(ctx->spi, portMAX_DELAY);
    if (err != ESP_OK) {
        return err;
    }

    spi_transaction_t t = {
        .cmd = CMD_WRAL | ADD_WRAL,
        .length = 8,
        .flags = SPI_TRANS_USE_TXDATA,
        .tx_data = {data},
        .user = ctx,
    };
    err = spi_device_polling_transmit(ctx->spi, &t);

    if (err == ESP_OK) {
        err = eeprom_wait_done(ctx);
    }

    spi_device_release_bus(ctx->spi);
    return err;
}
