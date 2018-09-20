/* Console example — Ethernet commands

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "tcpip_adapter.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_event_loop.h"
#include "esp_eth.h"
#include "argtable3/argtable3.h"
#include "iperf.h"
#include "sdkconfig.h"

#ifdef CONFIG_PHY_LAN8720
#include "eth_phy/phy_lan8720.h"
#define DEFAULT_ETHERNET_PHY_CONFIG phy_lan8720_default_ethernet_config
#endif
#ifdef CONFIG_PHY_TLK110
#include "eth_phy/phy_tlk110.h"
#define DEFAULT_ETHERNET_PHY_CONFIG phy_tlk110_default_ethernet_config
#endif

static tcpip_adapter_ip_info_t ip;
static bool started = false;
static EventGroupHandle_t eth_event_group;
static const int GOTIP_BIT = BIT0;

#define PIN_PHY_POWER CONFIG_PHY_POWER_PIN
#define PIN_SMI_MDC CONFIG_PHY_SMI_MDC_PIN
#define PIN_SMI_MDIO CONFIG_PHY_SMI_MDIO_PIN

#ifdef CONFIG_PHY_USE_POWER_PIN

static void phy_device_power_enable_via_gpio(bool enable)
{
    assert(DEFAULT_ETHERNET_PHY_CONFIG.phy_power_enable);

    if (!enable) {
        DEFAULT_ETHERNET_PHY_CONFIG.phy_power_enable(false);
    }

    gpio_pad_select_gpio(PIN_PHY_POWER);
    gpio_set_direction(PIN_PHY_POWER, GPIO_MODE_OUTPUT);
    if (enable == true) {
        gpio_set_level(PIN_PHY_POWER, 1);
        ESP_LOGI(__func__, "Power On Ethernet PHY");
    } else {
        gpio_set_level(PIN_PHY_POWER, 0);
        ESP_LOGI(__func__, "Power Off Ethernet PHY");
    }

    vTaskDelay(1); // Allow the power up/down to take effect, min 300us

    if (enable) {
        /* call the default PHY-specific power on function */
        DEFAULT_ETHERNET_PHY_CONFIG.phy_power_enable(true);
    }
}
#endif

static void eth_gpio_config_rmii(void)
{
    phy_rmii_configure_data_interface_pins();
    phy_rmii_smi_configure_pins(PIN_SMI_MDC, PIN_SMI_MDIO);
}

/* "ethernet" command */

static struct {
    struct arg_str *control;
    struct arg_end *end;
} eth_control_args;

static int eth_cmd_control(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&eth_control_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, eth_control_args.end, argv[0]);
        return 1;
    }

    if (!strncmp(eth_control_args.control->sval[0], "start", 5) && !started) {
        ESP_ERROR_CHECK(esp_eth_enable());
        started = true;
    }
    if (!strncmp(eth_control_args.control->sval[0], "stop", 4) && started) {
        ESP_ERROR_CHECK(esp_eth_disable());
        started = false;
    }
    if (!strncmp(eth_control_args.control->sval[0], "info", 4)) {
        uint8_t mac_addr[6];
        esp_eth_get_mac(mac_addr);
        printf("HW ADDR: " MACSTR "\r\n", MAC2STR(mac_addr));
        tcpip_adapter_get_ip_info(ESP_IF_ETH, &ip);
        printf("ETHIP: " IPSTR "\r\n", IP2STR(&ip.ip));
        printf("ETHMASK: " IPSTR "\r\n", IP2STR(&ip.netmask));
        printf("ETHGW: " IPSTR "\r\n", IP2STR(&ip.gw));
    }
    return 0;
}

/* "iperf" command */

static struct {
    struct arg_str *ip;
    struct arg_lit *server;
    struct arg_lit *udp;
    struct arg_int *port;
    struct arg_int *interval;
    struct arg_int *time;
    struct arg_lit *abort;
    struct arg_end *end;
} iperf_args;

static int eth_cmd_iperf(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&iperf_args);
    iperf_cfg_t cfg;

    if (nerrors != 0) {
        arg_print_errors(stderr, iperf_args.end, argv[0]);
        return 0;
    }

    memset(&cfg, 0, sizeof(cfg));

    /* iperf -a */
    if (iperf_args.abort->count != 0) {
        iperf_stop();
        return 0;
    }

    if (((iperf_args.ip->count == 0) && (iperf_args.server->count == 0)) ||
            ((iperf_args.ip->count != 0) && (iperf_args.server->count != 0))) {
        ESP_LOGE(__func__, "Wrong mode! ESP32 should run in client or server mode");
        return 0;
    }

    /* iperf -s */
    if (iperf_args.ip->count == 0) {
        cfg.flag |= IPERF_FLAG_SERVER;
    }
    /* iperf -c SERVER_ADDRESS */
    else {
        cfg.dip = ipaddr_addr(iperf_args.ip->sval[0]);
        cfg.flag |= IPERF_FLAG_CLIENT;
    }

    /* acquiring for ip, could blocked here */
    xEventGroupWaitBits(eth_event_group, GOTIP_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

    cfg.sip = ip.ip.addr;
    if (cfg.sip == 0) {
        return 0;
    }

    /* iperf -u */
    if (iperf_args.udp->count == 0) {
        cfg.flag |= IPERF_FLAG_TCP;
    } else {
        cfg.flag |= IPERF_FLAG_UDP;
    }

    /* iperf -p */
    if (iperf_args.port->count == 0) {
        cfg.sport = IPERF_DEFAULT_PORT;
        cfg.dport = IPERF_DEFAULT_PORT;
    } else {
        if (cfg.flag & IPERF_FLAG_SERVER) {
            cfg.sport = iperf_args.port->ival[0];
            cfg.dport = IPERF_DEFAULT_PORT;
        } else {
            cfg.sport = IPERF_DEFAULT_PORT;
            cfg.dport = iperf_args.port->ival[0];
        }
    }

    /* iperf -i */
    if (iperf_args.interval->count == 0) {
        cfg.interval = IPERF_DEFAULT_INTERVAL;
    } else {
        cfg.interval = iperf_args.interval->ival[0];
        if (cfg.interval <= 0) {
            cfg.interval = IPERF_DEFAULT_INTERVAL;
        }
    }

    /* iperf -t */
    if (iperf_args.time->count == 0) {
        cfg.time = IPERF_DEFAULT_TIME;
    } else {
        cfg.time = iperf_args.time->ival[0];
        if (cfg.time <= cfg.interval) {
            cfg.time = cfg.interval;
        }
    }

    printf("mode=%s-%s sip=%d.%d.%d.%d:%d, dip=%d.%d.%d.%d:%d, interval=%d, time=%d\r\n",
           cfg.flag & IPERF_FLAG_TCP ? "tcp" : "udp",
           cfg.flag & IPERF_FLAG_SERVER ? "server" : "client",
           cfg.sip & 0xFF, (cfg.sip >> 8) & 0xFF, (cfg.sip >> 16) & 0xFF, (cfg.sip >> 24) & 0xFF, cfg.sport,
           cfg.dip & 0xFF, (cfg.dip >> 8) & 0xFF, (cfg.dip >> 16) & 0xFF, (cfg.dip >> 24) & 0xFF, cfg.dport,
           cfg.interval, cfg.time);

    iperf_start(&cfg);
    return 0;
}

static esp_err_t eth_event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id) {
    case SYSTEM_EVENT_ETH_START:
        started = true;
        break;
    case SYSTEM_EVENT_ETH_GOT_IP:
        memset(&ip, 0, sizeof(tcpip_adapter_ip_info_t));
        ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(ESP_IF_ETH, &ip));
        xEventGroupSetBits(eth_event_group, GOTIP_BIT);
        break;
    case SYSTEM_EVENT_ETH_STOP:
        xEventGroupClearBits(eth_event_group, GOTIP_BIT);
        started = false;
    default:
        break;
    }
    return ESP_OK;
}

void register_ethernet()
{
    eth_event_group = xEventGroupCreate();
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(eth_event_handler, NULL));

    eth_config_t config = DEFAULT_ETHERNET_PHY_CONFIG;
    config.phy_addr = CONFIG_PHY_ADDRESS;
    config.gpio_config = eth_gpio_config_rmii;
    config.tcpip_input = tcpip_adapter_eth_input;
    config.clock_mode = CONFIG_PHY_CLOCK_MODE;
#ifdef CONFIG_PHY_USE_POWER_PIN
    /* Replace the default 'power enable' function with an example-specific one
     that toggles a power GPIO. */
    config.phy_power_enable = phy_device_power_enable_via_gpio;
#endif

    ESP_ERROR_CHECK(esp_eth_init(&config));

    eth_control_args.control = arg_str1(NULL, NULL, "<start|stop|info>", "Start/Stop Ethernet or Get info of Ethernet");
    eth_control_args.end = arg_end(1);
    const esp_console_cmd_t cmd = {
        .command = "ethernet",
        .help = "Control Ethernet interface",
        .hint = NULL,
        .func = eth_cmd_control,
        .argtable = &eth_control_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));

    iperf_args.ip = arg_str0("c", "client", "<ip>",
                             "run in client mode, connecting to <host>");
    iperf_args.server = arg_lit0("s", "server", "run in server mode");
    iperf_args.udp = arg_lit0("u", "udp", "use UDP rather than TCP");
    iperf_args.port = arg_int0("p", "port", "<port>",
                               "server port to listen on/connect to");
    iperf_args.interval = arg_int0("i", "interval", "<interval>",
                                   "seconds between periodic bandwidth reports");
    iperf_args.time = arg_int0("t", "time", "<time>", "time in seconds to transmit for (default 10 secs)");
    iperf_args.abort = arg_lit0("a", "abort", "abort running iperf");
    iperf_args.end = arg_end(1);
    const esp_console_cmd_t iperf_cmd = {
        .command = "iperf",
        .help = "iperf command",
        .hint = NULL,
        .func = &eth_cmd_iperf,
        .argtable = &iperf_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&iperf_cmd));
}
