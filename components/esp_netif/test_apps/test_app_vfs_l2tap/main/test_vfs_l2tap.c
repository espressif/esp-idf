/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "arpa/inet.h" // for ntohs, etc.
#include "lwip/prot/ethernet.h" // Ethernet headers
#include "soc/soc_caps.h"

#include "unity.h"
#include "test_utils.h"

#include "esp_vfs_l2tap.h"
#include "esp_eth_mac_esp.h"

#define ETH_FILTER_LE 0x7A05
#define ETH_FILTER_BE 0x057A
#define ETH_TYPE_PTP  0x88F7

#define ETH_START_BIT BIT(0)
#define ETH_STOP_BIT BIT(1)
#define ETH_CONNECT_BIT BIT(2)
#define ETH_GOT_IP_BIT BIT(3)

#define DEFAULT_SEND_DELAY_MS 1000

#define IN_BUFFER_SIZE 1500

static const char *TAG = "l2tap_test";

char *in_buffer;

typedef struct {
    esp_netif_t *eth_netif;
    esp_eth_mac_t *mac;
    esp_eth_phy_t *phy;
    void *glue;
    esp_eth_handle_t eth_handle;
} test_vfs_eth_network_t;

typedef struct {
    test_vfs_eth_network_t *eth_network_hndls_p;
    int32_t eth_type;
    uint32_t send_delay_ms;
} send_task_control_t;

typedef struct {
    struct eth_hdr header;
    union {
        int cnt;
        char str[44];
    };
} test_vfs_eth_tap_msg_t;

// PTPv2 header
typedef struct {
    uint8_t message_type;           // 4 bits: Message Type
    uint8_t version;                // 4 bits: PTP version
    uint16_t message_length;        // 16 bits: Total length of the PTP message
    uint8_t domain_number;          // 8 bits: Domain number
    uint8_t reserved1;              // Reserved (8 bits)
    uint16_t flags;                 // 16 bits: Flags field
    int64_t correction_field;       // 64 bits: Correction field
    uint32_t reserved2;             // Reserved (32 bits)
    uint64_t clock_identity;    // 64 bits: Clock identity
    uint16_t port_number;       // 16 bits: Port number
    uint16_t sequence_id;           // 16 bits: Sequence ID
    uint8_t control_field;          // 8 bits: Control field (deprecated)
    int8_t log_message_interval;    // 8 bits: Log message interval
} __attribute__((packed)) ptpv2_hdr_t;

typedef struct {
    ptpv2_hdr_t ptp_hdr;
    uint64_t timestamp;
} __attribute__((packed)) ptp_msg_t;

typedef struct {
    struct eth_hdr eth_hdr;
    ptp_msg_t ptp_msg;
} __attribute__((packed)) test_eth_ptp_msg_t;

/* =============================================================================
 *                              Common Routines
 * ============================================================================= */

void setUp(void) {
    in_buffer = calloc(IN_BUFFER_SIZE, sizeof(*in_buffer));
    if (!in_buffer) {
        abort();
    }
}

void tearDown(void) {
    free(in_buffer);
    in_buffer = NULL;
}

/**
 * @brief Event handler for Ethernet events
 *
 */
static void eth_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    EventGroupHandle_t eth_event_group = (EventGroupHandle_t)arg;

    uint8_t mac_addr[6] = {0};
    /* we can get the ethernet driver handle from event data */
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

    switch (event_id) {
    case ETHERNET_EVENT_CONNECTED:
        esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
        ESP_LOGI(TAG, "Ethernet Link Up");
        ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        xEventGroupSetBits(eth_event_group, ETH_CONNECT_BIT);
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Down");
        break;
    case ETHERNET_EVENT_START:
        xEventGroupSetBits(eth_event_group, ETH_START_BIT);
        ESP_LOGI(TAG, "Ethernet Started");
        break;
    case ETHERNET_EVENT_STOP:
        xEventGroupSetBits(eth_event_group, ETH_STOP_BIT);
        ESP_LOGI(TAG, "Ethernet Stopped");
        break;
    default:
        break;
    }
}

/**
 * @brief Event handler for IP_EVENT_ETH_GOT_IP
 *
 */
static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
    EventGroupHandle_t eth_event_group = (EventGroupHandle_t)arg;

    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    ESP_LOGI(TAG, "Ethernet Got IP Address");
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");

    xEventGroupSetBits(eth_event_group, ETH_GOT_IP_BIT);
}

/**
 * @brief Initializes Ethernet interface and starts driver (internal EMAC & IP101 PHY)
 *
 */
static void ethernet_init(test_vfs_eth_network_t *network_hndls)
{
    EventBits_t bits = 0;
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);

    test_case_uses_tcpip();
    TEST_ESP_OK(esp_event_loop_create_default());
    // create TCP/IP netif
    esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
    network_hndls->eth_netif = esp_netif_new(&netif_cfg);

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    network_hndls->mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
#ifdef CONFIG_IDF_TARGET_ESP32P4
    phy_config.reset_gpio_num = 51;
#endif // CONFIG_IDF_TARGET_ESP32P4
    network_hndls->phy = esp_eth_phy_new_generic(&phy_config);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(network_hndls->mac, network_hndls->phy);
    network_hndls->eth_handle = NULL;
    // install Ethernet driver
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &network_hndls->eth_handle));
    // combine driver with netif
    network_hndls->glue = esp_eth_new_netif_glue(network_hndls->eth_handle);
    TEST_ESP_OK(esp_netif_attach(network_hndls->eth_netif, network_hndls->glue));
    // register user defined event handlers
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    TEST_ESP_OK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, eth_event_group));

    // set PHY loopback mode
    bool loopback_en = true;
    esp_eth_ioctl(network_hndls->eth_handle, ETH_CMD_S_PHY_LOOPBACK, &loopback_en);

    TEST_ESP_OK(esp_eth_start(network_hndls->eth_handle));
    // wait for Ethernet start
    bits = xEventGroupWaitBits(eth_event_group, ETH_START_BIT, true, true, pdMS_TO_TICKS(10000));
    TEST_ASSERT((bits & ETH_START_BIT) == ETH_START_BIT);

    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(10000));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
}

/**
 * @brief De-initializes Ethernet interface
 *
 */
static void ethernet_deinit(test_vfs_eth_network_t *network_hndls)
{
    TEST_ESP_OK(esp_eth_stop(network_hndls->eth_handle));
    TEST_ESP_OK(esp_eth_del_netif_glue(network_hndls->glue));
    esp_eth_driver_uninstall(network_hndls->eth_handle);
    TEST_ESP_OK(network_hndls->phy->del(network_hndls->phy));
    TEST_ESP_OK(network_hndls->mac->del(network_hndls->mac));
    TEST_ESP_OK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, got_ip_event_handler));
    TEST_ESP_OK(esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler));
    esp_netif_destroy(network_hndls->eth_netif);
    TEST_ESP_OK(esp_event_loop_delete_default());
}

// Global test message
static test_vfs_eth_tap_msg_t s_test_msg = {
    .header = {
        .src.addr = {0},
        .dest.addr = {0},
        .type = ETH_FILTER_BE,
    },
    .str = "This is ESP32 L2 TAP test msg"
};

/**
 * @brief "Asynchronously" sends global test message (send can be delayed per actual needs)
 *
 */
static void send_task(void *task_param)
{
    send_task_control_t *send_control = (send_task_control_t *)task_param;
    test_vfs_eth_network_t *eth_network_hndls = send_control->eth_network_hndls_p;

    // Set test message source MAC address to the MAC of the Ethernet interface
    esp_eth_ioctl(eth_network_hndls->eth_handle, ETH_CMD_G_MAC_ADDR, &s_test_msg.header.src.addr);
    // Set test message destination MAC address to the MAC of the Ethernet interface to not be filtered out in loopback mode
    esp_eth_ioctl(eth_network_hndls->eth_handle, ETH_CMD_G_MAC_ADDR, &s_test_msg.header.dest.addr);

    if (send_control->eth_type >= 0 && send_control->eth_type <= 0xFFFF) {
        s_test_msg.header.type = htons(send_control->eth_type);
    } else {
        s_test_msg.header.type = ETH_FILTER_BE;
    }

    int eth_tap_fd_send = open("/dev/net/tap", O_NONBLOCK);
    TEST_ASSERT_NOT_EQUAL(-1, eth_tap_fd_send);

    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd_send, L2TAP_S_INTF_DEVICE, "ETH_DEF"));

    // delay message write
    vTaskDelay(pdMS_TO_TICKS(send_control->send_delay_ms));
    int n = write(eth_tap_fd_send, &s_test_msg, sizeof(s_test_msg));
    TEST_ASSERT_NOT_EQUAL(-1, n);

    TEST_ASSERT_EQUAL(0, close(eth_tap_fd_send));
    vTaskDelete(NULL);
}

/* =============================================================================
 *                              Tests
 * ============================================================================= */

/**
 * @brief Verifies vfs register/unregister functions
 *
 */
TEST_CASE("esp32 l2tap - vfs register", "[ethernet]")
{
    int eth_tap_fd;

    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_register(NULL));
    ESP_LOGI(TAG, "Verify that L2 TAP VFS can be registered only once...");
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_vfs_l2tap_intf_register(NULL));

    //ethernet_init(&eth_network_hndls);

    eth_tap_fd = open("/dev/net/tap", O_NONBLOCK);
    TEST_ASSERT_NOT_EQUAL(-1, eth_tap_fd);

    ESP_LOGI(TAG, "Verify that L2 TAP VFS cannot be unregistered prior closing FD's...");
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_vfs_l2tap_intf_unregister(NULL));
    TEST_ASSERT_EQUAL(0, close(eth_tap_fd));
    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_unregister(NULL));

    ESP_LOGI(TAG, "Verify that L2 TAP VFS can be registered with different base path...");
    l2tap_vfs_config_t config = {
        .base_path = "/dev/super_tap",
    };
    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_register(&config));
    eth_tap_fd = open("/dev/net/tap", O_NONBLOCK);
    TEST_ASSERT_EQUAL(-1, eth_tap_fd);
    eth_tap_fd = open("/dev/super_tap", O_NONBLOCK);
    TEST_ASSERT_NOT_EQUAL(-1, eth_tap_fd);

    ESP_LOGI(TAG, "Verify that L2 TAP VFS cannot be unregistered with default base path...");
    TEST_ASSERT_EQUAL(0, close(eth_tap_fd));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_vfs_l2tap_intf_unregister(NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_unregister("/dev/super_tap"));
}

/**
 * @brief Verifies open and close functions
 *
 */

typedef struct {
    int eth_tap_fd;
    SemaphoreHandle_t sem;
    bool on_select;
    int queue_frames_num;
} open_close_task_ctrl_t;

static void open_read_task(void *task_param)
{
    const size_t in_buf_size = 300;
    open_close_task_ctrl_t *task_control = (open_close_task_ctrl_t *)task_param;

    task_control->eth_tap_fd = open("/dev/net/tap", 0);
    TEST_ASSERT_NOT_EQUAL(-1, task_control->eth_tap_fd);

    // Set Ethernet interface on which to get raw frames
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(task_control->eth_tap_fd, L2TAP_S_INTF_DEVICE, "ETH_DEF"));
    // Check the Ethernet interface was assigned
    char *if_key_str;
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(task_control->eth_tap_fd, L2TAP_G_INTF_DEVICE, &if_key_str));
    TEST_ASSERT_EQUAL_STRING("ETH_DEF", if_key_str);

    // Set the Ethertype filter (frames with this type will be available through the eth_tap_fd)
    uint16_t eth_type_filter = ETH_FILTER_LE;
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(task_control->eth_tap_fd, L2TAP_S_RCV_FILTER, &eth_type_filter));

    if (task_control->queue_frames_num > 0) {
        for (int i = 0; i < task_control->queue_frames_num; i++) {
            TEST_ASSERT_NOT_EQUAL(-1, write(task_control->eth_tap_fd, &s_test_msg, sizeof(s_test_msg)));
        }
    } else {
        xSemaphoreGive(task_control->sem);
        if (task_control->on_select == true) {
            ESP_LOGI(TAG, "task1: going to block on select...");
            struct timeval tv;
            tv.tv_sec = 1;
            tv.tv_usec = 0;

            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(task_control->eth_tap_fd, &rfds);

            // it is expected that blocking select is not unblocked by close and it timeouts (the fd number may be reused later
            // though and so select released but that's not tested here)
            TEST_ASSERT_EQUAL(0, select(task_control->eth_tap_fd + 1, &rfds, NULL, NULL, &tv));
            ESP_LOGI(TAG, "task1: select timeout");

            // get an error when try to use closed fd
            TEST_ASSERT_EQUAL(-1, read(task_control->eth_tap_fd, in_buffer, in_buf_size));
        } else {
            ESP_LOGI(TAG, "task1: going to block on read...");
            // it is expected that blocking read is unblocked by close and we read zero bytes
            TEST_ASSERT_EQUAL(0, read(task_control->eth_tap_fd, in_buffer, in_buf_size));
            ESP_LOGI(TAG, "task1: unblocked");
        }
    }
    xSemaphoreGive(task_control->sem);

    vTaskDelete(NULL);
}

static void close_task(void *task_param)
{
    open_close_task_ctrl_t *task_control = (open_close_task_ctrl_t *)task_param;
    vTaskDelay(pdMS_TO_TICKS(500));
    ESP_LOGI(TAG, "task2: closing...");
    TEST_ASSERT_EQUAL(0, close(task_control->eth_tap_fd));

    if (task_control->queue_frames_num > 0) {
        // since there is no blocking "read" task in this scenario, we need to signal that close finished
        xSemaphoreGive(task_control->sem);
    }
    vTaskDelete(NULL);
}

TEST_CASE("esp32 l2tap - open/close", "[ethernet]")
{
    test_vfs_eth_network_t eth_network_hndls;

    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_register(NULL));
    ethernet_init(&eth_network_hndls);

    open_close_task_ctrl_t task_control;
    task_control.sem = xSemaphoreCreateBinary();
    task_control.on_select = false;
    task_control.queue_frames_num = 0;

    // ==========================================================
    // Close when blocking on read
    // ==========================================================
    ESP_LOGI(TAG, "Verify closing blocking read from higher priority task...");
    xTaskCreate(open_read_task, "open_read_task", 4096, &task_control, 5, NULL);
    TEST_ASSERT_NOT_EQUAL(pdFALSE, xSemaphoreTake(task_control.sem, pdMS_TO_TICKS(1000)));
    xTaskCreate(close_task, "close_task", 4096, &task_control, 10, NULL);
    TEST_ASSERT_NOT_EQUAL(pdFALSE, xSemaphoreTake(task_control.sem, pdMS_TO_TICKS(1000)));

    ESP_LOGI(TAG, "Verify closing blocking read from lower priority task...");
    xTaskCreate(open_read_task, "open_read_task", 4096, &task_control, 10, NULL);
    TEST_ASSERT_NOT_EQUAL(pdFALSE, xSemaphoreTake(task_control.sem, pdMS_TO_TICKS(1000)));
    xTaskCreate(close_task, "close_task", 4096, &task_control, 5, NULL);
    TEST_ASSERT_NOT_EQUAL(pdFALSE, xSemaphoreTake(task_control.sem, pdMS_TO_TICKS(1000)));

    // ==========================================================
    // Close when blocking on select
    // ==========================================================
    task_control.on_select = true;
    ESP_LOGI(TAG, "Verify closing blocking select from higher priority task...");
    xTaskCreate(open_read_task, "open_read_task", 4096, &task_control, 5, NULL);
    TEST_ASSERT_NOT_EQUAL(pdFALSE, xSemaphoreTake(task_control.sem, pdMS_TO_TICKS(2000)));
    xTaskCreate(close_task, "close_task", 4096, &task_control, 10, NULL);
    TEST_ASSERT_NOT_EQUAL(pdFALSE, xSemaphoreTake(task_control.sem, pdMS_TO_TICKS(2000)));

    ESP_LOGI(TAG, "Verify closing blocking select from lower priority task...");
    xTaskCreate(open_read_task, "open_read_task", 4096, &task_control, 10, NULL);
    TEST_ASSERT_NOT_EQUAL(pdFALSE, xSemaphoreTake(task_control.sem, pdMS_TO_TICKS(2000)));
    xTaskCreate(close_task, "close_task", 4096, &task_control, 5, NULL);
    TEST_ASSERT_NOT_EQUAL(pdFALSE, xSemaphoreTake(task_control.sem, pdMS_TO_TICKS(2000)));

    // ==========================================================
    // Close when buffered frames pending in L2 TAP
    // ==========================================================
    // indicate to queue frames
    task_control.queue_frames_num = 3;
    ESP_LOGI(TAG, "Verify closing from higher priority task when when buffered frames pending...");
    xTaskCreate(open_read_task, "open_read_task", 4096, &task_control, 5, NULL);
    TEST_ASSERT_NOT_EQUAL(pdFALSE, xSemaphoreTake(task_control.sem, pdMS_TO_TICKS(2000)));
    xTaskCreate(close_task, "close_task", 4096, &task_control, 10, NULL);
    TEST_ASSERT_NOT_EQUAL(pdFALSE, xSemaphoreTake(task_control.sem, pdMS_TO_TICKS(2000)));

    ESP_LOGI(TAG, "Verify closing from lower priority task when when buffered frames pending...");
    xTaskCreate(open_read_task, "open_read_task", 4096, &task_control, 10, NULL);
    TEST_ASSERT_NOT_EQUAL(pdFALSE, xSemaphoreTake(task_control.sem, pdMS_TO_TICKS(2000)));
    xTaskCreate(close_task, "close_task", 4096, &task_control, 5, NULL);
    TEST_ASSERT_NOT_EQUAL(pdFALSE, xSemaphoreTake(task_control.sem, pdMS_TO_TICKS(2000)));

    // ==========================================================
    // Limit of opened FD's
    // ==========================================================
    ESP_LOGI(TAG, "Verify limit of opened fds...");
    int eth_tap_fds[CONFIG_ESP_NETIF_L2_TAP_MAX_FDS + 10];
    int i;
    for (i = 0; i < CONFIG_ESP_NETIF_L2_TAP_MAX_FDS + 10; i++) {
        if ((eth_tap_fds[i] = open("/dev/net/tap", 0)) == -1) {
            break;
        }
    }
    TEST_ASSERT_EQUAL(CONFIG_ESP_NETIF_L2_TAP_MAX_FDS, i);
    while (--i >= 0) {
        TEST_ASSERT_EQUAL(0, close(eth_tap_fds[i]));
    }

    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_unregister(NULL));

    vSemaphoreDelete(task_control.sem);
    ethernet_deinit(&eth_network_hndls);
}

/* ============================================================================= */
/**
 * @brief Verifies that read does not block when fd is opened in non-blocking mode
 *
 */
TEST_CASE("esp32 l2tap - non blocking read", "[ethernet]")
{
    test_vfs_eth_network_t eth_network_hndls;
    int eth_tap_fd;
    int n;
    int loop_cnt = 0;

    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_register(NULL));
    ethernet_init(&eth_network_hndls);

    eth_tap_fd = open("/dev/net/tap", O_NONBLOCK);
    TEST_ASSERT_NOT_EQUAL(-1, eth_tap_fd);

    // Set Ethernet interface on which to get raw frames
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_S_INTF_DEVICE, "ETH_DEF"));
    // Check the Ethernet interface was assigned
    char *if_key_str;
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_G_INTF_DEVICE, &if_key_str));
    TEST_ASSERT_EQUAL_STRING("ETH_DEF", if_key_str);

    // Set the Ethertype filter (frames with this type will be available through the eth_tap_fd)
    uint16_t eth_type_filter = ETH_FILTER_LE;
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_S_RCV_FILTER, &eth_type_filter));

    // ==========================================================
    // Verify simple non-blocking read operations
    // ==========================================================
    ESP_LOGI(TAG, "Verify simple non-blocking read operations...");
    send_task_control_t send_task_ctrl = {
        .eth_network_hndls_p = &eth_network_hndls,
        .eth_type = -1,
        .send_delay_ms = DEFAULT_SEND_DELAY_MS,
    };
    xTaskCreate(send_task, "raw_eth_send_task", 1024, &send_task_ctrl, tskIDLE_PRIORITY + 2, NULL);

    // Verify the read does not block
    while (loop_cnt < 100) {
        if ((n = read(eth_tap_fd, in_buffer, IN_BUFFER_SIZE)) > 0) {
            ESP_LOG_BUFFER_HEX(TAG, in_buffer, n);
            ESP_LOGI(TAG, "recv test string: %s", ((test_vfs_eth_tap_msg_t *)in_buffer)->str);
            TEST_ASSERT_EQUAL_UINT8_ARRAY(&s_test_msg, in_buffer, n);
            break;
        } else {
            TEST_ASSERT_EQUAL(EAGAIN, errno);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
        loop_cnt++;
    }
    ESP_LOGI(TAG, "elapsed loop_cnts to read %d", loop_cnt);
    TEST_ASSERT_GREATER_THAN(0, loop_cnt);
    TEST_ASSERT_LESS_THAN(100, loop_cnt);
    TEST_ASSERT_EQUAL(sizeof(s_test_msg), n);

    // ==========================================================
    // Verify non-blocking successful read operations used along with select
    // ==========================================================
    ESP_LOGI(TAG, "Verify non-blocking successful read operations used along with select...");
    memset(in_buffer, 0, IN_BUFFER_SIZE);
    // Wait up to x seconds
    struct timeval tv;
    tv.tv_sec = 4;
    tv.tv_usec = 0;

    send_task_ctrl.eth_network_hndls_p = &eth_network_hndls;
    send_task_ctrl.send_delay_ms = DEFAULT_SEND_DELAY_MS;
    xTaskCreate(send_task, "raw_eth_send_task", 1024, &send_task_ctrl, tskIDLE_PRIORITY + 2, NULL);

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(eth_tap_fd, &rfds);

    TEST_ASSERT_NOT_EQUAL(-1, select(eth_tap_fd + 1, &rfds, NULL, NULL, &tv));
    TEST_ASSERT_GREATER_THAN(-1, FD_ISSET(eth_tap_fd, &rfds));
    loop_cnt = 0;
    while (loop_cnt < 100) {
        if ((n = read(eth_tap_fd, in_buffer, IN_BUFFER_SIZE)) > 0) {
            ESP_LOG_BUFFER_HEX(TAG, in_buffer, n);
            ESP_LOGI(TAG, "recv test string: %s", ((test_vfs_eth_tap_msg_t *)in_buffer)->str);
            TEST_ASSERT_EQUAL_UINT8_ARRAY(&s_test_msg, in_buffer, n);
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
        loop_cnt++;
    }
    ESP_LOGI(TAG, "elapsed loop_cnts to read %d", loop_cnt);
    TEST_ASSERT_EQUAL(0, loop_cnt);
    TEST_ASSERT_EQUAL(sizeof(s_test_msg), n);

    // ==========================================================
    // Verify non-blocking unsuccessful read operations used along with select
    // ==========================================================
    ESP_LOGI(TAG, "Verify non-blocking unsuccessful read operations used along with select...");
    memset(in_buffer, 0, IN_BUFFER_SIZE);
    // Wait up to x seconds
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    // no send_task => select will timeout

    FD_ZERO(&rfds);
    FD_SET(eth_tap_fd, &rfds);

    TEST_ASSERT_EQUAL(0, select(eth_tap_fd + 1, &rfds, NULL, NULL, &tv));
    TEST_ASSERT_EQUAL(EAGAIN, errno);

    n = read(eth_tap_fd, in_buffer, IN_BUFFER_SIZE);
    TEST_ASSERT_EQUAL(EAGAIN, errno);
    TEST_ASSERT_EQUAL(-1, n);

    TEST_ASSERT_EQUAL(0, close(eth_tap_fd));
    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_unregister(NULL));
    ethernet_deinit(&eth_network_hndls);
}

/* ============================================================================= */
/**
 * @brief Verifies that read blocks when fd opened in blocking mode
 *
 */
TEST_CASE("esp32 l2tap - blocking read", "[ethernet]")
{
    test_vfs_eth_network_t eth_network_hndls;
    int eth_tap_fd;
    int n;
    int loop_cnt = 0;

    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_register(NULL));
    ethernet_init(&eth_network_hndls);

    eth_tap_fd = open("/dev/net/tap", 0);
    TEST_ASSERT_NOT_EQUAL(-1, eth_tap_fd);

    // Set Ethernet interface on which to get raw frames
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_S_INTF_DEVICE, "ETH_DEF"));
    // Check the Ethernet interface was assigned
    char *if_key_str;
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_G_INTF_DEVICE, &if_key_str));
    TEST_ASSERT_EQUAL_STRING("ETH_DEF", if_key_str);

    // Set the Ethertype filter (frames with this type will be available through the eth_tap_fd)
    uint16_t eth_type_filter = ETH_FILTER_LE;
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_S_RCV_FILTER, &eth_type_filter));

    // ==========================================================
    // Verify simple blocking read operations
    // ==========================================================
    ESP_LOGI(TAG, "Verify simple blocking read operations...");
    send_task_control_t send_task_ctrl = {
        .eth_network_hndls_p = &eth_network_hndls,
        .eth_type = -1,
        .send_delay_ms = DEFAULT_SEND_DELAY_MS,
    };
    xTaskCreate(send_task, "raw_eth_send_task", 1024, &send_task_ctrl, tskIDLE_PRIORITY + 2, NULL);

    // Verify the read does block
    while (loop_cnt < 100) {
        if ((n = read(eth_tap_fd, in_buffer, IN_BUFFER_SIZE)) > 0) {
            ESP_LOG_BUFFER_HEX(TAG, in_buffer, n);
            ESP_LOGI(TAG, "recv test string: %s", ((test_vfs_eth_tap_msg_t *)in_buffer)->str);
            TEST_ASSERT_EQUAL_UINT8_ARRAY(&s_test_msg, in_buffer, n);
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
        loop_cnt++;
    }
    ESP_LOGI(TAG, "elapsed loop_cnts to read %d", loop_cnt);
    TEST_ASSERT_EQUAL(0, loop_cnt);
    TEST_ASSERT_EQUAL(sizeof(s_test_msg), n);

    TEST_ASSERT_EQUAL(0, close(eth_tap_fd));
    vTaskDelay(pdMS_TO_TICKS(50)); // just for sure to give some time to send task close fd
    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_unregister(NULL));
    ethernet_deinit(&eth_network_hndls);
}

/* ============================================================================= */
/**
 * @brief Verifies write
 *
 */
TEST_CASE("esp32 l2tap - write", "[ethernet]")
{
    test_vfs_eth_network_t eth_network_hndls;
    int eth_tap_fd;

    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_register(NULL));
    ethernet_init(&eth_network_hndls);

    eth_tap_fd = open("/dev/net/tap", 0);
    TEST_ASSERT_NOT_EQUAL(-1, eth_tap_fd);

    // Set Ethernet interface on which to get raw frames
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_S_INTF_DEVICE, "ETH_DEF"));
    // Check the Ethernet interface was assigned
    char *if_key_str;
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_G_INTF_DEVICE, &if_key_str));
    TEST_ASSERT_EQUAL_STRING("ETH_DEF", if_key_str);

    // Set the Ethertype filter (frames with this type will be available through the eth_tap_fd)
    uint16_t eth_type_filter = ETH_FILTER_LE;
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_S_RCV_FILTER, &eth_type_filter));


    ESP_LOGI(TAG, "Verify the write is not successful when use different Ethernet type than the fd is configured to...");
    test_vfs_eth_tap_msg_t test_msg;
    // Set test message source MAC address to the MAC of the Ethernet interface
    esp_eth_ioctl(eth_network_hndls.eth_handle, ETH_CMD_G_MAC_ADDR, &test_msg.header.src.addr);
    // Set test message destination MAC address to the MAC of the Ethernet interface to not be filtered out in loopback mode
    esp_eth_ioctl(eth_network_hndls.eth_handle, ETH_CMD_G_MAC_ADDR, &test_msg.header.dest.addr);
    // set different Ethernet type than the fd is configured to
    test_msg.header.type = htons(ETH_FILTER_LE + 10);
    TEST_ASSERT_EQUAL(-1, write(eth_tap_fd, &test_msg, sizeof(test_msg)));
    TEST_ASSERT_EQUAL(EBADMSG, errno);

    TEST_ASSERT_EQUAL(0, close(eth_tap_fd));
    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_unregister(NULL));
    ethernet_deinit(&eth_network_hndls);
}

/* ============================================================================= */
/**
 * @brief Verifies that concrurent access to shared resource (Ethernet) is correctly handled
 *
 */
#define NUM_OF_FDS 2
#define NUM_OF_TASKS 2

typedef struct {
    uint16_t task_id;
    uint16_t eth_filter;
    SemaphoreHandle_t semaphore;
    test_vfs_eth_network_t *eth_network_hndls_p;
} task_info_t;

static void multi_fds_task (void *task_param)
{
    task_info_t *task_info = (task_info_t *)task_param;
    uint16_t eth_filter = task_info->eth_filter;

    int eth_tap_fds[NUM_OF_FDS];
    test_vfs_eth_tap_msg_t recv_msg;
    test_vfs_eth_tap_msg_t test_msg;
    // Set test message source MAC address to the MAC of the Ethernet interface
    esp_eth_ioctl(task_info->eth_network_hndls_p->eth_handle, ETH_CMD_G_MAC_ADDR, &test_msg.header.src.addr);
    // Set test message destination MAC address to the MAC of the Ethernet interface to not be filtered out in loopback mode
    esp_eth_ioctl(task_info->eth_network_hndls_p->eth_handle, ETH_CMD_G_MAC_ADDR, &test_msg.header.dest.addr);

    for (int i = 0; i < sizeof(eth_tap_fds) / sizeof(int); i++) {
        eth_tap_fds[i] = open("/dev/net/tap", O_NONBLOCK);
        TEST_ASSERT_NOT_EQUAL(-1, eth_tap_fds[i]);

        // Set Ethernet interface on which to get raw frames
        TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fds[i], L2TAP_S_INTF_DEVICE, "ETH_DEF"));
        // Check the Ethernet interface was assigned
        char *if_key_str;
        TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fds[i], L2TAP_G_INTF_DEVICE, &if_key_str));
        TEST_ASSERT_EQUAL_STRING("ETH_DEF", if_key_str);

        uint16_t eth_type_filter = eth_filter + i;
        TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fds[i], L2TAP_S_RCV_FILTER, &eth_type_filter));
    }

    int msg_cnt;
    for (msg_cnt = 0; msg_cnt < 5000; msg_cnt++) {
        for (int i = 0; i < sizeof(eth_tap_fds) / sizeof(int); i++) {
            test_msg.header.type = htons(eth_filter + i);
            test_msg.cnt = msg_cnt;
            TEST_ASSERT_NOT_EQUAL(-1, write(eth_tap_fds[i], &test_msg, sizeof(test_msg)));

            memset(&recv_msg, 0, sizeof(recv_msg));

            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 300 * 1000;

            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(eth_tap_fds[i], &rfds);
            if (select(eth_tap_fds[i] + 1, &rfds, NULL, NULL, &tv) > -1) {
                if (FD_ISSET(eth_tap_fds[i], &rfds)) {
                    int n = read(eth_tap_fds[i], &recv_msg, sizeof(recv_msg));
                    TEST_ASSERT_GREATER_THAN(0, n);
                    TEST_ASSERT_EQUAL(msg_cnt, recv_msg.cnt);
                } else {
                    TEST_FAIL_MESSAGE("time out, frame was not successfully written (due to possible race condition)");
                }
            } else {
                TEST_FAIL_MESSAGE("select error");
            }
        }
    }

    for (int i = 0; i < sizeof(eth_tap_fds) / sizeof(int); i++) {
        TEST_ASSERT_EQUAL(0, close(eth_tap_fds[i]));
    }
    ESP_LOGI(TAG, "multi_fds_task %" PRIu16 "done", task_info->task_id);
    xSemaphoreGive(task_info->semaphore);
    vTaskDelete(NULL);
}

TEST_CASE("esp32 l2tap - read/write multiple fd's used by multiple tasks", "[ethernet]")
{
    test_vfs_eth_network_t eth_network_hndls;

    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_register(NULL));
    ethernet_init(&eth_network_hndls);

    task_info_t task_info[NUM_OF_TASKS];
    for (int i = 0; i < NUM_OF_TASKS; i++) {
        task_info[i].task_id = i;
        task_info[i].eth_filter = 0x750A + i * 100;
        task_info[i].semaphore = xSemaphoreCreateBinary();
        task_info[i].eth_network_hndls_p = &eth_network_hndls;
    }

    xTaskCreate(multi_fds_task, "multi_fds_task_1", 4096, &task_info[0], tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(multi_fds_task, "multi_fds_task_2", 4096, &task_info[1], tskIDLE_PRIORITY + 4, NULL);
    for (int i = 0; i < NUM_OF_TASKS; i++) {
        TEST_ASSERT_NOT_EQUAL(pdFALSE, xSemaphoreTake(task_info[i].semaphore, pdMS_TO_TICKS(20 * 1000)));
        vSemaphoreDelete(task_info[i].semaphore);
    }
    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_unregister(NULL));
    ethernet_deinit(&eth_network_hndls);
}

/* ============================================================================= */
/**
 * @brief Verifies time stamping feature
 *
 */
#if SOC_EMAC_IEEE1588V2_SUPPORTED
TEST_CASE("esp32 l2tap - time stamping", "[ethernet]")
{
    test_vfs_eth_network_t eth_network_hndls;

    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_register(NULL));
    ethernet_init(&eth_network_hndls);

    int eth_tap_fd = open("/dev/net/tap", 0);
    TEST_ASSERT_NOT_EQUAL(-1, eth_tap_fd);

    // Set Ethernet interface on which to get raw frames
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_S_INTF_DEVICE, "ETH_DEF"));
    // Check the Ethernet interface was assigned
    char *if_key_str;
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_G_INTF_DEVICE, &if_key_str));
    TEST_ASSERT_EQUAL_STRING("ETH_DEF", if_key_str);

    // Set the Ethertype filter (frames with this type will be available through the eth_tap_fd)
    uint16_t eth_type_filter = ETH_TYPE_PTP;
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_S_RCV_FILTER, &eth_type_filter));

    // Enable time stamping in driver
    eth_mac_ptp_config_t ptp_config = ETH_MAC_ESP_PTP_DEFAULT_CONFIG();
    TEST_ESP_OK(esp_eth_mac_ptp_enable(eth_network_hndls.mac, &ptp_config));

    test_eth_ptp_msg_t test_ptp_msg = {
        .eth_hdr = {
            .dest.addr = {0x01, 0x1b, 0x19, 0x0, 0x0, 0x0},
            .type = htons(ETH_TYPE_PTP)
        },
        .ptp_msg = {
            .ptp_hdr = {
                .message_type = 1,
                .version = 2,
                .message_length = htons(sizeof(ptp_msg_t)),
                .sequence_id = 0
            },
            .timestamp = 0,
        }
    };
    // Add PTP multicast destination MAC address to the filter
    TEST_ESP_OK(esp_eth_ioctl(eth_network_hndls.eth_handle, ETH_CMD_ADD_MAC_FILTER, test_ptp_msg.eth_hdr.dest.addr));

    TEST_ESP_OK(esp_eth_ioctl(eth_network_hndls.eth_handle, ETH_CMD_G_MAC_ADDR, &test_ptp_msg.eth_hdr.src.addr));

    uint16_t exp_sequence_id = test_ptp_msg.ptp_msg.ptp_hdr.sequence_id;
    // wrap "Info Records Buffer" into union to ensure proper alignment of data (this is typically needed when
    // accessing double word variables or structs containing double word variables)
    union {
        uint8_t info_recs_buff[L2TAP_IREC_SPACE(sizeof(struct timespec))];
        l2tap_irec_hdr_t align;
    } u;

    l2tap_extended_buff_t ptp_msg_ext_buff;
    ptp_msg_ext_buff.info_recs_len = sizeof(u.info_recs_buff);
    ptp_msg_ext_buff.info_recs_buff = u.info_recs_buff;

    l2tap_irec_hdr_t *ts_info = L2TAP_IREC_FIRST(&ptp_msg_ext_buff);
    ts_info->len = L2TAP_IREC_LEN(sizeof(struct timespec));
    ts_info->type = L2TAP_IREC_TIME_STAMP;

    ESP_LOGI(TAG, "Verify response to read TS when not enabled in TAP");
    test_ptp_msg.ptp_msg.ptp_hdr.sequence_id++;
    exp_sequence_id++;
    ptp_msg_ext_buff.buff = &test_ptp_msg;
    ptp_msg_ext_buff.buff_len = sizeof(test_ptp_msg);
    int n = write(eth_tap_fd, &ptp_msg_ext_buff, 0);
    // when input len is 0 and no special function of tap => expected standard behavior, i.e. nothing was written
    TEST_ASSERT_EQUAL(0, n);
    ptp_msg_ext_buff.buff = in_buffer;
    ptp_msg_ext_buff.buff_len = IN_BUFFER_SIZE;
    n = read(eth_tap_fd, &ptp_msg_ext_buff, 0);
    // when input len is 0 and no special function of tap => expected standard behavior, i.e. nothing was read
    TEST_ASSERT_EQUAL(0, n);

    // Enable time stamping in L2TAP, since now we can read TS
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_S_TIMESTAMP_EN));

    ESP_LOGI(TAG, "Verify response when trying to write/read in standard way (input len > 0) but tap configured as TS enabled");
    test_ptp_msg.ptp_msg.ptp_hdr.sequence_id++;
    exp_sequence_id++;
    n = write(eth_tap_fd, &test_ptp_msg, sizeof(test_ptp_msg));
    TEST_ASSERT_EQUAL(-1, n);
    TEST_ASSERT_EQUAL(EINVAL, errno);
    n = read(eth_tap_fd, &in_buffer, sizeof(test_ptp_msg));
    TEST_ASSERT_EQUAL(-1, n);
    TEST_ASSERT_EQUAL(EINVAL, errno);

    ESP_LOGI(TAG, "Verify response to invalid info record type for write");
    ts_info->type = 0xFF;
    test_ptp_msg.ptp_msg.ptp_hdr.sequence_id++;
    exp_sequence_id++;
    ptp_msg_ext_buff.buff = &test_ptp_msg;
    ptp_msg_ext_buff.buff_len = sizeof(test_ptp_msg);
    n = write(eth_tap_fd, &ptp_msg_ext_buff, 0);
    TEST_ASSERT_EQUAL(sizeof(test_ptp_msg), n); // invalid info record is ignored and write is successful
    // since write was successful, empty L2 TAP queue
    ptp_msg_ext_buff.buff = in_buffer;
    ptp_msg_ext_buff.buff_len = IN_BUFFER_SIZE;
    n = read(eth_tap_fd, &ptp_msg_ext_buff, 0);
    int exp_n = sizeof(test_ptp_msg) < 60 ? 60 : sizeof(test_ptp_msg);
    TEST_ASSERT_EQUAL(exp_n, n);
    TEST_ASSERT_EQUAL(exp_sequence_id, ((test_eth_ptp_msg_t *)in_buffer)->ptp_msg.ptp_hdr.sequence_id);


    ESP_LOGI(TAG, "Verify response to invalid record type for read (first need to write correctly)");
    ts_info->type = L2TAP_IREC_TIME_STAMP;
    test_ptp_msg.ptp_msg.ptp_hdr.sequence_id++;
    exp_sequence_id++;
    ptp_msg_ext_buff.buff = &test_ptp_msg;
    ptp_msg_ext_buff.buff_len = sizeof(test_ptp_msg);
    n = write(eth_tap_fd, &ptp_msg_ext_buff, 0);
    TEST_ASSERT_EQUAL(sizeof(test_ptp_msg), n);
    ts_info->type = 0xFF;
    ptp_msg_ext_buff.buff = in_buffer;
    ptp_msg_ext_buff.buff_len = IN_BUFFER_SIZE;
    n = read(eth_tap_fd, &ptp_msg_ext_buff, 0);
    exp_n = sizeof(test_ptp_msg) < 60 ? 60 : sizeof(test_ptp_msg); // minimum Ethernet frame has size of 60B
    TEST_ASSERT_EQUAL(exp_n, n); // invalid info record is ignored and read is successful

    ESP_LOGI(TAG, "Verify response to invalid record len for write");
    ts_info->type = L2TAP_IREC_TIME_STAMP;
    ts_info->len = L2TAP_IREC_LEN(1);
    test_ptp_msg.ptp_msg.ptp_hdr.sequence_id++;
    exp_sequence_id++;
    ptp_msg_ext_buff.buff = &test_ptp_msg;
    ptp_msg_ext_buff.buff_len = sizeof(test_ptp_msg);
    n = write(eth_tap_fd, &ptp_msg_ext_buff, 0);
    TEST_ASSERT_EQUAL(sizeof(test_ptp_msg), n); // write is successful
    TEST_ASSERT_EQUAL(L2TAP_IREC_INVALID, ts_info->type); // but the TS record is marked invalid
    // since write was successful, empty L2 TAP queue
    ptp_msg_ext_buff.buff = in_buffer;
    ptp_msg_ext_buff.buff_len = IN_BUFFER_SIZE;
    n = read(eth_tap_fd, &ptp_msg_ext_buff, 0);
    exp_n = sizeof(test_ptp_msg) < 60 ? 60 : sizeof(test_ptp_msg);
    TEST_ASSERT_EQUAL(exp_n, n);
    TEST_ASSERT_EQUAL(exp_sequence_id, ((test_eth_ptp_msg_t *)in_buffer)->ptp_msg.ptp_hdr.sequence_id);


    ESP_LOGI(TAG, "Verify response to invalid record len for read (first we need write correctly)");
    ts_info->type = L2TAP_IREC_TIME_STAMP;
    ts_info->len = L2TAP_IREC_LEN(sizeof(struct timespec));
    test_ptp_msg.ptp_msg.ptp_hdr.sequence_id++;
    exp_sequence_id++;
    ptp_msg_ext_buff.buff = &test_ptp_msg;
    ptp_msg_ext_buff.buff_len = sizeof(test_ptp_msg);
    n = write(eth_tap_fd, &ptp_msg_ext_buff, 0);
    TEST_ASSERT_EQUAL(sizeof(test_ptp_msg), n);
    ts_info->type = L2TAP_IREC_TIME_STAMP;
    ts_info->len = L2TAP_IREC_LEN(1);
    ptp_msg_ext_buff.buff = in_buffer;
    ptp_msg_ext_buff.buff_len = IN_BUFFER_SIZE;
    n = read(eth_tap_fd, &ptp_msg_ext_buff, 0);
    exp_n = sizeof(test_ptp_msg) < 60 ? 60 : sizeof(test_ptp_msg);
    TEST_ASSERT_EQUAL(exp_n, n); // read is successful
    TEST_ASSERT_EQUAL(L2TAP_IREC_INVALID, ts_info->type); // but the TS record is marked invalid

    ESP_LOGI(TAG, "Verify response to Info Record buffer is NULL for write");
    ts_info->type = L2TAP_IREC_TIME_STAMP;
    ts_info->len = L2TAP_IREC_LEN(sizeof(struct timespec));
    ptp_msg_ext_buff.buff = NULL;
    ptp_msg_ext_buff.buff_len = sizeof(test_ptp_msg);
    n = write(eth_tap_fd, &ptp_msg_ext_buff, 0);
    TEST_ASSERT_EQUAL(-1, n);
    TEST_ASSERT_EQUAL(EFAULT, errno);
    ESP_LOGI(TAG, "Verify response to Info Record buffer is NULL for read");
    ts_info->type = L2TAP_IREC_TIME_STAMP;
    ts_info->len = L2TAP_IREC_LEN(1);
    ptp_msg_ext_buff.buff = NULL;
    ptp_msg_ext_buff.buff_len = IN_BUFFER_SIZE;
    n = read(eth_tap_fd, &ptp_msg_ext_buff, 0);
    TEST_ASSERT_EQUAL(-1, n);
    TEST_ASSERT_EQUAL(EFAULT, errno);

    eth_mac_time_t ptp_time = {
        .seconds = 10,
        .nanoseconds = 412000
    };
    esp_eth_mac_set_ptp_time(eth_network_hndls.mac, &ptp_time);

    ESP_LOGI(TAG, "Verify retrieval of Tx and Rx time stamps");
    for (int i = 0; i < 4; i++) {
        ts_info->type = L2TAP_IREC_TIME_STAMP;
        ts_info->len = L2TAP_IREC_LEN(sizeof(struct timespec));
        test_ptp_msg.ptp_msg.ptp_hdr.sequence_id++;
        exp_sequence_id++;
        ptp_msg_ext_buff.buff = &test_ptp_msg;
        ptp_msg_ext_buff.buff_len = sizeof(test_ptp_msg);
        n = write(eth_tap_fd, &ptp_msg_ext_buff, 0);
        TEST_ASSERT_EQUAL(sizeof(test_ptp_msg), n);
        TEST_ASSERT_EQUAL(L2TAP_IREC_TIME_STAMP, ts_info->type);
        struct timespec *ts = (struct timespec *)ts_info->data;
        printf("tap tx TS: %lli.%09li\n", ts->tv_sec, ts->tv_nsec);
        TEST_ASSERT_NOT_EQUAL(0, ts->tv_sec);
        TEST_ASSERT_NOT_EQUAL(0, ts->tv_nsec);

        ptp_msg_ext_buff.buff = in_buffer;
        ptp_msg_ext_buff.buff_len = IN_BUFFER_SIZE;
        n = read(eth_tap_fd, &ptp_msg_ext_buff, 0);
        exp_n = sizeof(test_ptp_msg) < 60 ? 60 : sizeof(test_ptp_msg);
        TEST_ASSERT_EQUAL(exp_n, n);
        TEST_ASSERT_EQUAL(exp_sequence_id, ((test_eth_ptp_msg_t *)in_buffer)->ptp_msg.ptp_hdr.sequence_id);
        TEST_ASSERT_EQUAL(exp_n, ptp_msg_ext_buff.buff_len);
        TEST_ASSERT_EQUAL(L2TAP_IREC_TIME_STAMP, ts_info->type);
        printf("tap rx TS: %lli.%09li\n", ts->tv_sec, ts->tv_nsec);
        TEST_ASSERT_NOT_EQUAL(0, ts->tv_sec);
        TEST_ASSERT_NOT_EQUAL(0, ts->tv_nsec);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    TEST_ASSERT_EQUAL(0, close(eth_tap_fd));
    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_unregister(NULL));
    ethernet_deinit(&eth_network_hndls);
}
#endif // SOC_EMAC_IEEE1588V2_SUPPORTED

/* ============================================================================= */
/**
 * @brief Verifies proper functionality of ioctl RCV_FILTER option
 *
 */
TEST_CASE("esp32 l2tap - ioctl - RCV_FILTER", "[ethernet]")
{
    test_vfs_eth_network_t eth_network_hndls;
    int eth_tap_fd;

    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_register(NULL));
    ethernet_init(&eth_network_hndls);

    eth_tap_fd = open("/dev/net/tap", 0);
    TEST_ASSERT_NOT_EQUAL(-1, eth_tap_fd);
    ESP_LOGI(TAG, "Verify that RCV_FILTER is allowed to be configured only after interface is set...");
    uint16_t eth_type_filter = ETH_FILTER_LE;
    TEST_ASSERT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_S_RCV_FILTER, &eth_type_filter));
    TEST_ASSERT_EQUAL(EACCES, errno);
    TEST_ASSERT_EQUAL(0, close(eth_tap_fd));

    eth_tap_fd = open("/dev/net/tap", 0);
    TEST_ASSERT_NOT_EQUAL(-1, eth_tap_fd);

    // Set Ethernet interface on which to get raw frames
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_S_INTF_DEVICE, "ETH_DEF"));
    // Check the Ethernet interface was assigned
    char *if_key_str;
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_G_INTF_DEVICE, &if_key_str));
    TEST_ASSERT_EQUAL_STRING("ETH_DEF", if_key_str);

    // Set the Ethertype filter (frames with this type will be available through the eth_tap_fd)
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_S_RCV_FILTER, &eth_type_filter));
    uint16_t eth_type_filter_get = 0;
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_G_RCV_FILTER, &eth_type_filter_get));
    TEST_ASSERT_EQUAL(eth_type_filter, eth_type_filter_get);

    ESP_LOGI(TAG, "Confirm that we are able to receive message with default Ethertype...");
    send_task_control_t send_task_ctrl = {
        .eth_network_hndls_p = &eth_network_hndls,
        .eth_type = -1,
        .send_delay_ms = DEFAULT_SEND_DELAY_MS,
    };
    xTaskCreate(send_task, "raw_eth_send_task", 1024, &send_task_ctrl, tskIDLE_PRIORITY + 2, NULL);
    int n = read(eth_tap_fd, in_buffer, IN_BUFFER_SIZE);
    TEST_ASSERT_GREATER_THAN(0, n);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&s_test_msg, in_buffer, n);

    // Set new value of Ethernet type filter
    eth_type_filter = ETH_FILTER_LE + 100;
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_S_RCV_FILTER, &eth_type_filter));
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_G_RCV_FILTER, &eth_type_filter_get));
    TEST_ASSERT_EQUAL(eth_type_filter, eth_type_filter_get);
    // Send the test message with the old (default) value of the Ethernet filter
    xTaskCreate(send_task, "raw_eth_send_task", 1024, &send_task_ctrl, tskIDLE_PRIORITY + 2, NULL);
    ESP_LOGI(TAG, "Confirm that the message was not received (select timeouts)...");
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1000 * 1000;

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(eth_tap_fd, &rfds);
    TEST_ASSERT_EQUAL(0, select(eth_tap_fd + 1, &rfds, NULL, NULL, &tv));

    // Send the test message with the new value of the Ethernet filter
    send_task_ctrl.eth_type = eth_type_filter;
    xTaskCreate(send_task, "raw_eth_send_task", 1024, &send_task_ctrl, tskIDLE_PRIORITY + 2, NULL);
    ESP_LOGI(TAG, "Verify that the message with new Etherbet type is received...");
    n = read(eth_tap_fd, in_buffer, IN_BUFFER_SIZE);
    TEST_ASSERT_GREATER_THAN(0, n);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&s_test_msg, in_buffer, n);

    // Open another raw ethernet fd at the same interface and try to set the same filter as for the first one
    int eth_tap_fd_2 = open("/dev/net/tap", 0);
    TEST_ASSERT_NOT_EQUAL(-1, eth_tap_fd_2);
    // Set Ethernet interface on which to get raw frames
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd_2, L2TAP_S_INTF_DEVICE, "ETH_DEF"));
    // Check the Ethernet interface was assigned
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd_2, L2TAP_G_INTF_DEVICE, &if_key_str));
    TEST_ASSERT_EQUAL_STRING("ETH_DEF", if_key_str);

    ESP_LOGI(TAG, "Verify that the setting the same Ethernet type to other fd at the same interface was unsuccessful...");
    TEST_ASSERT_EQUAL(-1, ioctl(eth_tap_fd_2, L2TAP_S_RCV_FILTER, &eth_type_filter));
    TEST_ASSERT_EQUAL(EINVAL, errno);
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd_2, L2TAP_G_RCV_FILTER, &eth_type_filter_get));
    TEST_ASSERT_EQUAL(0, eth_type_filter_get);

    TEST_ASSERT_EQUAL(0, close(eth_tap_fd_2));
    TEST_ASSERT_EQUAL(0, close(eth_tap_fd));

    vTaskDelay(pdMS_TO_TICKS(50)); // just for sure to give some time to send task close fd
    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_unregister(NULL));
    ethernet_deinit(&eth_network_hndls);
}

/* ============================================================================= */
/**
 * @brief Verifies proper functionality of ioctl INTF_DEVICE option
 *
 */
TEST_CASE("esp32 l2tap - ioctl - INTF_DEVICE/DEVICE_DRV_HNDL", "[ethernet]")
{
    test_vfs_eth_network_t eth_network_hndls;
    int eth_tap_fd;

    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_register(NULL));
    ethernet_init(&eth_network_hndls);

    eth_tap_fd = open("/dev/net/tap", 0);
    TEST_ASSERT_NOT_EQUAL(-1, eth_tap_fd);

    // Set Ethernet interface on which to get raw frames
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_S_INTF_DEVICE, "ETH_DEF"));
    // Check the Ethernet interface was assigned
    char *if_key_str;
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_G_INTF_DEVICE, &if_key_str));
    TEST_ASSERT_EQUAL_STRING("ETH_DEF", if_key_str);

    // Check getter of direct Ethernet interface handle
    esp_eth_handle_t l2tap_eth_handle;
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_G_DEVICE_DRV_HNDL, &l2tap_eth_handle));
    TEST_ASSERT_EQUAL(eth_network_hndls.eth_handle, l2tap_eth_handle);

    // Set the Ethertype filter (frames with this type will be available through the eth_tap_fd)
    uint16_t eth_type_filter = ETH_FILTER_LE;
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_S_RCV_FILTER, &eth_type_filter));
    uint16_t eth_type_filter_get = 0;
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_G_RCV_FILTER, &eth_type_filter_get));
    TEST_ASSERT_EQUAL(eth_type_filter, eth_type_filter_get);

    ESP_LOGI(TAG, "Confirm that we are able to receive message at default netif...");
    send_task_control_t send_task_ctrl = {
        .eth_network_hndls_p = &eth_network_hndls,
        .eth_type = -1,
        .send_delay_ms = DEFAULT_SEND_DELAY_MS,
    };
    xTaskCreate(send_task, "raw_eth_send_task", 1024, &send_task_ctrl, tskIDLE_PRIORITY + 2, NULL);
    int n = read(eth_tap_fd, in_buffer, IN_BUFFER_SIZE);
    TEST_ASSERT_GREATER_THAN(0, n);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&s_test_msg, in_buffer, n);

    ESP_LOGI(TAG, "Try to set non-existing Ethernet interface...");
    TEST_ASSERT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_S_INTF_DEVICE, "ETH_NOT_DEF"));
    TEST_ASSERT_EQUAL(ENODEV, errno);
    ESP_LOGI(TAG, "Verify that previous setting is kept...");
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_G_INTF_DEVICE, &if_key_str));
    TEST_ASSERT_EQUAL_STRING("ETH_DEF", if_key_str);
    xTaskCreate(send_task, "raw_eth_send_task", 1024, &send_task_ctrl, tskIDLE_PRIORITY + 10, NULL); // set higher priority, we need to be sure that "send" task closes FD prior main task
    n = read(eth_tap_fd, in_buffer, IN_BUFFER_SIZE);
    TEST_ASSERT_GREATER_THAN(0, n);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&s_test_msg, in_buffer, n);

    TEST_ASSERT_EQUAL(0, close(eth_tap_fd));
    vTaskDelay(pdMS_TO_TICKS(50)); // just for sure to give some time to send task close fd

    ESP_LOGI(TAG, "Verify that we are able to set Ethernet interface directly via esp_eth_handle..");
    eth_tap_fd = open("/dev/net/tap", 0);
    TEST_ASSERT_NOT_EQUAL(-1, eth_tap_fd);

    // Set Ethernet interface on which to get raw frames
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_S_DEVICE_DRV_HNDL, eth_network_hndls.eth_handle));
    // Check getter of direct Ethernet interface handle
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_G_DEVICE_DRV_HNDL, &l2tap_eth_handle));
    TEST_ASSERT_EQUAL(eth_network_hndls.eth_handle, l2tap_eth_handle);

    // Set the Ethertype filter (frames with this type will be available through the eth_tap_fd)
    eth_type_filter = ETH_FILTER_LE;
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_S_RCV_FILTER, &eth_type_filter));
    eth_type_filter_get = 0;
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_G_RCV_FILTER, &eth_type_filter_get));
    TEST_ASSERT_EQUAL(eth_type_filter, eth_type_filter_get);

    xTaskCreate(send_task, "raw_eth_send_task", 1024, &send_task_ctrl, tskIDLE_PRIORITY + 2, NULL);
    n = read(eth_tap_fd, in_buffer, IN_BUFFER_SIZE);
    TEST_ASSERT_GREATER_THAN(0, n);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&s_test_msg, in_buffer, n);

    TEST_ASSERT_EQUAL(0, close(eth_tap_fd));

    vTaskDelay(pdMS_TO_TICKS(50)); // just for sure to give some time to send task close fd
    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_unregister(NULL));
    ethernet_deinit(&eth_network_hndls);
}

/* ============================================================================= */
/**
 * @brief Verifies proper functionality of ioctl unknown option
 *
 */
TEST_CASE("esp32 l2tap - ioctl - unknown", "[ethernet]")
{
    test_vfs_eth_network_t eth_network_hndls;
    int eth_tap_fd;

    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_register(NULL));
    ethernet_init(&eth_network_hndls);

    eth_tap_fd = open("/dev/net/tap", 0);
    TEST_ASSERT_NOT_EQUAL(-1, eth_tap_fd);

    // Try to execute unknown option
    uint32_t option_val = 0x0BAD;
    TEST_ASSERT_EQUAL(-1, ioctl(eth_tap_fd, 412, &option_val));

    TEST_ASSERT_EQUAL(0, close(eth_tap_fd));
    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_unregister(NULL));
    ethernet_deinit(&eth_network_hndls);
}

/* ============================================================================= */
/**
 * @brief Verifies proper functionality of fcntl
 *
 */
TEST_CASE("esp32 l2tap - fcntl", "[ethernet]")
{
    test_vfs_eth_network_t eth_network_hndls;
    int eth_tap_fd;

    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_register(NULL));
    ethernet_init(&eth_network_hndls);

    eth_tap_fd = open("/dev/net/tap", 0);
    TEST_ASSERT_NOT_EQUAL(-1, eth_tap_fd);

    // Set Ethernet interface on which to get raw frames
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_S_INTF_DEVICE, "ETH_DEF"));
    // Check the Ethernet interface was assigned
    char *if_key_str;
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_G_INTF_DEVICE, &if_key_str));
    TEST_ASSERT_EQUAL_STRING("ETH_DEF", if_key_str);

    // Set the Ethertype filter (frames with this type will be available through the eth_tap_fd)
    uint16_t eth_type_filter = ETH_FILTER_LE;
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_S_RCV_FILTER, &eth_type_filter));
    uint16_t eth_type_filter_get = 0;
    TEST_ASSERT_NOT_EQUAL(-1, ioctl(eth_tap_fd, L2TAP_G_RCV_FILTER, &eth_type_filter_get));
    TEST_ASSERT_EQUAL(eth_type_filter, eth_type_filter_get);

    send_task_control_t send_task_ctrl = {
        .eth_network_hndls_p = &eth_network_hndls,
        .eth_type = -1,
        .send_delay_ms = DEFAULT_SEND_DELAY_MS,
    };
    // Confirm the read blocks by default
    xTaskCreate(send_task, "raw_eth_send_task", 1024, &send_task_ctrl, tskIDLE_PRIORITY + 2, NULL);
    int n = read(eth_tap_fd, in_buffer, IN_BUFFER_SIZE);
    TEST_ASSERT_GREATER_THAN(0, n);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&s_test_msg, in_buffer, n);

    // Set non-blocking
    int flags = fcntl(eth_tap_fd, F_GETFL);
    TEST_ASSERT_GREATER_THAN(-1, flags);
    flags |= O_NONBLOCK;
    TEST_ASSERT_EQUAL(0, fcntl(eth_tap_fd, F_SETFL, flags));
    int flags_new = fcntl(eth_tap_fd, F_GETFL);
    TEST_ASSERT_GREATER_THAN(-1, flags_new);
    TEST_ASSERT_EQUAL(flags_new, flags);
    // Verify the read does not block
    int loop_cnt = 0;
    xTaskCreate(send_task, "raw_eth_send_task", 1024, &send_task_ctrl, tskIDLE_PRIORITY + 2, NULL);
    while (loop_cnt < 100) {
        if ((n = read(eth_tap_fd, in_buffer, IN_BUFFER_SIZE)) > 0) {
            TEST_ASSERT_EQUAL_UINT8_ARRAY(&s_test_msg, in_buffer, n);
            break;
        } else {
            TEST_ASSERT_EQUAL(EAGAIN, errno);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
        loop_cnt++;
    }
    ESP_LOGI(TAG, "elapsed loop_cnts to read %d", loop_cnt);
    TEST_ASSERT_GREATER_THAN(0, loop_cnt);
    TEST_ASSERT_LESS_THAN(100, loop_cnt);
    TEST_ASSERT_EQUAL(sizeof(s_test_msg), n);

    // Set back to blocking
    flags = fcntl(eth_tap_fd, F_GETFL);
    TEST_ASSERT_GREATER_THAN(-1, flags);
    flags &= ~O_NONBLOCK;
    TEST_ASSERT_EQUAL(0, fcntl(eth_tap_fd, F_SETFL, flags));
    flags_new = fcntl(eth_tap_fd, F_GETFL);
    TEST_ASSERT_GREATER_THAN(-1, flags_new);
    TEST_ASSERT_EQUAL(flags_new, flags);

    loop_cnt = 0;
    xTaskCreate(send_task, "raw_eth_send_task", 1024, &send_task_ctrl, tskIDLE_PRIORITY + 2, NULL);
    while (loop_cnt < 100) {
        if ((n = read(eth_tap_fd, in_buffer, IN_BUFFER_SIZE)) > 0) {
            TEST_ASSERT_EQUAL_UINT8_ARRAY(&s_test_msg, in_buffer, n);
            break;
        } else {
            TEST_ASSERT_NOT_EQUAL(EAGAIN, errno);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
        loop_cnt++;
    }
    ESP_LOGI(TAG, "elapsed loop_cnts to read %d", loop_cnt);
    TEST_ASSERT_EQUAL(0, loop_cnt);

    // Try to use unsupported operation
    flags = fcntl(eth_tap_fd, F_DUPFD, 0);
    TEST_ASSERT_EQUAL(-1, flags);
    TEST_ASSERT_EQUAL(ENOSYS, errno);

    // Try to set unsupported flag
    flags = fcntl(eth_tap_fd, F_SETFL, O_TRUNC);
    TEST_ASSERT_EQUAL(-1, flags);
    TEST_ASSERT_EQUAL(EINVAL, errno);
    flags = fcntl(eth_tap_fd, F_SETFL, O_TRUNC | O_NONBLOCK);
    TEST_ASSERT_EQUAL(-1, flags);
    TEST_ASSERT_EQUAL(EINVAL, errno);

    TEST_ASSERT_EQUAL(0, close(eth_tap_fd));

    vTaskDelay(pdMS_TO_TICKS(50)); // just for sure to give some time to send task close fd
    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_unregister(NULL));
    ethernet_deinit(&eth_network_hndls);
}

void app_main(void)
{
    unity_run_menu();
}
