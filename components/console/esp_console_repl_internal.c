/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <unistd.h>
#include <sys/cdefs.h> // __containerof
#include <sys/param.h>
#include <sys/fcntl.h>
#include "esp_console.h"
#include "console_private.h"
#include "esp_log.h"
#include "linenoise/linenoise.h"

#include "esp_vfs_eventfd.h"

#if CONFIG_VFS_SUPPORT_SELECT

static const char *TAG = "console.repl.internal";

static int s_interrupt_reading_fd = -1;
static uint64_t s_interrupt_reading_signal = 1;

static ssize_t read_bytes(int fd, void *buf, size_t max_bytes)
{
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);
    if (s_interrupt_reading_fd != -1) {
        FD_SET(s_interrupt_reading_fd, &read_fds);
    }
    int maxFd = MAX(fd, s_interrupt_reading_fd);
    /* call select to wait for either a read ready or an except to happen */
    int nread = select(maxFd + 1, &read_fds, NULL, NULL, NULL);
    if (nread < 0) {
        return -1;
    }

    if (FD_ISSET(s_interrupt_reading_fd, &read_fds)) {
        /* read termination request happened, return */
        int buf[sizeof(s_interrupt_reading_signal)];
        nread = read(s_interrupt_reading_fd, buf, sizeof(s_interrupt_reading_signal));
        if ((nread == sizeof(s_interrupt_reading_signal)) && (buf[0] == s_interrupt_reading_signal)) {
            return -1;
        }
    } else if (FD_ISSET(fd, &read_fds)) {
        /* a read ready triggered the select to return. call the
         * read function with the number of bytes max_bytes */
        nread = read(fd, buf, max_bytes);
    }
    return nread;
}

/* Strong definition of the weak definition of linenoiseSetReadCharacteristics in
 * linenoise.c. This function set the read to be non blocking and set the read
 * function used by linenoise to read_bytes. This function is compiled only if
 * esp_console_stop_repl is used. */
void linenoiseSetReadCharacteristics(void)
{
    /* Make sure we are using non blocking reads with
     * the select */
    int stdin_fileno = fileno(stdin);
    int flags = fcntl(stdin_fileno, F_GETFL);
    flags |= O_NONBLOCK;
    (void)fcntl(stdin_fileno, F_SETFL, flags);

    linenoiseSetReadFunction(read_bytes);
}

/* Strong definition of the weak definition of esp_console_internal_set_event_fd
 * in esp_console_common to allow the use of select with non blocking
 * read. This function is compiled only if esp_console_stop_repl
 * is used. */
esp_err_t esp_console_internal_set_event_fd(esp_console_repl_com_t *repl_com)
{
    /* Tell linenoise what file descriptor to add to the read file descriptor set,
     * that will be used to signal a read termination */
    esp_vfs_eventfd_config_t config = ESP_VFS_EVENTD_CONFIG_DEFAULT();
    esp_err_t ret = esp_vfs_eventfd_register(&config);
    if (ret == ESP_OK) {
        /* first time calling the eventfd register function */
        s_interrupt_reading_fd = eventfd(0, 0);
    } else if (ret == ESP_ERR_INVALID_STATE) {
        /* the evenfd has already been registered*/
    } else {
        /* issue with arg, this should not happen */
        return ESP_FAIL;
    }

    repl_com->state_mux = xSemaphoreCreateMutex();
    if (repl_com->state_mux == NULL) {
        ESP_LOGE(TAG, "state_mux create error");
        return ESP_ERR_NO_MEM;
    }
    xSemaphoreGive(repl_com->state_mux);

    return ESP_OK;
}

/* Strong definition of the weak definition of esp_console_common_deinit
 * in esp_console_common to allow the use of select with non blocking
 * read. This function is compiled only if esp_console_stop_repl
 * is used. */
esp_err_t esp_console_common_deinit(esp_console_repl_com_t *repl_com)
{
    // set the state to deinit to force the while loop in
    // esp_console_repl_task to break
    repl_com->state = CONSOLE_REPL_STATE_DEINIT;

    if (s_interrupt_reading_fd == -1) {
        return ESP_FAIL;
    }

    int nwrite = write(s_interrupt_reading_fd, &s_interrupt_reading_signal, sizeof(s_interrupt_reading_signal));
    if (nwrite != sizeof(s_interrupt_reading_signal)) {
        return ESP_FAIL;
    }

    // wait for the task to notify that
    // esp_console_repl_task returned
    assert(repl_com->state_mux != NULL);
    BaseType_t ret_val = xSemaphoreTake(repl_com->state_mux, portMAX_DELAY);
    assert(ret_val == pdTRUE);

    // delete the semaphore for the repl state
    vSemaphoreDelete(repl_com->state_mux);
    repl_com->state_mux =  NULL;

    /* Unregister the heap function to avoid memory leak, since it is created
     * every time a console init is called. */
    esp_err_t ret = esp_console_deregister_help_command();
    if (ret != ESP_OK) {
        return ret;
    }

    /* unregister eventfd to avoid memory leaks, since it is created every time a
     * console init is called */
    ret = esp_vfs_eventfd_unregister();
    if (ret != ESP_OK) {
        return ret;
    }

    /* free the history to avoid memory leak, since it is created
     * every time a console init is called. */
    linenoiseHistoryFree();

    return ESP_OK;
}
#endif // CONFIG_VFS_SUPPORT_SELECT

/* DO NOT move this function out of this file. All other definitions in this
 * file are strong definition of weak functions.
 *
 * Those function are used to provide a clean way to exit linenoise
 * and properly deinitialize the console by using select with non blocking
 * read instead of blocking read as the default way to read character implemented
 * in linenoise.
 *
 * If the user never calls this function, then the default read process is used and
 * those functions will be ignored by the linker. */
esp_err_t esp_console_stop_repl(esp_console_repl_t *repl)
{
    return repl->del(repl);
}
