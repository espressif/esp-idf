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
#include "freertos/semphr.h"
#include "argtable3/argtable3.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_twai.h"
#include "esp_twai_onchip.h"
#include "cmd_twai_internal.h"
#include "twai_utils_parser.h"

/** @brief Log tag for this module */
static const char *TAG = "cmd_twai_send";

/** @brief Command line arguments for sending frames - supports positional and option formats */
static struct {
    struct arg_str *controller;   /**< Controller ID (required) */
    struct arg_str *frame;        /**< Frame string (required) */
    struct arg_end *end;
} twai_send_args;

/**
 * @brief TX Callback for TWAI event handling
 *
 * @param[in] handle TWAI node handle
 * @param[in] event_data TX done event data
 * @param[in] user_ctx Controller context pointer
 *
 * @return @c true if higher priority task woken, @c false otherwise
 */
static bool twai_send_tx_done_cb(twai_node_handle_t handle, const twai_tx_done_event_data_t *event_data, void *user_ctx)
{
    ESP_UNUSED(handle);
    twai_controller_ctx_t *controller = (twai_controller_ctx_t *)user_ctx;
    int controller_id = controller - &g_twai_controller_ctx[0];

    if (!event_data->is_tx_success) {
        ESP_EARLY_LOGW(TAG, "Node %d: Bus error when sending frame with ID: 0x%X", controller_id, event_data->done_tx_frame->header.id);
    }
    return false;
}

/**
 * @brief Send a TWAI frame with the provided parameters
 *
 * @param[in] controller Pointer to the TWAI controller context
 * @param[in] frame Pointer to the TWAI frame to send
 * @param[in] timeout_ms Timeout in milliseconds to wait for TX completion
 *
 * @return @c ESP_OK on success, error code on failure
 */
static esp_err_t send_frame_sync(twai_controller_ctx_t *controller, const twai_frame_t *frame, uint32_t timeout_ms)
{
    ESP_RETURN_ON_FALSE(controller, ESP_ERR_INVALID_ARG, TAG, "Invalid controller pointer");

    int controller_id = controller - &g_twai_controller_ctx[0];
    twai_core_ctx_t *ctx = &controller->core_ctx;

    /* Check if TWAI driver is running */
    ESP_RETURN_ON_FALSE(atomic_load(&ctx->is_initialized), ESP_ERR_INVALID_STATE, TAG, "TWAI%d not initialized", controller_id);

    /* Transmit the frame */
    ESP_RETURN_ON_ERROR(twai_node_transmit(controller->node_handle, frame, timeout_ms), TAG, "Node %d: Failed to queue TX frame", controller_id);

    /* Wait for TX completion or timeout */
    ESP_RETURN_ON_ERROR(twai_node_transmit_wait_all_done(controller->node_handle, timeout_ms), TAG, "Node %d: TX not completed after %"PRIu32" ms", controller_id, timeout_ms);

    return ESP_OK;
}

/**
 * @brief Command handler for `twai_send twai0 123#AABBCC` command
 *
 * @param[in] argc Argument count
 * @param[in] argv Argument vector
 *
 * @return @c ESP_OK on success, error code on failure
 */
static int twai_send_handler(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&twai_send_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, twai_send_args.end, argv[0]);
        return ESP_ERR_INVALID_ARG;
    }

    /* Check for mandatory arguments */
    if (twai_send_args.controller->count == 0) {
        ESP_LOGE(TAG, "Controller ID is required");
        return ESP_ERR_INVALID_ARG;
    }

    /* Parse controller id */
    int controller_id = parse_controller_string(twai_send_args.controller->sval[0]);
    ESP_RETURN_ON_FALSE(controller_id >= 0, ESP_ERR_INVALID_ARG, TAG, "Invalid controller ID: %s", twai_send_args.controller->sval[0]);

    twai_controller_ctx_t *controller = get_controller_by_id(controller_id);
    ESP_RETURN_ON_FALSE(controller != NULL, ESP_ERR_INVALID_ARG, TAG, "Controller not found: %d", controller_id);

    /* Prepare frame buffer on stack for synchronous transmission */
    twai_frame_t frame = {0};
    uint8_t data_buffer[TWAI_FRAME_BUFFER_SIZE] = {0};
    frame.buffer = data_buffer;

    /* Check if frame string is provided */
    const char *frame_str = twai_send_args.frame->sval[0];

    const char *sep = NULL;
    int hash_count = 0;
    bool is_fd = false;
    int res = locate_hash(frame_str, &sep, &hash_count);
    ESP_RETURN_ON_FALSE(res == PARSE_OK, ESP_ERR_INVALID_ARG, TAG, "Failed to locate '#' in frame string: %s", frame_str);
    if (hash_count == 1) {
        is_fd = false;
    } else if (hash_count == 2) {
        is_fd = true;
    } else {
        ESP_LOGE(TAG, "Invalid '#' count in frame string: %s", frame_str);
        return ESP_ERR_INVALID_ARG;
    }

    /* Parse ID */
    size_t id_len = (size_t)(sep - frame_str);
    res = parse_twai_id(frame_str, id_len, &frame);
    ESP_RETURN_ON_FALSE(res == PARSE_OK, ESP_ERR_INVALID_ARG, TAG, "Invalid ID: %.*s, error code: %d", (int)id_len, frame_str, res);

    /* Parse frame body */
    const char *body = sep + hash_count;
    if (is_fd) {
#if CONFIG_EXAMPLE_ENABLE_TWAI_FD
        frame.header.fdf = 1;
        res = parse_twaifd_frame(body, &frame);
        ESP_RETURN_ON_FALSE(res == PARSE_OK, ESP_ERR_INVALID_ARG, TAG, "Invalid TWAI-FD frame: %.*s, error code: %d", (int)id_len, frame_str, res);
#else
        ESP_LOGE(TAG, "TWAI-FD not enabled in this build");
        return ESP_ERR_INVALID_ARG;
#endif
    } else {
        res = parse_classic_frame(body, &frame);
        ESP_RETURN_ON_FALSE(res == PARSE_OK, ESP_ERR_INVALID_ARG, TAG, "Invalid TWAI classic frame: %.*s, error code: %d", (int)id_len, frame_str, res);
    }

    /* Send frame with 1 second timeout */
    esp_err_t ret = send_frame_sync(controller, &frame, 1000);
    ESP_RETURN_ON_ERROR(ret, TAG, "Failed to send frame: %s", esp_err_to_name(ret));

    return ESP_OK;
}

void register_twai_send_commands(void)
{
    /* Initialize send context for all controllers */
    for (int i = 0; i < SOC_TWAI_CONTROLLER_NUM; i++) {
        twai_controller_ctx_t *controller = &g_twai_controller_ctx[i];
        controller->core_ctx.driver_cbs.on_tx_done = twai_send_tx_done_cb;
    }

    /* Register command arguments */
    twai_send_args.controller = arg_str1(NULL, NULL, "<controller>", "TWAI controller (e.g. twai0)");
    twai_send_args.frame = arg_str0(NULL, NULL, "<frame_str>", "Frame string in format 123#AABBCC (standard) or 12345678#AABBCC (extended)");
    twai_send_args.end = arg_end(20);

    /* Register command */
    const esp_console_cmd_t twai_send_cmd = {
        .command = "twai_send",
        .help = "Send a TWAI frame using string format\n"
        "Usage: twai_send <controller> <frame_str>\n"
        "\n"
        "Frame Formats:\n"
        "  Standard:    123#DEADBEEF          (11-bit ID)\n"
        "  Extended:    12345678#CAFEBABE     (29-bit ID)\n"
        "  RTR:        456#R or 456#R8       (Remote Transmission Request)\n"
        "  TWAI-FD:    123##1AABBCC          (FD frame with flags)\n"
        "\n"
        "Examples:\n"
        "  twai_send twai0 123#DEADBEEF                    # Standard frame\n"
        "  twai_send twai0 12345678#CAFEBABE              # Extended frame\n"
        "  twai_send twai0 456#R8                         # RTR frame\n"
        "  twai_send twai0 123##1DEADBEEFCAFEBABE        # TWAI-FD frame\n"
        ,
        .hint = "<controller> [<frame_str>]",
        .func = &twai_send_handler,
        .argtable = &twai_send_args
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&twai_send_cmd));
}

void unregister_twai_send_commands(void)
{
    /* Cleanup all controller send modules */
    for (int i = 0; i < SOC_TWAI_CONTROLLER_NUM; i++) {
        twai_controller_ctx_t *controller = &g_twai_controller_ctx[i];
        controller->core_ctx.driver_cbs.on_tx_done = NULL;
    }
}
