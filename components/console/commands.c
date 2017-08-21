// Copyright 2016-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/param.h>
#include "esp_log.h"
#include "esp_console.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"
#include "rom/queue.h"

#define ANSI_COLOR_DEFAULT      39      /** Default foreground color */

typedef struct cmd_item_ {
    /**
     * Command name (statically allocated by application)
     */
    const char* command;
    /**
     * Help text (statically allocated by application), may be NULL.
     */
    const char* help;
    /**
     * Hint text, usually lists possible arguments, dynamically allocated.
     * May be NULL.
     */
    char* hint;
    esp_console_cmd_func_t func;    //!< pointer to the command handler
    void* argtable;                 //!< optional pointer to arg table
    SLIST_ENTRY(cmd_item_) next;    //!< next command in the list
} cmd_item_t;

/** linked list of command structures */
static SLIST_HEAD(cmd_list_, cmd_item_) s_cmd_list;

/** run-time configuration options */
static esp_console_config_t s_config;

/** temporary buffer used for command line parsing */
static char* s_tmp_line_buf;

static const cmd_item_t* find_command_by_name(const char* name);

esp_err_t esp_console_init(const esp_console_config_t* config)
{
    if (s_tmp_line_buf) {
        return ESP_ERR_INVALID_STATE;
    }
    memcpy(&s_config, config, sizeof(s_config));
    if (s_config.hint_color == 0) {
        s_config.hint_color = ANSI_COLOR_DEFAULT;
    }
    s_tmp_line_buf = calloc(config->max_cmdline_length, 1);
    if (s_tmp_line_buf == NULL) {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

esp_err_t esp_console_deinit()
{
    if (!s_tmp_line_buf) {
        return ESP_ERR_INVALID_STATE;
    }
    free(s_tmp_line_buf);
    cmd_item_t *it, *tmp;
    SLIST_FOREACH_SAFE(it, &s_cmd_list, next, tmp) {
        free(it->hint);
        free(it);
    }
    return ESP_OK;
}

esp_err_t esp_console_cmd_register(const esp_console_cmd_t *cmd)
{
    cmd_item_t *item = (cmd_item_t *) calloc(1, sizeof(*item));
    if (item == NULL) {
        return ESP_ERR_NO_MEM;
    }
    if (cmd->command == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (strchr(cmd->command, ' ') != NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    item->command = cmd->command;
    item->help = cmd->help;
    if (cmd->hint) {
        /* Prepend a space before the hint. It separates command name and
         * the hint. arg_print_syntax below adds this space as well.
         */
        asprintf(&item->hint, " %s", cmd->hint);
    } else if (cmd->argtable) {
        /* Generate hint based on cmd->argtable */
        char* buf;
        size_t buf_size;
        FILE* f = open_memstream(&buf, &buf_size);
        arg_print_syntax(f, cmd->argtable, NULL);
        fclose(f);
        item->hint = buf;
    }
    item->argtable = cmd->argtable;
    item->func = cmd->func;
    cmd_item_t* last = SLIST_FIRST(&s_cmd_list);
    if (last == NULL) {
        SLIST_INSERT_HEAD(&s_cmd_list, item, next);
    } else {
        cmd_item_t* it;
        while ((it = SLIST_NEXT(last, next)) != NULL) {
            last = it;
        }
        SLIST_INSERT_AFTER(last, item, next);
    }
    return ESP_OK;
}

void esp_console_get_completion(const char *buf, linenoiseCompletions *lc)
{
    size_t len = strlen(buf);
    if (len == 0) {
        return;
    }
    cmd_item_t* it;
    SLIST_FOREACH(it, &s_cmd_list, next) {
        /* Check if command starts with buf */
        if (strncmp(buf, it->command, len) == 0) {
            linenoiseAddCompletion(lc, it->command);
        }
    }
}

const char* esp_console_get_hint(const char *buf, int *color, int *bold)
{
    int len = strlen(buf);
    cmd_item_t* it;
    SLIST_FOREACH(it, &s_cmd_list, next) {
        if (strlen(it->command) == len &&
                strncmp(buf, it->command, len) == 0) {
            *color = s_config.hint_color;
            *bold = s_config.hint_bold;
            return it->hint;
        }
    }
    return NULL;
}

static const cmd_item_t* find_command_by_name(const char* name)
{
    const cmd_item_t* cmd = NULL;
    cmd_item_t* it;
    SLIST_FOREACH(it, &s_cmd_list, next) {
        if (strcmp(name, it->command) == 0) {
            cmd = it;
            break;
        }
    }
    return cmd;
}

esp_err_t esp_console_run(const char* cmdline, int* cmd_ret)
{
    if (s_tmp_line_buf == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    char** argv = (char**) calloc(s_config.max_cmdline_args, sizeof(char*));
    if (argv == NULL) {
        return ESP_ERR_NO_MEM;
    }
    strlcpy(s_tmp_line_buf, cmdline, s_config.max_cmdline_length);

    size_t argc = esp_console_split_argv(s_tmp_line_buf, argv,
            s_config.max_cmdline_args);

    const cmd_item_t* cmd = find_command_by_name(argv[0]);
    if (cmd == NULL) {
        return ESP_ERR_NOT_FOUND;
    }
    *cmd_ret = (*cmd->func)(argc, argv);
    free(argv);
    return ESP_OK;
}

static int help_command(int argc, char** argv)
{
    cmd_item_t* it;

    /* Print summary of each command */
    SLIST_FOREACH(it, &s_cmd_list, next) {
        if (it->help == NULL) {
            continue;
        }
        /* First line: command name and hint
         * Pad all the hints to the same column
         */
        const char* hint = (it->hint) ? it->hint : "";
        printf("%-s %s\n", it->command, hint);
        /* Second line: print help.
         * Argtable has a nice helper function for this which does line
         * wrapping.
         */
        printf("  "); // arg_print_formatted does not indent the first line
        arg_print_formatted(stdout, 2, 78, it->help);
        /* Finally, print the list of arguments */
        if (it->argtable) {
            arg_print_glossary(stdout, (void**) it->argtable, "  %12s  %s\n");
        }
        printf("\n");
    }
    return 0;
}


esp_err_t esp_console_register_help_command()
{
    esp_console_cmd_t command = {
            .command = "help",
            .help = "Print the list of registered commands",
            .func = &help_command
    };
    return esp_console_cmd_register(&command);
}
