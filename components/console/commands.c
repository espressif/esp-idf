/*
 * SPDX-FileCopyrightText: 2016-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/param.h>
#include <unistd.h>
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_system.h"
#include "freertos/idf_additions.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"
#include "sys/queue.h"

#define ANSI_COLOR_DEFAULT      39      /** Default foreground color */

typedef struct cmd_item_ {
    esp_console_cmd_t def;
    char *hint;
    SLIST_ENTRY(cmd_item_) next;    //!< next command in the list
} cmd_item_t;

typedef void (*const fn_print_arg_t)(cmd_item_t*);

/** linked list of command structures */
static SLIST_HEAD(cmd_list_, cmd_item_) s_cmd_list;

/** run-time configuration options */
static esp_console_config_t s_config = {
    .heap_alloc_caps = MALLOC_CAP_DEFAULT
};

/** temporary buffer used for command line parsing */
static char *s_tmp_line_buf;

static const cmd_item_t *find_command_by_name(const char *name);

static esp_console_help_verbose_level_e s_verbose_level = ESP_CONSOLE_HELP_VERBOSE_LEVEL_1;



const esp_console_cmd_t *esp_console_get_by_name(const char *name)
{
    const cmd_item_t *cmd =  find_command_by_name(name);
    if (cmd) {
        return &cmd->def;
    }
    return NULL;
}

const esp_console_cmd_t *esp_console_get_iterate(const esp_console_cmd_t *prev)
{
    const cmd_item_t *cmd = NULL;
    if (prev == NULL) {
        cmd = SLIST_FIRST(&s_cmd_list);
    } else {
        cmd = SLIST_NEXT((cmd_item_t *)prev, next);
    }
    if (cmd) {
        return &cmd->def;
    }
    return NULL;
}

esp_err_t esp_console_init(const esp_console_config_t *config)
{
    if (!config) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_tmp_line_buf) {
        return ESP_ERR_INVALID_STATE;
    }
    memcpy(&s_config, config, sizeof(s_config));
    if (s_config.hint_color == 0) {
        s_config.hint_color = ANSI_COLOR_DEFAULT;
    }
    if (s_config.heap_alloc_caps == 0) {
        s_config.heap_alloc_caps = MALLOC_CAP_DEFAULT;
    }
    s_tmp_line_buf = heap_caps_calloc(1, config->max_cmdline_length, s_config.heap_alloc_caps);
    if (s_tmp_line_buf == NULL) {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

esp_err_t esp_console_deinit(void)
{
    if (!s_tmp_line_buf) {
        return ESP_ERR_INVALID_STATE;
    }
    free(s_tmp_line_buf);
    s_tmp_line_buf = NULL;
    cmd_item_t *it, *tmp;
    SLIST_FOREACH_SAFE(it, &s_cmd_list, next, tmp) {
        SLIST_REMOVE(&s_cmd_list, it, cmd_item_, next);
        free(it->hint);
        free(it);
    }
    return ESP_OK;
}

void esp_console_rm_item_free_hint(cmd_item_t *item)
{
    SLIST_REMOVE(&s_cmd_list, item, cmd_item_, next);
    free(item->hint);
}

esp_err_t esp_console_cmd_deregister(const char *cmd_name)
{
    cmd_item_t *item = (cmd_item_t *)find_command_by_name(cmd_name);
    if (item == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_console_rm_item_free_hint(item);
    heap_caps_free(item);
    return ESP_OK;
}

esp_err_t esp_console_cmd_register(const esp_console_cmd_t *cmd)
{
    cmd_item_t *item = NULL;
    if (!cmd || cmd->command == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (strchr(cmd->command, ' ') != NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if ((cmd->func == NULL && cmd->func_w_context == NULL)
            || (cmd->func != NULL && cmd->func_w_context != NULL)) {
        return ESP_ERR_INVALID_ARG;
    }
    item = (cmd_item_t *)find_command_by_name(cmd->command);
    if (!item) {
        // not registered before
        item = heap_caps_calloc(1, sizeof(*item), s_config.heap_alloc_caps);
        if (item == NULL) {
            return ESP_ERR_NO_MEM;
        }
    } else {
        // remove from list and free the old hint, because we will alloc new hint for the command
        esp_console_rm_item_free_hint(item);
    }

    item->def = *cmd;
    if (cmd->hint) {
        /* Prepend a space before the hint. It separates command name and
         * the hint. arg_print_syntax below adds this space as well.
         */
        int unused __attribute__((unused));
        unused = asprintf((char **)&item->hint, " %s", cmd->hint);
    } else if (cmd->argtable) {
        /* Generate hint based on cmd->argtable */
        arg_dstr_t ds = arg_dstr_create();
        arg_print_syntax_ds(ds, cmd->argtable, NULL);
        item->hint = strdup(arg_dstr_cstr(ds));
        arg_dstr_destroy(ds);
    }

    cmd_item_t *last;
    cmd_item_t *it;
#if CONFIG_CONSOLE_SORTED_HELP
    last = NULL;
    SLIST_FOREACH(it, &s_cmd_list, next) {
        if (strcmp(it->def.command, item->def.command) > 0) {
            break;
        }
        last = it;
    }
#else
    last = SLIST_FIRST(&s_cmd_list);
#endif
    if (last == NULL) {
        SLIST_INSERT_HEAD(&s_cmd_list, item, next);
    } else {
#if !CONFIG_CONSOLE_SORTED_HELP
        while ((it = SLIST_NEXT(last, next)) != NULL) {
            last = it;
        }
#endif
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
    cmd_item_t *it;
    SLIST_FOREACH(it, &s_cmd_list, next) {
        /* Check if command starts with buf */
        if (strncmp(buf, it->def.command, len) == 0) {
            linenoiseAddCompletion(lc, it->def.command);
        }
    }
}

const char *esp_console_get_hint(const char *buf, int *color, int *bold)
{
    size_t len = strlen(buf);
    cmd_item_t *it;
    SLIST_FOREACH(it, &s_cmd_list, next) {
        if (strlen(it->def.command) == len &&
                strncmp(buf, it->def.command, len) == 0) {
            *color = s_config.hint_color;
            *bold = s_config.hint_bold;
            return it->hint;
        }
    }
    return NULL;
}

static const cmd_item_t *find_command_by_name(const char *name)
{
    const cmd_item_t *cmd = NULL;
    cmd_item_t *it;
    size_t len = strlen(name);
    SLIST_FOREACH(it, &s_cmd_list, next) {
        if (strlen(it->def.command) == len &&
                strcmp(name, it->def.command) == 0) {
            cmd = it;
            break;
        }
    }
    return cmd;
}

esp_err_t esp_console_run(const char *cmdline, int *cmd_ret)
{
    if (s_tmp_line_buf == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    char **argv = (char **) heap_caps_calloc(s_config.max_cmdline_args, sizeof(char *), s_config.heap_alloc_caps);
    if (argv == NULL) {
        return ESP_ERR_NO_MEM;
    }
    strlcpy(s_tmp_line_buf, cmdline, s_config.max_cmdline_length);

    size_t argc = esp_console_split_argv(s_tmp_line_buf, argv,
                                         s_config.max_cmdline_args);
    if (argc == 0) {
        free(argv);
        return ESP_ERR_INVALID_ARG;
    }
    const cmd_item_t *cmd = find_command_by_name(argv[0]);
    if (cmd == NULL) {
        free(argv);
        return ESP_ERR_NOT_FOUND;
    }
    if (cmd->def.func) {
        *cmd_ret = (*cmd->def.func)(argc, argv);
    }
    if (cmd->def.func_w_context) {
        *cmd_ret = (*cmd->def.func_w_context)(cmd->def.context, argc, argv);
    }
    free(argv);
    return ESP_OK;
}


#ifdef CONFIG_CONSOLE_COMMAND_ON_TASK

typedef struct esp_console_task_handle {
    const cmd_item_t *cmd;       //!< Pointer to the command definition
    TaskHandle_t task_handle;    //!< Handle of the created task (protected by lock)
    FILE *_stdin;                //!< Pipe for command input
    FILE *_stdout;               //!< Pipe for command output
    FILE *_stderr;               //!< Pipe for command error output
    int exit_code;               //!< Exit code of the command (protected by lock)
    _lock_t lock;                //!< Lock to protect task_handle and exit_code
    size_t argc;                 //!< Number of command line arguments
    char *argv[0];               //!< Command line arguments (flexible array member)
} esp_console_task_handle_t;


static void task_cmd(void *arg) {
    esp_console_task_handle_t *task = (esp_console_task_handle_t *)arg;


    FILE *old_stdin = __getreent()->_stdin;
    FILE *old_stdout = __getreent()->_stdout;
    FILE *old_stderr = __getreent()->_stderr;

    if (task->_stdin)
        __getreent()->_stdin = task->_stdin;
    if (task->_stdout)
        __getreent()->_stdout = task->_stdout;
    if (task->_stderr)
        __getreent()->_stderr = task->_stderr;

    int exit_code = -1;
    if (task->cmd->def.func) {
        exit_code = task->cmd->def.func(task->argc, task->argv);
    }
    if (task->cmd->def.func_w_context) {
        exit_code = (*task->cmd->def.func_w_context)(task->cmd->def.context, task->argc, task->argv);
    }

    // Close sockets only if not pointing to standard fds
    if (task->_stdin) {
        fclose(task->_stdin);
        __getreent()->_stdin = old_stdin;
    }
    if (task->_stdout) {
        fclose(task->_stdout);
        __getreent()->_stdout = old_stdout;
    }
    if (task->_stderr) {
        fclose(task->_stderr);
        __getreent()->_stderr = old_stderr;
    }

    __lock_acquire(task->lock);
    task->exit_code = exit_code;
    task->task_handle = NULL;
    __lock_release(task->lock);

    vTaskDelete(NULL);
}

esp_err_t esp_console_run_on_task(const char *cmdline, FILE *_stdin, FILE *_stdout, FILE *_stderr, esp_console_task_handle_t **out_task)
{
    const size_t cmd_len = strlen(cmdline);
    if (!cmd_len || cmd_len >= s_config.max_cmdline_length) {
        return ESP_ERR_INVALID_ARG;
    }

    // Try to do all memory allocations in one go
    // Calculate the size of each component for clarity and maintainability
    const size_t task_struct_size = sizeof(esp_console_task_handle_t); // Size of the task struct
    const size_t argv_array_size = sizeof(char *) * s_config.max_cmdline_args; // Size of argv array
    const size_t cmdline_buf_size = cmd_len + 1; // Size of command line buffer (including null terminator)
    const size_t total_size = task_struct_size + argv_array_size + cmdline_buf_size;

    esp_console_task_handle_t *task = (esp_console_task_handle_t *) heap_caps_calloc(1, total_size, s_config.heap_alloc_caps);
    if (task == NULL) {
        return ESP_ERR_NO_MEM;
    }

    // The line buffer is placed after the struct and argv array
    char *line_buf = (char *) (((char *)task) + task_struct_size + argv_array_size);
    strlcpy(line_buf, cmdline, cmd_len+1);

    task->argc = esp_console_split_argv(line_buf, task->argv,
                                         s_config.max_cmdline_args);
    if (task->argc == 0) {
        free(task);
        return ESP_ERR_INVALID_ARG;
    }

    task->exit_code = -1;
    task->cmd = find_command_by_name(task->argv[0]);
    if (task->cmd == NULL) {
        free(task);
        return ESP_ERR_NOT_FOUND;
    }

    task->_stdin = _stdin;
    task->_stdout = _stdout;
    task->_stderr = _stderr;
    __lock_init(task->lock);

    uint32_t stack_size = task->cmd->def.stack_size ? task->cmd->def.stack_size : (CONFIG_CONSOLE_COMMAND_DEFAULT_TASK_STACK_SIZE);
    UBaseType_t priority = task->cmd->def.priority ? task->cmd->def.priority : (CONFIG_CONSOLE_COMMAND_DEFAULT_TASK_PRIORITY);
    BaseType_t handle = xTaskCreate(&task_cmd, task->argv[0], stack_size, task, priority, &task->task_handle);

    if (handle != pdPASS) {
        __lock_close(task->lock);
        free(task);
        return ESP_ERR_NO_MEM;
    }
    *out_task = task;

    return ESP_OK;
}

void esp_console_task_free(esp_console_task_handle_t *task)
{
    __lock_acquire(task->lock);
    TaskHandle_t handle = task->task_handle;
    __lock_release(task->lock);

    if (handle) {
        // Wait for the task to finish before deleting
        esp_console_wait_task(task, NULL);
        vTaskDelete(handle);

        __lock_acquire(task->lock);
        task->task_handle = NULL;
        __lock_release(task->lock);
    }
    __lock_close(task->lock);
    free(task);
}

bool esp_console_task_is_running(esp_console_task_handle_t *task)
{
    __lock_acquire(task->lock);
    TaskHandle_t handle = task->task_handle;
    __lock_release(task->lock);

    if (handle) {
        return eTaskGetState(handle) != eDeleted;
    }
    return false;
}

void esp_console_wait_task(esp_console_task_handle_t *task, int *cmd_ret)
{
    TaskHandle_t handle;

    __lock_acquire(task->lock);
    handle = task->task_handle;
    __lock_release(task->lock);

    if (handle) {
        // Wait until task is done
        while (eTaskGetState(handle) != eDeleted) {
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }

    if (cmd_ret) {
        __lock_acquire(task->lock);
        *cmd_ret = task->exit_code;
        __lock_release(task->lock);
    }
}

#endif // CONFIG_CONSOLE_COMMAND_ON_TASK

static struct {
    struct arg_str *help_cmd;
    struct arg_int *verbose_level;
    struct arg_end *end;
} help_args;

static void print_arg_help(cmd_item_t *it)
{
    /* First line: command name and hint
     * Pad all the hints to the same column
     */
    const char *hint = (it->hint) ? it->hint : "";
    printf("%-s %s\n", it->def.command, hint);
    /* Second line: print help.
     * Argtable has a nice helper function for this which does line
     * wrapping.
     */
    printf("  "); // arg_print_formatted does not indent the first line
    arg_print_formatted(stdout, 2, 78, it->def.help);
    /* Finally, print the list of arguments */
    if (it->def.argtable) {
        arg_print_glossary(stdout, (void **) it->def.argtable, "  %12s  %s\n");
    }
    printf("\n");
}

static void print_arg_command(cmd_item_t *it)
{
    const char *hint = (it->hint) ? it->hint : "";
    printf("%-s %s\n\n", it->def.command, hint);
}

static fn_print_arg_t print_verbose_level_arr[ESP_CONSOLE_HELP_VERBOSE_LEVEL_MAX_NUM] = {
    print_arg_command,
    print_arg_help,
};

static int help_command(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &help_args);

    if (nerrors != 0) {
        arg_print_errors(stderr, help_args.end, argv[0]);
        return 1;
    }

    cmd_item_t *it;
    int ret_value = 1;
    esp_console_help_verbose_level_e verbose_level;

    if (help_args.help_cmd->count == 0) {
        /* If verbose level is not provided, set it as default value */
        if (help_args.verbose_level->count == 0) {
            verbose_level = s_verbose_level;
        }
        /* Else use the verbose level from input argument */
        else {
            verbose_level = (esp_console_help_verbose_level_e)help_args.verbose_level->ival[0];
        }
        /* Check if selected verbose level is valid, proceed if yes, else return */
        if (verbose_level >= ESP_CONSOLE_HELP_VERBOSE_LEVEL_MAX_NUM) {
            printf("help: invalid verbose level %d", (int)verbose_level);
            return 1;
        }

        /* Print info of each command based on verbose level */
        SLIST_FOREACH(it, &s_cmd_list, next) {
            if (it->def.help == NULL) {
                continue;
            }
            print_verbose_level_arr[verbose_level](it);
        }
        ret_value = 0;
    } else {
        /* Print summary of given command, verbose option will be ignored */
        bool found_command = false;
        SLIST_FOREACH(it, &s_cmd_list, next) {
            if (it->def.help == NULL) {
                continue;
            }
            if (strcmp(help_args.help_cmd->sval[0], it->def.command) == 0) {
                print_arg_help(it);
                found_command = true;
                ret_value = 0;
                break;
            }
        }

        /* If given command has not been found, print error message*/
        if (!found_command) {
            printf("help: Unrecognized option '%s'. Please use correct command as argument "
                   "or type 'help' only to print help for all commands\n", help_args.help_cmd->sval[0]);
        }
    }

    return ret_value;
}

esp_err_t esp_console_register_help_command(void)
{
    help_args.help_cmd = arg_str0(NULL, NULL, "<string>", "Name of command");
    help_args.verbose_level = arg_intn("v", "verbose", "<0|1>", 0, 1,
                                       "If specified, list console commands with given verbose level");
    help_args.end = arg_end(2);

    esp_console_cmd_t command = {
        .command = "help",
        .help = "Print the summary of all registered commands if no arguments "
        "are given, otherwise print summary of given command.",
        .func = &help_command,
        .argtable = &help_args
    };
    return esp_console_cmd_register(&command);
    return ESP_OK;
}

esp_err_t esp_console_deregister_help_command(void)
{
    free(help_args.help_cmd);
    free(help_args.verbose_level);
    free(help_args.end);

    return esp_console_cmd_deregister("help");
}

esp_err_t esp_console_set_help_verbose_level(esp_console_help_verbose_level_e verbose_level)
{
    /* legal verbose level sanity check */
    if (verbose_level >= ESP_CONSOLE_HELP_VERBOSE_LEVEL_MAX_NUM) {
        return ESP_ERR_INVALID_ARG;
    }
    s_verbose_level = verbose_level;
    return ESP_OK;
}
