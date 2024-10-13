/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_check.h"
#include "esp_intr_alloc.h"
#include "esp_private/usb_console.h"
#include "esp_private/system_internal.h"
#include "esp_private/startup_internal.h"
#include "soc/periph_defs.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/usb_struct.h"
#include "soc/usb_reg.h"
#include "hal/soc_hal.h"
#include "esp_rom_uart.h"
#include "esp_rom_sys.h"
#include "esp_rom_caps.h"
#ifdef CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/usb/usb_dc.h"
#include "esp32s2/rom/usb/cdc_acm.h"
#include "esp32s2/rom/usb/usb_dfu.h"
#include "esp32s2/rom/usb/usb_device.h"
#include "esp32s2/rom/usb/usb_os_glue.h"
#include "esp32s2/rom/usb/usb_persist.h"
#include "esp32s2/rom/usb/chip_usb_dw_wrapper.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/usb/usb_dc.h"
#include "esp32s3/rom/usb/cdc_acm.h"
#include "esp32s3/rom/usb/usb_dfu.h"
#include "esp32s3/rom/usb/usb_device.h"
#include "esp32s3/rom/usb/usb_os_glue.h"
#include "esp32s3/rom/usb/usb_persist.h"
#include "esp32s3/rom/usb/chip_usb_dw_wrapper.h"
#endif

#define CDC_WORK_BUF_SIZE (ESP_ROM_CDC_ACM_WORK_BUF_MIN + CONFIG_ESP_CONSOLE_USB_CDC_RX_BUF_SIZE)

typedef enum {
    REBOOT_NONE,
    REBOOT_NORMAL,
    REBOOT_BOOTLOADER,
    REBOOT_BOOTLOADER_DFU,
} reboot_type_t;

static reboot_type_t s_queue_reboot = REBOOT_NONE;
static int s_prev_rts_state;
static intr_handle_t s_usb_int_handle;
static cdc_acm_device *s_cdc_acm_device;
static char s_usb_tx_buf[ACM_BYTES_PER_TX];
static size_t s_usb_tx_buf_pos;
static uint8_t cdcmem[CDC_WORK_BUF_SIZE];
static esp_usb_console_cb_t s_rx_cb;
static esp_usb_console_cb_t s_tx_cb;
static void *s_cb_arg;
static esp_timer_handle_t s_restart_timer;

static const char* TAG = "usb_console";

/* This lock is used for two purposes:
 * - To protect functions which write something to USB, e.g. esp_usb_console_write_buf.
 *   This is necessary since these functions may be called by esp_rom_printf, so the calls
 *   may preempt each other or happen concurrently.
 *   (The calls coming from regular 'printf', i.e. via VFS layer, are already protected
 *   by a mutex in the VFS driver.)
 * - To implement "osglue" functions of the USB stack. These normally require interrupts
 *   to be disabled. However on multi-core chips a critical section is necessary.
 */
static portMUX_TYPE s_lock = portMUX_INITIALIZER_UNLOCKED;
#ifdef CONFIG_ESP_CONSOLE_USB_CDC_SUPPORT_ETS_PRINTF
void esp_usb_console_write_char(char c);
#define ISR_FLAG  ESP_INTR_FLAG_IRAM
#else
#define ISR_FLAG  0
#endif // CONFIG_ESP_CONSOLE_USB_CDC_SUPPORT_ETS_PRINTF

/* Optional write lock routines; used only if esp_rom_printf output via CDC is enabled */
static inline void write_lock_acquire(void);
static inline void write_lock_release(void);

/* Other forward declarations */
void esp_usb_console_before_restart(void);

/* Called by ROM to disable the interrupts
 * Non-static to allow placement into IRAM by ldgen.
 */
void esp_usb_console_osglue_dis_int(void)
{
    portENTER_CRITICAL_SAFE(&s_lock);
}

/* Called by ROM to enable the interrupts
 * Non-static to allow placement into IRAM by ldgen.
 */
void esp_usb_console_osglue_ena_int(void)
{
    portEXIT_CRITICAL_SAFE(&s_lock);
}

/* Delay function called by ROM USB driver.
 * Non-static to allow placement into IRAM by ldgen.
 */
int esp_usb_console_osglue_wait_proc(int delay_us)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING ||
            !xPortCanYield()) {
        esp_rom_delay_us(delay_us);
        return delay_us;
    }
    if (delay_us == 0) {
        /* We should effectively yield */
        vPortYield();
        return 1;
    } else {
        /* Just delay */
        int ticks = MAX(delay_us / (portTICK_PERIOD_MS * 1000), 1);
        vTaskDelay(ticks);
        return ticks * portTICK_PERIOD_MS * 1000;
    }
}

/* Called by ROM CDC ACM driver from interrupt context./
 * Non-static to allow placement into IRAM by ldgen.
 */
void esp_usb_console_cdc_acm_cb(cdc_acm_device *dev, int status)
{
    if (status == USB_DC_RESET || status == USB_DC_CONNECTED) {
        s_prev_rts_state = 0;
    } else if (status == ACM_STATUS_LINESTATE_CHANGED) {
        uint32_t rts, dtr;
        cdc_acm_line_ctrl_get(dev, LINE_CTRL_RTS, &rts);
        cdc_acm_line_ctrl_get(dev, LINE_CTRL_DTR, &dtr);
        if (!rts && s_prev_rts_state) {
            if (dtr) {
                s_queue_reboot = REBOOT_BOOTLOADER;
            } else {
                s_queue_reboot = REBOOT_NORMAL;
            }
        }
        s_prev_rts_state = rts;
    } else if (status == ACM_STATUS_RX && s_rx_cb) {
        (*s_rx_cb)(s_cb_arg);
    } else if (status == ACM_STATUS_TX && s_tx_cb) {
        (*s_tx_cb)(s_cb_arg);
    }
}

/* Non-static to allow placement into IRAM by ldgen. */
void esp_usb_console_dfu_detach_cb(int timeout)
{
    s_queue_reboot = REBOOT_BOOTLOADER_DFU;
}

/* USB interrupt handler, forward the call to the ROM driver.
 * Non-static to allow placement into IRAM by ldgen.
 */
void esp_usb_console_interrupt(void *arg)
{
    usb_dc_check_poll_for_interrupts();
    /* Restart can be requested from esp_usb_console_cdc_acm_cb or esp_usb_console_dfu_detach_cb */
    if (s_queue_reboot != REBOOT_NONE) {
        /* We can't call esp_restart here directly, since this function is called from an ISR.
         * Instead, start an esp_timer and call esp_restart from the callback.
         */
        esp_err_t err = ESP_FAIL;
        if (s_restart_timer) {
            /* In case the timer is already running, stop it. No error check since this will fail if
             * the timer is not running.
             */
            esp_timer_stop(s_restart_timer);
            /* Start the timer again. 50ms seems to be not too long for the user to notice, but
             * enough for the USB console output to be flushed.
             */
            const int restart_timeout_us = 50 * 1000;
            err = esp_timer_start_once(s_restart_timer, restart_timeout_us);
        }
        if (err != ESP_OK) {
            /* Can't schedule a restart for some reason? Call the "no-OS" restart function directly. */
            esp_usb_console_before_restart();
            esp_restart_noos();
        }
    }
}

/* Called as esp_timer callback when the restart timeout expires.
 * Non-static to allow placement into IRAM by ldgen.
 */
void esp_usb_console_on_restart_timeout(void *arg)
{
    esp_restart();
}

/* Call the USB interrupt handler while any interrupts are pending,
 * but not more than a few times.
 * Non-static to allow placement into IRAM by ldgen.
 */
void esp_usb_console_poll_interrupts(void)
{
    const int max_poll_count = 10;
    for (int i = 0; (USB0.gintsts & USB0.gintmsk) != 0 && i < max_poll_count; i++) {
        usb_dc_check_poll_for_interrupts();
    }
}

/* This function gets registered as a restart handler.
 * Prepares USB peripheral for restart and sets up persistence.
 * Non-static to allow placement into IRAM by ldgen.
 */
void esp_usb_console_before_restart(void)
{
    esp_usb_console_poll_interrupts();
    usb_dc_prepare_persist();
    if (s_queue_reboot == REBOOT_BOOTLOADER) {
        chip_usb_set_persist_flags(USBDC_PERSIST_ENA);
        REG_WRITE(RTC_CNTL_OPTION1_REG, RTC_CNTL_FORCE_DOWNLOAD_BOOT);
    } else if (s_queue_reboot == REBOOT_BOOTLOADER_DFU) {
        chip_usb_set_persist_flags(USBDC_BOOT_DFU);
        REG_WRITE(RTC_CNTL_OPTION1_REG, RTC_CNTL_FORCE_DOWNLOAD_BOOT);
    } else {
        chip_usb_set_persist_flags(USBDC_PERSIST_ENA);
        esp_usb_console_poll_interrupts();
    }
}

/* Reset some static state in ROM, which survives when going from the
 * 2nd stage bootloader into the app. This cleans some variables which
 * indicates that the driver is already initialized, allowing us to
 * initialize it again, in the app.
 */
static void esp_usb_console_rom_cleanup(void)
{
    usb_dev_deinit();
    usb_dw_ctrl_deinit();
    uart_acm_dev = NULL;
}

esp_err_t esp_usb_console_init(void)
{
    esp_err_t err;
    err = esp_register_shutdown_handler(esp_usb_console_before_restart);
    if (err != ESP_OK) {
        return err;
    }

    esp_usb_console_rom_cleanup();

    /* Install OS hooks */
    rom_usb_osglue.int_dis_proc = esp_usb_console_osglue_dis_int;
    rom_usb_osglue.int_ena_proc = esp_usb_console_osglue_ena_int;
    rom_usb_osglue.wait_proc = esp_usb_console_osglue_wait_proc;

    /* Install interrupt.
     * In case of ESP_CONSOLE_USB_CDC_SUPPORT_ETS_PRINTF:
     *   Note that this the interrupt handler has to be placed into IRAM because
     *   the interrupt handler can also be called in polling mode, when
     *   interrupts are disabled, and a write to USB is performed with cache disabled.
     *   Since the handler function is in IRAM, we can register the interrupt as IRAM capable.
     *   It is not because we actually need the interrupt to work with cache disabled!
     */
    err = esp_intr_alloc(ETS_USB_INTR_SOURCE, ISR_FLAG | ESP_INTR_FLAG_INTRDISABLED,
                         esp_usb_console_interrupt, NULL, &s_usb_int_handle);
    if (err != ESP_OK) {
        esp_unregister_shutdown_handler(esp_usb_console_before_restart);
        return err;
    }

    /* Initialize USB / CDC */
    s_cdc_acm_device = cdc_acm_init(cdcmem, CDC_WORK_BUF_SIZE);
    usb_dc_check_poll_for_interrupts();

    /* Set callback for handling DTR/RTS lines and TX/RX events */
    cdc_acm_irq_callback_set(s_cdc_acm_device, esp_usb_console_cdc_acm_cb);
    cdc_acm_irq_state_enable(s_cdc_acm_device);

    /* Set callback for handling DFU detach */
    usb_dfu_set_detach_cb(esp_usb_console_dfu_detach_cb);

    /* Enable interrupts on USB peripheral side */
    USB0.gahbcfg |= USB_GLBLLNTRMSK_M;

    /* Enable the interrupt handler */
    esp_intr_enable(s_usb_int_handle);

#ifdef CONFIG_ESP_CONSOLE_USB_CDC_SUPPORT_ETS_PRINTF
    /* Install esp_rom_printf handler */
    esp_rom_output_set_as_console(ESP_ROM_USB_OTG_NUM);
    esp_rom_install_channel_putc(1, &esp_usb_console_write_char);
#endif // CONFIG_ESP_CONSOLE_USB_CDC_SUPPORT_ETS_PRINTF

    return ESP_OK;
}

/* This function runs as part of the startup code to initialize the restart timer.
 * This is not done as part of esp_usb_console_init since that function is called
 * too early, before esp_timer is fully initialized.
 * This gets called a bit later in the process when we can already register a timer.
 */
ESP_SYSTEM_INIT_FN(esp_usb_console_init_restart_timer, SECONDARY, BIT(0), 220)
{
    esp_timer_create_args_t timer_create_args = {
        .callback = &esp_usb_console_on_restart_timeout,
        .name = "usb_console_restart"
    };
    ESP_RETURN_ON_ERROR(esp_timer_create(&timer_create_args, &s_restart_timer), TAG, "failed to create the restart timer");
    return ESP_OK;
}

/* Non-static to allow placement into IRAM by ldgen.
 * Must be called with the write lock held.
 */
ssize_t esp_usb_console_flush_internal(size_t last_write_size)
{
    if (s_usb_tx_buf_pos == 0) {
        return 0;
    }
    assert(s_usb_tx_buf_pos >= last_write_size);
    ssize_t ret;
    size_t tx_buf_pos_before = s_usb_tx_buf_pos - last_write_size;
    size_t sent = cdc_acm_fifo_fill(s_cdc_acm_device, (const uint8_t*) s_usb_tx_buf, s_usb_tx_buf_pos);
    if (sent == last_write_size) {
        /* everything was sent */
        ret = last_write_size;
        s_usb_tx_buf_pos = 0;
    } else if (sent == 0) {
        /* nothing was sent, roll back to the original state */
        ret = 0;
        s_usb_tx_buf_pos = tx_buf_pos_before;
    } else {
        /* Some data was sent, but not all of the buffer.
         * We can still tell the caller that all the new data
         * was "sent" since it is in the buffer now.
         */
        ret = last_write_size;
        memmove(s_usb_tx_buf, s_usb_tx_buf + sent, s_usb_tx_buf_pos - sent);
        s_usb_tx_buf_pos = s_usb_tx_buf_pos - sent;
    }
    return ret;
}

ssize_t esp_usb_console_flush(void)
{
    if (s_cdc_acm_device == NULL) {
        return -1;
    }
    write_lock_acquire();
    int ret = esp_usb_console_flush_internal(0);
    write_lock_release();
    return ret;
}

ssize_t esp_usb_console_write_buf(const char* buf, size_t size)
{
    if (s_cdc_acm_device == NULL) {
        return -1;
    }
    if (size == 0) {
        return 0;
    }
    write_lock_acquire();
    ssize_t tx_buf_available = ACM_BYTES_PER_TX - s_usb_tx_buf_pos;
    ssize_t will_write = MIN(size, tx_buf_available);
    memcpy(s_usb_tx_buf + s_usb_tx_buf_pos, buf, will_write);
    s_usb_tx_buf_pos += will_write;

    ssize_t ret;
    if (s_usb_tx_buf_pos == ACM_BYTES_PER_TX || buf[size - 1] == '\n') {
        /* Buffer is full, or a newline is found.
         * For binary streams, we probably shouldn't do line buffering,
         * but text streams are likely going to be the most common case.
         */
        ret = esp_usb_console_flush_internal(will_write);
    } else {
        /* nothing sent out yet, but all the new data is in the buffer now */
        ret = will_write;
    }
    write_lock_release();
    return ret;
}

ssize_t esp_usb_console_read_buf(char *buf, size_t buf_size)
{
    if (s_cdc_acm_device == NULL) {
        return -1;
    }
    if (esp_usb_console_available_for_read() == 0) {
        return 0;
    }
    int bytes_read = cdc_acm_fifo_read(s_cdc_acm_device, (uint8_t*) buf, buf_size);
    return bytes_read;
}

esp_err_t esp_usb_console_set_cb(esp_usb_console_cb_t rx_cb, esp_usb_console_cb_t tx_cb, void *arg)
{
    if (s_cdc_acm_device == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    s_rx_cb = rx_cb;
    if (s_rx_cb) {
        cdc_acm_irq_rx_enable(s_cdc_acm_device);
    } else {
        cdc_acm_irq_rx_disable(s_cdc_acm_device);
    }
    s_tx_cb = tx_cb;
    if (s_tx_cb) {
        cdc_acm_irq_tx_enable(s_cdc_acm_device);
    } else {
        cdc_acm_irq_tx_disable(s_cdc_acm_device);
    }
    s_cb_arg = arg;
    return ESP_OK;
}

ssize_t esp_usb_console_available_for_read(void)
{
    if (s_cdc_acm_device == NULL) {
        return -1;
    }
    return cdc_acm_rx_fifo_cnt(s_cdc_acm_device);
}

bool esp_usb_console_write_available(void)
{
    if (s_cdc_acm_device == NULL) {
        return false;
    }
    return cdc_acm_irq_tx_ready(s_cdc_acm_device) != 0;
}

#ifdef CONFIG_ESP_CONSOLE_USB_CDC_SUPPORT_ETS_PRINTF
/* Used as an output function by esp_rom_printf.
 * The LF->CRLF replacement logic replicates the one in esp_rom_uart_putc.
 * Not static to allow placement into IRAM by ldgen.
 */
void esp_usb_console_write_char(char c)
{
    char cr = '\r';
    char lf = '\n';

    if (c == lf) {
        esp_usb_console_write_buf(&cr, 1);
        esp_usb_console_write_buf(&lf, 1);
    } else if (c == '\r') {
    } else {
        esp_usb_console_write_buf(&c, 1);
    }
}
static inline void write_lock_acquire(void)
{
    portENTER_CRITICAL_SAFE(&s_lock);
}
static inline void write_lock_release(void)
{
    portEXIT_CRITICAL_SAFE(&s_lock);
}

#else // CONFIG_ESP_CONSOLE_USB_CDC_SUPPORT_ETS_PRINTF

static inline void write_lock_acquire(void)
{
}

static inline void write_lock_release(void)
{
}
#endif // CONFIG_ESP_CONSOLE_USB_CDC_SUPPORT_ETS_PRINTF
