/* SPIFFS Image Generation on Build Example
 *
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense or CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "psa/crypto.h"

static const char *TAG = "example";

static void read_hello_txt(void)
{
    ESP_LOGI(TAG, "Reading hello.txt");

    // Open for reading hello.txt
    FILE* f = fopen("/spiffs/hello.txt", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open hello.txt");
        return;
    }

    char buf[64];
    memset(buf, 0, sizeof(buf));
    fread(buf, 1, sizeof(buf), f);
    fclose(f);

    // Display the read contents from the file
    ESP_LOGI(TAG, "Read from hello.txt: %s", buf);
}

static void compute_alice_txt_md5(void)
{
    ESP_LOGI(TAG, "Computing alice.txt MD5 hash");

    // The file alice.txt lives under a subdirectory, though SPIFFS itself is flat
    FILE* f = fopen("/spiffs/sub/alice.txt", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open alice.txt");
        return;
    }

    // Read file and compute the digest chunk by chunk
    #define MD5_MAX_LEN 16

    char buf[64];
    psa_status_t status;
    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
    psa_algorithm_t alg = PSA_ALG_MD5;
    status = psa_hash_setup(&operation, alg);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to setup hash operation");
        return;
    }

    size_t md5_len = PSA_HASH_LENGTH(alg);

    unsigned char digest[md5_len];

    size_t read;

    do {
        read = fread((void*) buf, 1, sizeof(buf), f);
        status = psa_hash_update(&operation, (unsigned const char*) buf, read);
        if (status != PSA_SUCCESS) {
            ESP_LOGE(TAG, "Failed to update hash operation");
            return;
        }
    } while(read == sizeof(buf));

    size_t md5len = 0;
    status = psa_hash_finish(&operation, digest, md5_len, &md5len);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to finish hash operation");
        return;
    }

    // Create a string of the digest
    char digest_str[MD5_MAX_LEN * 2];

    for (int i = 0; i < MD5_MAX_LEN; i++) {
        sprintf(&digest_str[i * 2], "%02x", (unsigned int)digest[i]);
    }

    // For reference, MD5 should be deeb71f585cbb3ae5f7976d5127faf2a
    ESP_LOGI(TAG, "Computed MD5 hash of alice.txt: %s", digest_str);

    fclose(f);
}

void app_main(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = false
    };

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    /* The following calls demonstrate reading files from the generated SPIFFS
     * image. The images should contain the same files and contents as the spiffs_image directory.
     */

    // Read and display the contents of a small text file (hello.txt)
    read_hello_txt();

    // Compute and display the MD5 hash of a large text file (alice.txt)
    compute_alice_txt_md5();

    // All done, unmount partition and disable SPIFFS
    esp_vfs_spiffs_unregister(NULL);
    ESP_LOGI(TAG, "SPIFFS unmounted");
}
