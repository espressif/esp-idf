/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h> // read/write
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "esp_err.h"
#include "esp_eth.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "esp_vfs_l2tap.h"
#include "lwip/prot/ethernet.h" // Ethernet header
#include "arpa/inet.h" // ntohs, etc.
#include "protocol_examples_common.h"

#if !defined(CONFIG_EXAMPLE_CONNECT_ETHERNET)
#error Ethernet interface is not configured to connect.
#endif

#define ETH_INTERFACE           "ETH_DEF"
#define ETH_TYPE_FILTER_BLOCK    0x2220
#define ETH_TYPE_FILTER_NOBLOCK  0x2221
#define ETH_TYPE_FILTER_TX       0x2223

#define INVALID_FD              -1

typedef struct {
    struct eth_hdr header;
    char payload[44];
} test_vfs_eth_tap_msg_t;

static const char *TAG = "l2tap_example";


/** Opens and configures L2 TAP file descriptor */
static int init_l2tap_fd(int flags, uint16_t eth_type_filter)
{
    int fd = open("/dev/net/tap", flags);
    if (fd < 0) {
        ESP_LOGE(TAG, "Unable to open L2 TAP interface: errno %d", errno);
        goto error;
    }
    ESP_LOGI(TAG, "/dev/net/tap fd %d successfully opened", fd);

    // Check fd block status (just for demonstration purpose)
    flags = 0;
    flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        ESP_LOGE(TAG, "Unable to get L2 TAP fd %d status flag: errno %d", fd, errno);
        goto error;
    }
    if (flags & O_NONBLOCK) {
        ESP_LOGI(TAG, "L2 TAP fd %d configured in non-blocking mode", fd);
    } else {
        ESP_LOGI(TAG, "L2 TAP fd %d configured in blocking mode", fd);
    }

    // Configure Ethernet interface on which to get raw frames
    int ret;
    if ((ret = ioctl(fd, L2TAP_S_INTF_DEVICE, ETH_INTERFACE)) == -1) {
        ESP_LOGE(TAG, "Unable to bound L2 TAP fd %d with Ethernet device: errno %d", fd, errno);
        goto error;
    }
    ESP_LOGI(TAG, "L2 TAP fd %d successfully bound to `%s`", fd, ETH_INTERFACE);

    // Configure Ethernet frames we want to filter out
    if ((ret = ioctl(fd, L2TAP_S_RCV_FILTER, &eth_type_filter)) == -1) {
        ESP_LOGE(TAG, "Unable to configure fd %d Ethernet type receive filter: errno %d", fd, errno);
        goto error;
    }
    ESP_LOGI(TAG, "L2 TAP fd %d Ethernet type filter configured to 0x%x", fd, eth_type_filter);

    return fd;
error:
    if (fd != INVALID_FD) {
        close(fd);
    }
    return INVALID_FD;
}

/** Creates "echo" message from received frame */
static void create_echo_frame(test_vfs_eth_tap_msg_t *in_frame, test_vfs_eth_tap_msg_t *out_frame, int len)
{
    // Set source address equal to our MAC address
    esp_eth_handle_t eth_hndl = get_example_eth_handle();
    uint8_t mac_addr[ETH_ADDR_LEN];
    esp_eth_ioctl(eth_hndl, ETH_CMD_G_MAC_ADDR, mac_addr);
    memcpy(out_frame->header.src.addr, mac_addr, ETH_ADDR_LEN);
    // Set destination address equal to source address from where the frame was received
    memcpy(out_frame->header.dest.addr, in_frame->header.src.addr, ETH_ADDR_LEN);
    // Set Ethernet type
    memcpy(&out_frame->header.type, &in_frame->header.type, sizeof(uint16_t));
    // Copy the payload
    memcpy(out_frame->payload, in_frame->payload, len - ETH_HEADER_LEN);
}

/** Demonstrates usage of L2 TAP in blocking mode */
static void echo_l2tap_task(void *pvParameters)
{
    uint8_t rx_buffer[128];
    int eth_tap_fd;

    // Open and configure L2 TAP File descriptor
    if ((eth_tap_fd = init_l2tap_fd(0, ETH_TYPE_FILTER_BLOCK)) == INVALID_FD) {
        goto error;
    }

    while (1) {
        ssize_t len = read(eth_tap_fd, rx_buffer, sizeof(rx_buffer));
        if (len > 0) {
            test_vfs_eth_tap_msg_t *recv_msg = (test_vfs_eth_tap_msg_t *)rx_buffer;
            ESP_LOGI(TAG, "fd %d received %d bytes from %.2x:%.2x:%.2x:%.2x:%.2x:%.2x", eth_tap_fd,
                        len, recv_msg->header.src.addr[0], recv_msg->header.src.addr[1], recv_msg->header.src.addr[2],
                        recv_msg->header.src.addr[3], recv_msg->header.src.addr[4], recv_msg->header.src.addr[5]);
            // Construct echo frame
            test_vfs_eth_tap_msg_t echo_msg;
            create_echo_frame(recv_msg, &echo_msg, len);

            // Send the echo message
            ssize_t ret = write(eth_tap_fd, &echo_msg, len);
            if (ret == -1) {
                ESP_LOGE(TAG, "L2 TAP fd %d write error: errno: %d", eth_tap_fd, errno);
                break;
            }
        } else {
            ESP_LOGE(TAG, "L2 TAP fd %d read error: errno %d", eth_tap_fd, errno);
            break;
        }
    }
    close(eth_tap_fd);
error:
    vTaskDelete(NULL);
}

/** Demonstrates usage of L2 TAP non-blocking mode with select */
static void nonblock_l2tap_echo_task(void *pvParameters)
{
    uint8_t rx_buffer[128];
    int eth_tap_fd;

    // Open and configure L2 TAP File descriptor
    if ((eth_tap_fd = init_l2tap_fd(O_NONBLOCK, ETH_TYPE_FILTER_NOBLOCK)) == INVALID_FD) {
        goto error;
    }

    while (1) {
        struct timeval tv;
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(eth_tap_fd, &rfds);

        int ret_sel = select(eth_tap_fd + 1, &rfds, NULL, NULL, &tv);
        if (ret_sel > 0) {
            ssize_t len = read(eth_tap_fd, rx_buffer, sizeof(rx_buffer));
            if (len > 0) {
                test_vfs_eth_tap_msg_t *recv_msg = (test_vfs_eth_tap_msg_t *)rx_buffer;
                ESP_LOGI(TAG, "fd %d received %d bytes from %.2x:%.2x:%.2x:%.2x:%.2x:%.2x", eth_tap_fd,
                            len, recv_msg->header.src.addr[0], recv_msg->header.src.addr[1], recv_msg->header.src.addr[2],
                            recv_msg->header.src.addr[3], recv_msg->header.src.addr[4], recv_msg->header.src.addr[5]);
                // Construct echo frame
                test_vfs_eth_tap_msg_t echo_msg;
                create_echo_frame(recv_msg, &echo_msg, len);

                // Send the echo message
                ssize_t ret = write(eth_tap_fd, &echo_msg, len);
                if (ret == -1) {
                    ESP_LOGE(TAG, "L2 TAP fd %d write error: errno: %d", eth_tap_fd, errno);
                    break;
                }
            } else {
                ESP_LOGE(TAG, "L2 TAP fd %d read error: errno %d", eth_tap_fd, errno);
                break;
            }
        } else if (ret_sel == 0) {
            ESP_LOGD(TAG, "L2 TAP select timeout");
        } else {
            ESP_LOGE(TAG, "L2 TAP select error: errno %d", errno);
            break;
        }
    }
    close(eth_tap_fd);
error:
    vTaskDelete(NULL);
}

/** Demonstrates of how to construct Ethernet frame for transmit via L2 TAP */
static void hello_tx_l2tap_task(void *pvParameters)
{
    uint16_t eth_type_filter = ETH_TYPE_FILTER_TX;
    int eth_tap_fd;

    // Open and configure L2 TAP File descriptor
    if ((eth_tap_fd = init_l2tap_fd(0, eth_type_filter)) == INVALID_FD) {
        goto error;
    }

    // Construct "Hello" frame
    test_vfs_eth_tap_msg_t hello_msg = {
        .header = {
            .src.addr = {0},
            .dest.addr = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, // broadcast address
            .type = htons(eth_type_filter)                     // convert to big endian (network) byte order
        },
        .payload = "ESP32 hello to everybody!"
    };
    esp_eth_handle_t eth_hndl = get_example_eth_handle();
    esp_eth_ioctl(eth_hndl, ETH_CMD_G_MAC_ADDR, hello_msg.header.src.addr);

    while (1) {
        // Send the Hello frame
        ssize_t ret = write(eth_tap_fd, &hello_msg, ETH_HEADER_LEN + strlen(hello_msg.payload));
        if (ret == -1) {
            ESP_LOGE(TAG, "L2 TAP fd %d write error: errno: %d", eth_tap_fd, errno);
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
    close(eth_tap_fd);
error:
    vTaskDelete(NULL);
}

void app_main(void)
{
    // Initialize L2 TAP VFS interface
    ESP_ERROR_CHECK(esp_vfs_l2tap_intf_register(NULL));

    // Initialize NVS Flash
    ESP_ERROR_CHECK(nvs_flash_init());
    // Initialize TCP/IP network interface (should be called only once in application)
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    esp_eth_handle_t eth_hndl = get_example_eth_handle();
    uint8_t mac_addr[ETH_ADDR_LEN];
    esp_eth_ioctl(eth_hndl, ETH_CMD_G_MAC_ADDR, mac_addr);
    ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    // Echo received message back to sender (blocks indefinitely)
    xTaskCreate(echo_l2tap_task, "echo", 4096, NULL, 6, NULL);
    // Echo received message back to sender (blocks with timeout)
    xTaskCreate(nonblock_l2tap_echo_task, "echo_no-block", 4096, NULL, 5, NULL);
    // Periodically broadcast "Hello message"
    xTaskCreate(hello_tx_l2tap_task, "hello_tx", 4096, NULL, 4, NULL);
}
