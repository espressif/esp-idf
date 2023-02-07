/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
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
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "arpa/inet.h" // for ntohs, etc.
#include "lwip/prot/ethernet.h" // Ethernet headers

#include "unity.h"
#include "test_utils.h"

#include "esp_vfs_l2tap.h"

#define ETH_FILTER_LE 0x7A05
#define ETH_FILTER_BE 0x057A

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
    network_hndls->phy = esp_eth_phy_new_ip101(&phy_config);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(network_hndls->mac, network_hndls->phy);
    network_hndls->eth_handle = NULL;
    // install Ethernet driver
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &network_hndls->eth_handle));
    // combine driver with netif
    network_hndls->glue = esp_eth_new_netif_glue(network_hndls->eth_handle);
    TEST_ESP_OK(esp_netif_attach(network_hndls->eth_netif, network_hndls->glue));
    // register user defined event handers
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
    /* driver should be uninstalled within 2 seconds */
    //TEST_ESP_OK(test_uninstall_driver(eth_handle, 2000));
    esp_eth_driver_uninstall(network_hndls->eth_handle);
    TEST_ESP_OK(network_hndls->phy->del(network_hndls->phy));
    TEST_ESP_OK(network_hndls->mac->del(network_hndls->mac));
    TEST_ESP_OK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, got_ip_event_handler));
    TEST_ESP_OK(esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler));
    esp_netif_destroy(network_hndls->eth_netif);
    TEST_ESP_OK(esp_event_loop_delete_default());
}

// Global test message send by "send_task"
static test_vfs_eth_tap_msg_t s_test_msg = {
    .header = {
        .src.addr = {0},
        .dest.addr = { 0x01, 0x00, 0x00, 0x00, 0xBE, 0xEF },
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

    esp_eth_ioctl(eth_network_hndls->eth_handle, ETH_CMD_G_MAC_ADDR, &s_test_msg.header.src.addr);

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
        // it is expected that blocking read is unblocked by close
        TEST_ASSERT_EQUAL(-1, read(task_control->eth_tap_fd, in_buffer, in_buf_size));
        ESP_LOGI(TAG, "task1: unblocked");
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
    // Close blocking read from lower priority task
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
    // Close blocking read from lower priority task
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
    test_vfs_eth_tap_msg_t test_msg = {
        .header = {
            .src.addr = {0},
            .dest.addr = { 0x01, 0x00, 0x00, 0x00, 0xBE, 0xEF },
            .type = 0,
        }
    };

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
} task_info_t;

static void multi_fds_task (void *task_param)
{
    task_info_t *task_info = (task_info_t *)task_param;
    uint16_t eth_filter = task_info->eth_filter;

    int eth_tap_fds[NUM_OF_FDS];
    test_vfs_eth_tap_msg_t recv_msg;
    test_vfs_eth_tap_msg_t test_msg = {
        .header = {
            .src.addr = {0},
            .dest.addr = { 0x01, 0x00, 0x00, 0x00, 0xBE, 0xEF },
            .type = 0,
        }
    };

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
    int new_fd = fcntl(eth_tap_fd, F_DUPFD, 0);
    TEST_ASSERT_EQUAL(-1, new_fd);
    TEST_ASSERT_EQUAL(ENOSYS, errno);

    TEST_ASSERT_EQUAL(0, close(eth_tap_fd));

    vTaskDelay(pdMS_TO_TICKS(50)); // just for sure to give some time to send task close fd
    TEST_ASSERT_EQUAL(ESP_OK, esp_vfs_l2tap_intf_unregister(NULL));
    ethernet_deinit(&eth_network_hndls);
}

void app_main(void)
{
    unity_run_menu();
}
