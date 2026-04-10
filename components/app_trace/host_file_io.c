/*
 * SPDX-FileCopyrightText: 2017-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
//
// Hot It Works
// ************
//
// This module implements host file I/O protocol on top of apptrace module.
// The protocol is enough simple. It sends command with arguments to the host and receives response from it.
// Responses contains return values of respective file I/O API. This value is returned to the caller.
// Commands has the following format:
//   * Header. See esp_apptrace_fcmd_hdr_t.
//   * Operation arguments. See file operation helper structures below.

#include <string.h>
#include <stdbool.h>
#include "esp_app_trace.h"
#include "esp_log.h"

const static char *TAG = "esp_host_file_io";

#define ESP_APPTRACE_FILE_CMD_FOPEN     0x0
#define ESP_APPTRACE_FILE_CMD_FCLOSE    0x1
#define ESP_APPTRACE_FILE_CMD_FWRITE    0x2
#define ESP_APPTRACE_FILE_CMD_FREAD     0x3
#define ESP_APPTRACE_FILE_CMD_FSEEK     0x4
#define ESP_APPTRACE_FILE_CMD_FTELL     0x5
#define ESP_APPTRACE_FILE_CMD_STOP      0x6 // indicates that there is no files to transfer
#define ESP_APPTRACE_FILE_CMD_FEOF      0x7

/** File operation header */
typedef struct {
    uint8_t   cmd; ///< Command ID
} esp_apptrace_fcmd_hdr_t;

/** Helper structure for fopen */
typedef struct {
    const char *path;
    uint16_t path_len;
    const char *mode;
    uint16_t mode_len;
} esp_apptrace_fopen_args_t;

/** Helper structure for fclose */
typedef struct {
    void *file;
} esp_apptrace_fclose_args_t;

/** Helper structure for fwrite */
typedef struct {
    void *  buf;
    size_t  size;
    void *  file;
} esp_apptrace_fwrite_args_t;

/** Helper structure for fread */
typedef struct {
    size_t  size;
    void *  file;
} esp_apptrace_fread_args_t;

/** Helper structure for fseek */
typedef struct {
    long    offset;
    int     whence;
    void *  file;
} esp_apptrace_fseek_args_t;

/** Helper structure for feof */
typedef struct {
    void *file;
} esp_apptrace_feof_args_t;

/** Helper structure for ftell */
typedef struct {
    void *file;
} esp_apptrace_ftell_args_t;

#define ESP_APPTRACE_FILE_RECV_TIMEOUT_US 100000 // 100ms
#define ESP_APPTRACE_FRAME_STX 0x02
#define ESP_APPTRACE_FRAME_ETX 0x03
/**
 * Send a file command to the host.
 * For the UART destination, the command is wrapped in a frame protocol.
 * The frame protocol is as follows:
 * - STX (0x02)
 * - Length of the command and arguments (2 bytes)
 * - Command ID (1 byte)
 * - Arguments (variable length)
 * - ETX (0x03)
 */
static esp_err_t esp_apptrace_file_cmd_send(uint8_t cmd, void (*prep_args)(uint8_t *, void *), void *args, uint32_t args_len)
{
    const bool is_uart = esp_apptrace_get_destination() == ESP_APPTRACE_DEST_UART;
    const size_t payload_off    = is_uart ? 3u : 0u; // STX + LEN
    const size_t payload_len    = sizeof(cmd) + (size_t)args_len; // CMD + ARGS
    const size_t total_len      = payload_off + payload_len + (is_uart ? 1u /*ETX*/ : 0u);
    size_t inx = 0;

    if (is_uart && payload_len > 0xFFFFu) {
        return ESP_ERR_INVALID_SIZE;
    }

    ESP_EARLY_LOGV(TAG, "%s cmd:%d len:%d", __func__, cmd, total_len);
    uint8_t *buf = esp_apptrace_buffer_get(total_len, ESP_APPTRACE_TMO_INFINITE);
    if (!buf) {
        return ESP_ERR_NO_MEM;
    }

    if (is_uart) {
        buf[inx++] = ESP_APPTRACE_FRAME_STX;
        buf[inx++] = (uint8_t)(payload_len & 0xFF);
        buf[inx++] = (uint8_t)((payload_len >> 8) & 0xFF);
    }

    // Payload: CMD then ARGS
    buf[inx++] = cmd;
    if (args_len) {
        prep_args(&buf[inx], args);
        inx += args_len;
    }

    if (is_uart) {
        buf[inx++] = ESP_APPTRACE_FRAME_ETX;
    }

    // now indicate that this buffer is ready to be sent off to host
    esp_err_t ret = esp_apptrace_buffer_put(buf, ESP_APPTRACE_TMO_INFINITE);//TODO: finite tmo
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to put apptrace buffer (%d)!", ret);
        return ret;
    }

    ret = esp_apptrace_flush(ESP_APPTRACE_TMO_INFINITE);//TODO: finite tmo
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to flush apptrace buffer (%d)!", ret);
        return ret;
    }

    return ESP_OK;
}

static esp_err_t esp_apptrace_file_rsp_recv(uint8_t *buf, uint32_t buf_len)
{
    uint32_t tot_rd = 0;
    while (tot_rd < buf_len) {
        uint32_t rd_size = buf_len - tot_rd;
        esp_err_t ret = esp_apptrace_read(buf + tot_rd, &rd_size, ESP_APPTRACE_FILE_RECV_TIMEOUT_US);
        if (ret != ESP_OK) {
            ESP_EARLY_LOGE(TAG, "Failed to read (%d)!", ret);
            return ret;
        }
        ESP_EARLY_LOGV(TAG, "%s read %" PRIu32 " bytes", __FUNCTION__, rd_size);
        tot_rd += rd_size;
    }

    return ESP_OK;
}

static void esp_apptrace_fopen_args_prepare(uint8_t *buf, void *priv)
{
    esp_apptrace_fopen_args_t *args = priv;

    memcpy(buf, args->path, args->path_len);
    memcpy(buf + args->path_len, args->mode, args->mode_len);
}

void *esp_apptrace_fopen(const char *path, const char *mode)
{
    esp_apptrace_fopen_args_t cmd_args;

    ESP_EARLY_LOGV(TAG, "esp_apptrace_fopen '%s' '%s'", path, mode);
    if (path == NULL || mode == NULL) {
        return 0;
    }

    cmd_args.path = path;
    cmd_args.path_len = strlen(path) + 1;
    cmd_args.mode = mode;
    cmd_args.mode_len = strlen(mode) + 1;

    esp_err_t ret = esp_apptrace_file_cmd_send(ESP_APPTRACE_FILE_CMD_FOPEN, esp_apptrace_fopen_args_prepare,
                                               &cmd_args, cmd_args.path_len + cmd_args.mode_len);
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to send file cmd (%d)!", ret);
        return NULL;
    }

    // now read the answer
    void *resp;
    ret = esp_apptrace_file_rsp_recv((uint8_t *)&resp, sizeof(resp));
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to read response (%d)!", ret);
        return NULL;
    }

    return resp;
}

static void esp_apptrace_fclose_args_prepare(uint8_t *buf, void *priv)
{
    esp_apptrace_fclose_args_t *args = priv;

    memcpy(buf, &args->file, sizeof(args->file));
}

int esp_apptrace_fclose(void *stream)
{
    esp_apptrace_fclose_args_t cmd_args;

    cmd_args.file = stream;
    esp_err_t ret = esp_apptrace_file_cmd_send(ESP_APPTRACE_FILE_CMD_FCLOSE, esp_apptrace_fclose_args_prepare,
                                               &cmd_args, sizeof(cmd_args));
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to send file cmd (%d)!", ret);
        return EOF;
    }

    // now read the answer
    int resp;
    ret = esp_apptrace_file_rsp_recv((uint8_t *)&resp, sizeof(resp));
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to read response (%d)!", ret);
        return EOF;
    }

    return resp;
}

static void esp_apptrace_fwrite_args_prepare(uint8_t *buf, void *priv)
{
    esp_apptrace_fwrite_args_t *args = priv;

    memcpy(buf, &args->file, sizeof(args->file));
    memcpy(buf + sizeof(args->file), args->buf, args->size);
}

size_t esp_apptrace_fwrite(const void *ptr, size_t size, size_t nmemb, void *stream)
{
    esp_apptrace_fwrite_args_t cmd_args;

    ESP_EARLY_LOGV(TAG, "esp_apptrace_fwrite f %p l %d", stream, size * nmemb);

    if (ptr == NULL) {
        return 0;
    }

    cmd_args.buf = (void *)ptr;
    cmd_args.size = size * nmemb;
    cmd_args.file = stream;
    esp_err_t ret = esp_apptrace_file_cmd_send(ESP_APPTRACE_FILE_CMD_FWRITE, esp_apptrace_fwrite_args_prepare,
                                               &cmd_args, sizeof(cmd_args.file) + cmd_args.size);
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to send file cmd (%d)!", ret);
        return 0;
    }

    // now read the answer
    size_t resp;
    ret = esp_apptrace_file_rsp_recv((uint8_t *)&resp, sizeof(resp));
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to read response (%d)!", ret);
        return 0;
    }
    /* OpenOCD writes it like that:
     *      fwrite(buf, size, 1, file);
     * So, if 1 was returned that means fwrite succeed
     */
    return resp == 1 ? nmemb : 0;
}

static void esp_apptrace_fread_args_prepare(uint8_t *buf, void *priv)
{
    esp_apptrace_fread_args_t *args = priv;

    memcpy(buf, &args->file, sizeof(args->file));
    memcpy(buf + sizeof(args->file), &args->size, sizeof(args->size));
}

size_t esp_apptrace_fread(void *ptr, size_t size, size_t nmemb, void *stream)
{
    esp_apptrace_fread_args_t cmd_args;

    ESP_EARLY_LOGV(TAG, "esp_apptrace_fread f %p l %d", stream, size * nmemb);

    if (ptr == NULL) {
        return 0;
    }

    cmd_args.size = size * nmemb;
    cmd_args.file = stream;
    esp_err_t ret = esp_apptrace_file_cmd_send(ESP_APPTRACE_FILE_CMD_FREAD, esp_apptrace_fread_args_prepare,
                                               &cmd_args, sizeof(cmd_args));
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to send file cmd (%d)!", ret);
        return 0;
    }

    // now read the answer
    size_t resp;
    ret = esp_apptrace_file_rsp_recv((uint8_t *)&resp, sizeof(resp));
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to read response (%d)!", ret);
        return 0;
    }
    if (resp == 0) {
        return 0;
    }

    ret = esp_apptrace_file_rsp_recv(ptr, resp);
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to read file data (%d)!", ret);
        return 0;
    }
    /* OpenOCD reads it like that:
     *      fread(buf, 1 ,size, file);
     * So, total read bytes count returns
     */
    return resp / size; // return the number of items read
}

static void esp_apptrace_fseek_args_prepare(uint8_t *buf, void *priv)
{
    esp_apptrace_fseek_args_t *args = priv;

    memcpy(buf, &args->file, sizeof(args->file));
    memcpy(buf + sizeof(args->file), &args->offset, sizeof(args->offset));
    memcpy(buf + sizeof(args->file) + sizeof(args->offset), &args->whence, sizeof(args->whence));
}

int esp_apptrace_fseek(void *stream, long offset, int whence)
{
    esp_apptrace_fseek_args_t cmd_args;

    ESP_EARLY_LOGV(TAG, "esp_apptrace_fseek f %p o 0x%lx w %d", stream, offset, whence);

    cmd_args.file = stream;
    cmd_args.offset = offset;
    cmd_args.whence = whence;
    esp_err_t ret = esp_apptrace_file_cmd_send(ESP_APPTRACE_FILE_CMD_FSEEK, esp_apptrace_fseek_args_prepare,
                                               &cmd_args, sizeof(cmd_args));
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to send file cmd (%d)!", ret);
        return -1;
    }

    // now read the answer
    int resp;
    ret = esp_apptrace_file_rsp_recv((uint8_t *)&resp, sizeof(resp));
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to read response (%d)!", ret);
        return -1;
    }

    return resp;
}

static void esp_apptrace_ftell_args_prepare(uint8_t *buf, void *priv)
{
    esp_apptrace_ftell_args_t *args = priv;

    memcpy(buf, &args->file, sizeof(args->file));
}

int esp_apptrace_ftell(void *stream)
{
    esp_apptrace_ftell_args_t cmd_args;

    cmd_args.file = stream;
    esp_err_t ret = esp_apptrace_file_cmd_send(ESP_APPTRACE_FILE_CMD_FTELL, esp_apptrace_ftell_args_prepare,
                                               &cmd_args, sizeof(cmd_args));
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to send file cmd (%d)!", ret);
        return -1;
    }

    // now read the answer
    int resp;
    ret = esp_apptrace_file_rsp_recv((uint8_t *)&resp, sizeof(resp));
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to read response (%d)!", ret);
        return -1;
    }

    return resp;
}

int esp_apptrace_fstop(void)
{
    ESP_EARLY_LOGV(TAG, "%s", __func__);
    esp_err_t ret = esp_apptrace_file_cmd_send(ESP_APPTRACE_FILE_CMD_STOP, NULL, NULL, 0);
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to send files transfer stop cmd (%d)!", ret);
    }
    return ret;
}

static void esp_apptrace_feof_args_prepare(uint8_t *buf, void *priv)
{
    esp_apptrace_feof_args_t *args = priv;

    memcpy(buf, &args->file, sizeof(args->file));
}

int esp_apptrace_feof(void *stream)
{
    esp_apptrace_feof_args_t cmd_args;

    cmd_args.file = stream;
    esp_err_t ret = esp_apptrace_file_cmd_send(ESP_APPTRACE_FILE_CMD_FEOF, esp_apptrace_feof_args_prepare,
                                               &cmd_args, sizeof(cmd_args));
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to send file cmd (%d)!", ret);
        return EOF;
    }

    // now read the answer
    int resp;
    ret = esp_apptrace_file_rsp_recv((uint8_t *)&resp, sizeof(resp));
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to read response (%d)!", ret);
        return EOF;
    }

    return resp;
}
