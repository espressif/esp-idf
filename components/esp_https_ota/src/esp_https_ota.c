/*
 * SPDX-FileCopyrightText: 2017-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_https_ota.h>
#include <esp_log.h>
#include <esp_ota_ops.h>
#include <errno.h>
#include <sys/param.h>
#include <inttypes.h>
#include "esp_check.h"
#include "esp_efuse.h"
#include "hal/efuse_hal.h"

ESP_EVENT_DEFINE_BASE(ESP_HTTPS_OTA_EVENT);

#define IMAGE_HEADER_SIZE (1024)

/* This is kept sufficiently large enough to cover image format headers
 * and also this defines default minimum OTA buffer chunk size */
#define DEFAULT_OTA_BUF_SIZE (IMAGE_HEADER_SIZE)

_Static_assert(DEFAULT_OTA_BUF_SIZE > (sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t) + sizeof(esp_app_desc_t) + 1), "OTA data buffer too small");

#define DEFAULT_REQUEST_SIZE (64 * 1024)

static const int DEFAULT_MAX_AUTH_RETRIES = 10;

static const char *TAG = "esp_https_ota";

typedef enum {
    ESP_HTTPS_OTA_INIT,
    ESP_HTTPS_OTA_BEGIN,
    ESP_HTTPS_OTA_IN_PROGRESS,
    ESP_HTTPS_OTA_SUCCESS,
    ESP_HTTPS_OTA_RESUME,
} esp_https_ota_state;

struct esp_https_ota_handle {
    esp_ota_handle_t update_handle;
    struct {                                  /*!< Details of staging and final partitions for OTA update */
        const esp_partition_t *staging;       /*!< New image will be downloaded in this staging partition. If NULL then a free app partition (passive app partition) is selected as the staging partition. */
        const esp_partition_t *final;         /*!< Final destination partition which is intended to be updated. Its type/subtype shall be used for verification. If NULL, staging partition is considered as the final partition. */
        bool finalize_with_copy;              /*!< Flag to copy the staging image to the final at the end of OTA update */
    } partition;
    esp_http_client_handle_t http_client;
    char *ota_upgrade_buf;
    size_t ota_upgrade_buf_size;
    int binary_file_len;
    int image_length;
#if CONFIG_ESP_HTTPS_OTA_ENABLE_PARTIAL_DOWNLOAD
    int max_http_request_size;
    bool partial_http_download;
#endif
    esp_https_ota_state state;
    bool bulk_flash_erase;
    bool ota_resumption;
    int max_authorization_retries;
#if CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
    decrypt_cb_t decrypt_cb;
    void *decrypt_user_ctx;
    uint16_t enc_img_header_size;
#endif
};

typedef struct esp_https_ota_handle esp_https_ota_t;

static bool redirection_required(int status_code)
{
    switch (status_code) {
        case HttpStatus_MovedPermanently:
        case HttpStatus_Found:
        case HttpStatus_SeeOther:
        case HttpStatus_TemporaryRedirect:
        case HttpStatus_PermanentRedirect:
            return true;
        default:
            return false;
    }
    return false;
}

static bool process_again(int status_code)
{
    switch (status_code) {
        case HttpStatus_MovedPermanently:
        case HttpStatus_Found:
        case HttpStatus_SeeOther:
        case HttpStatus_TemporaryRedirect:
        case HttpStatus_PermanentRedirect:
        case HttpStatus_Unauthorized:
            return true;
        default:
            return false;
    }
    return false;
}

static esp_err_t _http_handle_response_code(esp_https_ota_t *https_ota_handle, int status_code)
{
    esp_err_t err = ESP_FAIL;
    if (redirection_required(status_code)) {
        err = esp_http_client_set_redirection(https_ota_handle->http_client);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "URL redirection Failed");
            return err;
        }
    } else if (status_code == HttpStatus_NotModified) {
        ESP_LOGI(TAG, "OTA image not modified since last request (status code: %d)", status_code);
        return ESP_ERR_HTTP_NOT_MODIFIED;
    } else if (status_code == HttpStatus_RangeNotSatisfiable) {
        ESP_LOGI(TAG, "Requested range is incorrect (status code: %d)", status_code);
        return ESP_ERR_HTTP_RANGE_NOT_SATISFIABLE;
    } else if (status_code == HttpStatus_Unauthorized) {
        if (https_ota_handle->max_authorization_retries == 0) {
            ESP_LOGE(TAG, "Reached max_authorization_retries (%d)", status_code);
            return ESP_FAIL;
        }
        https_ota_handle->max_authorization_retries--;
        err = esp_http_client_add_auth(https_ota_handle->http_client);
        if (err!= ESP_OK) {
            ESP_LOGE(TAG, "Authorization Failed");
            return err;
        }
    } else if(status_code == HttpStatus_NotFound || status_code == HttpStatus_Forbidden) {
        ESP_LOGE(TAG, "File not found(%d)", status_code);
        return ESP_FAIL;
    } else if (status_code >= HttpStatus_BadRequest && status_code < HttpStatus_InternalError) {
        ESP_LOGE(TAG, "Client error (%d)", status_code);
        return ESP_FAIL;
    } else if (status_code >= HttpStatus_InternalError) {
        ESP_LOGE(TAG, "Server error (%d)", status_code);
        return ESP_FAIL;
    } else if (https_ota_handle->ota_resumption
#if CONFIG_ESP_HTTPS_OTA_ENABLE_PARTIAL_DOWNLOAD
        && !https_ota_handle->partial_http_download
#endif
        && status_code != HttpStatus_PartialContent) {
            ESP_LOGW(TAG, "Server ignored the requested Range header");
            ESP_LOGW(TAG, "OTA resumption requires server with range request support.");
        return ESP_ERR_HTTP_RANGE_NOT_SATISFIABLE;
    }

    char upgrade_data_buf[256];
    // process_again() returns true only in case of redirection.
    if (process_again(status_code)) {
        while (1) {
            /*
             *  In case of redirection, esp_http_client_read() is called
             *  to clear the response buffer of http_client.
             */
            int data_read = esp_http_client_read(https_ota_handle->http_client, upgrade_data_buf, sizeof(upgrade_data_buf));
            if (data_read <= 0) {
                return ESP_OK;
            }
        }
    }
    return ESP_OK;
}

static esp_err_t _http_connect(esp_https_ota_t *https_ota_handle)
{
    esp_err_t err = ESP_FAIL;
    int status_code, header_ret;
    do {
        char *post_data = NULL;
        /* Send POST request if body is set.
         * Note: Sending POST request is not supported if partial_http_download
         * is enabled
         */
        int post_len = esp_http_client_get_post_field(https_ota_handle->http_client, &post_data);

#if CONFIG_ESP_HTTPS_OTA_ENABLE_PARTIAL_DOWNLOAD
        // If support_persistent_connection is enabled and this is a subsequent request, skip connection
        if (esp_http_client_is_persistent_connection(https_ota_handle->http_client) && https_ota_handle->state == ESP_HTTPS_OTA_IN_PROGRESS) {
            ESP_LOGD(TAG, "Using existing connection for partial download");
            err = esp_http_client_prepare(https_ota_handle->http_client);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to reset HTTP client response state: %s", esp_err_to_name(err));
                return err;
            }
            err = esp_http_client_request_send(https_ota_handle->http_client, post_len);
        } else
#endif
        {
            err = esp_http_client_open(https_ota_handle->http_client, post_len);
        }
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
            return err;
        }
        if (post_len) {
            int write_len = 0;
            while (post_len > 0) {
                write_len = esp_http_client_write(https_ota_handle->http_client, post_data, post_len);
                if (write_len < 0) {
                    ESP_LOGE(TAG, "Write failed");
                    return ESP_FAIL;
                }
                post_len -= write_len;
                post_data += write_len;
            }
        }
        header_ret = esp_http_client_fetch_headers(https_ota_handle->http_client);
        if (header_ret < 0) {
            return header_ret;
        }
        status_code = esp_http_client_get_status_code(https_ota_handle->http_client);
        err = _http_handle_response_code(https_ota_handle, status_code);
        if (err != ESP_OK) {
            return err;
        }
    } while (process_again(status_code));
    return err;
}

static void _http_cleanup(esp_http_client_handle_t client)
{
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
}

// Table to lookup ota event name
static const char* ota_event_name_table[] = {
    "ESP_HTTPS_OTA_START",
    "ESP_HTTPS_OTA_CONNECTED",
    "ESP_HTTPS_OTA_GET_IMG_DESC",
    "ESP_HTTPS_OTA_VERIFY_CHIP_ID",
    "ESP_HTTPS_OTA_VERIFY_CHIP_REVISION",
    "ESP_HTTPS_OTA_DECRYPT_CB",
    "ESP_HTTPS_OTA_WRITE_FLASH",
    "ESP_HTTPS_OTA_UPDATE_BOOT_PARTITION",
    "ESP_HTTPS_OTA_FINISH",
    "ESP_HTTPS_OTA_ABORT",
};

#if CONFIG_ESP_HTTPS_OTA_EVENT_POST_TIMEOUT == -1
#define ESP_HTTPS_OTA_EVENT_POST_TIMEOUT portMAX_DELAY
#else
#define ESP_HTTPS_OTA_EVENT_POST_TIMEOUT pdMS_TO_TICKS(CONFIG_ESP_HTTPS_OTA_EVENT_POST_TIMEOUT)
#endif

static void esp_https_ota_dispatch_event(int32_t event_id, const void* event_data, size_t event_data_size)
{
    if (esp_event_post(ESP_HTTPS_OTA_EVENT, event_id, event_data, event_data_size, ESP_HTTPS_OTA_EVENT_POST_TIMEOUT) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to post https_ota event: %s", ota_event_name_table[event_id]);
    }
}

#if CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
static esp_err_t esp_https_ota_decrypt_cb(esp_https_ota_t *handle, decrypt_cb_arg_t *args)
{
    esp_https_ota_dispatch_event(ESP_HTTPS_OTA_DECRYPT_CB, NULL, 0);

    esp_err_t ret = handle->decrypt_cb(args, handle->decrypt_user_ctx);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Decrypt callback failed %d", ret);
        return ret;
    }
    if (args->data_out_len > 0) {
        return ESP_OK;
    } else {
        return ESP_HTTPS_OTA_IN_PROGRESS;
    }
}

static void esp_https_ota_decrypt_cb_free_buf(void *buffer)
{
    free(buffer);
}
#endif // CONFIG_ESP_HTTPS_OTA_DECRYPT_CB

static esp_err_t _ota_write(esp_https_ota_t *https_ota_handle, const void *buffer, size_t buf_len)
{
    if (buffer == NULL || https_ota_handle == NULL) {
        return ESP_FAIL;
    }
    esp_err_t err = esp_ota_write(https_ota_handle->update_handle, buffer, buf_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error: esp_ota_write failed! err=0x%x", err);
    } else {
        https_ota_handle->binary_file_len += buf_len;
        ESP_LOGD(TAG, "Written image length %d", https_ota_handle->binary_file_len);
        err = ESP_ERR_HTTPS_OTA_IN_PROGRESS;
    }
    esp_https_ota_dispatch_event(ESP_HTTPS_OTA_WRITE_FLASH, (void *)(&https_ota_handle->binary_file_len), sizeof(int));

#if CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
    esp_https_ota_decrypt_cb_free_buf((void *) buffer);
#endif
    return err;
}

static bool is_server_verification_enabled(const esp_https_ota_config_t *ota_config) {
    return  (ota_config->http_config->cert_pem
            || ota_config->http_config->use_global_ca_store
            || ota_config->http_config->crt_bundle_attach != NULL);
}

esp_err_t esp_https_ota_begin(const esp_https_ota_config_t *ota_config, esp_https_ota_handle_t *handle)
{
    esp_https_ota_dispatch_event(ESP_HTTPS_OTA_START, NULL, 0);

    esp_err_t err;

    if (handle == NULL || ota_config == NULL || ota_config->http_config == NULL) {
        ESP_LOGE(TAG, "esp_https_ota_begin: Invalid argument");
        if (handle) {
            *handle = NULL;
        }
        return ESP_ERR_INVALID_ARG;
    }

    if (!is_server_verification_enabled(ota_config)) {
#if CONFIG_ESP_HTTPS_OTA_ALLOW_HTTP
        ESP_LOGW(TAG, "Continuing with insecure option because CONFIG_ESP_HTTPS_OTA_ALLOW_HTTP is set.");
#else
        ESP_LOGE(TAG, "No option for server verification is enabled in esp_http_client config.");
        *handle = NULL;
        return ESP_ERR_INVALID_ARG;
#endif
    }

#if CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
    if (ota_config->decrypt_cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (ota_config->ota_resumption) {
        // OTA resumption is not supported for pre-encrypted firmware case
        return ESP_ERR_NOT_SUPPORTED;
    }
#endif

    esp_https_ota_t *https_ota_handle = calloc(1, sizeof(esp_https_ota_t));
    if (!https_ota_handle) {
        ESP_LOGE(TAG, "Couldn't allocate memory to upgrade data buffer");
        *handle = NULL;
        return ESP_ERR_NO_MEM;
    }

#if CONFIG_ESP_HTTPS_OTA_ENABLE_PARTIAL_DOWNLOAD
    https_ota_handle->partial_http_download = ota_config->partial_http_download;
    https_ota_handle->max_http_request_size = (ota_config->max_http_request_size == 0) ? DEFAULT_REQUEST_SIZE : ota_config->max_http_request_size;
#endif
    https_ota_handle->ota_resumption = ota_config->ota_resumption;
    https_ota_handle->max_authorization_retries = ota_config->http_config->max_authorization_retries;

    if (https_ota_handle->max_authorization_retries == 0) {
        https_ota_handle->max_authorization_retries = DEFAULT_MAX_AUTH_RETRIES;
    } else if (https_ota_handle->max_authorization_retries == -1) {
        https_ota_handle->max_authorization_retries = 0;
    }

    if (ota_config->ota_resumption) {
        // We allow resumption only if we have minimum buffer size already written to flash
        if (ota_config->ota_image_bytes_written >= DEFAULT_OTA_BUF_SIZE) {
            // For FE case the flash is written in multiples of 16 bytes. So, we need to align the offset to 16 bytes.
            https_ota_handle->binary_file_len = esp_efuse_is_flash_encryption_enabled() ? (ota_config->ota_image_bytes_written & ~0xF) : ota_config->ota_image_bytes_written;
            ESP_LOGD(TAG, "Resuming OTA from offset: %d", https_ota_handle->binary_file_len);
        }
    }

    /* Initiate HTTP Connection */
    https_ota_handle->http_client = esp_http_client_init(ota_config->http_config);
    if (https_ota_handle->http_client == NULL) {
        ESP_LOGE(TAG, "Failed to initialise HTTP connection");
        err = ESP_FAIL;
        goto failure;
    }

    if (ota_config->http_client_init_cb) {
        err = ota_config->http_client_init_cb(https_ota_handle->http_client);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "http_client_init_cb returned 0x%x", err);
            goto http_cleanup;
        }
    }

    /*
     * If OTA resumption is enabled, always make a range request first to detect if server
     * supports range requests. This helps fail early if server doesn't support range requests.
     * - If resuming from NVS recovered offset: use 'bytes=offset-'
     * - If starting fresh: use 'bytes=0-'
     * This applies to both partial and non-partial download cases.
     */
    if (https_ota_handle->ota_resumption
#if CONFIG_ESP_HTTPS_OTA_ENABLE_PARTIAL_DOWNLOAD
        && !https_ota_handle->partial_http_download
#endif
    ) {
        char *header_val = NULL;
        asprintf(&header_val, "bytes=%d-", https_ota_handle->binary_file_len);
        if (header_val == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory for HTTP header");
            err = ESP_ERR_NO_MEM;
            goto http_cleanup;
        }
        esp_http_client_set_header(https_ota_handle->http_client, "Range", header_val);
        free(header_val);
    }

#if CONFIG_ESP_HTTPS_OTA_ENABLE_PARTIAL_DOWNLOAD
    if (https_ota_handle->partial_http_download) {
        esp_http_client_set_method(https_ota_handle->http_client, HTTP_METHOD_HEAD);

        err = esp_http_client_perform(https_ota_handle->http_client);
        if (err == ESP_OK) {
            int status = esp_http_client_get_status_code(https_ota_handle->http_client);
            if (status != HttpStatus_Ok) {
                // If server doesn't support HEAD request, we need to get image length from GET request
                // using Range header
                esp_http_client_set_header(https_ota_handle->http_client, "Range", "bytes=0-0");
                esp_http_client_set_method(https_ota_handle->http_client, HTTP_METHOD_GET);

                err = esp_http_client_perform(https_ota_handle->http_client);
                if (err == ESP_OK) {
                    status = esp_http_client_get_status_code(https_ota_handle->http_client);
                    if (status != HttpStatus_Ok && status != HttpStatus_PartialContent) {
                        ESP_LOGE(TAG, "Received incorrect http status %d", status);
                        err = ESP_FAIL;
                        goto http_cleanup;
                    }
                } else {
                    ESP_LOGE(TAG, "ESP HTTP client perform failed: %d", err);
                    goto http_cleanup;
                }
                esp_http_client_set_header(https_ota_handle->http_client, "Range", NULL);

                if (status == HttpStatus_Ok) {
                    // If server responds with 200 OK, we can get image length from content-length header
                    https_ota_handle->image_length = esp_http_client_get_content_length(https_ota_handle->http_client);
                } else {
                    // If server responds with 206 Partial Content, we can get image length from content-range header
                    https_ota_handle->image_length = esp_http_client_get_content_range(https_ota_handle->http_client);
                }
            } else {
                https_ota_handle->image_length = esp_http_client_get_content_length(https_ota_handle->http_client);
            }
        } else {
            ESP_LOGE(TAG, "ESP HTTP client perform failed: %d", err);
            goto http_cleanup;
        }

        if (https_ota_handle->image_length == -1) {
            ESP_LOGE(TAG, "Failed to get image length from http response");
            err = ESP_FAIL;
            goto http_cleanup;
        }
#if CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
        /* In case of pre ecnrypted OTA, actual image size of OTA binary includes header size
         * which stored in variable enc_img_header_size */
        https_ota_handle->image_length -= ota_config->enc_img_header_size;
#endif
        esp_http_client_close(https_ota_handle->http_client);

        const int range_start = https_ota_handle->binary_file_len;
        const int range_end = (https_ota_handle->image_length > https_ota_handle->max_http_request_size + range_start) ?
            (range_start + https_ota_handle->max_http_request_size - 1) :
            https_ota_handle->image_length;

        // Additional sanity to not set Range header if it covers whole image range
        if ((range_end - range_start) < https_ota_handle->image_length) {
            char *header_val = NULL;
            asprintf(&header_val, "bytes=%d-%d", range_start, range_end);
            if (header_val == NULL) {
                ESP_LOGE(TAG, "Failed to allocate memory for HTTP header");
                err = ESP_ERR_NO_MEM;
                goto http_cleanup;
            }
            esp_http_client_set_header(https_ota_handle->http_client, "Range", header_val);
            free(header_val);
        }
        esp_http_client_set_method(https_ota_handle->http_client, HTTP_METHOD_GET);
    }
#endif

    err = _http_connect(https_ota_handle);
    if (err == ESP_ERR_HTTP_RANGE_NOT_SATISFIABLE && https_ota_handle->ota_resumption) {
        ESP_LOGE(TAG, "OTA resumption failed with err: %d.", err);
        ESP_LOGI(TAG, "Falling back to OTA without resumption and restarting download from beginning");
        https_ota_handle->ota_resumption = false;
        https_ota_handle->binary_file_len = 0;

        // If range in request header is not satisfiable, restart download from beginning
        esp_http_client_delete_header(https_ota_handle->http_client, "Range");

#if CONFIG_ESP_HTTPS_OTA_ENABLE_PARTIAL_DOWNLOAD
        if (https_ota_handle->partial_http_download && https_ota_handle->image_length > https_ota_handle->max_http_request_size) {
            char *header_val = NULL;
            asprintf(&header_val, "bytes=0-%d", https_ota_handle->max_http_request_size - 1);
            if (header_val == NULL) {
                ESP_LOGE(TAG, "Failed to allocate memory for HTTP header");
                err = ESP_ERR_NO_MEM;
                goto http_cleanup;
            }
            esp_http_client_set_header(https_ota_handle->http_client, "Range", header_val);
            free(header_val);
        }
#endif
        esp_http_client_close(https_ota_handle->http_client);
        esp_http_client_clear_response_buffer(https_ota_handle->http_client);
        err = _http_connect(https_ota_handle);
    }

    if (err != ESP_OK) {
        if (err != ESP_ERR_HTTP_NOT_MODIFIED) {
            ESP_LOGE(TAG, "Failed to establish HTTP connection");
        }
        goto http_cleanup;
    } else {
        esp_https_ota_dispatch_event(ESP_HTTPS_OTA_CONNECTED, NULL, 0);
    }

#if CONFIG_ESP_HTTPS_OTA_ENABLE_PARTIAL_DOWNLOAD
    if (!https_ota_handle->partial_http_download)
#endif
    {
        https_ota_handle->image_length = esp_http_client_get_content_length(https_ota_handle->http_client);
#if CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
        /* In case of pre ecnrypted OTA, actual image size of OTA binary includes header size
        which stored in variable enc_img_header_size*/
        https_ota_handle->image_length -= ota_config->enc_img_header_size;
#endif
    }

    https_ota_handle->partition.staging = NULL;
    ESP_LOGI(TAG, "Starting OTA...");
    if (ota_config->partition.staging != NULL) {
        https_ota_handle->partition.staging = esp_partition_verify(ota_config->partition.staging);
    } else {
        https_ota_handle->partition.staging = esp_ota_get_next_update_partition(NULL);
    }
    if (https_ota_handle->partition.staging == NULL) {
        ESP_LOGE(TAG, "Given staging partition or another suitable Passive OTA partition could not be found");
        err = ESP_FAIL;
        goto http_cleanup;
    }
    ESP_LOGI(TAG, "Writing to <%s> partition at offset 0x%" PRIx32,
        https_ota_handle->partition.staging->label, https_ota_handle->partition.staging->address);

    if (ota_config->partition.final == NULL) {
        https_ota_handle->partition.final = https_ota_handle->partition.staging;
    } else {
        if (ota_config->partition.staging != ota_config->partition.final) {
            const esp_partition_t *final = esp_partition_verify(ota_config->partition.final);
            if (final == NULL) {
                ESP_LOGE(TAG, "Given final partition not found");
                err = ESP_FAIL;
                goto http_cleanup;
            }
            https_ota_handle->partition.final = final;
            https_ota_handle->partition.finalize_with_copy = ota_config->partition.finalize_with_copy;
        }
    }

    const int alloc_size = MAX(ota_config->http_config->buffer_size, DEFAULT_OTA_BUF_SIZE);
    if (ota_config->buffer_caps != 0) {
        https_ota_handle->ota_upgrade_buf = (char *)heap_caps_malloc(alloc_size, ota_config->buffer_caps);
    } else {
        https_ota_handle->ota_upgrade_buf = (char *)malloc(alloc_size);
    }
    if (!https_ota_handle->ota_upgrade_buf) {
        ESP_LOGE(TAG, "Couldn't allocate memory to upgrade data buffer");
        err = ESP_ERR_NO_MEM;
        goto http_cleanup;
    }
#if CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
    https_ota_handle->decrypt_cb = ota_config->decrypt_cb;
    https_ota_handle->decrypt_user_ctx = ota_config->decrypt_user_ctx;
    https_ota_handle->enc_img_header_size = ota_config->enc_img_header_size;
#endif
    https_ota_handle->ota_upgrade_buf_size = alloc_size;
    https_ota_handle->bulk_flash_erase = ota_config->bulk_flash_erase;
    *handle = (esp_https_ota_handle_t)https_ota_handle;
    https_ota_handle->state = https_ota_handle->binary_file_len ? ESP_HTTPS_OTA_RESUME : ESP_HTTPS_OTA_BEGIN;
    return ESP_OK;

http_cleanup:
    _http_cleanup(https_ota_handle->http_client);
failure:
    free(https_ota_handle);
    *handle = NULL;
    return err;
}

static esp_err_t read_header(esp_https_ota_t *handle)
{
    /*
     * `data_read_size` holds number of bytes needed to read complete header.
     * `bytes_read` holds number of bytes read.
     */
    int data_read_size = IMAGE_HEADER_SIZE;
    int data_read = 0, bytes_read = 0;
    /*
     * while loop is added to download complete image headers, even if the headers
     * are not sent in a single packet.
     */
    while (data_read_size > 0 && !esp_http_client_is_complete_data_received(handle->http_client)) {
        data_read = esp_http_client_read(handle->http_client,
                                          (handle->ota_upgrade_buf + bytes_read),
                                          data_read_size);
        if (data_read < 0) {
            if (data_read == -ESP_ERR_HTTP_EAGAIN) {
                ESP_LOGD(TAG, "ESP_ERR_HTTP_EAGAIN invoked: Call timed out before data was ready");
                continue;
            }
            ESP_LOGE(TAG, "Connection closed, errno = %d", errno);
            break;
        }
        data_read_size -= data_read;
        bytes_read += data_read;
    }
    if (data_read_size > 0) {
        ESP_LOGE(TAG, "Complete headers were not received");
        return ESP_FAIL;
    }
    handle->binary_file_len = bytes_read;
    return ESP_OK;
}

static esp_err_t get_description_from_image(esp_https_ota_handle_t https_ota_handle, void *new_img_info)
{
    esp_https_ota_dispatch_event(ESP_HTTPS_OTA_GET_IMG_DESC, NULL, 0);

#if CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
    // This API is not supported in case firmware image is encrypted in nature.
    // It is recommended to retrieve image description through decryption callback in application layer.
    return ESP_ERR_NOT_SUPPORTED;
#endif

    esp_https_ota_t *handle = (esp_https_ota_t *)https_ota_handle;
    if (handle == NULL || new_img_info == NULL)  {
        ESP_LOGE(TAG, "esp_https_ota_get_img_desc: Invalid argument");
        return ESP_ERR_INVALID_ARG;
    }
    if (handle->state < ESP_HTTPS_OTA_BEGIN) {
        ESP_LOGE(TAG, "esp_https_ota_get_img_desc: Invalid state");
        return ESP_ERR_INVALID_STATE;
    }

    unsigned img_info_len = 0;
    if (handle->partition.final->type == ESP_PARTITION_TYPE_APP) {
        img_info_len = sizeof(esp_app_desc_t);
    } else if (handle->partition.final->type == ESP_PARTITION_TYPE_BOOTLOADER) {
        img_info_len = sizeof(esp_bootloader_desc_t);
    } else {
        ESP_LOGE(TAG, "This partition type (%d) is not supported", handle->partition.final->type);
        return ESP_FAIL;
    }

    const int offset = sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t);
    void *img_info = NULL;

    if (handle->binary_file_len >= offset + img_info_len) {
        esp_err_t ret = esp_partition_read(handle->partition.staging, offset, handle->ota_upgrade_buf, img_info_len);
        ESP_RETURN_ON_ERROR(ret, TAG, "partition read failed %d", ret);
        img_info = (void *) handle->ota_upgrade_buf;
    } else {
        if (read_header(handle) != ESP_OK) {
            return ESP_FAIL;
        }
        img_info = (void *)&handle->ota_upgrade_buf[offset];
    }

    if (handle->partition.final->type == ESP_PARTITION_TYPE_APP) {
        esp_app_desc_t *app_info = (esp_app_desc_t *)img_info;
        if (app_info->magic_word != ESP_APP_DESC_MAGIC_WORD) {
            ESP_LOGE(TAG, "Incorrect app descriptor magic");
            return ESP_FAIL;
        }
    } else if (handle->partition.final->type == ESP_PARTITION_TYPE_BOOTLOADER) {
        esp_bootloader_desc_t *bootloader_info = (esp_bootloader_desc_t *)img_info;
        if (bootloader_info->magic_byte != ESP_BOOTLOADER_DESC_MAGIC_BYTE) {
            ESP_LOGE(TAG, "Incorrect bootloader descriptor magic");
            return ESP_FAIL;
        }
    }

    memcpy(new_img_info, img_info, img_info_len);
    return ESP_OK;
}

esp_err_t esp_https_ota_get_img_desc(esp_https_ota_handle_t https_ota_handle, esp_app_desc_t *new_app_info)
{
    return get_description_from_image(https_ota_handle, new_app_info);
}

esp_err_t esp_https_ota_get_bootloader_img_desc(esp_https_ota_handle_t https_ota_handle, esp_bootloader_desc_t *new_img_info)
{
    return get_description_from_image(https_ota_handle, new_img_info);
}

static const esp_app_desc_t *esp_https_ota_get_app_desc(const void *data_buf)
{
    return (const esp_app_desc_t *)((const uint8_t *)data_buf +
           sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t));
}

static esp_err_t esp_https_ota_verify_image(const void *data_buf, esp_partition_type_t part_type, bool verify_spi_mode)
{
    const esp_image_header_t *img_hdr = (const esp_image_header_t *) data_buf;

    // Dispatch verification events
    esp_https_ota_dispatch_event(ESP_HTTPS_OTA_VERIFY_CHIP_ID, (void *)(&img_hdr->chip_id), sizeof(esp_chip_id_t));
    esp_https_ota_dispatch_event(ESP_HTTPS_OTA_VERIFY_CHIP_REVISION, (void *)(&img_hdr->min_chip_rev_full), sizeof(uint16_t));

    // Get app descriptor only if SPI mode verification is needed
    const esp_app_desc_t *app_desc = verify_spi_mode ? esp_https_ota_get_app_desc(data_buf) : NULL;

    return esp_ota_check_image_validity(part_type, img_hdr, app_desc);
}

esp_err_t esp_https_ota_perform(esp_https_ota_handle_t https_ota_handle)
{
    esp_https_ota_t *handle = (esp_https_ota_t *)https_ota_handle;
    if (handle == NULL) {
        ESP_LOGE(TAG, "esp_https_ota_perform: Invalid argument");
        return ESP_ERR_INVALID_ARG;
    }
    if (handle->state < ESP_HTTPS_OTA_BEGIN) {
        ESP_LOGE(TAG, "esp_https_ota_perform: Invalid state");
        return ESP_FAIL;
    }

    esp_err_t err;
    int data_read;
    const size_t erase_size = handle->bulk_flash_erase ? (handle->image_length > 0 ? handle->image_length : OTA_SIZE_UNKNOWN) : OTA_WITH_SEQUENTIAL_WRITES;
    switch (handle->state) {
        case ESP_HTTPS_OTA_BEGIN:
            err = esp_ota_begin(handle->partition.staging, erase_size, &handle->update_handle);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "esp_ota_begin failed (%s)", esp_err_to_name(err));
                return err;
            }
            esp_ota_set_final_partition(handle->update_handle, handle->partition.final, handle->partition.finalize_with_copy);
            handle->state = ESP_HTTPS_OTA_IN_PROGRESS;

            /**
             * If the final partition is not an app or bootloader, return ESP_ERR_HTTPS_OTA_IN_PROGRESS
             * As there is no need to read header and verify chip id and chip revision for custom partition.
             * Directly jump to the OTA in progress state.
             */
            if (handle->partition.final->type != ESP_PARTITION_TYPE_APP
                && handle->partition.final->type != ESP_PARTITION_TYPE_BOOTLOADER) {
                return ESP_ERR_HTTPS_OTA_IN_PROGRESS;
            }
            /* In case `esp_https_ota_get_img_desc` was invoked first,
               then the image data read there should be written to OTA partition
               */
            int binary_file_len = 0;
            if (handle->binary_file_len) {
                binary_file_len = handle->binary_file_len;
            } else {
                if (read_header(handle) != ESP_OK) {
                    return ESP_FAIL;
                }
                binary_file_len = IMAGE_HEADER_SIZE;
            }
            /*
            * Header length gets added to handle->binary_file_len in _ota_write
            * Clear handle->binary_file_len to avoid additional bytes in upgrade image size calculation
            */
            handle->binary_file_len = 0;

            const void *data_buf = (const void *) handle->ota_upgrade_buf;
#if CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
            decrypt_cb_arg_t args = {};
            args.data_in = handle->ota_upgrade_buf;
            args.data_in_len = binary_file_len;
            err = esp_https_ota_decrypt_cb(handle, &args);
            if (err == ESP_OK) {
                data_buf = args.data_out;
                binary_file_len = args.data_out_len;
            } else {
                ESP_LOGE(TAG, "Decryption of image header failed");
                return ESP_FAIL;
            }
#endif // CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
            if (handle->partition.final->type == ESP_PARTITION_TYPE_APP || handle->partition.final->type == ESP_PARTITION_TYPE_BOOTLOADER) {
                bool verify_spi_mode = false;
#if CONFIG_ESP_HTTPS_OTA_VERIFY_SPI_MODE
                verify_spi_mode = (handle->partition.final->type == ESP_PARTITION_TYPE_APP);
#endif
                err = esp_https_ota_verify_image(data_buf, handle->partition.final->type, verify_spi_mode);
                if (err != ESP_OK) {
                    return err;
                }
            }
            return _ota_write(handle, data_buf, binary_file_len);
        case ESP_HTTPS_OTA_RESUME:
            ESP_LOGD(TAG, "OTA resumption case");
            err = esp_ota_resume(handle->partition.staging, erase_size, handle->binary_file_len, &handle->update_handle);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "esp_ota_resume failed (%s)", esp_err_to_name(err));
                return err;
            }
            esp_ota_set_final_partition(handle->update_handle, handle->partition.final, handle->partition.finalize_with_copy);
            handle->state = ESP_HTTPS_OTA_IN_PROGRESS;
            /* falls through */
        case ESP_HTTPS_OTA_IN_PROGRESS:
            data_read = esp_http_client_read(handle->http_client,
                                             handle->ota_upgrade_buf,
                                             handle->ota_upgrade_buf_size);
            if (data_read == 0) {
                /*
                 *  esp_http_client_is_complete_data_received is added to check whether
                 *  complete image is received.
                 */
                if (!esp_http_client_is_complete_data_received(handle->http_client)) {
                    ESP_LOGE(TAG, "Connection closed before complete data was received!");
                    return ESP_FAIL;
                }
                ESP_LOGD(TAG, "Connection closed");
            } else if (data_read > 0) {
                const void *data_buf = (const void *) handle->ota_upgrade_buf;
                int data_len = data_read;
#if CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
                decrypt_cb_arg_t args = {};
                args.data_in = handle->ota_upgrade_buf;
                args.data_in_len = data_read;
                err = esp_https_ota_decrypt_cb(handle, &args);
                if (err == ESP_OK) {
                    data_buf = args.data_out;
                    data_len = args.data_out_len;
                } else {
                    return err;
                }
#endif // CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
                return _ota_write(handle, data_buf, data_len);
            } else {
                if (data_read == -ESP_ERR_HTTP_EAGAIN) {
                    ESP_LOGD(TAG, "ESP_ERR_HTTP_EAGAIN invoked: Call timed out before data was ready");
                    return ESP_ERR_HTTPS_OTA_IN_PROGRESS;
                }
                ESP_LOGE(TAG, "data read %d, errno %d", data_read, errno);
                return ESP_FAIL;
            }
#if CONFIG_ESP_HTTPS_OTA_ENABLE_PARTIAL_DOWNLOAD
            if (!handle->partial_http_download || (handle->partial_http_download && handle->image_length == handle->binary_file_len))
#endif
            {
                handle->state = ESP_HTTPS_OTA_SUCCESS;
            }
            break;
         default:
            ESP_LOGE(TAG, "Invalid ESP HTTPS OTA State");
            return ESP_FAIL;
            break;
    }
#if CONFIG_ESP_HTTPS_OTA_ENABLE_PARTIAL_DOWNLOAD
    if (handle->partial_http_download) {
        if (handle->state == ESP_HTTPS_OTA_IN_PROGRESS && handle->image_length > handle->binary_file_len) {
            // Only close connection if support_persistent_connection is not enabled
            if (!esp_http_client_is_persistent_connection(handle->http_client)) {
                esp_http_client_close(handle->http_client);
            }
            char *header_val = NULL;
            int header_size = 0;
#if CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
            header_size = handle->enc_img_header_size;
#endif
            if ((handle->image_length - handle->binary_file_len) > handle->max_http_request_size) {
                asprintf(&header_val, "bytes=%d-%d", handle->binary_file_len + header_size, (handle->binary_file_len + header_size + handle->max_http_request_size - 1));
            } else {
                asprintf(&header_val, "bytes=%d-", handle->binary_file_len + header_size);
            }
            if (header_val == NULL) {
                ESP_LOGE(TAG, "Failed to allocate memory for HTTP header");
                return ESP_ERR_NO_MEM;
            }
            esp_http_client_set_header(handle->http_client, "Range", header_val);
            free(header_val);
            err = _http_connect(handle);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to establish HTTP connection");
                return ESP_FAIL;
            }
            ESP_LOGD(TAG, "Connection start");
            return ESP_ERR_HTTPS_OTA_IN_PROGRESS;
        }
    }
#endif
    return ESP_OK;
}

bool esp_https_ota_is_complete_data_received(esp_https_ota_handle_t https_ota_handle)
{
    bool ret = false;
    esp_https_ota_t *handle = (esp_https_ota_t *)https_ota_handle;
#if CONFIG_ESP_HTTPS_OTA_ENABLE_PARTIAL_DOWNLOAD
    if (handle->partial_http_download) {
        ret = (handle->image_length == handle->binary_file_len);
    } else
#endif
    {
        ret = esp_http_client_is_complete_data_received(handle->http_client);
    }
    return ret;
}

esp_err_t esp_https_ota_finish(esp_https_ota_handle_t https_ota_handle)
{
    esp_https_ota_t *handle = (esp_https_ota_t *)https_ota_handle;
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (handle->state < ESP_HTTPS_OTA_BEGIN) {
        return ESP_FAIL;
    }

    esp_err_t err = ESP_OK;
    switch (handle->state) {
        case ESP_HTTPS_OTA_SUCCESS:
        case ESP_HTTPS_OTA_IN_PROGRESS:
            err = esp_ota_end(handle->update_handle);
            /* falls through */
        case ESP_HTTPS_OTA_BEGIN:
        case ESP_HTTPS_OTA_RESUME:
            if (handle->ota_upgrade_buf) {
                free(handle->ota_upgrade_buf);
            }
            if (handle->http_client) {
                _http_cleanup(handle->http_client);
            }
            break;
        default:
            ESP_LOGE(TAG, "Invalid ESP HTTPS OTA State");
            break;
    }

    if ((handle->partition.final->type == ESP_PARTITION_TYPE_APP) && (err == ESP_OK) && (handle->state == ESP_HTTPS_OTA_SUCCESS)) {
        if (handle->partition.final->subtype >= ESP_PARTITION_SUBTYPE_APP_OTA_0 && handle->partition.final->subtype < ESP_PARTITION_SUBTYPE_APP_OTA_MAX) {
            // Do not allow the boot partition to be set as the Factory partition, because ota_data will be erased in this case.
            // The user can call it afterward if needed.
            err = esp_ota_set_boot_partition(handle->partition.staging);
        }
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_ota_set_boot_partition failed! err=0x%x", err);
        } else {
            esp_https_ota_dispatch_event(ESP_HTTPS_OTA_UPDATE_BOOT_PARTITION, (void *)(&handle->partition.final->subtype), sizeof(esp_partition_subtype_t));
        }
    }
    free(handle);
    esp_https_ota_dispatch_event(ESP_HTTPS_OTA_FINISH, NULL, 0);

    return err;
}

esp_err_t esp_https_ota_abort(esp_https_ota_handle_t https_ota_handle)
{
    esp_https_ota_dispatch_event(ESP_HTTPS_OTA_ABORT, NULL, 0);

    esp_https_ota_t *handle = (esp_https_ota_t *)https_ota_handle;
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (handle->state < ESP_HTTPS_OTA_BEGIN) {
        return ESP_FAIL;
    }

    esp_err_t err = ESP_OK;
    switch (handle->state) {
        case ESP_HTTPS_OTA_SUCCESS:
        case ESP_HTTPS_OTA_IN_PROGRESS:
            err = esp_ota_abort(handle->update_handle);
            /* falls through */
        case ESP_HTTPS_OTA_BEGIN:
        case ESP_HTTPS_OTA_RESUME:
            if (handle->ota_upgrade_buf) {
                free(handle->ota_upgrade_buf);
            }
            if (handle->http_client) {
                _http_cleanup(handle->http_client);
            }
            break;
        default:
            err = ESP_ERR_INVALID_STATE;
            ESP_LOGE(TAG, "Invalid ESP HTTPS OTA State");
            break;
    }
    free(handle);
    return err;
}

int esp_https_ota_get_status_code(esp_https_ota_handle_t https_ota_handle)
{
    esp_https_ota_t *handle = (esp_https_ota_t *) https_ota_handle;
    if (handle == NULL || handle->http_client == NULL) {
        return -1;
    }
    return esp_http_client_get_status_code(handle->http_client);
}

int esp_https_ota_get_image_len_read(esp_https_ota_handle_t https_ota_handle)
{
    esp_https_ota_t *handle = (esp_https_ota_t *)https_ota_handle;
    if (handle == NULL) {
        return -1;
    }
    if (handle->state < ESP_HTTPS_OTA_IN_PROGRESS) {
        return -1;
    }
    return handle->binary_file_len;
}

int esp_https_ota_get_image_size(esp_https_ota_handle_t https_ota_handle)
{
    esp_https_ota_t *handle = (esp_https_ota_t *)https_ota_handle;
    if (handle == NULL) {
        return -1;
    }
    if (handle->state < ESP_HTTPS_OTA_BEGIN) {
        return -1;
    }
    return handle->image_length;
}

esp_err_t esp_https_ota(const esp_https_ota_config_t *ota_config)
{
    if (ota_config == NULL || ota_config->http_config == NULL) {
        ESP_LOGE(TAG, "esp_https_ota: Invalid argument");
        return ESP_ERR_INVALID_ARG;
    }

    if (ota_config->ota_resumption) {
        ESP_LOGE(TAG, "OTA resumption is not supported in esp_https_ota API");
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_https_ota_handle_t https_ota_handle = NULL;
    esp_err_t err = esp_https_ota_begin(ota_config, &https_ota_handle);
    if (err != ESP_OK) {
        return err;
    }

    if (https_ota_handle == NULL) {
        return ESP_FAIL;
    }

    while (1) {
        err = esp_https_ota_perform(https_ota_handle);
        if (err != ESP_ERR_HTTPS_OTA_IN_PROGRESS) {
            break;
        }
    }

    if (err != ESP_OK) {
        esp_https_ota_abort(https_ota_handle);
        return err;
    }

    esp_err_t ota_finish_err = esp_https_ota_finish(https_ota_handle);
    if (ota_finish_err != ESP_OK) {
        return ota_finish_err;
    }
    return ESP_OK;
}
