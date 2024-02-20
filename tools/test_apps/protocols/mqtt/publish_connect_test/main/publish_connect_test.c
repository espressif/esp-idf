/* MQTT publish-connect test

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "esp_system.h"
#include "mqtt_client.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "esp_log.h"
#include "publish_connect_test.h"

static const char *TAG = "publish_connect_test";

command_context_t command_context;
connection_args_t connection_args;
publish_setup_args_t publish_setup_args;
publish_args_t publish_args;

#define RETURN_ON_PARSE_ERROR(args) do { \
    int nerrors = arg_parse(argc, argv, (void **) &(args)); \
    if (nerrors != 0) { \
        arg_print_errors(stderr, (args).end, argv[0]); \
        return 1; \
    }} while(0)


static int do_free_heap(int argc, char **argv) {
    (void)argc;
    (void)argv;
    ESP_LOGI(TAG, "Note free memory: %d bytes", esp_get_free_heap_size());
    return 0;
}

static int do_init(int argc, char **argv) {
    (void)argc;
    (void)argv;
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtts://127.0.0.1:1234",
        .network.disable_auto_reconnect = true
    };
    command_context.mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    if(!command_context.mqtt_client) {
        ESP_LOGE(TAG, "Failed to initialize client");
        return 1;
    }
    publish_init_flags();
    ESP_LOGI(TAG, "Mqtt client initialized");
    return 0;
}

static int do_start(int argc, char **argv) {
    (void)argc;
    (void)argv;
    if(esp_mqtt_client_start(command_context.mqtt_client) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start mqtt client task");
        return 1;
    }
    ESP_LOGI(TAG, "Mqtt client started");
    return 0;
}

static int do_stop(int argc, char **argv) {
    (void)argc;
    (void)argv;
    if(esp_mqtt_client_stop(command_context.mqtt_client) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to stop mqtt client task");
        return 1;
    }
    ESP_LOGI(TAG, "Mqtt client stoped");
    return 0;
}

static int do_disconnect(int argc, char **argv) {
    (void)argc;
    (void)argv;
    if(esp_mqtt_client_disconnect(command_context.mqtt_client) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to request disconnection");
        return 1;
    }
    ESP_LOGI(TAG, "Mqtt client disconnected");
    return 0;
}

static int do_connect_setup(int argc, char **argv) {
    (void)argc;
    (void)argv;
    connect_setup(&command_context);
    return 0;
}

static int do_connect_teardown(int argc, char **argv) {
    (void)argc;
    (void)argv;
    connect_teardown(&command_context);
    return 0;
}

static int do_reconnect(int argc, char **argv) {
    (void)argc;
    (void)argv;
    if(esp_mqtt_client_reconnect(command_context.mqtt_client) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to request reconnection");
        return 1;
    }
    ESP_LOGI(TAG, "Mqtt client will reconnect");
    return 0;
    ;
}

static int do_destroy(int argc, char **argv) {
    (void)argc;
    (void)argv;
    esp_mqtt_client_destroy(command_context.mqtt_client);
    command_context.mqtt_client = NULL;
    ESP_LOGI(TAG, "mqtt client for tests destroyed");
    return 0;
}

static int do_connect(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &connection_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, connection_args.end, argv[0]);
        return 1;
    }
    if(!command_context.mqtt_client) {
        ESP_LOGE(TAG, "MQTT client not initialized, call init first");
        return 1;
    }
    connection_test(&command_context, *connection_args.uri->sval, *connection_args.test_case->ival);
    return 0;
}

static int do_publish_setup(int argc, char **argv) {
    RETURN_ON_PARSE_ERROR(publish_setup_args);
    if(command_context.data) {
        free(command_context.data);
    }
    command_context.data = calloc(1, sizeof(publish_context_t));
    ((publish_context_t*)command_context.data)->pattern = strdup(*publish_setup_args.pattern->sval);
    ((publish_context_t*)command_context.data)->pattern_repetitions = *publish_setup_args.pattern_repetitions->ival;
    publish_setup(&command_context, *publish_setup_args.transport->sval);
    return 0;
}

static int do_publish(int argc, char **argv) {
    RETURN_ON_PARSE_ERROR(publish_args);
    publish_test(&command_context, publish_args.expected_to_publish->ival[0], publish_args.qos->ival[0], publish_args.enqueue->ival[0]);
    return 0;
}

static int do_publish_report(int argc, char **argv) {
    (void)argc;
    (void)argv;
    publish_context_t * ctx = command_context.data;
    ESP_LOGI(TAG,"Test Report : Messages received %d, %d expected", ctx->nr_of_msg_received, ctx->nr_of_msg_expected);

    return 0;
}
void register_common_commands(void) {
    const esp_console_cmd_t init = {
        .command = "init",
        .help = "Run inition test\n",
        .hint = NULL,
        .func = &do_init,
    };

    const esp_console_cmd_t start = {
        .command = "start",
        .help = "Run startion test\n",
        .hint = NULL,
        .func = &do_start,
    };
    const esp_console_cmd_t stop = {
        .command = "stop",
        .help = "Run stopion test\n",
        .hint = NULL,
        .func = &do_stop,
    };
    const esp_console_cmd_t destroy = {
        .command = "destroy",
        .help = "Run destroyion test\n",
        .hint = NULL,
        .func = &do_destroy,
    };
    const esp_console_cmd_t free_heap = {
        .command = "free_heap",
        .help = "Run destroyion test\n",
        .hint = NULL,
        .func = &do_free_heap,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&init));
    ESP_ERROR_CHECK(esp_console_cmd_register(&start));
    ESP_ERROR_CHECK(esp_console_cmd_register(&stop));
    ESP_ERROR_CHECK(esp_console_cmd_register(&destroy));
    ESP_ERROR_CHECK(esp_console_cmd_register(&free_heap));
}
void register_publish_commands(void) {
    publish_setup_args.transport  = arg_str1(NULL,NULL,"<transport>", "Selected transport to test");
    publish_setup_args.pattern  = arg_str1(NULL,NULL,"<pattern>", "Message pattern repeated to build big messages");
    publish_setup_args.pattern_repetitions  = arg_int1(NULL,NULL,"<pattern repetitions>", "How many times the pattern is repeated");
    publish_setup_args.end = arg_end(1);

    publish_args.expected_to_publish  = arg_int1(NULL,NULL,"<number of messages>", "How many times the pattern is repeated");
    publish_args.qos  = arg_int1(NULL,NULL,"<qos>", "How many times the pattern is repeated");
    publish_args.enqueue  = arg_int1(NULL,NULL,"<enqueue>", "How many times the pattern is repeated");
    publish_args.end = arg_end(1);
    const esp_console_cmd_t publish_setup = {
        .command = "publish_setup",
        .help = "Run publish test\n",
        .hint = NULL,
        .func = &do_publish_setup,
        .argtable = &publish_setup_args
    };

    const esp_console_cmd_t publish = {
        .command = "publish",
        .help = "Run publish test\n",
        .hint = NULL,
        .func = &do_publish,
        .argtable = &publish_args
    };
    const esp_console_cmd_t publish_report = {
        .command = "publish_report",
        .help = "Run destroyion test\n",
        .hint = NULL,
        .func = &do_publish_report,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&publish_setup));
    ESP_ERROR_CHECK(esp_console_cmd_register(&publish));
    ESP_ERROR_CHECK(esp_console_cmd_register(&publish_report));
}
void register_connect_commands(void){
    connection_args.uri = arg_str1(NULL,NULL,"<broker uri>", "Broker address");
    connection_args.test_case = arg_int1(NULL, NULL, "<test case>","Selected test case");
    connection_args.end = arg_end(1);

    const esp_console_cmd_t connect = {
        .command = "connect",
        .help = "Run connection test\n",
        .hint = NULL,
        .func = &do_connect,
        .argtable = &connection_args
    };

    const esp_console_cmd_t reconnect = {
        .command = "reconnect",
        .help = "Run reconnection test\n",
        .hint = NULL,
        .func = &do_reconnect,
    };
    const esp_console_cmd_t connection_setup = {
        .command = "connection_setup",
        .help = "Run reconnection test\n",
        .hint = NULL,
        .func = &do_connect_setup,
    };
    const esp_console_cmd_t connection_teardown = {
        .command = "connection_teardown",
        .help = "Run reconnection test\n",
        .hint = NULL,
        .func = &do_connect_teardown,
    };
    const esp_console_cmd_t disconnect = {
        .command = "disconnect",
        .help = "Run disconnection test\n",
        .hint = NULL,
        .func = &do_disconnect,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&connect));
    ESP_ERROR_CHECK(esp_console_cmd_register(&disconnect));
    ESP_ERROR_CHECK(esp_console_cmd_register(&reconnect));
    ESP_ERROR_CHECK(esp_console_cmd_register(&connection_setup));
    ESP_ERROR_CHECK(esp_console_cmd_register(&connection_teardown));
}

void app_main(void)
{
    static const size_t max_line = 256;

    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("wifi", ESP_LOG_ERROR);
    esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
    esp_log_level_set("outbox", ESP_LOG_VERBOSE);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = "mqtt>";
    repl_config.max_cmdline_length = max_line;
    esp_console_register_help_command();
    register_common_commands();
    register_connect_commands();
    register_publish_commands();

    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
