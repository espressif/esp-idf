/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <stdatomic.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "argtable3/argtable3.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_err.h"
#include "esp_twai.h"
#include "esp_twai_onchip.h"
#include "cmd_twai_internal.h"
#include "esp_check.h"
#include "twai_utils_parser.h"

static const char *TAG = "cmd_twai_core";

/** @brief Command line arguments for twai_init command */
static struct {
    struct arg_str *controller;
    struct arg_int *rate;
    struct arg_lit *loopback;
    struct arg_lit *self_test;
    struct arg_lit *listen;
    struct arg_int *fd_rate;
    struct arg_int *tx_gpio;
    struct arg_int *rx_gpio;
    struct arg_int *clk_out_gpio;
    struct arg_int *bus_off_gpio;
    struct arg_end *end;
} twai_init_args;

/** @brief Command line arguments for twai_deinit command */
static struct {
    struct arg_str *controller;
    struct arg_end *end;
} twai_deinit_args;

/** @brief Command line arguments for twai_info command */
static struct {
    struct arg_str *controller;
    struct arg_end *end;
} twai_info_args;

/** @brief Command line arguments for twai_recover command */
static struct {
    struct arg_str *controller;
    struct arg_int *timeout;
    struct arg_end *end;
} twai_recover_args;

/**
 * @brief State change callback for TWAI controller
 *
 * @param[in] handle TWAI node handle
 * @param[in] edata Event data with state information
 * @param[in] user_ctx Controller context pointer
 *
 * @return @c true if higher priority task woken, @c false otherwise
 */
static bool twai_state_change_callback(twai_node_handle_t handle, const twai_state_change_event_data_t *edata, void *user_ctx)
{
    ESP_UNUSED(handle);
    twai_controller_ctx_t *controller = (twai_controller_ctx_t *)user_ctx;
    bool higher_task_awoken = false;

    if (edata->new_sta == TWAI_ERROR_BUS_OFF) {
        int id = (int)(controller - g_twai_controller_ctx);
        ESP_EARLY_LOGW(TAG, "TWAI%d entered Bus-Off state, use 'twai_recover twai%d' to recover", id, id);
    } else if (edata->old_sta == TWAI_ERROR_BUS_OFF && edata->new_sta == TWAI_ERROR_ACTIVE) {
        int id = (int)(controller - g_twai_controller_ctx);
        ESP_EARLY_LOGI(TAG, "TWAI%d recovered from Bus-Off state", id);
    }
    return higher_task_awoken;
}

/**
 * @brief Create and configure a TWAI controller
 *
 * @param[in] controller Controller context to start
 *
 * @return TWAI node handle on success, @c NULL on failure
 */
static twai_node_handle_t twai_start(twai_controller_ctx_t *controller)
{
    twai_node_handle_t res = NULL;
    esp_err_t ret = ESP_OK;
    twai_core_ctx_t *ctx = &controller->core_ctx;

    /* Check if the TWAI driver is already running */
    if (atomic_load(&ctx->is_initialized)) {
        ESP_LOGD(TAG, "TWAI driver is already running. Please stop it first.");
        return controller->node_handle;
    }

    if (controller->node_handle) {
        ESP_LOGW(TAG, "Cleaning up old TWAI node handle");
        ret = twai_node_delete(controller->node_handle);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "Failed to delete old TWAI node: %s", esp_err_to_name(ret));
        }
        controller->node_handle = NULL;
    }

#if CONFIG_EXAMPLE_ENABLE_TWAI_FD
    if (ctx->driver_config.data_timing.bitrate > 0) {
        if (ctx->driver_config.data_timing.bitrate < ctx->driver_config.bit_timing.bitrate) {
            ESP_LOGW(TAG, "TWAI-FD disabled: data bitrate (%" PRIu32 ") must be higher than arbitration bitrate (%" PRIu32 ")",
                     ctx->driver_config.data_timing.bitrate, ctx->driver_config.bit_timing.bitrate);
            ctx->driver_config.data_timing.bitrate = 0;  /* Disable FD */
        } else {
            ESP_LOGD(TAG, "TWAI-FD enabled: Arbitration=%" PRIu32 " bps, Data=%" PRIu32 " bps",
                     ctx->driver_config.bit_timing.bitrate, ctx->driver_config.data_timing.bitrate);
        }
    }
#endif

    ESP_GOTO_ON_ERROR(twai_new_node_onchip(&(ctx->driver_config), &(controller->node_handle)),
                      err, TAG, "Failed to create TWAI node");
    res = controller->node_handle;

    /* Register event callbacks including our state change callback */
    ctx->driver_cbs.on_state_change = twai_state_change_callback;
    ESP_GOTO_ON_ERROR(twai_node_register_event_callbacks(controller->node_handle, &(ctx->driver_cbs), controller),
                      err_node, TAG, "Failed to register callbacks");

    ESP_GOTO_ON_ERROR(twai_node_enable(controller->node_handle),
                      err_node, TAG, "Failed to enable node");

    atomic_store(&ctx->is_initialized, true);
    return res;

err_node:
    if (controller->node_handle) {
        ret = twai_node_delete(controller->node_handle);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "Failed to delete TWAI node during error cleanup: %s", esp_err_to_name(ret));
        }
        controller->node_handle = NULL;
    }
err:
    return NULL;
}

/**
 * @brief Stop a TWAI controller, disable the node and delete it
 *
 * @param[in] controller Controller context to stop
 *
 * @return @c ESP_OK on success, error code on failure
 */
static esp_err_t twai_stop(twai_controller_ctx_t *controller)
{
    twai_core_ctx_t *ctx = &controller->core_ctx;
    esp_err_t ret = ESP_OK;

    if (!atomic_load(&ctx->is_initialized)) {
        ESP_LOGI(TAG, "TWAI not running");
        return ret;
    }

    if (controller->node_handle) {
        ret = twai_node_disable(controller->node_handle);
        ESP_RETURN_ON_ERROR(ret, TAG, "Failed to disable TWAI node: %s", esp_err_to_name(ret));
        ret = twai_node_delete(controller->node_handle);
        ESP_RETURN_ON_ERROR(ret, TAG, "Failed to delete TWAI node: %s", esp_err_to_name(ret));
        controller->node_handle = NULL;
    }

    atomic_store(&ctx->is_initialized, false);
    return ret;
}

/**
 * @brief Initialize and start TWAI controller `twai_init twai0 -t 4 -r 5 -b 500000` command handler
 *
 * @param[in] argc Argument count
 * @param[in] argv Argument vector
 *
 * @return @c ESP_OK on success, error code on failure
 *
 * @note Parses GPIO, timing, and mode configuration. -t=TX GPIO, -r=RX GPIO, -b=bitrate
 */
static int twai_init_handler(int argc, char **argv)
{
    esp_err_t ret = ESP_OK;
    int controller_id;
    twai_controller_ctx_t *controller;
    twai_core_ctx_t *ctx;
    int tx_gpio, rx_gpio, clk_gpio, bus_off_gpio;

    int nerrors = arg_parse(argc, argv, (void **)&twai_init_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, twai_init_args.end, argv[0]);
        return ESP_ERR_INVALID_ARG;
    }

    /* Check required arguments first */
    if (twai_init_args.controller->count == 0) {
        ESP_LOGE(TAG, "Controller argument is required (e.g., twai0, twai1)");
        return ESP_ERR_INVALID_ARG;
    }
    if (twai_init_args.tx_gpio->count == 0) {
        ESP_LOGE(TAG, "TX GPIO argument is required (-t <gpio>)");
        return ESP_ERR_INVALID_ARG;
    }
    if (twai_init_args.rx_gpio->count == 0) {
        ESP_LOGE(TAG, "RX GPIO argument is required (-r <gpio>)");
        return ESP_ERR_INVALID_ARG;
    }

    controller_id = parse_controller_string(twai_init_args.controller->sval[0]);
    ret = (controller_id >= 0) ? ESP_OK : ESP_ERR_INVALID_ARG;
    ESP_GOTO_ON_ERROR(ret, err, TAG, "Invalid controller ID");

    controller = get_controller_by_id(controller_id);
    ret = (controller != NULL) ? ESP_OK : ESP_ERR_INVALID_ARG;
    ESP_GOTO_ON_ERROR(ret, err, TAG, "Controller %d not found", controller_id);

    ctx = &controller->core_ctx;
    ret = (!atomic_load(&ctx->is_initialized)) ? ESP_OK : ESP_ERR_INVALID_STATE;
    ESP_GOTO_ON_ERROR(ret, err, TAG, "TWAI%d already running", controller_id);

    /* Configure TX GPIO */
    tx_gpio = twai_init_args.tx_gpio->ival[0];
    ret = (tx_gpio >= 0 && GPIO_IS_VALID_OUTPUT_GPIO(tx_gpio)) ? ESP_OK : ESP_ERR_INVALID_ARG;
    ESP_GOTO_ON_ERROR(ret, err, TAG, "Invalid TX GPIO: %d", tx_gpio);
    ctx->driver_config.io_cfg.tx = tx_gpio;
    ESP_LOGI(TAG, "TX GPIO set to %d", tx_gpio);

    /* Configure RX GPIO */
    rx_gpio = twai_init_args.rx_gpio->ival[0];
    ret = GPIO_IS_VALID_GPIO(rx_gpio) ? ESP_OK : ESP_ERR_INVALID_ARG;
    ESP_GOTO_ON_ERROR(ret, err, TAG, "Invalid RX GPIO: %d", rx_gpio);
    ctx->driver_config.io_cfg.rx = rx_gpio;
    ESP_LOGI(TAG, "RX GPIO set to %d", rx_gpio);

    /* Configure optional clock output GPIO */
    if (twai_init_args.clk_out_gpio->count > 0) {
        clk_gpio = twai_init_args.clk_out_gpio->ival[0];
        ret = GPIO_IS_VALID_OUTPUT_GPIO(clk_gpio) ? ESP_OK : ESP_ERR_INVALID_ARG;
        ESP_GOTO_ON_ERROR(ret, err, TAG, "Invalid CLK out GPIO: %d", clk_gpio);
        ctx->driver_config.io_cfg.quanta_clk_out = clk_gpio;
        ESP_LOGI(TAG, "Clock output GPIO set to %d", clk_gpio);
    } else {
        ctx->driver_config.io_cfg.quanta_clk_out = -1;
        ESP_LOGI(TAG, "Clock output disabled");
    }

    /* Configure optional bus-off indicator GPIO */
    if (twai_init_args.bus_off_gpio->count > 0) {
        bus_off_gpio = twai_init_args.bus_off_gpio->ival[0];
        if (bus_off_gpio >= 0) {
            ret = GPIO_IS_VALID_OUTPUT_GPIO(bus_off_gpio) ? ESP_OK : ESP_ERR_INVALID_ARG;
            ESP_GOTO_ON_ERROR(ret, err, TAG, "Invalid bus-off GPIO: %d", bus_off_gpio);
            ctx->driver_config.io_cfg.bus_off_indicator = bus_off_gpio;
            ESP_LOGI(TAG, "Bus-off indicator GPIO set to %d", bus_off_gpio);
        }
    } else {
        ctx->driver_config.io_cfg.bus_off_indicator = -1;
        ESP_LOGI(TAG, "Bus-off indicator disabled");
    }

    /* Verify required IO configuration */
    ret = (ctx->driver_config.io_cfg.tx >= 0 && ctx->driver_config.io_cfg.rx >= 0) ? ESP_OK : ESP_ERR_INVALID_ARG;
    ESP_GOTO_ON_ERROR(ret, err, TAG, "Both TX and RX GPIO must be configured");

    /* Update timing configuration */
    if (twai_init_args.rate->count > 0) {
        ctx->driver_config.bit_timing.bitrate = twai_init_args.rate->ival[0];
    }
    ESP_LOGI(TAG, "Bitrate set to %" PRIu32 " bps", ctx->driver_config.bit_timing.bitrate);

    ctx->driver_config.flags.enable_loopback    = (twai_init_args.loopback->count   > 0);
    ctx->driver_config.flags.enable_self_test   = (twai_init_args.self_test->count  > 0);
    ctx->driver_config.flags.enable_listen_only = (twai_init_args.listen->count     > 0);

    if (ctx->driver_config.flags.enable_loopback) {
        ESP_LOGI(TAG, "Loopback mode enabled");
    }
    if (ctx->driver_config.flags.enable_self_test) {
        ESP_LOGI(TAG, "Self-test mode enabled");
    }
    if (ctx->driver_config.flags.enable_listen_only) {
        ESP_LOGI(TAG, "Listen-only mode enabled");
    }

#if CONFIG_EXAMPLE_ENABLE_TWAI_FD
    if (twai_init_args.fd_rate->count > 0) {
        ctx->driver_config.data_timing.bitrate = twai_init_args.fd_rate->ival[0];
    } else {
        ctx->driver_config.data_timing.bitrate = CONFIG_EXAMPLE_DEFAULT_FD_BITRATE;
    }
#else
    ctx->driver_config.data_timing.bitrate = 0;  /* FD disabled */
#endif
    ESP_LOGI(TAG, "FD bitrate set to %" PRIu32, ctx->driver_config.data_timing.bitrate);

    /* Always configure timestamp frequency to 1MHz */
    ctx->driver_config.timestamp_resolution_hz = 1000000;

    /* Start TWAI controller */
    controller->node_handle = twai_start(controller);
    ret = (controller->node_handle != NULL) ? ESP_OK : ESP_FAIL;
    ESP_GOTO_ON_ERROR(ret, err, TAG, "Failed to start TWAI controller");

    return ESP_OK;

err:
    return ret;
}

/**
 * @brief Stop and deinitialize TWAI controller command handler
 *
 * @param[in] argc Argument count
 * @param[in] argv Argument vector
 *
 * @return @c ESP_OK on success, error code on failure
 *
 * @note Stops dump monitoring and controller
 */
static int twai_deinit_handler(int argc, char **argv)
{
    int controller_id;
    esp_err_t ret = ESP_OK;

    int nerrors = arg_parse(argc, argv, (void **)&twai_deinit_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, twai_deinit_args.end, argv[0]);
        return ESP_ERR_INVALID_ARG;
    }

    if (twai_deinit_args.controller->count == 0) {
        ESP_LOGE(TAG, "Controller ID is required");
        return ESP_ERR_INVALID_ARG;
    }

    controller_id = parse_controller_string(twai_deinit_args.controller->sval[0]);
    ret = (controller_id >= 0) ? ESP_OK : ESP_ERR_INVALID_ARG;
    ESP_GOTO_ON_ERROR(ret, err, TAG, "Invalid controller ID");

    twai_controller_ctx_t* controller = get_controller_by_id(controller_id);
    ret = (controller != NULL) ? ESP_OK : ESP_ERR_INVALID_ARG;
    ESP_GOTO_ON_ERROR(ret, err, TAG, "Controller %d not found", controller_id);

    twai_core_ctx_t *ctx = &controller->core_ctx;

    if (!atomic_load(&ctx->is_initialized)) {
        ESP_LOGI(TAG, "TWAI%d not running", controller_id);
        return ESP_OK;
    }

    /* Auto-stop dump monitoring if it's running */
    esp_err_t dump_ret = twai_dump_stop_internal(controller_id);
    if (dump_ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to stop dump for controller %d: %s", controller_id, esp_err_to_name(dump_ret));
    }

    ret = twai_stop(controller);
    ESP_RETURN_ON_ERROR(ret, TAG, "Failed to stop TWAI%d: %s", controller_id, esp_err_to_name(ret));

    return ESP_OK;

err:
    return ret;
}

/**
 * @brief Recover from Bus-Off state `twai_recover twai0` command handler
 *
 * @param[in] argc Argument count
 * @param[in] argv Argument vector
 *
 * @return @c ESP_OK on success, error code on failure
 *
 * @note Supports async, blocking, and timeout modes
 */
static int twai_recover_handler(int argc, char **argv)
{
    int controller_id;
    esp_err_t ret = ESP_OK;

    int nerrors = arg_parse(argc, argv, (void **)&twai_recover_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, twai_recover_args.end, argv[0]);
        return ESP_ERR_INVALID_ARG;
    }

    if (twai_recover_args.controller->count == 0) {
        ESP_LOGE(TAG, "Controller ID is required");
        return ESP_ERR_INVALID_ARG;
    }

    controller_id = parse_controller_string(twai_recover_args.controller->sval[0]);
    ESP_GOTO_ON_FALSE(controller_id >= 0, ESP_ERR_INVALID_ARG, err, TAG, "Invalid controller ID");

    twai_controller_ctx_t *controller = get_controller_by_id(controller_id);
    ESP_GOTO_ON_FALSE(controller != NULL, ESP_ERR_INVALID_ARG, err, TAG, "Controller %d not found", controller_id);

    if (!controller->node_handle) {
        ESP_LOGE(TAG, "TWAI%d not initialized", controller_id);
        return ESP_ERR_INVALID_STATE;
    }

    /* timeout: -1 = block, 0 = async, >0 = timeout (ms) */
    int32_t timeout_ms = -1;
    if (twai_recover_args.timeout->count > 0) {
        timeout_ms = twai_recover_args.timeout->ival[0];
    }

    if (timeout_ms < -1) {
        ESP_LOGE(TAG, "Invalid timeout value: %d (must be -1, 0 or positive)", timeout_ms);
        return ESP_ERR_INVALID_ARG;
    }

    twai_node_status_t node_status;
    ret = twai_node_get_info(controller->node_handle, &node_status, NULL);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "Failed to get node%d status: %s", controller_id, esp_err_to_name(ret));

    ret = (node_status.state == TWAI_ERROR_BUS_OFF) ? ESP_OK : ESP_ERR_INVALID_STATE;
    ESP_GOTO_ON_ERROR(ret, err, TAG, "Recovery not needed - node is %s", twai_state_to_string(node_status.state));

    ESP_LOGI(TAG, "Starting recovery from Bus-Off state...");
    ret = twai_node_recover(controller->node_handle);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "Failed to start recovery: %s", esp_err_to_name(ret));

    if (timeout_ms == 0) {
        ESP_LOGI(TAG, "Recovery initiated (async mode)");
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Waiting for recovery to complete...");
    uint32_t elapsed_ms = 0;
    const uint32_t check_interval_ms = 100;
    uint32_t limit_ms = (timeout_ms < 0) ? UINT32_MAX : (uint32_t)timeout_ms;

    while (elapsed_ms < limit_ms) {
        vTaskDelay(pdMS_TO_TICKS(check_interval_ms));
        elapsed_ms += check_interval_ms;

        ret = twai_node_get_info(controller->node_handle, &node_status, NULL);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "Failed to check recovery status: %s", esp_err_to_name(ret));

        if (node_status.state == TWAI_ERROR_ACTIVE) {
            ESP_LOGI(TAG, "Recovery completed successfully in %" PRIu32 " ms", elapsed_ms);
            return ESP_OK;
        }

        if (elapsed_ms % 1000 == 0) {
            ESP_LOGI(TAG, "Recovery in progress... (state: %s, elapsed: %" PRIu32 " ms)",
                     twai_state_to_string(node_status.state), elapsed_ms);
        }
    }

    ESP_LOGI(TAG, "Recovery timeout after %" PRIu32 " ms (current state: %s)",
             limit_ms, twai_state_to_string(node_status.state));
    return ESP_ERR_TIMEOUT;

err:
    return ret;
}

/**
 * @brief Display controller information `twai_info twai0` command handler
 *
 * @param[in] argc Argument count
 * @param[in] argv Argument vector
 *
 * @return @c ESP_OK on success, error code on failure
 *
 * @note Shows status, configuration, and error counters
 */
static int twai_info_handler(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&twai_info_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, twai_info_args.end, argv[0]);
        return ESP_ERR_INVALID_ARG;
    }

    if (twai_info_args.controller->count == 0) {
        ESP_LOGE(TAG, "Controller ID is required");
        return ESP_ERR_INVALID_ARG;
    }

    int controller_id = parse_controller_string(twai_info_args.controller->sval[0]);
    if (controller_id < 0) {
        return ESP_ERR_INVALID_ARG;
    }

    twai_controller_ctx_t* controller = get_controller_by_id(controller_id);
    if (!controller) {
        return ESP_ERR_INVALID_ARG;
    }

    twai_core_ctx_t *ctx = &controller->core_ctx;
    char tx_gpio_buf[16], rx_gpio_buf[16];

    if (!atomic_load(&ctx->is_initialized)) {
        printf("TWAI%d Status: Stopped\n", controller_id);
    } else if (controller->node_handle) {
        twai_node_status_t node_status;
        if (twai_node_get_info(controller->node_handle, &node_status, NULL) == ESP_OK &&
                node_status.state == TWAI_ERROR_BUS_OFF) {
            printf("TWAI%d Status: Bus-Off\n", controller_id);
        } else {
            printf("TWAI%d Status: Running\n", controller_id);
        }
    } else {
        printf("TWAI%d Status: Initializing\n", controller_id);
    }

    /* Node status and error counters */
    if (controller->node_handle && atomic_load(&ctx->is_initialized)) {
        twai_node_status_t node_status;
        esp_err_t ret = twai_node_get_info(controller->node_handle, &node_status, NULL);
        if (ret == ESP_OK) {
            printf("Node State: %s\n", twai_state_to_string(node_status.state));
            printf("Error Counters: TX=%u, RX=%u\n", node_status.tx_error_count, node_status.rx_error_count);
            if (node_status.state == TWAI_ERROR_BUS_OFF) {
                printf("  ! Use 'twai_recover twai%d' to recover from Bus-Off\n", controller_id);
            }
        } else {
            printf("Node State: Unable to read status\n");
        }
    } else {
        printf("Node State: Not initialized\n");
    }

    /* Configuration */
    printf("Bitrate: %" PRIu32 " bps", ctx->driver_config.bit_timing.bitrate);
#if CONFIG_EXAMPLE_ENABLE_TWAI_FD
    if (ctx->driver_config.data_timing.bitrate > 0) {
        printf(" (FD: %" PRIu32 " bps)", ctx->driver_config.data_timing.bitrate);
    }
#endif
    printf("\n");

    format_gpio_pin(ctx->driver_config.io_cfg.tx, tx_gpio_buf, sizeof(tx_gpio_buf));
    format_gpio_pin(ctx->driver_config.io_cfg.rx, rx_gpio_buf, sizeof(rx_gpio_buf));
    printf("GPIOs: TX=%s, RX=%s\n", tx_gpio_buf, rx_gpio_buf);

    /* Special modes (only if not normal) */
    if (ctx->driver_config.flags.enable_loopback ||
            ctx->driver_config.flags.enable_self_test ||
            ctx->driver_config.flags.enable_listen_only) {
        printf("Modes: ");
        bool first = true;
        if (ctx->driver_config.flags.enable_self_test) {
            printf("Self-Test");
            first = false;
        }
        if (ctx->driver_config.flags.enable_loopback) {
            if (!first) {
                printf(", ");
            }
            printf("Loopback");
            first = false;
        }
        if (ctx->driver_config.flags.enable_listen_only) {
            if (!first) {
                printf(", ");
            }
            printf("Listen-Only");
        }
        printf("\n");
    }

    return ESP_OK;
}

/**
 * @brief Register TWAI core commands with console
 *
 * @note Initializes controllers and registers all command handlers
 */
void register_twai_core_commands(void)
{
    // Initialize all controllers
    for (int i = 0; i < SOC_TWAI_CONTROLLER_NUM; i++) {
        twai_controller_ctx_t* controller = &g_twai_controller_ctx[i];
        twai_core_ctx_t *ctx = &controller->core_ctx;

        ctx->driver_config = (twai_onchip_node_config_t) {
            .io_cfg = {
                .tx = GPIO_NUM_NC,
                .rx = GPIO_NUM_NC,
                .quanta_clk_out = GPIO_NUM_NC,
                .bus_off_indicator = GPIO_NUM_NC,
            },
            .clk_src = 0,
            .bit_timing = {
                .bitrate = CONFIG_EXAMPLE_DEFAULT_BITRATE,
                .sp_permill = 0,
                .ssp_permill = 0,
            },
            .data_timing = {
#if CONFIG_EXAMPLE_ENABLE_TWAI_FD
                .bitrate = CONFIG_EXAMPLE_DEFAULT_FD_BITRATE,
#endif
            },
            .fail_retry_cnt = -1,
            .tx_queue_depth = CONFIG_EXAMPLE_TX_QUEUE_LEN,
            .intr_priority = 0,
            .flags = {
                .enable_self_test = false,     /* Default: Self-test disabled */
                .enable_loopback = false,      /* Default: Loopback disabled */
                .enable_listen_only = false,   /* Default: Listen-only disabled */
                .no_receive_rtr = 0,
            },
        };

        atomic_init(&ctx->is_initialized, false);

        ESP_LOGD(TAG, "Default config set for TWAI%d (TX=%d, RX=%d).",
                 i, ctx->driver_config.io_cfg.tx, ctx->driver_config.io_cfg.rx);
    }
    /* Register command arguments */
    twai_init_args.controller = arg_str1(NULL, NULL, "<controller>", "TWAI controller (twai0, twai1, etc.)");
    twai_init_args.tx_gpio = arg_int1("t", "tx", "<gpio>", "TX GPIO pin number (required, e.g., 4)");
    twai_init_args.rx_gpio = arg_int1("r", "rx", "<gpio>", "RX GPIO pin number (required, e.g., 5)");
    twai_init_args.rate = arg_int0("b", "bitrate", "<bps>", "Arbitration bitrate in bps (default: 500000)");
    twai_init_args.loopback = arg_lit0(NULL, "loopback", "Enable loopback mode for testing");
    twai_init_args.self_test = arg_lit0(NULL, "self-test", "Enable self-test mode for testing");
    twai_init_args.listen = arg_lit0(NULL, "listen", "Enable listen-only mode (no transmission)");
    twai_init_args.fd_rate = arg_int0("B", "fd-bitrate", "<bps>", "TWAI-FD data bitrate in bps (optional)");
    twai_init_args.clk_out_gpio = arg_int0("c", "clk-out", "<gpio>", "Clock output GPIO pin (optional)");
    twai_init_args.bus_off_gpio = arg_int0("o", "bus-off", "<gpio>", "Bus-off indicator GPIO pin (optional)");
    twai_init_args.end = arg_end(20);

    twai_deinit_args.controller = arg_str1(NULL, NULL, "<controller>", "TWAI controller (twai0, twai1)");
    twai_deinit_args.end = arg_end(20);

    twai_info_args.controller = arg_str1(NULL, NULL, "<controller>", "TWAI controller (twai0, twai1)");
    twai_info_args.end = arg_end(20);

    twai_recover_args.controller = arg_str1(NULL, NULL, "<controller>", "TWAI controller (twai0, twai1)");
    twai_recover_args.timeout = arg_int0("t", "timeout", "<ms>", "Recovery timeout in milliseconds (default: -1=block)\n             -1 = block until complete\n             0 = async (return immediately)\n             >0 = timeout in ms");
    twai_recover_args.end = arg_end(20);

    /* Register commands */
    const esp_console_cmd_t twai_init_cmd = {
        .command = "twai_init",
        .help = "Initialize and start the TWAI driver\n"
        "Usage: twai_init <controller> -t <tx_gpio> -r <rx_gpio> [options]\n"
        "Example: twai_init twai0 -t 4 -r 5 -b 500000\n"
        "Example: twai_init twai0 -t 4 -r 5 --loopback --self-test",
        .hint = NULL,
        .func = &twai_init_handler,
        .argtable = &twai_init_args
    };

    const esp_console_cmd_t twai_deinit_cmd = {
        .command = "twai_deinit",
        .help = "Stop and deinitialize the TWAI driver",
        .hint = NULL,
        .func = &twai_deinit_handler,
        .argtable = &twai_deinit_args
    };

    const esp_console_cmd_t twai_info_cmd = {
        .command = "twai_info",
        .help = "Display TWAI controller information and status",
        .hint = NULL,
        .func = &twai_info_handler,
        .argtable = &twai_info_args
    };

    const esp_console_cmd_t twai_recover_cmd = {
        .command = "twai_recover",
        .help = "Recover TWAI controller from Bus-Off error state\n"
        "Usage:\n"
        "  twai_recover <controller>                # Block until complete (default)\n"
        "  twai_recover <controller> -t 0           # Async recovery\n"
        "  twai_recover <controller> -t 5000        # 5 second timeout\n"
        "\n"
        "Examples:\n"
        "  twai_recover twai0                       # Block until complete\n"
        "  twai_recover twai0 -t 0                  # Async recovery\n"
        "  twai_recover twai1 -t 15000              # 15 second timeout",
        .hint = NULL,
        .func = &twai_recover_handler,
        .argtable = &twai_recover_args
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&twai_init_cmd));
    ESP_ERROR_CHECK(esp_console_cmd_register(&twai_deinit_cmd));
    ESP_ERROR_CHECK(esp_console_cmd_register(&twai_info_cmd));
    ESP_ERROR_CHECK(esp_console_cmd_register(&twai_recover_cmd));
}

/**
 * @brief Unregister TWAI core commands and cleanup resources
 */
void unregister_twai_core_commands(void)
{
    esp_err_t ret = ESP_OK;
    /* Cleanup all controllers */
    for (int i = 0; i < SOC_TWAI_CONTROLLER_NUM; i++) {
        twai_controller_ctx_t *controller = &g_twai_controller_ctx[i];
        twai_core_ctx_t *ctx = &controller->core_ctx;

        /* Stop dump and other modules first to avoid callback issues */
        ret = twai_dump_stop_internal(i);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "Failed to stop dump for controller %d: %s", i, esp_err_to_name(ret));
        }

        /* Disable and delete TWAI node if it exists */
        if (controller->node_handle) {
            if (atomic_load(&ctx->is_initialized)) {
                ret = twai_node_disable(controller->node_handle);
                if (ret != ESP_OK) {
                    ESP_LOGW(TAG, "Failed to disable TWAI node for controller %d: %s", i, esp_err_to_name(ret));
                }
            }

            ret = twai_node_delete(controller->node_handle);
            if (ret != ESP_OK) {
                ESP_LOGW(TAG, "Failed to delete TWAI node for controller %d: %s", i, esp_err_to_name(ret));
            } else {
                ESP_LOGD(TAG, "Deleted TWAI node for controller %d", i);
            }
            controller->node_handle = NULL;
        }

        /* Clear initialization flag */
        atomic_store(&ctx->is_initialized, false);

        /* Clear callbacks */
        memset(&ctx->driver_cbs, 0, sizeof(ctx->driver_cbs));
    }

    ESP_LOGI(TAG, "TWAI core commands unregistered and resources cleaned up");
}
