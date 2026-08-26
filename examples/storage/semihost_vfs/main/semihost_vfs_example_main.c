/* SPIFFS filesystem example.
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_cpu.h"
#include "esp_vfs_semihost.h"

static const char *TAG = "example";

#define STRINGIFY(s) STRINGIFY2(s)
#define STRINGIFY2(s) #s

static uint8_t s_buf[512];

void app_main(void)
{
    // Register host FS at '/host'. On the host file will be written/read in the current semihosting dir of OpenOCD
    esp_err_t ret = esp_vfs_semihost_register("/host");
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register semihost driver (%s)!", esp_err_to_name(ret));
        return;
    }

    ESP_LOGW(TAG, "Switch to semihosted stdout");
    FILE *fout = freopen("/host/esp32_stdout.txt", "w", stdout);
    if (fout == NULL) {
        ESP_LOGE(TAG, "Failed to reopen stdout (%d)!", errno);
        return;
    }
    // Increase file buffer to perform data transfers using larger chunks.
    // Every read/write triggers breakpoint, so transferring of small chunks is quite inefficient.
    setvbuf(fout, (char *)s_buf, _IOFBF, sizeof(s_buf));

    // this will be printed to the file on host
    ESP_LOGW(TAG, "Switched to semihosted stdout");
    for (int i = 0; i < 100; i++) {
        // printf is also redirected and sends data to the file on host
        printf("Semihosted stdout write %d\n", i);
    }
    ESP_LOGW(TAG, "Switch to UART stdout");
    fflush(fout); // ensure that all data are sent to the host file
    // ftell can also be used, get file size before closing it in `freopen`
    int count = ftell(fout);
    stdout = freopen("/dev/console", "w", fout);
    if (stdout == NULL) {
        ESP_LOGE(TAG, "Failed to reopen semihosted stdout (%d)!", errno);
        return;
    }
    // all remaining messages will be printed to UART
    ESP_LOGW(TAG, "Switched back to UART stdout");
    ESP_LOGI(TAG, "Wrote %d bytes", count);

    printf("====================== HOST DATA START =========================\n");
    // open() can also be used to access files on the host
    int fd = open("/host/host_file.txt", O_RDONLY, 0);
    if (fd == -1) {
        ESP_LOGE(TAG, "Failed to open file (%d)!", errno);
        return;
    }
    ssize_t read_bytes;
    count = 0;
    do {
        read_bytes = read(fd, s_buf, sizeof(s_buf));
        if(read_bytes == -1) {
            ESP_LOGE(TAG, "Failed to read file (%d)!", errno);
        } else if(read_bytes > 0) {
            fwrite(s_buf, 1, read_bytes, stdout);
            count += read_bytes;
        }
    } while(read_bytes > 0);
    printf("====================== HOST DATA END =========================\n");
    ESP_LOGI(TAG, "Read %d bytes", count);
    if (close(fd) == -1) {
        ESP_LOGE(TAG, "Failed to close file (%d)!", errno);
    }

    ret = esp_vfs_semihost_unregister("/host");
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to unregister semihost driver (%s)!", esp_err_to_name(ret));
    }
}
