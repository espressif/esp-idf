/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <sys/param.h>
#include <sys/fcntl.h>
#include "esp_console.h"
#include "console_private.h"
#include "esp_log.h"
#include "linenoise/linenoise.h"
#include "esp_stdio.h"

#include "esp_vfs_eventfd.h"
#if CONFIG_IDF_TARGET_LINUX
#include <unistd.h>
#endif

static const char *TAG = "console.common";

void esp_console_repl_task(void *args)
{
    esp_console_repl_universal_t *repl_conf = (esp_console_repl_universal_t *) args;
    esp_console_repl_com_t *repl_com = &repl_conf->repl_com;

    /* Waiting for task notify. This happens when `esp_console_start_repl()`
     * function is called. */
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    if (repl_com->state_mux != NULL) {
        xSemaphoreTake(repl_com->state_mux, portMAX_DELAY);
    }

    /* Change standard input and output of the task if the requested UART is
     * NOT the default one. This block will replace stdin, stdout and stderr.
     */
#if CONFIG_LIBC_PICOLIBC
    // TODO IDF-14901
    if (repl_com->_stdin) {
        stdin = repl_com->_stdin;
        stdout = stderr = repl_com->_stdout;
    } else {
        linenoise_init_with_global_stdio();
    }
#else
#if !CONFIG_IDF_TARGET_LINUX
    esp_console_setup_standard_stream(repl_conf->dev_config);
#endif // !CONFIG_IDF_TARGET_LINUX
#endif // CONFIG_LIBC_PICOLIBC

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
               "On Windows, try using Windows Terminal or Putty instead.\r\n");
    }

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

#if CONFIG_LIBC_PICOLIBC
    linenoise_close_stdio();
#endif

    if (repl_com->state_mux != NULL) {
        xSemaphoreGive(repl_com->state_mux);
    }
    ESP_LOGD(TAG, "The End");
    vTaskDelete(NULL);
}

esp_err_t esp_console_setup_prompt(const char *prompt, esp_console_repl_com_t *repl_com)
{
    /* set command line prompt */
    const char *prompt_temp = "esp>";
    if (prompt) {
        prompt_temp = prompt;
    }
    snprintf(repl_com->prompt, CONSOLE_PROMPT_MAX_LEN - 1, LOG_COLOR_I "%s " LOG_RESET_COLOR, prompt_temp);

    /* Figure out if the terminal supports escape sequences */
    /* TODO IDF-14901: It is not appropriate to probe the current thread's console here.
     * The esp_console_repl_task can open its own stdin/stdout for use.
     * However, linenoiseProbe() cannot be moved to esp_console_repl_task
     * to preserve user expectations. Consider the following usage pattern:
     *     esp_console_start_repl(repl);
     *     printf("!!!ready!!!");
     * Users expect that when "!!!ready!!!" is printed, the console is already available.
     * If linenoiseProbe() were moved to esp_console_repl_task, race conditions
     * between threads combined with usleep() calls inside linenoiseProbe() could
     * change this behavior. Currently, there is already a race between threads,
     * but since esp_console_repl_task does not call any sleep functions, everything
     * works as users expect.
     */
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

esp_err_t esp_console_setup_history(const char *history_path, uint32_t max_history_len, esp_console_repl_com_t *repl_com)
{
    repl_com->history_save_path = history_path;
    if (history_path) {
        /* Load command history from filesystem */
        linenoiseHistoryLoad(history_path);
    }

    /* Set command history size */
    if (linenoiseHistorySetMaxLen(max_history_len) != 1) {
        ESP_LOGE(TAG, "set max history length to %"PRIu32" failed", max_history_len);
        return ESP_FAIL;
    }
    return ESP_OK;
}

__attribute__((weak)) esp_err_t esp_console_internal_set_event_fd(esp_console_repl_com_t *repl_com)
{
    return ESP_OK;
}

esp_err_t esp_console_common_init(size_t max_cmdline_length, esp_console_repl_com_t *repl_com)
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
        return ret;
    }

    ret = esp_console_register_help_command();
    if (ret != ESP_OK) {
        return ret;
    }

    /* Configure linenoise line completion library */
    /* Enable multiline editing. If not set, long commands will scroll within single line */
    linenoiseSetMultiLine(1);

    /* Tell linenoise where to get command completions and hints */
    linenoiseSetCompletionCallback(&esp_console_get_completion);
    linenoiseSetHintsCallback((linenoiseHintsCallback *)&esp_console_get_hint);

    /* set the maximum command line length. the function returns -1 or
     * 0 so it is ok to cast it to esp_err_t here. */
    ret = linenoiseSetMaxLineLen(repl_com->max_cmdline_length);
    if (ret != ESP_OK) {
        return ret;
    }

#if CONFIG_VFS_SUPPORT_SELECT  || CONFIG_IDF_TARGET_LINUX
    ret = esp_console_internal_set_event_fd(repl_com);
    if (ret != ESP_OK) {
        return ret;
    }
#endif

    return ESP_OK;
}

__attribute__((weak)) esp_err_t esp_console_common_deinit(esp_console_repl_com_t *repl_com)
{
    // set the state to deinit to force the while loop in
    // esp_console_repl_task to break
    repl_com->state = CONSOLE_REPL_STATE_DEINIT;

    /* Unregister the heap function to avoid memory leak, since it is created
     * every time a console init is called. */
    esp_err_t ret = esp_console_deregister_help_command();
    if (ret != ESP_OK) {
        return ret;
    }

    /* free the history to avoid memory leak, since it is created
     * every time a console init is called. */
    linenoiseHistoryFree();

    return ESP_OK;
}

esp_err_t esp_console_delete_repl_stdio(esp_console_repl_t *repl)
{
    esp_console_repl_com_t *repl_com = (esp_console_repl_com_t*)repl;
    esp_console_repl_universal_t *universal_repl = (esp_console_repl_universal_t*)repl_com;

    const esp_err_t ret = esp_console_common_deinit(repl_com);
    if (ret != ESP_OK) {
        if (ret == ESP_ERR_INVALID_STATE) {
            ESP_LOGE(TAG, "already de-initialized");
        }
        return ret;
    }

    esp_console_deinit();

    esp_stdio_uninstall_io_driver();

    free(universal_repl);

    return ESP_OK;
}

esp_err_t esp_console_new_repl_stdio(const esp_console_repl_config_t *repl_config, esp_console_repl_t **ret_repl)
{
    esp_err_t ret = ESP_OK;
    esp_console_repl_universal_t *universal_repl = NULL;
    if (!repl_config || !ret_repl) {
        ret = ESP_ERR_INVALID_ARG;
        goto _exit;
    }

    // allocate memory for console REPL context
    universal_repl = calloc(1, sizeof(esp_console_repl_universal_t));
    if (!universal_repl) {
        ret = ESP_ERR_NO_MEM;
        goto _exit;
    }

    /* Drain stdout before reconfiguring it */
    fflush(stdout);
    fsync(fileno(stdout));

    /* the IO related initialization will be performed within the task
     * created to run esp_console_repl_task */

    /* initialize console, common part */
    ret = esp_console_common_init(repl_config->max_cmdline_length,
                                  &universal_repl->repl_com);
    if (ret != ESP_OK) {
        goto _exit;
    }

    /* setup history */
    ret = esp_console_setup_history(repl_config->history_save_path, repl_config->max_history_len, &universal_repl->repl_com);
    if (ret != ESP_OK) {
        goto _exit;
    }

    /* setup prompt */
    ret = esp_console_setup_prompt(repl_config->prompt, &universal_repl->repl_com);
    if (ret != ESP_OK) {
        goto _exit;
    }

    ret = esp_stdio_install_io_driver();
    if (ret != ESP_OK) {
        goto _exit;
    }

    /* Fill the structure here as it will be used directly by the created task. */
    universal_repl->repl_com.state = CONSOLE_REPL_STATE_INIT;
    universal_repl->repl_com.repl_core.del = esp_console_delete_repl_stdio;

    /* Spawn a single thread to run REPL, we need to pass `universal_repl` to it as
     * it also requires the uart channel. */
    if (xTaskCreatePinnedToCore(esp_console_repl_task, "console_repl", repl_config->task_stack_size,
                                universal_repl, repl_config->task_priority, &universal_repl->repl_com.task_hdl, repl_config->task_core_id) != pdTRUE) {
        ret = ESP_FAIL;
        goto _exit;
    }

    *ret_repl = &universal_repl->repl_com.repl_core;
    return ESP_OK;
_exit:
    if (universal_repl) {
        esp_console_deinit();
        free(universal_repl);
    }
    if (ret_repl) {
        *ret_repl = NULL;
    }
    return ret;
}

esp_err_t esp_console_start_repl(esp_console_repl_t *repl)
{
    esp_console_repl_com_t *repl_com = (esp_console_repl_com_t*)repl;
    // check if already initialized
    if (repl_com->state != CONSOLE_REPL_STATE_INIT) {
        return ESP_ERR_INVALID_STATE;
    }

    repl_com->state = CONSOLE_REPL_STATE_START;
    xTaskNotifyGive(repl_com->task_hdl);
    return ESP_OK;
}
