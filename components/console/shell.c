/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

#include "esp_console.h"
#include "esp_err.h"
#include "esp_shell.h"

#include "freertos/task.h"
#include "linenoise/linenoise.h"

#define READ_BUFFER_SIZE 256
#define MAX_TASK_DEPTH 5
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

typedef struct {
    const char *command_line;
    esp_console_task_handle_t *task_handle;

} cli_task_stack_entry_t;

static int fpipe_close(FILE *pipes[2])
{
    int ret = 0;
    if (pipes[0]) {
        if (fclose(pipes[0]) != 0) {
            ret = -1;
        }
    }
    if (pipes[1]) {
        if (fclose(pipes[1]) != 0) {
            ret = -1;
        }
    }
    return ret;
}

static int fpipe(FILE *pipes[2])
{
    int fds[2];
    if (pipe(fds) != 0) {
        return -1;
    }
    FILE *read_end = fdopen(fds[0], "r");
    FILE *write_end = fdopen(fds[1], "w");
    if (!read_end || !write_end) {
        fpipe_close(pipes);
        return -1;
    }
    pipes[0] = read_end;
    pipes[1] = write_end;
    return 0;
}

/*
 * This function runs a single command line pipeline with optional output redirection.
 * It handles the actual pipeline execution and I/O multiplexing.
 */
static esp_err_t esp_shell_run_pipeline(char *command_line, FILE *const input_file, FILE *const output_file, int *cmd_ret)
{
    cli_task_stack_entry_t cli_stack[MAX_TASK_DEPTH];
    int stack_index = 0;
    *cmd_ret = 0;
    esp_err_t err = ESP_OK;

    // Split by '|' to get individual commands in pipeline
    char *saveptr = NULL;
    char *token = strtok_r(command_line, "|", &saveptr);

    FILE *input_stream = input_file;
    while (token) {

        // Skip leading spaces.
        while (*token == ' ') {
            token++;
        }

        // Check if this is the last token by peeking ahead
        char *saveptr_peek = saveptr;
        char *next_token = strtok_r(NULL, "|", &saveptr_peek);
        bool is_pipe = (next_token != NULL);

        // Skip empty tokens
        if (*token == '\0') {
            token = strtok_r(NULL, "|", &saveptr);
            continue;
        }

        if (stack_index >= ARRAY_SIZE(cli_stack)) {
            fprintf(stderr, "Command stack overflow\n");
            err = ESP_FAIL;
            break;
        }

        // Prepare next task entry, from here we increment stack_index so we need to be careful on error handling
        cli_task_stack_entry_t *task = &cli_stack[stack_index++];
        *task = (cli_task_stack_entry_t) {
            .command_line = token,
            .task_handle = NULL,
        };

        FILE *stdout_pipe[2] = { NULL, NULL };

        // Create pipes for stdout, if not last command that will be directed to stdout
        FILE *output_stream = output_file;
        uint8_t flags = 0;
        if (is_pipe) {
            if (fpipe(stdout_pipe) != 0) {
                fprintf(stderr, "Failed to create output pipe: %s\n", strerror(errno));
                err = ESP_ERR_NO_MEM;
                stack_index--;
                break;
            }
            output_stream = stdout_pipe[1];
            flags |= ESP_CONSOLE_TASK_CLOSE_STDOUT;
        }

        if (input_stream != input_file) {
            flags |= ESP_CONSOLE_TASK_CLOSE_STDIN;
        }

        // If successful, we account on this function to close stdin and stdout when task is completed
        err = esp_console_run_on_task(token, input_stream, output_stream, stderr, flags, &task->task_handle);

        if (err != ESP_OK) {
            switch (err) {
            case ESP_ERR_NOT_FOUND:
                fprintf(stderr, "Unrecognized command '%s'\n", token);
                break;
            default:
                fprintf(stderr, "Command '%s' Internal error: %s\n", token, esp_err_to_name(err));
                break;
            }
            fpipe_close(stdout_pipe);
            stack_index--;
            break;
        }

        // Make sure next command's input comes from this command's output
        if (is_pipe) {
            input_stream = stdout_pipe[0];
        }
        token = strtok_r(NULL, "|", &saveptr);
    }

    // As long as there are tasks running, monitor their output and stdin
    while (stack_index > 0) {
        cli_task_stack_entry_t *tail_entry = &cli_stack[stack_index - 1];

        // Use the wait function to get exit code, it will be instant if already terminated (because of exception)
        int exit_code;
        esp_console_wait_task(tail_entry->task_handle, &exit_code);

        if (exit_code == 0) {
            //printf("Command '%s' executed successfully\n", tail_entry->command_line);
        } else {
            fprintf(stderr, "Command '%s' returned exit code: %d\n", tail_entry->command_line, exit_code);
            // Accumulate non-zero exit codes for now, so we report failure if any command fails
            *cmd_ret += exit_code;
        }

        esp_console_task_free(tail_entry->task_handle);
        stack_index--;
    }

    return err;
}

/*
 * This function runs a single command line, which may be part of a pipeline.
 * It sets up output redirection and delegates pipeline execution to esp_shell_run_pipeline.
 */
static esp_err_t esp_shell_run_single(char *command_line, int *cmd_ret)
{
    // Check for input and output redirection
    char *output_redirect_pos = NULL;
    char *input_redirect_pos = strstr(command_line, "<");
    char *append_pos = strstr(command_line, ">>");
    char *write_pos = strstr(command_line, ">");
    bool append_mode = false;
    FILE *input_file = stdin;
    FILE *output_file = stdout;
    char *input_filename = NULL;
    char *output_filename = NULL;

    // Handle input redirection first
    if (input_redirect_pos) {
        // Null-terminate the command part before '<'
        *input_redirect_pos = '\0';

        // Find the filename after '<'
        input_filename = input_redirect_pos + 1;
        while (*input_filename == ' ') {
            input_filename++;    // Skip spaces
        }

        // Find end of input filename (stop at output redirection or end of string)
        char *input_end = input_filename;
        while (*input_end && *input_end != '>' && *input_end != ' ') {
            input_end++;
        }

        // Create a copy of the input filename
        size_t input_len = input_end - input_filename;
        char input_file_buffer[input_len + 1];
        strncpy(input_file_buffer, input_filename, input_len);
        input_file_buffer[input_len] = '\0';

        if (input_file_buffer[0] != '\0') {

            input_file = fopen(input_file_buffer, "r");
            if (!input_file) {
                fprintf(stderr, "Failed to open file '%s' for reading: %s\n", input_file_buffer, strerror(errno));
                return ESP_FAIL;
            }
        } else {
            fprintf(stderr, "No filename specified after '<'\n");
            return ESP_FAIL;
        }

        // Update positions to search for output redirection after input redirection
        if (*input_end) {
            append_pos = strstr(input_end, ">>");
            write_pos = strstr(input_end, ">");
        } else {
            append_pos = NULL;
            write_pos = NULL;
        }
    }

    // Check for >> first (longer pattern), then >
    if (append_pos) {
        output_redirect_pos = append_pos;
        append_mode = true;
    } else if (write_pos) {
        output_redirect_pos = write_pos;
        append_mode = false;
    }

    if (output_redirect_pos) {
        // Null-terminate the command part before '>' or '>>'
        *output_redirect_pos = '\0';

        // Find the filename after '>' or '>>'
        output_filename = output_redirect_pos + (append_mode ? 2 : 1);
        while (*output_filename == ' ') {
            output_filename++;    // Skip spaces
        }

        // Remove trailing spaces/newlines from filename
        char *end = output_filename + strlen(output_filename) - 1;
        while (end > output_filename && (*end == ' ' || *end == '\n' || *end == '\r')) {
            *end-- = '\0';
        }

        if (*output_filename != '\0') {

            output_file = fopen(output_filename, append_mode ? "a" : "w");
            if (!output_file) {
                fprintf(stderr, "Failed to open file '%s' for %s: %s\n", output_filename,
                        append_mode ? "appending" : "writing", strerror(errno));
                if (input_file != stdin) {
                    fclose(input_file);
                }
                return ESP_FAIL;
            }
        } else {
            fprintf(stderr, "No filename specified after '%s'\n", append_mode ? ">>" : ">");
            if (input_file != stdin) {
                fclose(input_file);
            }
            return ESP_FAIL;
        }
    }

    // Run the pipeline with the specified input and output files
    esp_err_t result = esp_shell_run_pipeline(command_line, input_file, output_file, cmd_ret);

    // Close files if they were opened
    if (input_file != stdin) {
        fclose(input_file);
    }
    if (output_file != stdout) {
        fclose(output_file);
    }

    return result;
}

/*
 * This function runs a command line, which may contain multiple commands separated by ';', '&&', or '||'.
 * ';' - Execute commands sequentially, continue on failure
 * '&&' - Execute commands sequentially, break on failure
 * '||' - Execute commands sequentially, skip next command on success
 */
esp_err_t esp_shell_run(char *command_line, int *cmd_ret)
{
    char *current_pos = command_line;
    *cmd_ret = 0;
    enum { OP_SEMICOLON, OP_AND, OP_OR } operator_type;

    while (*current_pos != '\0') {
        // Find the next separator (';', '&&', or '||')
        char *separator_pos = NULL;
        operator_type = OP_SEMICOLON; // Default to semicolon

        // Look for '&&' and '||' first (longer patterns)
        char *and_pos = strstr(current_pos, "&&");
        char *or_pos = strstr(current_pos, "||");
        char *semicolon_pos = strchr(current_pos, ';');

        // Find the earliest separator
        char *earliest_pos = NULL;
        if (and_pos && (!earliest_pos || and_pos < earliest_pos)) {
            earliest_pos = and_pos;
            operator_type = OP_AND;
        }
        if (or_pos && (!earliest_pos || or_pos < earliest_pos)) {
            earliest_pos = or_pos;
            operator_type = OP_OR;
        }
        if (semicolon_pos && (!earliest_pos || semicolon_pos < earliest_pos)) {
            earliest_pos = semicolon_pos;
            operator_type = OP_SEMICOLON;
        }

        separator_pos = earliest_pos;

        // Extract the current command
        char *command_end = separator_pos ? separator_pos : current_pos + strlen(current_pos);
        size_t command_len = command_end - current_pos;

        // Create a null-terminated copy of the command
        char command_buffer[command_len + 1];
        strncpy(command_buffer, current_pos, command_len);
        command_buffer[command_len] = '\0';

        // Skip leading spaces
        char *command = command_buffer;
        while (*command == ' ') {
            command++;
        }

        // Skip trailing spaces
        char *end = command + strlen(command) - 1;
        while (end > command && *end == ' ') {
            *end-- = '\0';
        }

        // Skip empty commands
        if (*command != '\0') {
            esp_err_t ret = esp_shell_run_single(command, cmd_ret);
            bool command_failed = (ret != ESP_OK || *cmd_ret != 0);

            // Handle failure/success based on operator type
            if (operator_type == OP_AND) {
                // '&&' operator: break on failure
                if (command_failed) {
                    return ret != ESP_OK ? ret : ESP_OK;
                }
            } else if (operator_type == OP_OR) {
                // '||' operator: break on success
                if (!command_failed) {
                    return ESP_OK;
                }
            }
            // ';' operator: continue regardless (do nothing)
        }

        // Move to next command
        if (separator_pos) {
            int separator_len = (operator_type == OP_AND || operator_type == OP_OR) ? 2 : 1;
            current_pos = separator_pos + separator_len;
        } else {
            break;
        }
    }

    return ESP_OK;
}
