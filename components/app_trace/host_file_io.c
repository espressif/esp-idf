/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
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
#include "esp_app_trace.h"

#if CONFIG_APPTRACE_ENABLE

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

static esp_err_t esp_apptrace_file_cmd_send(esp_apptrace_dest_t dest, uint8_t cmd, void (*prep_args)(uint8_t *, void *), void *args, uint32_t args_len)
{
    esp_err_t ret;
    esp_apptrace_fcmd_hdr_t *hdr;

    ESP_EARLY_LOGV(TAG, "%s %d", __func__, cmd);
    uint8_t *ptr = esp_apptrace_buffer_get(dest, sizeof(*hdr) + args_len, ESP_APPTRACE_TMO_INFINITE); //TODO: finite tmo
    if (ptr == NULL) {
        return ESP_ERR_NO_MEM;
    }

    hdr = (esp_apptrace_fcmd_hdr_t *)ptr;
    hdr->cmd = cmd;
    if (prep_args) {
        prep_args(ptr + sizeof(hdr->cmd), args);
    }

    // now indicate that this buffer is ready to be sent off to host
    ret = esp_apptrace_buffer_put(dest, ptr, ESP_APPTRACE_TMO_INFINITE);//TODO: finite tmo
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to put apptrace buffer (%d)!", ret);
        return ret;
    }

    ret = esp_apptrace_flush(dest, ESP_APPTRACE_TMO_INFINITE);//TODO: finite tmo
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to flush apptrace buffer (%d)!", ret);
        return ret;
    }

    return ESP_OK;
}

static esp_err_t esp_apptrace_file_rsp_recv(esp_apptrace_dest_t dest, uint8_t *buf, uint32_t buf_len)
{
    uint32_t tot_rd = 0;
    while (tot_rd < buf_len) {
        uint32_t rd_size = buf_len - tot_rd;
        esp_err_t ret = esp_apptrace_read(dest, buf + tot_rd, &rd_size, ESP_APPTRACE_TMO_INFINITE); //TODO: finite tmo
        if (ret != ESP_OK) {
            ESP_EARLY_LOGE(TAG, "Failed to read (%d)!", ret);
            return ret;
        }
        ESP_EARLY_LOGV(TAG, "%s read %d bytes", __FUNCTION__, rd_size);
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

void *esp_apptrace_fopen(esp_apptrace_dest_t dest, const char *path, const char *mode)
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

    esp_err_t ret = esp_apptrace_file_cmd_send(dest, ESP_APPTRACE_FILE_CMD_FOPEN, esp_apptrace_fopen_args_prepare,
                        &cmd_args, cmd_args.path_len+cmd_args.mode_len);
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to send file cmd (%d)!", ret);
        return NULL;
    }

    // now read the answer
    void *resp;
    ret = esp_apptrace_file_rsp_recv(dest, (uint8_t *)&resp, sizeof(resp));
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

int esp_apptrace_fclose(esp_apptrace_dest_t dest, void *stream)
{
    esp_apptrace_fclose_args_t cmd_args;

    cmd_args.file = stream;
    esp_err_t ret = esp_apptrace_file_cmd_send(dest, ESP_APPTRACE_FILE_CMD_FCLOSE, esp_apptrace_fclose_args_prepare,
                        &cmd_args, sizeof(cmd_args));
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to send file cmd (%d)!", ret);
        return EOF;
    }

    // now read the answer
    int resp;
    ret = esp_apptrace_file_rsp_recv(dest, (uint8_t *)&resp, sizeof(resp));
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

size_t esp_apptrace_fwrite(esp_apptrace_dest_t dest, const void *ptr, size_t size, size_t nmemb, void *stream)
{
    esp_apptrace_fwrite_args_t cmd_args;

    ESP_EARLY_LOGV(TAG, "esp_apptrace_fwrite f %p l %d", stream, size*nmemb);

    if (ptr == NULL) {
        return 0;
    }

    cmd_args.buf = (void *)ptr;
    cmd_args.size = size * nmemb;
    cmd_args.file = stream;
    esp_err_t ret = esp_apptrace_file_cmd_send(dest, ESP_APPTRACE_FILE_CMD_FWRITE, esp_apptrace_fwrite_args_prepare,
                        &cmd_args, sizeof(cmd_args.file)+cmd_args.size);
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to send file cmd (%d)!", ret);
        return 0;
    }

    // now read the answer
    size_t resp;
    ret = esp_apptrace_file_rsp_recv(dest, (uint8_t *)&resp, sizeof(resp));
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

size_t esp_apptrace_fread(esp_apptrace_dest_t dest, void *ptr, size_t size, size_t nmemb, void *stream)
{
    esp_apptrace_fread_args_t cmd_args;

    ESP_EARLY_LOGV(TAG, "esp_apptrace_fread f %p l %d", stream, size*nmemb);

    if (ptr == NULL) {
        return 0;
    }

    cmd_args.size = size * nmemb;
    cmd_args.file = stream;
    esp_err_t ret = esp_apptrace_file_cmd_send(dest, ESP_APPTRACE_FILE_CMD_FREAD, esp_apptrace_fread_args_prepare,
                        &cmd_args, sizeof(cmd_args));
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to send file cmd (%d)!", ret);
        return 0;
    }

    // now read the answer
    size_t resp;
    ret = esp_apptrace_file_rsp_recv(dest, (uint8_t *)&resp, sizeof(resp));
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to read response (%d)!", ret);
        return 0;
    }
    if (resp == 0) {
        return 0;
    }

    ret = esp_apptrace_file_rsp_recv(dest, ptr, resp);
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to read file data (%d)!", ret);
        return 0;
    }
    /* OpenOCD reads it like that:
     *      fread(buf, 1 ,size, file);
     * So, total read bytes count returns
     */
    return resp/size; // return the number of items read
}

static void esp_apptrace_fseek_args_prepare(uint8_t *buf, void *priv)
{
    esp_apptrace_fseek_args_t *args = priv;

    memcpy(buf, &args->file, sizeof(args->file));
    memcpy(buf + sizeof(args->file), &args->offset, sizeof(args->offset));
    memcpy(buf + sizeof(args->file) + sizeof(args->offset), &args->whence, sizeof(args->whence));
}

int esp_apptrace_fseek(esp_apptrace_dest_t dest, void *stream, long offset, int whence)
{
    esp_apptrace_fseek_args_t cmd_args;

    ESP_EARLY_LOGV(TAG, "esp_apptrace_fseek f %p o 0x%lx w %d", stream, offset, whence);

    cmd_args.file = stream;
    cmd_args.offset = offset;
    cmd_args.whence = whence;
    esp_err_t ret = esp_apptrace_file_cmd_send(dest, ESP_APPTRACE_FILE_CMD_FSEEK, esp_apptrace_fseek_args_prepare,
                        &cmd_args, sizeof(cmd_args));
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to send file cmd (%d)!", ret);
        return -1;
    }

    // now read the answer
    int resp;
    ret = esp_apptrace_file_rsp_recv(dest, (uint8_t *)&resp, sizeof(resp));
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

int esp_apptrace_ftell(esp_apptrace_dest_t dest, void *stream)
{
    esp_apptrace_ftell_args_t cmd_args;

    cmd_args.file = stream;
    esp_err_t ret = esp_apptrace_file_cmd_send(dest, ESP_APPTRACE_FILE_CMD_FTELL, esp_apptrace_ftell_args_prepare,
                        &cmd_args, sizeof(cmd_args));
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to send file cmd (%d)!", ret);
        return -1;
    }

    // now read the answer
    int resp;
    ret = esp_apptrace_file_rsp_recv(dest, (uint8_t *)&resp, sizeof(resp));
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to read response (%d)!", ret);
        return -1;
    }

    return resp;
}

int esp_apptrace_fstop(esp_apptrace_dest_t dest)
{
    ESP_EARLY_LOGV(TAG, "%s", __func__);
    esp_err_t ret = esp_apptrace_file_cmd_send(dest, ESP_APPTRACE_FILE_CMD_STOP, NULL, NULL, 0);
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

int esp_apptrace_feof(esp_apptrace_dest_t dest, void *stream)
{
    esp_apptrace_feof_args_t cmd_args;

    cmd_args.file = stream;
    esp_err_t ret = esp_apptrace_file_cmd_send(dest, ESP_APPTRACE_FILE_CMD_FEOF, esp_apptrace_feof_args_prepare,
                        &cmd_args, sizeof(cmd_args));
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to send file cmd (%d)!", ret);
        return EOF;
    }

    // now read the answer
    int resp;
    ret = esp_apptrace_file_rsp_recv(dest, (uint8_t *)&resp, sizeof(resp));
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Failed to read response (%d)!", ret);
        return EOF;
    }

    return resp;
}

#endif
