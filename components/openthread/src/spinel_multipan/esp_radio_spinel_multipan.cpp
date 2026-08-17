/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

#include "common/code_utils.hpp"
#include "driver/uart.h"
#include "driver/uart_vfs.h"
#include "esp_check.h"
#include "esp_openthread_common_macro.h"
#include "esp_radio_spinel_host.h"
#include "esp_radio_spinel_multipan.h"
#include "esp_radio_spinel_uart_transport.hpp"
#include "esp_vfs_dev.h"
#include "esp_vfs_eventfd.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "hdlc.hpp"
#include "lib/spinel/spinel.h"
#include "lib/spinel/spinel_interface.hpp"
#include "openthread/error.h"
#include "openthread/platform/time.h"

static const char *TAG = "spinel_multipan";

#define MULTIPAN_RX_QUEUE_DEPTH     8
#define MULTIPAN_WORKER_STACK_SIZE  8192
#define MULTIPAN_WORKER_PRIORITY    6
#define MULTIPAN_WRITE_TIMEOUT_MS   2000

static constexpr uint16_t kMaxSpinelFrame = ot::Spinel::SpinelInterface::kMaxFrameSize;

typedef struct {
    uint16_t len;
    uint8_t buf[kMaxSpinelFrame];
} multipan_frame_msg_t;

typedef struct {
    bool in_use;
    int wait_fd;
    QueueHandle_t rx_queue;
    uint32_t drop_count;
    ot::Spinel::SpinelInterface::RxFrameBuffer *rx_buffer;
    ot::Spinel::SpinelInterface::ReceiveFrameCallback callback;
    void *callback_context;
} multipan_slot_t;

static SemaphoreHandle_t s_uart_tx_lock = NULL;
static SemaphoreHandle_t s_slots_lock = NULL;
static bool s_multipan_inited = false;

static esp_radio_spinel_uart_config_t s_uart_config = {};

static int s_uart_port = -1;
static int s_uart_fd = -1;
static int s_uart_rx_stop_fd = -1;
static TaskHandle_t s_uart_rx_task = NULL;
static SemaphoreHandle_t s_uart_rx_exit_sem = NULL;

static ot::Spinel::FrameBuffer<kMaxSpinelFrame> s_hdlc_frame_buffer;
static ot::Spinel::FrameBuffer<kMaxSpinelFrame> s_encoder_buffer;
static ot::Hdlc::Decoder s_hdlc_decoder;

static uint8_t s_registered_count = 0;
static multipan_slot_t s_slots[ESP_RADIO_SPINEL_HOST_INTERFACE_COUNT];

static bool iid_valid(int8_t iid)
{
    return iid >= 0 && iid < ESP_RADIO_SPINEL_HOST_INTERFACE_COUNT;
}

static esp_err_t wait_uart_writable(int uart_fd, uint32_t timeout_ms)
{
    struct timeval timeout = {
        static_cast<time_t>(timeout_ms / MS_PER_S),
        static_cast<suseconds_t>((timeout_ms % MS_PER_S) * US_PER_MS),
    };
    uint64_t now = otPlatTimeGet();
    uint64_t end = now + static_cast<uint64_t>(timeout_ms) * US_PER_MS;

    while (true) {
        fd_set write_fds;
        fd_set error_fds;
        FD_ZERO(&write_fds);
        FD_ZERO(&error_fds);
        FD_SET(uart_fd, &write_fds);
        FD_SET(uart_fd, &error_fds);

        int rval = select(uart_fd + 1, NULL, &write_fds, &error_fds, &timeout);
        if (rval > 0) {
            if (FD_ISSET(uart_fd, &write_fds)) {
                return ESP_OK;
            }
            if (FD_ISSET(uart_fd, &error_fds)) {
                return ESP_FAIL;
            }
        } else if ((rval < 0) && (errno != EINTR)) {
            return ESP_FAIL;
        }

        now = otPlatTimeGet();
        if (end <= now) {
            break;
        }
        uint64_t remain = end - now;
        timeout.tv_sec = static_cast<time_t>(remain / US_PER_S);
        timeout.tv_usec = static_cast<suseconds_t>(remain % US_PER_S);
    }
    return ESP_ERR_TIMEOUT;
}

static esp_err_t multipan_uart_open(void)
{
    char uart_path[32];
    snprintf(uart_path, sizeof(uart_path), "/dev/uart/%d", s_uart_config.port);

    bool is_uart_registered = (access(uart_path, F_OK) == 0);
    if (!is_uart_registered) {
        uart_vfs_dev_register();
    }

    ESP_RETURN_ON_ERROR(uart_param_config(s_uart_config.port, &s_uart_config.uart_config), TAG,
                        "uart_param_config failed");
    ESP_RETURN_ON_ERROR(
        uart_set_pin(s_uart_config.port, s_uart_config.tx_pin, s_uart_config.rx_pin, UART_PIN_NO_CHANGE,
                     UART_PIN_NO_CHANGE),
        TAG, "uart_set_pin failed");
    ESP_RETURN_ON_ERROR(uart_driver_install(s_uart_config.port, CONFIG_OPENTHREAD_SPINEL_UART_DRIVER_BUFFER_SIZE, 0, 0,
                                            NULL, 0),
                        TAG, "uart_driver_install failed");
    uart_vfs_dev_use_driver(s_uart_config.port);
    uart_vfs_dev_port_set_tx_line_endings(s_uart_config.port, ESP_LINE_ENDINGS_LF);
    uart_vfs_dev_port_set_rx_line_endings(s_uart_config.port, ESP_LINE_ENDINGS_LF);

    s_uart_fd = open(uart_path, O_RDWR | O_NONBLOCK);
    if (s_uart_fd < 0) {
        (void)uart_driver_delete(s_uart_config.port);
        ESP_LOGE(TAG, "open UART failed: %s", uart_path);
        return ESP_FAIL;
    }
    s_uart_port = s_uart_config.port;
    return ESP_OK;
}

static void multipan_uart_close(void)
{
    if (s_uart_fd >= 0) {
        int uart_fd = s_uart_fd;
        s_uart_fd = -1;
        close(uart_fd);
    }
    if (s_uart_port >= 0) {
        (void)uart_driver_delete(static_cast<uart_port_t>(s_uart_port));
        s_uart_port = -1;
    }
}

static ssize_t multipan_uart_write(const uint8_t *data, size_t size)
{
    int uart_fd = s_uart_fd;
    ESP_RETURN_ON_FALSE(uart_fd >= 0, (errno = EIO, -1), TAG, "UART is not open");

    const uint8_t *bytes = data;
    size_t remaining = size;
    uint64_t start_us = otPlatTimeGet();

    while (remaining > 0) {
        ssize_t written = write(uart_fd, bytes, remaining);
        if (written > 0) {
            remaining -= static_cast<size_t>(written);
            bytes += written;
            continue;
        }
        if (written < 0 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
            return -1;
        }

        uint64_t now = otPlatTimeGet();
        uint64_t elapsed_ms = (now - start_us) / US_PER_MS;
        ESP_RETURN_ON_FALSE(elapsed_ms < MULTIPAN_WRITE_TIMEOUT_MS, (errno = EIO, -1), TAG, "UART write timeout");
        ESP_RETURN_ON_FALSE(wait_uart_writable(uart_fd, MULTIPAN_WRITE_TIMEOUT_MS - static_cast<uint32_t>(elapsed_ms)) ==
                                ESP_OK,
                            (errno = EIO, -1), TAG, "UART not writable");
    }
    return static_cast<ssize_t>(size);
}

static void multipan_signal_slot(multipan_slot_t *slot)
{
    if (slot->wait_fd < 0) {
        return;
    }
    uint64_t one = 1;
    ssize_t w = write(slot->wait_fd, &one, sizeof(one));
    (void)w;
}

static void multipan_enqueue_frame(int idx, const uint8_t *frame, uint16_t len)
{
    ESP_RETURN_ON_FALSE(idx >= 0 && idx < ESP_RADIO_SPINEL_HOST_INTERFACE_COUNT, , TAG, "invalid client idx %d", idx);

    multipan_slot_t *slot = &s_slots[idx];
    multipan_frame_msg_t msg = {};
    msg.len = len;
    memcpy(msg.buf, frame, len);

    xSemaphoreTake(s_slots_lock, portMAX_DELAY);
    if (!slot->in_use || slot->rx_queue == NULL) {
        xSemaphoreGive(s_slots_lock);
        return;
    }
    if (xQueueSend(slot->rx_queue, &msg, 0) == pdTRUE) {
        multipan_signal_slot(slot);
    } else {
        slot->drop_count++;
        ESP_LOGW(TAG, "client %d rx queue full, dropped frame (drops=%lu)", idx, (unsigned long)slot->drop_count);
    }
    xSemaphoreGive(s_slots_lock);
}

static void multipan_fanout_spinel_frame(const uint8_t *frame, uint16_t len, otError error)
{
    ESP_RETURN_ON_FALSE(error == OT_ERROR_NONE, , TAG, "dropped invalid HDLC frame, error=%d", static_cast<int>(error));
    ESP_RETURN_ON_FALSE(len > 0 && len <= kMaxSpinelFrame, , TAG, "dropped decoded frame with invalid length=%u", len);

    spinel_iid_t iid = SPINEL_HEADER_GET_IID(frame[0]);
    if (iid == SPINEL_HEADER_GET_IID(OPENTHREAD_SPINEL_CONFIG_BROADCAST_IID)) {
        for (int i = 0; i < ESP_RADIO_SPINEL_HOST_INTERFACE_COUNT; i++) {
            multipan_enqueue_frame(i, frame, len);
        }
    } else if (iid < ESP_RADIO_SPINEL_HOST_INTERFACE_COUNT) {
        multipan_enqueue_frame(iid, frame, len);
    } else {
        ESP_LOGW(TAG, "dropped frame with unsupported iid=%u", (unsigned)iid);
    }
}

static void multipan_hdlc_frame_handler(void *context, otError error)
{
    (void)context;
    multipan_fanout_spinel_frame(s_hdlc_frame_buffer.GetFrame(), s_hdlc_frame_buffer.GetLength(), error);
    s_hdlc_frame_buffer.Clear();
}

static void multipan_uart_rx_task(void *context)
{
    (void)context;
    uint8_t uart_rx_buf[256];
    const int uart_fd = s_uart_fd;
    const int stop_fd = s_uart_rx_stop_fd;

    while (true) {
        if (stop_fd < 0 || uart_fd < 0) {
            break;
        }

        fd_set read_fds;
        fd_set error_fds;
        FD_ZERO(&read_fds);
        FD_ZERO(&error_fds);
        FD_SET(uart_fd, &read_fds);
        FD_SET(uart_fd, &error_fds);
        FD_SET(stop_fd, &read_fds);
        int max_fd = (uart_fd > stop_fd) ? uart_fd : stop_fd;

        int ret = select(max_fd + 1, &read_fds, NULL, &error_fds, NULL);
        if (ret <= 0) {
            continue;
        }
        if (FD_ISSET(stop_fd, &read_fds)) {
            uint64_t stop_signal = 0;
            (void)read(stop_fd, &stop_signal, sizeof(stop_signal));
            break;
        }
        if (FD_ISSET(uart_fd, &error_fds)) {
            continue;
        }

        ssize_t read_len = 0;
        do {
            read_len = read(uart_fd, uart_rx_buf, sizeof(uart_rx_buf));
            if (read_len <= 0) {
                break;
            }
            s_hdlc_decoder.Decode(uart_rx_buf, static_cast<uint16_t>(read_len));
        } while (read_len > 0);
    }

    s_uart_rx_task = NULL;
    if (s_uart_rx_exit_sem != NULL) {
        xSemaphoreGive(s_uart_rx_exit_sem);
    }
    vTaskDelete(NULL);
}

static void multipan_uart_rx_stop(void)
{
    if (s_uart_rx_task == NULL) {
        return;
    }
    uint64_t stop_signal = 1;
    (void)write(s_uart_rx_stop_fd, &stop_signal, sizeof(stop_signal));
    (void)xSemaphoreTake(s_uart_rx_exit_sem, portMAX_DELAY);
}

static void multipan_locks_deinit(void)
{
    if (s_uart_tx_lock != NULL) {
        vSemaphoreDelete(s_uart_tx_lock);
        s_uart_tx_lock = NULL;
    }
    if (s_slots_lock != NULL) {
        vSemaphoreDelete(s_slots_lock);
        s_slots_lock = NULL;
    }
}

static esp_err_t multipan_locks_init(void)
{
    s_uart_tx_lock = xSemaphoreCreateMutex();
    ESP_RETURN_ON_FALSE(s_uart_tx_lock != NULL, ESP_ERR_NO_MEM, TAG, "failed to create UART TX mutex");
    s_slots_lock = xSemaphoreCreateMutex();
    if (s_slots_lock == NULL) {
        vSemaphoreDelete(s_uart_tx_lock);
        s_uart_tx_lock = NULL;
        ESP_LOGE(TAG, "failed to create slots mutex");
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

static void multipan_rx_deinit(void)
{
    multipan_uart_rx_stop();
    if (s_uart_rx_stop_fd >= 0) {
        close(s_uart_rx_stop_fd);
        s_uart_rx_stop_fd = -1;
    }
    if (s_uart_rx_exit_sem != NULL) {
        vSemaphoreDelete(s_uart_rx_exit_sem);
        s_uart_rx_exit_sem = NULL;
    }
}

static esp_err_t multipan_rx_init(void)
{
    s_uart_rx_exit_sem = xSemaphoreCreateBinary();
    ESP_RETURN_ON_FALSE(s_uart_rx_exit_sem != NULL, ESP_ERR_NO_MEM, TAG, "failed to create worker exit sem");
    s_uart_rx_stop_fd = eventfd(0, 0);
    ESP_RETURN_ON_FALSE(s_uart_rx_stop_fd >= 0, ESP_FAIL, TAG, "failed to create worker stop fd");

    s_hdlc_frame_buffer.Clear();
    s_hdlc_decoder.Init(s_hdlc_frame_buffer, multipan_hdlc_frame_handler, NULL);

    s_uart_rx_task = NULL;
    ESP_RETURN_ON_FALSE(xTaskCreate(multipan_uart_rx_task, "ot_mpan_uart", MULTIPAN_WORKER_STACK_SIZE, NULL,
                                    MULTIPAN_WORKER_PRIORITY, &s_uart_rx_task) == pdPASS,
                        ESP_ERR_NO_MEM, TAG, "failed to create multipan worker");
    return ESP_OK;
}

static void multipan_slots_reset(void)
{
    for (int i = 0; i < ESP_RADIO_SPINEL_HOST_INTERFACE_COUNT; i++) {
        s_slots[i].wait_fd = -1;
        s_slots[i].rx_queue = NULL;
        s_slots[i].in_use = false;
        s_slots[i].drop_count = 0;
        s_slots[i].rx_buffer = nullptr;
        s_slots[i].callback = nullptr;
        s_slots[i].callback_context = nullptr;
    }
}

static void multipan_slot_release(int idx)
{
    multipan_slot_t *slot = &s_slots[idx];
    if (slot->wait_fd >= 0) {
        close(slot->wait_fd);
        slot->wait_fd = -1;
    }
    if (slot->rx_queue != NULL) {
        vQueueDelete(slot->rx_queue);
        slot->rx_queue = NULL;
    }
    slot->in_use = false;
    slot->drop_count = 0;
    slot->rx_buffer = nullptr;
    slot->callback = nullptr;
    slot->callback_context = nullptr;
    if (s_registered_count > 0) {
        s_registered_count--;
    }
}

static void multipan_host_channel_deinit(QueueHandle_t *rx_queue, int *event_fd)
{
    if (event_fd != NULL && *event_fd >= 0) {
        close(*event_fd);
        *event_fd = -1;
    }
    if (rx_queue != NULL && *rx_queue != NULL) {
        vQueueDelete(*rx_queue);
        *rx_queue = NULL;
    }
}

static esp_err_t multipan_host_channel_init(QueueHandle_t *rx_queue, int *event_fd)
{
    *rx_queue = xQueueCreate(MULTIPAN_RX_QUEUE_DEPTH, sizeof(multipan_frame_msg_t));
    ESP_RETURN_ON_FALSE(*rx_queue != NULL, ESP_ERR_NO_MEM, TAG, "failed to create rx queue");
    *event_fd = eventfd(0, 0);
    if (*event_fd < 0) {
        vQueueDelete(*rx_queue);
        *rx_queue = NULL;
        ESP_LOGE(TAG, "failed to create client eventfd");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t multipan_slot_claim(QueueHandle_t rx_queue, int event_fd, int8_t *iid)
{
    int8_t assigned = -1;

    for (int8_t i = 0; i < ESP_RADIO_SPINEL_HOST_INTERFACE_COUNT; i++) {
        if (!s_slots[i].in_use) {
            assigned = i;
            break;
        }
    }
    ESP_RETURN_ON_FALSE(assigned >= 0, ESP_ERR_NO_MEM, TAG, "no free spinel iid");

    s_slots[assigned].in_use = true;
    s_slots[assigned].wait_fd = event_fd;
    s_slots[assigned].rx_queue = rx_queue;
    s_slots[assigned].drop_count = 0;
    s_slots[assigned].rx_buffer = nullptr;
    s_slots[assigned].callback = nullptr;
    s_slots[assigned].callback_context = nullptr;
    s_registered_count++;
    *iid = assigned;
    return ESP_OK;
}

esp_err_t esp_radio_spinel_multipan_init(const esp_radio_spinel_multipan_radio_config_t *radio_config)
{
    esp_err_t ret = ESP_OK;

    ESP_RETURN_ON_FALSE(!s_multipan_inited, ESP_ERR_INVALID_STATE, TAG, "multipan already initialized");
    ESP_RETURN_ON_FALSE(radio_config != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid radio config");
    ESP_RETURN_ON_FALSE(radio_config->radio_mode == RADIO_MODE_UART_RCP, ESP_ERR_NOT_SUPPORTED, TAG,
                        "only RADIO_MODE_UART_RCP is currently supported");

    ESP_GOTO_ON_ERROR(multipan_locks_init(), fail, TAG, "failed to init locks");

    s_uart_config = radio_config->radio_uart_config;
    ESP_GOTO_ON_ERROR(multipan_uart_open(), fail, TAG, "failed to open UART");
    ESP_GOTO_ON_ERROR(multipan_rx_init(), fail, TAG, "failed to start RX task");
    multipan_slots_reset();

    s_multipan_inited = true;
    return ESP_OK;

fail:
    multipan_rx_deinit();
    multipan_uart_close();
    memset(&s_uart_config, 0, sizeof(s_uart_config));
    multipan_locks_deinit();
    return ret;
}

esp_err_t esp_radio_spinel_multipan_deinit(void)
{
    ESP_RETURN_ON_FALSE(s_multipan_inited, ESP_ERR_INVALID_STATE, TAG, "multipan not initialized");
    ESP_RETURN_ON_FALSE(s_registered_count == 0, ESP_ERR_INVALID_STATE, TAG,
                        "deinit rejected: %u client(s) still open", s_registered_count);

    s_multipan_inited = false;
    multipan_slots_reset();
    multipan_rx_deinit();
    multipan_uart_close();
    memset(&s_uart_config, 0, sizeof(s_uart_config));
    multipan_locks_deinit();
    return ESP_OK;
}

esp_err_t esp_radio_spinel_uart_transport_open(const esp_radio_spinel_uart_config_t *config,
                                               const esp_radio_spinel_uart_transport_hooks_t *hooks, int8_t *iid,
                                               int *wait_fd)
{
    esp_err_t ret = ESP_OK;
    QueueHandle_t rx_queue = NULL;
    int event_fd = -1;

    (void)hooks;
    (void)config;

    ESP_RETURN_ON_FALSE(wait_fd != NULL && iid != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid transport open args");
    ESP_RETURN_ON_FALSE(s_multipan_inited, ESP_ERR_INVALID_STATE, TAG, "multipan not initialized");

    ESP_GOTO_ON_ERROR(multipan_host_channel_init(&rx_queue, &event_fd), fail, TAG, "failed to init host channel");
    xSemaphoreTake(s_slots_lock, portMAX_DELAY);
    ret = multipan_slot_claim(rx_queue, event_fd, iid);
    xSemaphoreGive(s_slots_lock);
    ESP_GOTO_ON_ERROR(ret, fail, TAG, "failed to claim spinel slot");

    *wait_fd = event_fd;
    ESP_LOGI(TAG, "allocated spinel iid=%d", *iid);
    return ESP_OK;

fail:
    multipan_host_channel_deinit(&rx_queue, &event_fd);
    return ret;
}

esp_err_t esp_radio_spinel_uart_transport_close(int8_t iid)
{
    ESP_RETURN_ON_FALSE(iid_valid(iid), ESP_ERR_INVALID_ARG, TAG, "invalid iid");
    if (!s_multipan_inited) {
        return ESP_OK;
    }

    xSemaphoreTake(s_slots_lock, portMAX_DELAY);
    if (s_slots[iid].in_use) {
        multipan_slot_release(iid);
    }
    xSemaphoreGive(s_slots_lock);
    return ESP_OK;
}

int esp_radio_spinel_uart_transport_read(int8_t iid)
{
    ESP_RETURN_ON_FALSE(iid_valid(iid), (errno = EINVAL, -1), TAG, "invalid iid");

    multipan_slot_t *slot = &s_slots[iid];
    ESP_RETURN_ON_FALSE(slot->in_use && slot->wait_fd >= 0 && slot->rx_queue != NULL, (errno = EBADF, -1), TAG, "iid is not open");

    uint64_t v = 0;
    (void)read(slot->wait_fd, &v, sizeof(v));

    multipan_frame_msg_t msg = {};
    while (xQueueReceive(slot->rx_queue, &msg, 0) == pdTRUE) {
        if (slot->rx_buffer == nullptr || slot->callback == nullptr) {
            continue;
        }
        uint16_t max_len = slot->rx_buffer->GetFrameMaxLength();
        if (msg.len == 0 || msg.len > max_len) {
            ESP_LOGW(TAG, "dropping spinel frame: len=%u max=%u", msg.len, max_len);
            continue;
        }
        memcpy(slot->rx_buffer->GetFrame(), msg.buf, msg.len);
        otError error = slot->rx_buffer->SetLength(msg.len);
        if (error != OT_ERROR_NONE) {
            ESP_LOGE(TAG, "dropping spinel frame: %s", otThreadErrorToString(error));
            slot->rx_buffer->DiscardFrame();
            continue;
        }
        ESP_LOGD(TAG, "received spinel frame iid=%d", iid);
        slot->callback(slot->callback_context);
    }
    return 0;
}

ssize_t esp_radio_spinel_uart_transport_write(int8_t iid, const void *buf, size_t len)
{
    ESP_RETURN_ON_FALSE(iid_valid(iid) && buf != NULL, (errno = EINVAL, -1), TAG, "invalid write args");
    if (len == 0) {
        return 0;
    }
    if (len > kMaxSpinelFrame) {
        errno = EMSGSIZE;
        return -1;
    }

    ESP_RETURN_ON_FALSE(s_slots[iid].in_use, (errno = EBADF, -1), TAG, "iid is not open");

    otError error = OT_ERROR_NONE;
    ssize_t ret = -1;

    xSemaphoreTake(s_uart_tx_lock, portMAX_DELAY);
    s_encoder_buffer.Clear();
    ot::Hdlc::Encoder hdlc_encoder(s_encoder_buffer);
    SuccessOrExit(error = hdlc_encoder.BeginFrame());
    SuccessOrExit(error = hdlc_encoder.Encode(static_cast<const uint8_t *>(buf), static_cast<uint16_t>(len)));
    SuccessOrExit(error = hdlc_encoder.EndFrame());
    ESP_GOTO_ON_FALSE(multipan_uart_write(s_encoder_buffer.GetFrame(), s_encoder_buffer.GetLength()) ==
                          static_cast<ssize_t>(s_encoder_buffer.GetLength()),
                      -1, exit, TAG, "UART write failed");
    ret = static_cast<ssize_t>(len);

exit:
    xSemaphoreGive(s_uart_tx_lock);
    if (error != OT_ERROR_NONE && ret < 0) {
        errno = EIO;
    }
    return ret;
}

esp_err_t esp_radio_spinel_uart_transport_recover(int8_t iid)
{
    /* UART and HDLC belong to the RX task, shared by all hosts. Re-installing
     * the port from one iid would drop the other. Host wait_fd is an eventfd
     * and stays valid. RCP-level recovery is left to RadioSpinel. */
    (void)iid;
    return ESP_OK;
}

uint32_t esp_radio_spinel_uart_transport_get_bus_speed(int8_t iid)
{
    (void)iid;
    return s_uart_config.uart_config.baud_rate;
}

esp_err_t esp_radio_spinel_uart_transport_bind_rx(int8_t iid,
                                                  ot::Spinel::SpinelInterface::ReceiveFrameCallback callback,
                                                  void *context,
                                                  ot::Spinel::SpinelInterface::RxFrameBuffer *frame_buffer)
{
    ESP_RETURN_ON_FALSE(iid_valid(iid), ESP_ERR_INVALID_ARG, TAG, "invalid iid");
    ESP_RETURN_ON_FALSE(frame_buffer != nullptr, ESP_ERR_INVALID_ARG, TAG, "invalid rx buffer");
    ESP_RETURN_ON_FALSE(s_slots[iid].in_use, ESP_ERR_INVALID_STATE, TAG, "iid is not open");

    s_slots[iid].rx_buffer = frame_buffer;
    s_slots[iid].callback = callback;
    s_slots[iid].callback_context = context;
    return ESP_OK;
}

void esp_radio_spinel_uart_transport_unbind_rx(int8_t iid)
{
    if (!iid_valid(iid)) {
        return;
    }
    s_slots[iid].rx_buffer = nullptr;
    s_slots[iid].callback = nullptr;
    s_slots[iid].callback_context = nullptr;
}
