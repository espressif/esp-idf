/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "unity.h"

#include "mock_http_server_client.h"

static const char *TAG = "mock_srv_client";

/* -----------------------------------------------------------------------
 * Internal helpers
 * --------------------------------------------------------------------- */

/**
 * Parse the HTTP status code from the beginning of a raw response buffer.
 * Expects "HTTP/1.x NNN ..." format.  Returns -1 on any parse failure.
 */
static int parse_status_code(const char *data, size_t len)
{
    /* Minimum: "HTTP/1.1 200 " = 13 chars */
    if (len < 13) {
        return -1;
    }
    /* Find the first space (end of "HTTP/1.x") */
    const char *p = memchr(data, ' ', len < 16 ? len : 16);
    if (!p) {
        return -1;
    }
    p++; /* point at status code digits */
    if ((size_t)(p - data) + 3 > len) {
        return -1;
    }
    int code = 0;
    for (int i = 0; i < 3; i++) {
        if (p[i] < '0' || p[i] > '9') {
            return -1;
        }
        code = code * 10 + (p[i] - '0');
    }
    /* Sanity: valid HTTP status codes are 100–599 */
    if (code < 100 || code > 599) {
        return -1;
    }
    return code;
}

/* -----------------------------------------------------------------------
 * Public API
 * --------------------------------------------------------------------- */

mock_server_response_t *mock_server_send_request(uint16_t port,
                                                   const mock_server_request_t *req)
{
    mock_server_response_t *resp = calloc(1, sizeof(*resp));
    if (!resp) {
        ESP_LOGE(TAG, "calloc() failed");
        return NULL;
    }
    resp->status_code = -1;

    /* ---- Create socket and connect ---- */
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        ESP_LOGE(TAG, "socket() failed: errno=%d", errno);
        free(resp);
        return NULL;
    }

    struct sockaddr_in addr = {
        .sin_family      = AF_INET,
        .sin_port        = htons(port),
        .sin_addr.s_addr = htonl(INADDR_LOOPBACK),
    };
    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        ESP_LOGE(TAG, "connect() to 127.0.0.1:%u failed: errno=%d", port, errno);
        close(fd);
        free(resp);
        return NULL;
    }
    ESP_LOGD(TAG, "connected to 127.0.0.1:%u", port);

    /* ---- Send the scripted request ---- */
    size_t total_len = req->len ? req->len : strlen(req->data);
    size_t sent      = 0;

    /* How many bytes to send in each write() call */
    int max_per_write = (req->max_bytes_per_write > 0)
                        ? req->max_bytes_per_write
                        : (int)total_len;

    /* After how many sent bytes to stop writing.
     * 0 or negative = send everything (zero is the zero-init default for
     * unset struct fields, so it must mean "no early close"). */
    int close_after = req->close_after_bytes;

    while (sent < total_len) {
        /* Honour early-close limit */
        if (close_after > 0 && (int)sent >= close_after) {
            break;
        }

        int chunk = max_per_write;
        /* Don't exceed the early-close byte count */
        if (close_after > 0 && (int)(sent + chunk) > close_after) {
            chunk = close_after - (int)sent;
        }
        /* Don't exceed total length */
        if ((size_t)chunk > total_len - sent) {
            chunk = (int)(total_len - sent);
        }

        int n = send(fd, req->data + sent, (size_t)chunk, 0);
        if (n <= 0) {
            ESP_LOGE(TAG, "send() failed after %zu bytes: errno=%d", sent, errno);
            close(fd);
            free(resp);
            return NULL;
        }
        sent += (size_t)n;
        ESP_LOGD(TAG, "sent %d bytes (%zu/%zu total)", n, sent, total_len);

        if (req->write_delay_ms > 0 && sent < total_len) {
            vTaskDelay(pdMS_TO_TICKS(req->write_delay_ms));
        }
    }

    /* Signal end-of-request to the server if we stopped sending early */
    if (close_after > 0) {
        shutdown(fd, SHUT_WR);
        ESP_LOGD(TAG, "shutdown(SHUT_WR) after %zu bytes", sent);
    }

    /* ---- Read the response ---- */
    int timeout_ms = (req->recv_timeout_ms > 0) ? req->recv_timeout_ms : 2000;

    /* Record absolute deadline so that EINTR retries don't reset the clock. */
    struct timeval deadline;
    gettimeofday(&deadline, NULL);
    deadline.tv_sec  += timeout_ms / 1000;
    deadline.tv_usec += (timeout_ms % 1000) * 1000;
    if (deadline.tv_usec >= 1000000) {
        deadline.tv_sec++;
        deadline.tv_usec -= 1000000;
    }

    size_t received = 0;
    while (received < sizeof(resp->data) - 1) {
        /* Calculate remaining time until deadline. */
        struct timeval now, remaining;
        gettimeofday(&now, NULL);
        remaining.tv_sec  = deadline.tv_sec  - now.tv_sec;
        remaining.tv_usec = deadline.tv_usec - now.tv_usec;
        if (remaining.tv_usec < 0) {
            remaining.tv_sec--;
            remaining.tv_usec += 1000000;
        }
        if (remaining.tv_sec < 0) {
            ESP_LOGD(TAG, "recv deadline elapsed after %zu bytes", received);
            break;
        }

        /* Wait for data (or deadline) — handles EINTR safely. */
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);
        int ready = select(fd + 1, &read_fds, NULL, NULL, &remaining);
        if (ready < 0) {
            if (errno == EINTR) {
                continue; /* signal interrupted select — recalculate remaining time */
            }
            ESP_LOGE(TAG, "select() error: errno=%d", errno);
            break;
        }
        if (ready == 0) {
            ESP_LOGD(TAG, "recv deadline elapsed after %zu bytes", received);
            break;
        }

        int n = recv(fd, resp->data + received, sizeof(resp->data) - 1 - received, 0);
        if (n < 0) {
            if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            ESP_LOGE(TAG, "recv() error after %zu bytes: errno=%d", received, errno);
            break;
        }
        if (n == 0) {
            resp->server_closed = true;
            ESP_LOGD(TAG, "server closed connection after %zu bytes", received);
            break;
        }
        received += (size_t)n;
    }

    resp->len            = received;
    resp->data[received] = '\0'; /* NUL-terminate for easy string inspection */

    if (received > 0) {
        resp->status_code = parse_status_code(resp->data, received);
        ESP_LOGD(TAG, "response: status=%d, len=%zu", resp->status_code, received);
    } else {
        ESP_LOGW(TAG, "no response received from server");
    }

    close(fd);
    return resp;
}

void mock_server_response_free(mock_server_response_t *resp)
{
    free(resp);
}

void mock_server_assert_status(const mock_server_response_t *resp, int expected_status)
{
    if (resp->len == 0) {
        TEST_FAIL_MESSAGE("mock_server: no response received from server");
        return;
    }
    if (resp->status_code == -1) {
        /* Print the raw response to help diagnose the issue */
        ESP_LOGE(TAG, "unparsable response (%zu bytes): %.80s", resp->len, resp->data);
        TEST_FAIL_MESSAGE("mock_server: server response is not a valid HTTP status line");
        return;
    }
    TEST_ASSERT_EQUAL_MESSAGE(expected_status, resp->status_code,
                              "HTTP status code mismatch");
}
