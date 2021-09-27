// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_https_ota.h>
#include <esp_log.h>
#include <esp_ota_ops.h>
#include <errno.h>
#include <sys/param.h>

#define IMAGE_HEADER_SIZE sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t) + sizeof(esp_app_desc_t) + 1
#define DEFAULT_OTA_BUF_SIZE IMAGE_HEADER_SIZE
#define DEFAULT_REQUEST_SIZE (64 * 1024)
static const char *TAG = "esp_https_ota";

typedef enum {
    ESP_HTTPS_OTA_INIT,
    ESP_HTTPS_OTA_BEGIN,
    ESP_HTTPS_OTA_IN_PROGRESS,
    ESP_HTTPS_OTA_SUCCESS,
} esp_https_ota_state;

struct esp_https_ota_handle {
    esp_ota_handle_t update_handle;
    const esp_partition_t *update_partition;
    esp_http_client_handle_t http_client;
    char *ota_upgrade_buf;
    size_t ota_upgrade_buf_size;
    int binary_file_len;
    int image_length;
    int max_http_request_size;
    esp_https_ota_state state;
    bool bulk_flash_erase;
    bool partial_http_download;
};

typedef struct esp_https_ota_handle esp_https_ota_t;

static bool process_again(int status_code)
{
    switch (status_code) {
        case HttpStatus_MovedPermanently:
        case HttpStatus_Found:
        case HttpStatus_TemporaryRedirect:
        case HttpStatus_Unauthorized:
            return true;
        default:
            return false;
    }
    return false;
}

static esp_err_t _http_handle_response_code(esp_http_client_handle_t http_client, int status_code)
{
    esp_err_t err;
    if (status_code == HttpStatus_MovedPermanently || status_code == HttpStatus_Found || status_code == HttpStatus_TemporaryRedirect) {
        err = esp_http_client_set_redirection(http_client);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "URL redirection Failed");
            return err;
        }
    } else if (status_code == HttpStatus_Unauthorized) {
        esp_http_client_add_auth(http_client);
    } else if(status_code == HttpStatus_NotFound || status_code == HttpStatus_Forbidden) {
        ESP_LOGE(TAG, "File not found(%d)", status_code);
        return ESP_FAIL;
    } else if (status_code >= HttpStatus_BadRequest && status_code < HttpStatus_InternalError) {
        ESP_LOGE(TAG, "Client error (%d)", status_code);
        return ESP_FAIL;
    } else if (status_code >= HttpStatus_InternalError) {
        ESP_LOGE(TAG, "Server error (%d)", status_code);
        return ESP_FAIL;
    }

    char upgrade_data_buf[DEFAULT_OTA_BUF_SIZE];
    // process_again() returns true only in case of redirection.
    if (process_again(status_code)) {
        while (1) {
            /*
             *  In case of redirection, esp_http_client_read() is called
             *  to clear the response buffer of http_client.
             */
            int data_read = esp_http_client_read(http_client, upgrade_data_buf, DEFAULT_OTA_BUF_SIZE);
            if (data_read <= 0) {
                return ESP_OK;
            }
        }
    }
    return ESP_OK;
}

static esp_err_t _http_connect(esp_http_client_handle_t http_client)
{
    esp_err_t err = ESP_FAIL;
    int status_code, header_ret;
    do {
        char *post_data = NULL;
        /* Send POST request if body is set.
         * Note: Sending POST request is not supported if partial_http_download
         * is enabled
         */
        int post_len = esp_http_client_get_post_field(http_client, &post_data);
        err = esp_http_client_open(http_client, post_len);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
            return err;
        }
        if (post_len) {
            int write_len = 0;
            while (post_len > 0) {
                write_len = esp_http_client_write(http_client, post_data, post_len);
                if (write_len < 0) {
                    ESP_LOGE(TAG, "Write failed");
                    return ESP_FAIL;
                }
                post_len -= write_len;
                post_data += write_len;
            }
        }
        header_ret = esp_http_client_fetch_headers(http_client);
        if (header_ret < 0) {
            return header_ret;
        }
        status_code = esp_http_client_get_status_code(http_client);
        err = _http_handle_response_code(http_client, status_code);
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
    return err;
}

static bool is_server_verification_enabled(esp_https_ota_config_t *ota_config) {
    return  (ota_config->http_config->cert_pem
            || ota_config->http_config->use_global_ca_store
            || ota_config->http_config->crt_bundle_attach != NULL);
}

esp_err_t esp_https_ota_begin(esp_https_ota_config_t *ota_config, esp_https_ota_handle_t *handle)
{
    esp_err_t err;

    if (handle == NULL || ota_config == NULL || ota_config->http_config == NULL) {
        ESP_LOGE(TAG, "esp_https_ota_begin: Invalid argument");
        if (handle) {
            *handle = NULL;
        }
        return ESP_ERR_INVALID_ARG;
    }

    if (!is_server_verification_enabled(ota_config)) {
#if CONFIG_OTA_ALLOW_HTTP
        ESP_LOGW(TAG, "Continuing with insecure option because CONFIG_OTA_ALLOW_HTTP is set.");
#else
        ESP_LOGE(TAG, "No option for server verification is enabled in esp_http_client config.");
        *handle = NULL;
        return ESP_ERR_INVALID_ARG;
#endif
    }

    esp_https_ota_t *https_ota_handle = calloc(1, sizeof(esp_https_ota_t));
    if (!https_ota_handle) {
        ESP_LOGE(TAG, "Couldn't allocate memory to upgrade data buffer");
        *handle = NULL;
        return ESP_ERR_NO_MEM;
    }

    https_ota_handle->partial_http_download = ota_config->partial_http_download;
    https_ota_handle->max_http_request_size = (ota_config->max_http_request_size == 0) ? DEFAULT_REQUEST_SIZE : ota_config->max_http_request_size;

    /* Initiate HTTP Connection */
    https_ota_handle->http_client = esp_http_client_init(ota_config->http_config);
    if (https_ota_handle->http_client == NULL) {
        ESP_LOGE(TAG, "Failed to initialise HTTP connection");
        err = ESP_FAIL;
        goto failure;
    }

    if (https_ota_handle->partial_http_download) {
        esp_http_client_set_method(https_ota_handle->http_client, HTTP_METHOD_HEAD);
        err = esp_http_client_perform(https_ota_handle->http_client);
        if (err != ESP_OK || esp_http_client_get_status_code(https_ota_handle->http_client) != HttpStatus_Ok) {
            ESP_LOGE(TAG, "Failed to get image length");
            goto http_cleanup;
        }
        https_ota_handle->image_length = esp_http_client_get_content_length(https_ota_handle->http_client);
        esp_http_client_close(https_ota_handle->http_client);

        if (https_ota_handle->image_length > https_ota_handle->max_http_request_size) {
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
        esp_http_client_set_method(https_ota_handle->http_client, HTTP_METHOD_GET);
    }

    if (ota_config->http_client_init_cb) {
        err = ota_config->http_client_init_cb(https_ota_handle->http_client);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "http_client_init_cb returned 0x%x", err);
            goto failure;
        }
    }

    err = _http_connect(https_ota_handle->http_client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to establish HTTP connection");
        goto http_cleanup;
    }

    if (!https_ota_handle->partial_http_download) {
        https_ota_handle->image_length = esp_http_client_get_content_length(https_ota_handle->http_client);
    }

    https_ota_handle->update_partition = NULL;
    ESP_LOGI(TAG, "Starting OTA...");
    https_ota_handle->update_partition = esp_ota_get_next_update_partition(NULL);
    if (https_ota_handle->update_partition == NULL) {
        ESP_LOGE(TAG, "Passive OTA partition not found");
        err = ESP_FAIL;
        goto http_cleanup;
    }
    ESP_LOGI(TAG, "Writing to partition subtype %d at offset 0x%x",
        https_ota_handle->update_partition->subtype, https_ota_handle->update_partition->address);

    const int alloc_size = MAX(ota_config->http_config->buffer_size, DEFAULT_OTA_BUF_SIZE);
    https_ota_handle->ota_upgrade_buf = (char *)malloc(alloc_size);
    if (!https_ota_handle->ota_upgrade_buf) {
        ESP_LOGE(TAG, "Couldn't allocate memory to upgrade data buffer");
        err = ESP_ERR_NO_MEM;
        goto http_cleanup;
    }
    https_ota_handle->ota_upgrade_buf_size = alloc_size;
    https_ota_handle->bulk_flash_erase = ota_config->bulk_flash_erase;
    https_ota_handle->binary_file_len = 0;
    *handle = (esp_https_ota_handle_t)https_ota_handle;
    https_ota_handle->state = ESP_HTTPS_OTA_BEGIN;
    return ESP_OK;

http_cleanup:
    _http_cleanup(https_ota_handle->http_client);
failure:
    free(https_ota_handle);
    *handle = NULL;
    return err;
}

esp_err_t esp_https_ota_get_img_desc(esp_https_ota_handle_t https_ota_handle, esp_app_desc_t *new_app_info)
{
    esp_https_ota_t *handle = (esp_https_ota_t *)https_ota_handle;
    if (handle == NULL || new_app_info == NULL)  {
        ESP_LOGE(TAG, "esp_https_ota_read_img_desc: Invalid argument");
        return ESP_ERR_INVALID_ARG;
    }
    if (handle->state < ESP_HTTPS_OTA_BEGIN) {
        ESP_LOGE(TAG, "esp_https_ota_read_img_desc: Invalid state");
        return ESP_FAIL;
    }
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
        /*
         * As esp_http_client_read doesn't return negative error code if select fails, we rely on
         * `errno` to check for underlying transport connectivity closure if any
         */
        if (errno == ENOTCONN || errno == ECONNRESET || errno == ECONNABORTED || data_read < 0) {
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
    memcpy(new_app_info, &handle->ota_upgrade_buf[sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t)], sizeof(esp_app_desc_t));
    return ESP_OK;
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
    const int erase_size = handle->bulk_flash_erase ? OTA_SIZE_UNKNOWN : OTA_WITH_SEQUENTIAL_WRITES;
    switch (handle->state) {
        case ESP_HTTPS_OTA_BEGIN:
            err = esp_ota_begin(handle->update_partition, erase_size, &handle->update_handle);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "esp_ota_begin failed (%s)", esp_err_to_name(err));
                return err;
            }
            handle->state = ESP_HTTPS_OTA_IN_PROGRESS;
            /* In case `esp_https_ota_read_img_desc` was invoked first,
               then the image data read there should be written to OTA partition
               */
            if (handle->binary_file_len) {
                /*
                 * Header length gets added to handle->binary_file_len in _ota_write
                 * Clear handle->binary_file_len to avoid additional 289 bytes in binary_file_len
                 */
                int binary_file_len = handle->binary_file_len;
                handle->binary_file_len = 0;
                return _ota_write(handle, (const void *)handle->ota_upgrade_buf, binary_file_len);
            }
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
                bool is_recv_complete = esp_http_client_is_complete_data_received(handle->http_client);
                /*
                 * As esp_http_client_read doesn't return negative error code if select fails, we rely on
                 * `errno` to check for underlying transport connectivity closure if any.
                 * Incase the complete data has not been received but the server has sent
                 * an ENOTCONN or ECONNRESET, failure is returned. We close with success
                 * if complete data has been received.
                 */
                if ((errno == ENOTCONN || errno == ECONNRESET || errno == ECONNABORTED) && !is_recv_complete) {
                    ESP_LOGE(TAG, "Connection closed, errno = %d", errno);
                    return ESP_FAIL;
                } else if (!is_recv_complete) {
                    return ESP_ERR_HTTPS_OTA_IN_PROGRESS;
                }
                ESP_LOGD(TAG, "Connection closed");
            } else if (data_read > 0) {
                return _ota_write(handle, (const void *)handle->ota_upgrade_buf, data_read);
            } else {
                ESP_LOGE(TAG, "data read %d, errno %d", data_read, errno);
                return ESP_FAIL;
            }
            if (!handle->partial_http_download || (handle->partial_http_download && handle->image_length == handle->binary_file_len)) {
                handle->state = ESP_HTTPS_OTA_SUCCESS;
            }
            break;
         default:
            ESP_LOGE(TAG, "Invalid ESP HTTPS OTA State");
            return ESP_FAIL;
            break;
    }
    if (handle->partial_http_download) {
        if (handle->state == ESP_HTTPS_OTA_IN_PROGRESS && handle->image_length > handle->binary_file_len) {
            esp_http_client_close(handle->http_client);
            char *header_val = NULL;
            if ((handle->image_length - handle->binary_file_len) > handle->max_http_request_size) {
                asprintf(&header_val, "bytes=%d-%d", handle->binary_file_len, (handle->binary_file_len + handle->max_http_request_size - 1));
            } else {
                asprintf(&header_val, "bytes=%d-", handle->binary_file_len);
            }
            if (header_val == NULL) {
                ESP_LOGE(TAG, "Failed to allocate memory for HTTP header");
                return ESP_ERR_NO_MEM;
            }
            esp_http_client_set_header(handle->http_client, "Range", header_val);
            free(header_val);
            err = _http_connect(handle->http_client);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to establish HTTP connection");
                return ESP_FAIL;
            }
            ESP_LOGD(TAG, "Connection start");
            return ESP_ERR_HTTPS_OTA_IN_PROGRESS;
        }
    }
    return ESP_OK;
}

bool esp_https_ota_is_complete_data_received(esp_https_ota_handle_t https_ota_handle)
{
    bool ret = false;
    esp_https_ota_t *handle = (esp_https_ota_t *)https_ota_handle;
    if (handle->partial_http_download) {
        ret = (handle->image_length == handle->binary_file_len);
    } else {
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

    if ((err == ESP_OK) && (handle->state == ESP_HTTPS_OTA_SUCCESS)) {
        esp_err_t err = esp_ota_set_boot_partition(handle->update_partition);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_ota_set_boot_partition failed! err=0x%x", err);
        }
    }
    free(handle);
    return err;
}

esp_err_t esp_https_ota_abort(esp_https_ota_handle_t https_ota_handle)
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
            err = esp_ota_abort(handle->update_handle);
            /* falls through */
        case ESP_HTTPS_OTA_BEGIN:
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

esp_err_t esp_https_ota(const esp_http_client_config_t *config)
{
    if (!config) {
        ESP_LOGE(TAG, "esp_http_client config not found");
        return ESP_ERR_INVALID_ARG;
    }

    esp_https_ota_config_t ota_config = {
        .http_config = config,
    };

    esp_https_ota_handle_t https_ota_handle = NULL;
    esp_err_t err = esp_https_ota_begin(&ota_config, &https_ota_handle);
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
