/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_netif.h"

// Use linux system sockets to connect to tap interface
#define LWIP_HDR_LINUX_SYS_SOCKETS_H
#include <sys/socket.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <lwip/sys.h>
#include "errno.h"

#include <linux/if.h>
#include <linux/if_tun.h>

#define DEVTAP "/dev/net/tun"
#define DEVTAP_NAME "tap0"


typedef struct tap_io {
    esp_netif_driver_base_t base;
    int fd;
} tap_io_t;

static const char *TAG = "tap-netif";

static void tapio_input_task(void *arg)
{
    tap_io_t *io = arg;
    fd_set fdset;
    int ret;
    while (1) {
        FD_ZERO(&fdset);
        FD_SET(io->fd, &fdset);

        /* Wait for a packet to arrive. */
        ret = select(io->fd + 1, &fdset, NULL, NULL, NULL);

        if (ret == 1) {
            /* Handle incoming packet. */
            ssize_t readlen;
            char buf[1518]; /* max packet size including VLAN excluding CRC */

            /* Obtain the size of the packet and put it into the "len"
               variable. */
            readlen = read(io->fd, buf, sizeof(buf));
            if (readlen < 0) {
                ESP_LOGE(TAG, "Failed to read from tap fd: returned %ld", readlen);
                exit(1);
            }
#if CONFIG_EXAMPLE_CONNECT_TAPIF_IN_LOSS
            if (((double)rand()/(double)RAND_MAX) < ((double)CONFIG_EXAMPLE_CONNECT_TAPIF_IN_LOSS)/100.0) {
                ESP_LOGW(TAG, "Simulated packet drop on input");
                continue;
            }
#endif
            esp_netif_receive(io->base.netif, buf, readlen, NULL);
        } else if (ret == -1) {
            if (errno == EINTR /* Interrupted system call (used by FreeRTOS simulated interrupts) */) {
                vTaskDelay(1); // yield to the FreeRTOS simulator
            } else {
                ESP_LOGE(TAG, "tapif_thread: select() error(%d), %s", errno, strerror(errno));
            }
        }
    }
}

static esp_err_t tapio_start(esp_netif_t *esp_netif, void *arg)
{
    tap_io_t *io = arg;
    io->base.netif = esp_netif;
    esp_netif_action_start(esp_netif, 0, 0, 0);

    sys_thread_new("tapio_rx", tapio_input_task, io, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);

    return ESP_OK;
}

void *tapio_create(void)
{
    static tap_io_t tap_io = {};
    tap_io.base.post_attach = tapio_start;
    tap_io.fd = open(DEVTAP, O_RDWR);
    if (tap_io.fd == -1) {
        ESP_LOGE(TAG, "Cannot open tap device %s", DEVTAP);
        return NULL;
    }

    struct ifreq ifr = {};
    memset(&ifr, 0, sizeof(ifr));

    strncpy(ifr.ifr_name, DEVTAP_NAME, sizeof(ifr.ifr_name));
    ifr.ifr_name[sizeof(ifr.ifr_name)-1] = 0; /* ensure \0 termination */
    ifr.ifr_flags = IFF_TAP|IFF_NO_PI;
    if (ioctl(tap_io.fd, TUNSETIFF, (void *) &ifr) < 0) {
        ESP_LOGE(TAG, "Cannot configure ioctl(TUNSETIFF) for \"%s\"", DEVTAP);
        return NULL;
    }


    return &tap_io;
}

esp_err_t tapio_output(void *h, void *buffer, size_t len)
{
    tap_io_t *io = h;
    ssize_t written;

#if CONFIG_EXAMPLE_CONNECT_TAPIF_OUT_LOSS
    if (((double)rand()/(double)RAND_MAX) < ((double)CONFIG_EXAMPLE_CONNECT_TAPIF_OUT_LOSS)/100.0) {
        ESP_LOGW(TAG, "Simulated packet drop on output");
        return ESP_OK; /* ESP_OK because we simulate packet loss on cable */
    }
#endif

    /* signal that packet should be sent(); */
    written = write(io->fd, buffer, len);
    if (written < len) {
        ESP_LOGE(TAG, "Failed to write from tap fd: returned %ld", written);
        return ESP_FAIL;
    }
    return ESP_OK;
}
