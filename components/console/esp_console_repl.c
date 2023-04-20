/*
 * SPDX-FileCopyrightText: 2016-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/cdefs.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "esp_vfs_cdcacm.h"
#include "esp_vfs_usb_serial_jtag.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/usb_serial_jtag.h"
#include "linenoise/linenoise.h"

static const char *TAG = "console.repl";

#define CONSOLE_PROMPT_MAX_LEN (32)
#define CONSOLE_PATH_MAX_LEN   (ESP_VFS_PATH_MAX)

typedef enum {
    CONSOLE_REPL_STATE_DEINIT,
    CONSOLE_REPL_STATE_INIT,
    CONSOLE_REPL_STATE_START,
} repl_state_t;

typedef struct {
    esp_console_repl_t repl_core;        // base class
    char prompt[CONSOLE_PROMPT_MAX_LEN]; // Prompt to be printed before each line
    repl_state_t state;
    const char *history_save_path;
    TaskHandle_t task_hdl;              // REPL task handle
    size_t max_cmdline_length;          // Maximum length of a command line. If 0, default value will be used.
} esp_console_repl_com_t;

typedef struct {
    esp_console_repl_com_t repl_com; // base class
    int uart_channel;                // uart channel number
} esp_console_repl_universal_t;

static void esp_console_repl_task(void *args);
#if CONFIG_ESP_CONSOLE_UART_DEFAULT || CONFIG_ESP_CONSOLE_UART_CUSTOM
static esp_err_t esp_console_repl_uart_delete(esp_console_repl_t *repl);
#endif // CONFIG_ESP_CONSOLE_UART_DEFAULT || CONFIG_ESP_CONSOLE_UART_CUSTOM
#if CONFIG_ESP_CONSOLE_USB_CDC
static esp_err_t esp_console_repl_usb_cdc_delete(esp_console_repl_t *repl);
#endif // CONFIG_ESP_CONSOLE_USB_CDC
#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
static esp_err_t esp_console_repl_usb_serial_jtag_delete(esp_console_repl_t *repl);
#endif //CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
static esp_err_t esp_console_common_init(size_t max_cmdline_length, esp_console_repl_com_t *repl_com);
static esp_err_t esp_console_setup_prompt(const char *prompt, esp_console_repl_com_t *repl_com);
static esp_err_t esp_console_setup_history(const char *history_path, uint32_t max_history_len, esp_console_repl_com_t *repl_com);

#if CONFIG_ESP_CONSOLE_USB_CDC
esp_err_t esp_console_new_repl_usb_cdc(const esp_console_dev_usb_cdc_config_t *dev_config, const esp_console_repl_config_t *repl_config, esp_console_repl_t **ret_repl)
{
    esp_err_t ret = ESP_OK;
    esp_console_repl_universal_t *cdc_repl = NULL;
    if (!repl_config | !dev_config | !ret_repl) {
        ret = ESP_ERR_INVALID_ARG;
        goto _exit;
    }
    // allocate memory for console REPL context
    cdc_repl = calloc(1, sizeof(esp_console_repl_universal_t));
    if (!cdc_repl) {
        ret = ESP_ERR_NO_MEM;
        goto _exit;
    }

    /* Minicom, screen, idf_monitor send CR when ENTER key is pressed */
    esp_vfs_dev_cdcacm_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    esp_vfs_dev_cdcacm_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

    /* Enable blocking mode on stdin and stdout */
    fcntl(fileno(stdout), F_SETFL, 0);
    fcntl(fileno(stdin), F_SETFL, 0);

    // initialize console, common part
    ret = esp_console_common_init(repl_config->max_cmdline_length, &cdc_repl->repl_com);
    if (ret != ESP_OK) {
        goto _exit;
    }

    // setup history
    ret = esp_console_setup_history(repl_config->history_save_path, repl_config->max_history_len, &cdc_repl->repl_com);
    if (ret != ESP_OK) {
        goto _exit;
    }

    // setup prompt
    esp_console_setup_prompt(repl_config->prompt, &cdc_repl->repl_com);

    /* Fill the structure here as it will be used directly by the created task. */
    cdc_repl->uart_channel = CONFIG_ESP_CONSOLE_UART_NUM;
    cdc_repl->repl_com.state = CONSOLE_REPL_STATE_INIT;
    cdc_repl->repl_com.repl_core.del = esp_console_repl_usb_cdc_delete;

    /* spawn a single thread to run REPL */
    if (xTaskCreate(esp_console_repl_task, "console_repl", repl_config->task_stack_size,
                    cdc_repl, repl_config->task_priority, &cdc_repl->repl_com.task_hdl) != pdTRUE) {
        ret = ESP_FAIL;
        goto _exit;
    }

    *ret_repl = &cdc_repl->repl_com.repl_core;
    return ESP_OK;
_exit:
    if (cdc_repl) {
        esp_console_deinit();
        free(cdc_repl);
    }
    if (ret_repl) {
        *ret_repl = NULL;
    }
    return ret;
}
#endif // CONFIG_ESP_CONSOLE_USB_CDC

#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
esp_err_t esp_console_new_repl_usb_serial_jtag(const esp_console_dev_usb_serial_jtag_config_t *dev_config, const esp_console_repl_config_t *repl_config, esp_console_repl_t **ret_repl)
{
    esp_console_repl_universal_t *usb_serial_jtag_repl = NULL;
    if (!repl_config | !dev_config | !ret_repl) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;
    // allocate memory for console REPL context
    usb_serial_jtag_repl = calloc(1, sizeof(esp_console_repl_universal_t));
    if (!usb_serial_jtag_repl) {
        ret = ESP_ERR_NO_MEM;
        goto _exit;
    }

    /* Minicom, screen, idf_monitor send CR when ENTER key is pressed */
    esp_vfs_dev_usb_serial_jtag_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    esp_vfs_dev_usb_serial_jtag_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

    /* Enable blocking mode on stdin and stdout */
    fcntl(fileno(stdout), F_SETFL, 0);
    fcntl(fileno(stdin), F_SETFL, 0);

    usb_serial_jtag_driver_config_t usb_serial_jtag_config = USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT();

    /* Install USB-SERIAL-JTAG driver for interrupt-driven reads and writes */
    ret = usb_serial_jtag_driver_install(&usb_serial_jtag_config);
    if (ret != ESP_OK) {
        goto _exit;
    }

    // initialize console, common part
    ret = esp_console_common_init(repl_config->max_cmdline_length, &usb_serial_jtag_repl->repl_com);
    if (ret != ESP_OK) {
        goto _exit;
    }

    /* Tell vfs to use usb-serial-jtag driver */
    esp_vfs_usb_serial_jtag_use_driver();

    // setup history
    ret = esp_console_setup_history(repl_config->history_save_path, repl_config->max_history_len, &usb_serial_jtag_repl->repl_com);
    if (ret != ESP_OK) {
        goto _exit;
    }

    // setup prompt
    esp_console_setup_prompt(repl_config->prompt, &usb_serial_jtag_repl->repl_com);

    /* Fill the structure here as it will be used directly by the created task. */
    usb_serial_jtag_repl->uart_channel = CONFIG_ESP_CONSOLE_UART_NUM;
    usb_serial_jtag_repl->repl_com.state = CONSOLE_REPL_STATE_INIT;
    usb_serial_jtag_repl->repl_com.repl_core.del = esp_console_repl_usb_serial_jtag_delete;

    /* spawn a single thread to run REPL */
    if (xTaskCreate(esp_console_repl_task, "console_repl", repl_config->task_stack_size,
                    usb_serial_jtag_repl, repl_config->task_priority, &usb_serial_jtag_repl->repl_com.task_hdl) != pdTRUE) {
        ret = ESP_FAIL;
        goto _exit;
    }

    *ret_repl = &usb_serial_jtag_repl->repl_com.repl_core;
    return ESP_OK;
_exit:
    if (usb_serial_jtag_repl) {
        esp_console_deinit();
        free(usb_serial_jtag_repl);
    }
    if (ret_repl) {
        *ret_repl = NULL;
    }
    return ret;
}
#endif // CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG

#if CONFIG_ESP_CONSOLE_UART_DEFAULT || CONFIG_ESP_CONSOLE_UART_CUSTOM
esp_err_t esp_console_new_repl_uart(const esp_console_dev_uart_config_t *dev_config, const esp_console_repl_config_t *repl_config, esp_console_repl_t **ret_repl)
{
    esp_err_t ret = ESP_OK;
    esp_console_repl_universal_t *uart_repl = NULL;
    if (!repl_config | !dev_config | !ret_repl) {
        ret = ESP_ERR_INVALID_ARG;
        goto _exit;
    }
    // allocate memory for console REPL context
    uart_repl = calloc(1, sizeof(esp_console_repl_universal_t));
    if (!uart_repl) {
        ret = ESP_ERR_NO_MEM;
        goto _exit;
    }

    /* Drain stdout before reconfiguring it */
    fflush(stdout);
    fsync(fileno(stdout));

    /* Minicom, screen, idf_monitor send CR when ENTER key is pressed */
    esp_vfs_dev_uart_port_set_rx_line_endings(dev_config->channel, ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    esp_vfs_dev_uart_port_set_tx_line_endings(dev_config->channel, ESP_LINE_ENDINGS_CRLF);

    /* Configure UART. Note that REF_TICK/XTAL is used so that the baud rate remains
     * correct while APB frequency is changing in light sleep mode.
     */
#if SOC_UART_SUPPORT_REF_TICK
    uart_sclk_t clk_source = UART_SCLK_REF_TICK;
    // REF_TICK clock can't provide a high baudrate
    if (dev_config->baud_rate > 1 * 1000 * 1000) {
        clk_source = UART_SCLK_DEFAULT;
        ESP_LOGW(TAG, "light sleep UART wakeup might not work at the configured baud rate");
    }
#elif SOC_UART_SUPPORT_XTAL_CLK
    uart_sclk_t clk_source = UART_SCLK_XTAL;
#else
    #error "No UART clock source is aware of DFS"
#endif // SOC_UART_SUPPORT_xxx
    const uart_config_t uart_config = {
        .baud_rate = dev_config->baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .source_clk = clk_source,
    };

    uart_param_config(dev_config->channel, &uart_config);
    uart_set_pin(dev_config->channel, dev_config->tx_gpio_num, dev_config->rx_gpio_num, -1, -1);

    /* Install UART driver for interrupt-driven reads and writes */
    ret = uart_driver_install(dev_config->channel, 256, 0, 0, NULL, 0);
    if (ret != ESP_OK) {
        goto _exit;
    }

    /* Tell VFS to use UART driver */
    esp_vfs_dev_uart_use_driver(dev_config->channel);

    // initialize console, common part
    ret = esp_console_common_init(repl_config->max_cmdline_length, &uart_repl->repl_com);
    if (ret != ESP_OK) {
        goto _exit;
    }

    // setup history
    ret = esp_console_setup_history(repl_config->history_save_path, repl_config->max_history_len, &uart_repl->repl_com);
    if (ret != ESP_OK) {
        goto _exit;
    }

    // setup prompt
    esp_console_setup_prompt(repl_config->prompt, &uart_repl->repl_com);

    /* Fill the structure here as it will be used directly by the created task. */
    uart_repl->uart_channel = dev_config->channel;
    uart_repl->repl_com.state = CONSOLE_REPL_STATE_INIT;
    uart_repl->repl_com.repl_core.del = esp_console_repl_uart_delete;

    /* Spawn a single thread to run REPL, we need to pass `uart_repl` to it as
     * it also requires the uart channel. */
    if (xTaskCreate(esp_console_repl_task, "console_repl", repl_config->task_stack_size,
                    uart_repl, repl_config->task_priority, &uart_repl->repl_com.task_hdl) != pdTRUE) {
        ret = ESP_FAIL;
        goto _exit;
    }

    *ret_repl = &uart_repl->repl_com.repl_core;
    return ESP_OK;
_exit:
    if (uart_repl) {
        esp_console_deinit();
        uart_driver_delete(dev_config->channel);
        free(uart_repl);
    }
    if (ret_repl) {
        *ret_repl = NULL;
    }
    return ret;
}
#endif // CONFIG_ESP_CONSOLE_UART_DEFAULT || CONFIG_ESP_CONSOLE_UART_CUSTOM

esp_err_t esp_console_start_repl(esp_console_repl_t *repl)
{
    esp_err_t ret = ESP_OK;
    esp_console_repl_com_t *repl_com = __containerof(repl, esp_console_repl_com_t, repl_core);
    // check if already initialized
    if (repl_com->state != CONSOLE_REPL_STATE_INIT) {
        ret = ESP_ERR_INVALID_STATE;
        goto _exit;
    }

    repl_com->state = CONSOLE_REPL_STATE_START;
    xTaskNotifyGive(repl_com->task_hdl);
    return ESP_OK;
_exit:
    return ret;
}

static esp_err_t esp_console_setup_prompt(const char *prompt, esp_console_repl_com_t *repl_com)
{
    /* set command line prompt */
    const char *prompt_temp = "esp>";
    if (prompt) {
        prompt_temp = prompt;
    }
    snprintf(repl_com->prompt, CONSOLE_PROMPT_MAX_LEN - 1, LOG_COLOR_I "%s " LOG_RESET_COLOR, prompt_temp);

    /* Figure out if the terminal supports escape sequences */
    int probe_status = linenoiseProbe();
    if (probe_status) {
        /* zero indicates success */
        linenoiseSetDumbMode(1);
#if CONFIG_LOG_COLORS
        /* Since the terminal doesn't support escape sequences,
         * don't use color codes in the s_prompt.
         */
        snprintf(repl_com->prompt, CONSOLE_PROMPT_MAX_LEN - 1, "%s ", prompt_temp);
#endif //CONFIG_LOG_COLORS
    }

    return ESP_OK;
}

static esp_err_t esp_console_setup_history(const char *history_path, uint32_t max_history_len, esp_console_repl_com_t *repl_com)
{
    esp_err_t ret = ESP_OK;

    repl_com->history_save_path = history_path;
    if (history_path) {
        /* Load command history from filesystem */
        linenoiseHistoryLoad(history_path);
    }

    /* Set command history size */
    if (linenoiseHistorySetMaxLen(max_history_len) != 1) {
        ESP_LOGE(TAG, "set max history length to %"PRIu32" failed", max_history_len);
        ret = ESP_FAIL;
        goto _exit;
    }
    return ESP_OK;
_exit:
    return ret;
}

static esp_err_t esp_console_common_init(size_t max_cmdline_length, esp_console_repl_com_t *repl_com)
{
    esp_err_t ret = ESP_OK;
    /* Initialize the console */
    esp_console_config_t console_config = ESP_CONSOLE_CONFIG_DEFAULT();
    repl_com->max_cmdline_length = console_config.max_cmdline_length;
    /* Replace the default command line length if passed as a parameter */
    if (max_cmdline_length != 0) {
        console_config.max_cmdline_length = max_cmdline_length;
        repl_com->max_cmdline_length = max_cmdline_length;
    }

#if CONFIG_LOG_COLORS
    console_config.hint_color = atoi(LOG_COLOR_CYAN);
#else
    console_config.hint_color = -1;
#endif
    ret = esp_console_init(&console_config);
    if (ret != ESP_OK) {
        goto _exit;
    }

    ret = esp_console_register_help_command();
    if (ret != ESP_OK) {
        goto _exit;
    }

    /* Configure linenoise line completion library */
    /* Enable multiline editing. If not set, long commands will scroll within single line */
    linenoiseSetMultiLine(1);

    /* Tell linenoise where to get command completions and hints */
    linenoiseSetCompletionCallback(&esp_console_get_completion);
    linenoiseSetHintsCallback((linenoiseHintsCallback *)&esp_console_get_hint);

    return ESP_OK;
_exit:
    return ret;
}

#if CONFIG_ESP_CONSOLE_UART_DEFAULT || CONFIG_ESP_CONSOLE_UART_CUSTOM
static esp_err_t esp_console_repl_uart_delete(esp_console_repl_t *repl)
{
    esp_err_t ret = ESP_OK;
    esp_console_repl_com_t *repl_com = __containerof(repl, esp_console_repl_com_t, repl_core);
    esp_console_repl_universal_t *uart_repl = __containerof(repl_com, esp_console_repl_universal_t, repl_com);
    // check if already de-initialized
    if (repl_com->state == CONSOLE_REPL_STATE_DEINIT) {
        ESP_LOGE(TAG, "already de-initialized");
        ret = ESP_ERR_INVALID_STATE;
        goto _exit;
    }
    repl_com->state = CONSOLE_REPL_STATE_DEINIT;
    esp_console_deinit();
    esp_vfs_dev_uart_use_nonblocking(uart_repl->uart_channel);
    uart_driver_delete(uart_repl->uart_channel);
    free(uart_repl);
_exit:
    return ret;
}
#endif // CONFIG_ESP_CONSOLE_UART_DEFAULT || CONFIG_ESP_CONSOLE_UART_CUSTOM

#if CONFIG_ESP_CONSOLE_USB_CDC
static esp_err_t esp_console_repl_usb_cdc_delete(esp_console_repl_t *repl)
{
    esp_err_t ret = ESP_OK;
    esp_console_repl_com_t *repl_com = __containerof(repl, esp_console_repl_com_t, repl_core);
    esp_console_repl_universal_t *cdc_repl = __containerof(repl_com, esp_console_repl_universal_t, repl_com);
    // check if already de-initialized
    if (repl_com->state == CONSOLE_REPL_STATE_DEINIT) {
        ESP_LOGE(TAG, "already de-initialized");
        ret = ESP_ERR_INVALID_STATE;
        goto _exit;
    }
    repl_com->state = CONSOLE_REPL_STATE_DEINIT;
    esp_console_deinit();
    free(cdc_repl);
_exit:
    return ret;
}
#endif // CONFIG_ESP_CONSOLE_USB_CDC

#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
static esp_err_t esp_console_repl_usb_serial_jtag_delete(esp_console_repl_t *repl)
{
    esp_err_t ret = ESP_OK;
    esp_console_repl_com_t *repl_com = __containerof(repl, esp_console_repl_com_t, repl_core);
    esp_console_repl_universal_t *usb_serial_jtag_repl = __containerof(repl_com, esp_console_repl_universal_t, repl_com);
    // check if already de-initialized
    if (repl_com->state == CONSOLE_REPL_STATE_DEINIT) {
        ESP_LOGE(TAG, "already de-initialized");
        ret = ESP_ERR_INVALID_STATE;
        goto _exit;
    }
    repl_com->state = CONSOLE_REPL_STATE_DEINIT;
    esp_console_deinit();
    esp_vfs_usb_serial_jtag_use_nonblocking();
    usb_serial_jtag_driver_uninstall();
    free(usb_serial_jtag_repl);
_exit:
    return ret;
}
#endif // CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG

static void esp_console_repl_task(void *args)
{
    esp_console_repl_universal_t *repl_conf = (esp_console_repl_universal_t *) args;
    esp_console_repl_com_t *repl_com = &repl_conf->repl_com;
    const int uart_channel = repl_conf->uart_channel;

    /* Waiting for task notify. This happens when `esp_console_start_repl()`
     * function is called. */
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    /* Change standard input and output of the task if the requested UART is
     * NOT the default one. This block will replace stdin, stdout and stderr.
     */
    if (uart_channel != CONFIG_ESP_CONSOLE_UART_NUM) {
        char path[CONSOLE_PATH_MAX_LEN] = { 0 };
        snprintf(path, CONSOLE_PATH_MAX_LEN, "/dev/uart/%d", uart_channel);

        stdin = fopen(path, "r");
        stdout = fopen(path, "w");
        stderr = stdout;
    }

    /* Disable buffering on stdin of the current task.
     * If the console is ran on a different UART than the default one,
     * buffering shall only be disabled for the current one. */
    setvbuf(stdin, NULL, _IONBF, 0);

    /* This message shall be printed here and not earlier as the stdout
     * has just been set above. */
    printf("\r\n"
           "Type 'help' to get the list of commands.\r\n"
           "Use UP/DOWN arrows to navigate through command history.\r\n"
           "Press TAB when typing command name to auto-complete.\r\n");

    if (linenoiseIsDumbMode()) {
        printf("\r\n"
               "Your terminal application does not support escape sequences.\n\n"
               "Line editing and history features are disabled.\n\n"
               "On Windows, try using Putty instead.\r\n");
    }

    linenoiseSetMaxLineLen(repl_com->max_cmdline_length);
    while (repl_com->state == CONSOLE_REPL_STATE_START) {
        char *line = linenoise(repl_com->prompt);
        if (line == NULL) {
            ESP_LOGD(TAG, "empty line");
            /* Ignore empty lines */
            continue;
        }
        /* Add the command to the history */
        linenoiseHistoryAdd(line);
        /* Save command history to filesystem */
        if (repl_com->history_save_path) {
            linenoiseHistorySave(repl_com->history_save_path);
        }

        /* Try to run the command */
        int ret;
        esp_err_t err = esp_console_run(line, &ret);
        if (err == ESP_ERR_NOT_FOUND) {
            printf("Unrecognized command\n");
        } else if (err == ESP_ERR_INVALID_ARG) {
            // command was empty
        } else if (err == ESP_OK && ret != ESP_OK) {
            printf("Command returned non-zero error code: 0x%x (%s)\n", ret, esp_err_to_name(ret));
        } else if (err != ESP_OK) {
            printf("Internal error: %s\n", esp_err_to_name(err));
        }
        /* linenoise allocates line buffer on the heap, so need to free it */
        linenoiseFree(line);
    }
    ESP_LOGD(TAG, "The End");
    vTaskDelete(NULL);
}
