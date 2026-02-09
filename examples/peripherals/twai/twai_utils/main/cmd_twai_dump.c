/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <string.h>
#include <stdatomic.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "argtable3/argtable3.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_err.h"
#include "esp_twai.h"
#include "esp_twai_onchip.h"
#include "cmd_twai_internal.h"
#include "esp_check.h"
#include "twai_utils_parser.h"

#define DUMP_OUTPUT_LINE_SIZE 128
/**
 * @brief Structure for queuing received frames with embedded buffer
 */
typedef struct {
    twai_frame_t frame;        /**< TWAI frame with embedded buffer */
    uint8_t buffer[TWAI_FRAME_BUFFER_SIZE];        /**< Frame data buffer (supports both TWAI and TWAI-FD) */
} rx_queue_item_t;

/** @brief Command line arguments structure */
static struct {
    struct arg_str *controller_filter;  /**< Format: <controller>[,<id>:<mask>[,<id>:<mask>...]] */
    struct arg_lit *stop;               /**< Stop option: --stop */
    struct arg_str *timestamp;          /**< Timestamp mode: -t <mode> */
    struct arg_end *end;
} twai_dump_args;

static const char *TAG = "cmd_twai_dump";

/**
 * @brief Parse TWAI filters from a string and configure the controller
 *
 * @param[in] filter_str Filter string to parse
 * @param[in] controller Controller context to configure
 * @param[out] mask_count_out Number of mask filters configured
 *
 * @return ESP_OK on success, error code on failure
 */
static esp_err_t parse_twai_filters(const char *filter_str, twai_controller_ctx_t *controller, int *mask_count, int *range_count)
{
    int mask_idx = 0;
#ifdef SOC_TWAI_RANGE_FILTER_NUM
    int range_idx = 0;
#endif

    size_t slen = strlen(filter_str);
    if (filter_str && slen > 0) {

        const char *start = filter_str;
        const char *comma;

        while (start && *start) {
            comma = strchr(start, ',');
            size_t tok_len = comma ? (size_t)(comma - start) : strlen(start);

            if (tok_len == 0) {
                start = comma ? comma + 1 : NULL;
                continue;
            }

            uint32_t lhs, rhs;
            size_t lhs_chars, rhs_chars;
            bool is_mask_filter = false;
#if SOC_TWAI_RANGE_FILTER_NUM
            bool is_range_filter = false;
#endif

            /* Try mask filter first: "id:mask" */
            if (parse_pair_token(start, tok_len, ':', &lhs, &lhs_chars, &rhs, &rhs_chars) == PARSE_OK) {
                ESP_RETURN_ON_FALSE(mask_idx < SOC_TWAI_MASK_FILTER_NUM, ESP_ERR_INVALID_ARG, TAG,
                                    "Too many mask filters (max %d)", SOC_TWAI_MASK_FILTER_NUM);
                is_mask_filter = true;
            }
#if SOC_TWAI_RANGE_FILTER_NUM
            /* Try range filter: "low-high" */
            else if (parse_pair_token(start, tok_len, '-', &lhs, &lhs_chars, &rhs, &rhs_chars) == PARSE_OK) {
                ESP_RETURN_ON_FALSE(range_idx < SOC_TWAI_RANGE_FILTER_NUM, ESP_ERR_INVALID_ARG, TAG,
                                    "Too many range filters (max %d)", SOC_TWAI_RANGE_FILTER_NUM);
                is_range_filter = true;
            }
#endif
            else {
                ESP_LOGE(TAG, "Invalid filter token: %.*s", (int)tok_len, start);
                return ESP_ERR_INVALID_ARG;
            }

            /* Common processing: determine if extended frame and validate */
            bool is_ext = (lhs_chars > TWAI_STD_ID_CHAR_LEN) || (rhs_chars > TWAI_STD_ID_CHAR_LEN) ||
                          (lhs > TWAI_STD_ID_MASK) || (rhs > TWAI_STD_ID_MASK);
            uint32_t id_domain = is_ext ? TWAI_EXT_ID_MASK : TWAI_STD_ID_MASK;

            /* Validate values are within domain */
            ESP_RETURN_ON_FALSE(lhs <= id_domain && rhs <= id_domain, ESP_ERR_INVALID_ARG, TAG,
                                "Filter values exceed %s domain", is_ext ? "extended" : "standard");

            if (is_mask_filter) {
                /* Configure mask filter */
                twai_mask_filter_config_t *cfg = &controller->dump_ctx.mask_filter_configs[mask_idx];
                cfg->id = lhs;
                cfg->mask = rhs;
                cfg->is_ext = is_ext;

                ESP_LOGD(TAG, "Parsed mask filter %d: ID=0x%08" PRIX32 ", mask=0x%08" PRIX32 " (%s)",
                         mask_idx, cfg->id, cfg->mask, is_ext ? "extended" : "standard");
                mask_idx++;
            }
#if SOC_TWAI_RANGE_FILTER_NUM
            else if (is_range_filter) {
                /* Additional validation for range filter */
                ESP_RETURN_ON_FALSE(lhs <= rhs, ESP_ERR_INVALID_ARG, TAG,
                                    "Range filter: low (0x%08" PRIX32 ") > high (0x%08" PRIX32 ")", lhs, rhs);

                /* Configure range filter */
                twai_range_filter_config_t *cfg = &controller->dump_ctx.range_filter_configs[range_idx];
                cfg->range_low = lhs;
                cfg->range_high = rhs;
                cfg->is_ext = is_ext;

                ESP_LOGD(TAG, "Parsed range filter %d: low=0x%08" PRIX32 ", high=0x%08" PRIX32 " (%s)",
                         range_idx, cfg->range_low, cfg->range_high, is_ext ? "extended" : "standard");
                range_idx++;
            }
#endif

            start = comma ? comma + 1 : NULL;
        }
    }

    *mask_count = mask_idx;
#if SOC_TWAI_RANGE_FILTER_NUM
    *range_count = range_idx;
#endif
    return ESP_OK;
}

/**
 * @brief TWAI receive done callback for dump functionality
 *
 * @param[in] handle TWAI node handle
 * @param[in] event_data Receive event data
 * @param[in] user_ctx Controller context pointer
 *
 * @return @c true if higher priority task woken, @c false otherwise
 */
static IRAM_ATTR bool twai_dump_rx_done_cb(twai_node_handle_t handle, const twai_rx_done_event_data_t *event_data, void *user_ctx)
{
    ESP_UNUSED(handle);
    ESP_UNUSED(event_data);
    twai_controller_ctx_t *controller = (twai_controller_ctx_t *)user_ctx;
    BaseType_t higher_priority_task_woken = pdFALSE;

    /* Validate user_ctx pointer */
    if (controller == NULL || !atomic_load(&controller->dump_ctx.is_running)) {
        return false;
    }

    /* Check if queue exists before using */
    if (controller->dump_ctx.rx_queue == NULL) {
        return false;
    }

    rx_queue_item_t item = {0};
    item.frame.buffer = item.buffer;
    item.frame.buffer_len = sizeof(item.buffer);

    if (ESP_OK == twai_node_receive_from_isr(handle, &item.frame)) {
        /* Non-blocking queue send with explicit error handling */
        if (xQueueSendFromISR(controller->dump_ctx.rx_queue, &item, &higher_priority_task_woken) != pdTRUE) {
            ESP_EARLY_LOGW(TAG, "app queue full");
        }
    }

    return (higher_priority_task_woken == pdTRUE);
}

/**
 * @brief Frame reception task for dump functionality
 *
 * @param[in] parameter Controller context pointer
 */
static void dump_task(void *parameter)
{
    twai_controller_ctx_t *controller = (twai_controller_ctx_t *)parameter;
    twai_dump_ctx_t *dump_ctx = &(controller->dump_ctx);
    int controller_id = controller - g_twai_controller_ctx;
    char output_line[DUMP_OUTPUT_LINE_SIZE];

    ESP_LOGD(TAG, "Dump task started for controller %d", controller_id);

    while (atomic_load(&dump_ctx->is_running)) {
        rx_queue_item_t item;
        if (xQueueReceive(dump_ctx->rx_queue, &item, pdMS_TO_TICKS(CONFIG_EXAMPLE_DUMP_TASK_TIMEOUT_MS)) == pdPASS) {
            item.frame.buffer = item.buffer;    // point to the new buffer

            format_twaidump_frame(dump_ctx->timestamp_mode, &item.frame, item.frame.header.timestamp,
                                  dump_ctx->start_time_us, &dump_ctx->last_frame_time_us,
                                  controller_id, output_line, sizeof(output_line));
            printf("%s", output_line);
        }
    }

    /* Clean up our own resources */
    vTaskSuspendAll();
    dump_ctx->dump_task_handle = NULL;
    xTaskResumeAll();

    vTaskDelete(NULL);
}

/**
 * @brief Initialize TWAI dump module for a controller
 *
 * @param[in] controller Controller context to initialize
 *
 * @return @c ESP_OK on success, error code on failure
 */
static esp_err_t twai_dump_init_controller(twai_controller_ctx_t *controller)
{
    /* Just register the callback, resources will be created when dump starts */
    controller->core_ctx.driver_cbs.on_rx_done = twai_dump_rx_done_cb;

    /* Initialize atomic flags and handles */
    atomic_init(&controller->dump_ctx.is_running, false);
    controller->dump_ctx.rx_queue = NULL;
    controller->dump_ctx.dump_task_handle = NULL;

    return ESP_OK;
}

/**
 * @brief Start dump for a controller - create resources and task
 *
 * @param[in] controller Controller context to start dump for
 *
 * @return @c ESP_OK on success, error code on failure
 */
static esp_err_t twai_dump_start_controller(twai_controller_ctx_t *controller)
{
    int controller_id = controller - g_twai_controller_ctx;
    twai_dump_ctx_t *dump_ctx = &controller->dump_ctx;

    /* Create frame queue */
    dump_ctx->rx_queue = xQueueCreate(CONFIG_EXAMPLE_DUMP_QUEUE_SIZE, sizeof(rx_queue_item_t));
    if (!dump_ctx->rx_queue) {
        ESP_LOGE(TAG, "Failed to create frame queue for controller %d", controller_id);
        return ESP_ERR_NO_MEM;
    }

    /* Set running flag before creating task */
    atomic_store(&dump_ctx->is_running, true);

    /* Create dump task */
    BaseType_t task_ret = xTaskCreate(
                              dump_task,
                              "twai_dump_task",
                              CONFIG_EXAMPLE_DUMP_TASK_STACK_SIZE,
                              controller,  /* Pass controller as user data */
                              CONFIG_EXAMPLE_DUMP_TASK_PRIORITY,
                              &dump_ctx->dump_task_handle);
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(task_ret == pdPASS, ESP_ERR_NO_MEM, err, TAG, "Failed to create dump task for controller %d", controller_id);

    return ESP_OK;

err:
    atomic_store(&dump_ctx->is_running, false);
    if (dump_ctx->rx_queue != NULL) {
        vQueueDelete(dump_ctx->rx_queue);
        dump_ctx->rx_queue = NULL;
    }
    return ret;
}

/**
 * @brief Deinitialize TWAI dump module for a controller
 *
 * @param[in] controller Controller context to deinitialize
 */
static void twai_dump_deinit_controller(twai_controller_ctx_t *controller)
{
    int controller_id = controller - g_twai_controller_ctx;
    twai_dump_stop_internal(controller_id);

    /* Clear callback */
    controller->core_ctx.driver_cbs.on_rx_done = NULL;

    ESP_LOGD(TAG, "Dump module deinitialized for controller %d", controller_id);
}

/**
 * @brief Command handler for twai_dump command
 *
 * @param[in] argc Argument count
 * @param[in] argv Argument vector
 *
 * @return @c ESP_OK on success, error code on failure
 */
static int twai_dump_handler(int argc, char **argv)
{
    esp_err_t ret = ESP_OK;
    int nerrors = arg_parse(argc, argv, (void **)&twai_dump_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, twai_dump_args.end, argv[0]);
        return ESP_ERR_INVALID_ARG;
    }

    /* Stop dump */
    if (twai_dump_args.stop->count > 0) {
        /* For --stop option, controller ID is in the controller_filter argument */
        const char *controller_str = twai_dump_args.controller_filter->sval[0];
        int controller_id = parse_controller_string(controller_str);
        ESP_RETURN_ON_FALSE(controller_id >= 0, ESP_ERR_INVALID_ARG, TAG, "Invalid controller ID: %s", controller_str);
        twai_controller_ctx_t *controller = get_controller_by_id(controller_id);
        ESP_RETURN_ON_FALSE(controller != NULL, ESP_ERR_INVALID_ARG, TAG, "Failed to get controller for ID: %d", controller_id);

        ret = twai_dump_stop_internal(controller_id);
        ESP_RETURN_ON_FALSE(ret == ESP_OK, ret, TAG, "Failed to stop dump on controller %d", controller_id);

        return ESP_OK;
    }

    /* Start dump */
    const char *controller_str = twai_dump_args.controller_filter->sval[0];

    /* Parse controller ID, e.g. "twai0" -> 0 */
    int controller_id = -1;
    const char *filter_str = NULL;
    filter_str = parse_controller_id(controller_str, &controller_id);
    ESP_RETURN_ON_FALSE(controller_id >= 0, ESP_ERR_INVALID_ARG, TAG, "Failed to parse controller ID");
    twai_controller_ctx_t *controller = get_controller_by_id(controller_id);
    ESP_RETURN_ON_FALSE(controller != NULL, ESP_ERR_INVALID_ARG, TAG, "Failed to get controller for ID: %d", controller_id);

    /* Check if already running */
    if (atomic_load(&controller->dump_ctx.is_running)) {
        ESP_LOGW(TAG, "Dump already running for controller %d", controller_id); // Already running, no need to start again
        return ESP_OK;
    }

    /* Parse filter string directly using simplified logic */
    int mask_count = 0;
#ifdef SOC_TWAI_RANGE_FILTER_NUM
    int range_count = 0;
#endif /* SOC_TWAI_RANGE_FILTER_NUM */

    /* Clear filter configs first */
    memset(controller->dump_ctx.mask_filter_configs, 0, sizeof(controller->dump_ctx.mask_filter_configs));
#if SOC_TWAI_RANGE_FILTER_NUM
    memset(controller->dump_ctx.range_filter_configs, 0, sizeof(controller->dump_ctx.range_filter_configs));
#endif /* SOC_TWAI_RANGE_FILTER_NUM */

    /* Parse filters using the helper function */
#ifdef SOC_TWAI_RANGE_FILTER_NUM
    ret = parse_twai_filters(filter_str, controller, &mask_count, &range_count);
#else
    ret = parse_twai_filters(filter_str, controller, &mask_count, NULL);
#endif /* SOC_TWAI_RANGE_FILTER_NUM */
    ESP_RETURN_ON_ERROR(ret, TAG, "Failed to parse filters: %s", esp_err_to_name(ret));

    /* Check if controller is initialized */
    if (!atomic_load(&controller->core_ctx.is_initialized)) {
        ESP_LOGE(TAG, "TWAI%d not initialized", (controller - g_twai_controller_ctx));
        return ESP_ERR_INVALID_STATE;
    }

    /* Configure filters */
#if SOC_TWAI_RANGE_FILTER_NUM
    if (mask_count > 0 || range_count > 0) {
#else
    if (mask_count > 0) {
#endif
        /* Always disable and reconfigure to apply new filter settings */
        ret = twai_node_disable(controller->node_handle);
        ESP_RETURN_ON_ERROR(ret, TAG, "Failed to disable TWAI node%d for filter configuration: %s", controller_id, esp_err_to_name(ret));

        if (mask_count > 0) {
            for (int i = 0; i < mask_count; i++) {
                ret = twai_node_config_mask_filter(controller->node_handle, i,
                                                   &controller->dump_ctx.mask_filter_configs[i]);
                ESP_RETURN_ON_FALSE(ret == ESP_OK, ret, TAG, "Failed to configure mask filter %d", i);
                ESP_LOGD(TAG, "Configured mask filter %d: %08X : %08X", i,
                         controller->dump_ctx.mask_filter_configs[i].id,
                         controller->dump_ctx.mask_filter_configs[i].mask);
            }
        }
#if SOC_TWAI_RANGE_FILTER_NUM
        ret = (SOC_TWAI_RANGE_FILTER_NUM > 0) ? ESP_OK : ESP_ERR_INVALID_STATE;
        ESP_RETURN_ON_ERROR(ret, TAG, "TWAI%d does not support %d range filters", controller_id, SOC_TWAI_RANGE_FILTER_NUM);
        if (range_count > 0) {
            for (int i = 0; i < range_count; i++) {
                ret = twai_node_config_range_filter(controller->node_handle, i,
                                                    &controller->dump_ctx.range_filter_configs[i]);
                ESP_RETURN_ON_FALSE(ret == ESP_OK, ret, TAG, "Failed to configure range filter %d", i);

                /* If no mask filter is configured, disable mask filter 0 which enabled by default */
                if (mask_count == 0) {
                    twai_mask_filter_config_t mfilter_cfg = {
                        .id = 0xFFFFFFFF,
                        .mask = 0xFFFFFFFF,
                    };
                    esp_err_t mask_ret = twai_node_config_mask_filter(controller->node_handle, 0, &mfilter_cfg);
                    ESP_RETURN_ON_ERROR(mask_ret, TAG, "Failed to configure node%d default mask filter: %s", controller_id, esp_err_to_name(mask_ret));
                }
                ESP_LOGD(TAG, "Configured range filter %d: %08X - %08X", i,
                         controller->dump_ctx.range_filter_configs[i].range_low,
                         controller->dump_ctx.range_filter_configs[i].range_high);
            }
        }
#endif /* SOC_TWAI_RANGE_FILTER_NUM */
        esp_err_t enable_ret = twai_node_enable(controller->node_handle);
        ESP_RETURN_ON_ERROR(enable_ret, TAG, "Failed to enable TWAI node%d after filter configuration: %s", controller_id, esp_err_to_name(enable_ret));
    }

    /* Parse timestamp mode */
    controller->dump_ctx.timestamp_mode = TIMESTAMP_MODE_NONE;
    if (twai_dump_args.timestamp->count > 0) {
        char mode = twai_dump_args.timestamp->sval[0][0];
        switch (mode) {
        case 'a': case 'd': case 'z': case 'n':
            controller->dump_ctx.timestamp_mode = (timestamp_mode_t)mode;
            break;
        default:
            ESP_LOGE(TAG, "Invalid timestamp mode: %c (use a/d/z/n)", mode);
            return ESP_ERR_INVALID_ARG;
        }
    }

    /* Start dump task and create resources */
    ret = twai_dump_start_controller(controller);
    ESP_RETURN_ON_FALSE(ret == ESP_OK, ret, TAG, "Failed to start dump task");

    return ESP_OK;
}

/**
 * @brief Stop dump and wait for task to exit naturally
 *
 * @param[in] controller_id Controller ID to stop dump for
 *
 * @return @c ESP_OK on success, error code on failure
 */
esp_err_t twai_dump_stop_internal(int controller_id)
{
    if (controller_id < 0 || controller_id >= SOC_TWAI_CONTROLLER_NUM) {
        ESP_LOGE(TAG, "Invalid controller ID: %d", controller_id);
        return ESP_ERR_INVALID_ARG;
    }

    twai_controller_ctx_t *controller = get_controller_by_id(controller_id);
    ESP_RETURN_ON_FALSE(controller != NULL, ESP_ERR_INVALID_ARG, TAG, "Invalid controller ID: %d", controller_id);
    twai_dump_ctx_t *dump_ctx = &controller->dump_ctx;

    if (!atomic_load(&dump_ctx->is_running)) {
        ESP_LOGD(TAG, "Dump not running for controller %d", controller_id);
        return ESP_OK;
    }

    /* Signal task to stop */
    if (dump_ctx->dump_task_handle) {
        atomic_store(&dump_ctx->is_running, false);
        ESP_LOGD(TAG, "Signaled dump task to stop for controller %d", controller_id);

        /* Wait for dump task to finish */
        int timeout_ms = CONFIG_EXAMPLE_DUMP_TASK_TIMEOUT_MS * 2;
        vTaskDelay(pdMS_TO_TICKS(timeout_ms));

        ESP_RETURN_ON_FALSE(dump_ctx->dump_task_handle == NULL, ESP_ERR_TIMEOUT, TAG,
                            "Dump task did not exit naturally, timeout after %d ms", timeout_ms);
    }

    /* Clean up queue */
    if (dump_ctx->rx_queue != NULL) {
        vQueueDelete(dump_ctx->rx_queue);
        dump_ctx->rx_queue = NULL;
    }

    return ESP_OK;
}

/**
 * @brief Register TWAI dump commands with console
 */
void register_twai_dump_commands(void)
{
    /* Initialize all controller dump modules */
    for (int i = 0; i < SOC_TWAI_CONTROLLER_NUM; i++) {
        twai_controller_ctx_t *controller = &g_twai_controller_ctx[i];
        esp_err_t ret = twai_dump_init_controller(controller);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "Failed to initialize dump module for TWAI%d: %s", i, esp_err_to_name(ret));
        }
    }

    /* Register command */
    twai_dump_args.controller_filter = arg_str1(NULL, NULL, "<controller> [filter]",
                                                "Controller ID and optional filters");
    twai_dump_args.stop = arg_lit0(NULL, "stop",
                                   "Stop monitoring the specified controller");
    twai_dump_args.timestamp = arg_str0("t", "timestamp", "<mode>",
                                        "Timestamp mode: a=absolute, d=delta, z=zero, n=none (default: n)");
    twai_dump_args.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "twai_dump",
        .help = "Monitor TWAI bus messages with timestamps\n"
        "Usage:\n"
        "  twai_dump [-t <mode>] <controller>[,filter...]\n"
        "  twai_dump <controller> --stop\n"
        "\n"
        "Options:\n"
        "  -t <mode>     Timestamp mode: a=absolute, d=delta, z=zero, n=none (default: n)\n"
        "  --stop        Stop monitoring the specified controller\n"
        "\n"
        "Filter formats:\n"
        "  id:mask       Mask filter (e.g., 123:7FF)\n"
        "  low-high      Range filter (e.g., a-15)\n"
        "\n"
        "Examples:\n"
        "  twai_dump twai0                 # Monitor without timestamps (default)\n"
        "  twai_dump -t a twai0            # Monitor with absolute timestamps\n"
        "  twai_dump -t d twai0            # Monitor with delta timestamps\n"
        "  twai_dump -t n twai0,123:7FF    # Monitor ID 0x123 without timestamps\n"
        "  twai_dump twai0,a-15            # Monitor range: [0xa, 0x15]\n"
        "  twai_dump twai0,123:7FF,a-15    # Mix mask and range filters\n"
        "  twai_dump twai0,000-666         # Monitor range: [0x000, 0x666]\n"
        "  twai_dump twai0 --stop          # Stop monitoring TWAI0\n"
        ,
        .hint = NULL,
        .func = &twai_dump_handler,
        .argtable = &twai_dump_args
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/**
 * @brief Unregister dump commands and cleanup resources
 */
void unregister_twai_dump_commands(void)
{
    /* Cleanup all controller dump modules */
    for (int i = 0; i < SOC_TWAI_CONTROLLER_NUM; i++) {
        twai_controller_ctx_t *controller = &g_twai_controller_ctx[i];
        twai_dump_deinit_controller(controller);
    }

    ESP_LOGI(TAG, "TWAI dump commands unregistered and resources cleaned up");
}
