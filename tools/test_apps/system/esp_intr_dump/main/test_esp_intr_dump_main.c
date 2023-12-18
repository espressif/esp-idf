/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "esp_intr_alloc.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "soc/interrupts.h"

static void start_console(void);
static void register_intr_commands(void);

void app_main(void)
{
    start_console();
}

static void intr_handler_dummy(void* arg)
{
}

static void start_console(void)
{
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();

    /* Pin repl task to ensure all interrupts are allocated on the same core */
    repl_config.task_core_id = 0;

    /* Prompt to be printed before each line.
     * This can be customized, made dynamic, etc.
     */
    repl_config.prompt = "test_intr_dump>";
    repl_config.max_cmdline_length = 80;

    /* Register commands */
    esp_console_register_help_command();
    register_intr_commands();

#if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) || defined(CONFIG_ESP_CONSOLE_UART_CUSTOM)
    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));

#elif defined(CONFIG_ESP_CONSOLE_USB_CDC)
    esp_console_dev_usb_cdc_config_t hw_config = ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_cdc(&hw_config, &repl_config, &repl));

#elif defined(CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG)
    esp_console_dev_usb_serial_jtag_config_t hw_config = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_serial_jtag(&hw_config, &repl_config, &repl));

#else
#error Unsupported console type
#endif

    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}

static struct {
    struct arg_str *source;
    struct arg_str *flags;
    struct arg_end *end;
}  intr_alloc_args;

static int cmd_intr_alloc(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &intr_alloc_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, intr_alloc_args.end, argv[0]);
        return 1;
    }
    assert(intr_alloc_args.source->count == 1);
    assert(intr_alloc_args.flags->count == 1);
    const char* source_str = intr_alloc_args.source->sval[0];
    const char* flags_str = intr_alloc_args.flags->sval[0];

    // find the interrupt source number
    int source_num = -1;
    for (int i = 0; i < ETS_MAX_INTR_SOURCE; i++) {
        if (strcmp(source_str, esp_isr_names[i]) == 0) {
            source_num = i;
            break;
        }
    }
    if (source_num == -1) {
        printf("Unknown interrupt source: %s. Please check components/soc/" CONFIG_IDF_TARGET "/interrupts.c for the list\n", source_str);
        return 1;
    }

    // parse flags
    typedef struct {
        const char* name;
        int mask;
    } flag_desc_t;

    const flag_desc_t flag_desc[] = {
        { .name = "LEVEL1", .mask = ESP_INTR_FLAG_LEVEL1 },
        { .name = "LEVEL2", .mask = ESP_INTR_FLAG_LEVEL2 },
        { .name = "LEVEL3", .mask = ESP_INTR_FLAG_LEVEL3 },
        { .name = "LEVEL4", .mask = ESP_INTR_FLAG_LEVEL4 },
        { .name = "LEVEL5", .mask = ESP_INTR_FLAG_LEVEL5 },
        { .name = "LEVEL6", .mask = ESP_INTR_FLAG_LEVEL6 },
        { .name = "NMI", .mask = ESP_INTR_FLAG_NMI },
        { .name = "SHARED", .mask = ESP_INTR_FLAG_SHARED },
        { .name = "EDGE", .mask = ESP_INTR_FLAG_EDGE },
        { .name = "IRAM", .mask = ESP_INTR_FLAG_IRAM },
        { .name = "LOWMED", .mask = ESP_INTR_FLAG_LOWMED },
        { .name = "HIGH", .mask = ESP_INTR_FLAG_HIGH },
    };

    // split flags_str string into tokens separated by | character and get the final bitmask
    int flags = 0;
    char* flags_str_copy = strdup(flags_str);
    char* token = strtok(flags_str_copy, "|");
    while (token != NULL) {
        bool found = false;
        for (int i = 0; i < sizeof(flag_desc)/sizeof(flag_desc[0]); i++) {
            if (strcmp(token, flag_desc[i].name) == 0) {
                flags |= flag_desc[i].mask;
                found = true;
                break;
            }
        }
        if (!found) {
            printf("Unknown flag: %s\n", token);
            free(flags_str_copy);
            return 1;
        }
        token = strtok(NULL, "|");
    }
    free(flags_str_copy);

    // allocate the interrupt
    esp_err_t ret = esp_intr_alloc(source_num, flags, intr_handler_dummy, NULL, NULL);
    if (ret != ESP_OK) {
        printf("Failed to allocate interrupt (source: %d, flags: 0x%x): %s\n", source_num, flags, esp_err_to_name(ret));
        return 1;
    }

    printf("Allocated %s %s\n", source_str, flags_str);

    return 0;
}

int cmd_intr_dump(int argc, char **argv)
{
    esp_intr_dump(stdout);
    return 0;
}

static void register_intr_commands(void)
{
    intr_alloc_args.source = arg_str1(NULL, NULL, "<source>", "Interrupt source name");
    intr_alloc_args.flags = arg_str1(NULL, NULL, "<flags>", "Interrupt flags separated by | character: LEVEL1|LEVEL2|...|LEVEL6|NMI|SHARED|EDGE|IRAM|LOWMED|HIGH");
    intr_alloc_args.end = arg_end(2);
    const esp_console_cmd_t cmd_intr_alloc_info = {
        .command = "intr_alloc",
        .help = "Allocate an interrupt",
        .hint = NULL,
        .func = &cmd_intr_alloc,
        .argtable = &intr_alloc_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd_intr_alloc_info));

    const esp_console_cmd_t cmd_intr_dump_info = {
        .command = "intr_dump",
        .help = "Dump interrupt information",
        .hint = NULL,
        .func = &cmd_intr_dump,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd_intr_dump_info));
}
