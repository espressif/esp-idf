#include <stdio.h>
#include <string.h>
#include "esp_console.h"
#include "esp_err.h"
#include "argtable3/argtable3.h"
#include "sdkconfig.h"
#include "ota.h"
#include "cmd_ota.h"

static struct {
    struct arg_str *subcmd;
    struct arg_str *mode;
    struct arg_end *end;
} ota_args;

static void print_ota_help(void)
{
    printf("ota <cmd>\n");
    printf("  info              : print running/boot/next partition\n");
    printf("  confirm           : mark current app valid (cancel rollback)\n");
#if CONFIG_XMODEM_OTA_SUPPORT
    printf("  start xmodem      : start XMODEM OTA on UART%d\n", CONFIG_XMODEM_OTA_UART_PORT);
    printf("  xmodem            : same as 'start xmodem'\n");
#endif
#if CONFIG_HTTPS_OTA_SUPPORT
    printf("  start https       : start HTTPS OTA (not implemented yet)\n");
#endif
    printf("  help              : show this help\n");
}

#if CONFIG_XMODEM_OTA_SUPPORT
static void print_xmodem_hint(void)
{
    printf("XMODEM OTA: waiting on UART%d TX=GPIO%d RX=GPIO%d @%d\n",
           CONFIG_XMODEM_OTA_UART_PORT,
           CONFIG_XMODEM_OTA_UART_TX_PIN,
           CONFIG_XMODEM_OTA_UART_RX_PIN,
           CONFIG_XMODEM_OTA_UART_BAUD_RATE);
    printf("Wire USB-TTL: TX->%d RX->%d GND->GND\n",
           CONFIG_XMODEM_OTA_UART_RX_PIN,
           CONFIG_XMODEM_OTA_UART_TX_PIN);
    printf("Send: sx -X proj_s31/build/proj_s31.bin < /dev/ttyUSBx > /dev/ttyUSBx\n");
    printf("(keep this monitor open on the board USB-UART port)\n");
}
#endif

static int cmd_ota(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&ota_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, ota_args.end, argv[0]);
        print_ota_help();
        return 1;
    }

    const char *sub = ota_args.subcmd->sval[0];

    if (strcmp(sub, "help") == 0) {
        print_ota_help();
        return 0;
    }

    if (strcmp(sub, "info") == 0) {
        ota_print_info();
        return 0;
    }

    if (strcmp(sub, "confirm") == 0) {
        esp_err_t err = ota_mark_valid();
        if (err != ESP_OK) {
            printf("ota confirm failed: %s\n", esp_err_to_name(err));
            return 1;
        }
        printf("ota confirm ok\n");
        return 0;
    }

#if CONFIG_XMODEM_OTA_SUPPORT
    if (strcmp(sub, "xmodem") == 0) {
        print_xmodem_hint();
        esp_err_t err = ota_start(OTA_TYPE_XMODEM);
        if (err != ESP_OK) {
            printf("ota xmodem failed: %s\n", esp_err_to_name(err));
            return 1;
        }
        printf("ota xmodem started (background)\n");
        return 0;
    }
#endif

    if (strcmp(sub, "start") == 0) {
        if (ota_args.mode->count == 0) {
            printf("Missing mode. Usage: ota start <xmodem|https>\n");
            return 1;
        }
        const char *mode = ota_args.mode->sval[0];
#if CONFIG_XMODEM_OTA_SUPPORT
        if (strcmp(mode, "xmodem") == 0) {
            print_xmodem_hint();
            esp_err_t err = ota_start(OTA_TYPE_XMODEM);
            if (err != ESP_OK) {
                printf("ota start xmodem failed: %s\n", esp_err_to_name(err));
                return 1;
            }
            printf("ota xmodem started (background)\n");
            return 0;
        }
#endif
#if CONFIG_HTTPS_OTA_SUPPORT
        if (strcmp(mode, "https") == 0) {
            esp_err_t err = ota_start(OTA_TYPE_HTTPS);
            if (err != ESP_OK) {
                printf("ota start https failed: %s\n", esp_err_to_name(err));
                return 1;
            }
            return 0;
        }
#endif
        printf("Unsupported mode '%s'\n", mode);
        print_ota_help();
        return 1;
    }

    printf("Unknown subcommand '%s'\n", sub);
    print_ota_help();
    return 1;
}

void register_ota(void)
{
    ota_args.subcmd = arg_str1(NULL, NULL, "<cmd>", "info|confirm|start|xmodem|help");
    ota_args.mode = arg_str0(NULL, NULL, "<mode>", "xmodem|https (for start)");
    ota_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "ota",
        .help = "OTA commands: info|confirm|start <mode>|xmodem|help",
        .hint = NULL,
        .func = &cmd_ota,
        .argtable = &ota_args,
    };

    ESP_ERROR_CHECK(ota_init());
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}
