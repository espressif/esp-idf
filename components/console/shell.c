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
#include "esp_shell.h"

#include "freertos/task.h"
#include "linenoise/linenoise.h"

#define READ_BUFFER_SIZE 256
#define MAX(a,b) ((a) > (b) ? (a) : (b))

typedef struct {
    const char *command_line;
    esp_console_task_handle_t *task_handle;

    int stdout_pipe[2];
} cli_task_stack_entry_t;

/*
 * This function runs a single command line pipeline with optional output redirection.
 * It handles the actual pipeline execution and I/O multiplexing.
 */
static esp_err_t esp_shell_run_pipeline(char *command_line, FILE *output_file, int *cmd_ret)
{
    cli_task_stack_entry_t cli_stack[5];
    int stack_index = 0;
    *cmd_ret = 0;

    int stdin_pipe[2];
    if (pipe(stdin_pipe) != 0) {
        fprintf(stderr, "Failed to create input pipe: %s\n", strerror(errno));
        return ESP_FAIL;
    }

    // Split by '|' to get individual commands in pipeline
    char *saveptr = NULL;
    char *token = strtok_r(command_line, "|", &saveptr);

    // Next task should read from the previous task's stdout
    int input_fd = stdin_pipe[0];

    while (token) {

        // Skip leading spaces.
        while (*token == ' ') {
            token++;
        }

        // Skip empty tokens
        if (*token == '\0') {
            token = strtok_r(NULL, "|", &saveptr);
            continue;
        }

        if (stack_index >= sizeof(cli_stack) / sizeof(cli_stack[0])) {
            fprintf(stderr, "Command stack overflow\n");
            return ESP_FAIL;
        }

        cli_stack[stack_index].command_line = token;
        cli_stack[stack_index].task_handle = NULL;

        // Create pipes for stdout
        if (pipe(cli_stack[stack_index].stdout_pipe) != 0) {
            fprintf(stderr, "Failed to create output pipe: %s\n", strerror(errno));
            return ESP_FAIL;
        }

        esp_err_t err = esp_console_run_on_task(token, fdopen(input_fd, "r"), fdopen(cli_stack[stack_index].stdout_pipe[1], "w"), NULL,
                                                &cli_stack[stack_index].task_handle);

        if (err == ESP_ERR_NOT_FOUND) {
            fprintf(stderr, "Unrecognized command '%s'\n", token);
            *cmd_ret = err;
            goto abort_pipe;
        } else if (err == ESP_ERR_INVALID_ARG) {
            // command was empty
            *cmd_ret = 1;
            goto abort_pipe;
        } else if (err != ESP_OK) {
            fprintf(stderr, "Command '%s' Internal error: %s\n", token, esp_err_to_name(err));
            *cmd_ret = err;
            goto abort_pipe;
        }

        input_fd = cli_stack[stack_index].stdout_pipe[0]; // Pipe stdout from previous task to next task's stdin
        stack_index++;
        token = strtok_r(NULL, "|", &saveptr);
    }

    while (1) {

        // Check for completed tasks, clean up and get exit codes
        while (stack_index > 0) {
            cli_task_stack_entry_t *entry = &cli_stack[stack_index - 1];
            if (esp_console_task_is_running(entry->task_handle)) {
                break;
            }

            // Use the wait function to get exit code, it will be instant if already terminated (because of exception)
            int exit_code;
            esp_console_wait_task(entry->task_handle, &exit_code);

            if (exit_code == 0) {
                //printf("Command '%s' executed successfully\n", entry->command_line);
                *cmd_ret = 0;
            } else {
                fprintf(stderr, "Command '%s' returned exit code: %d\n", entry->command_line, exit_code);
                // Accumulate non-zero exit codes for now, so we report failure if any command fails
                *cmd_ret += exit_code;
            }

            esp_console_task_free(entry->task_handle);
            stack_index--;
        }

        // If no more tasks running, exit loop
        if (stack_index == 0) {
            break;
        }

        fd_set readfds;
        fd_set exceptfds;
        struct timeval tv;
        FD_ZERO(&readfds);
        FD_ZERO(&exceptfds);
        FD_SET(fileno(stdin), &readfds);

        cli_task_stack_entry_t *tail_entry = &cli_stack[stack_index - 1];

        FD_SET(tail_entry->stdout_pipe[0], &readfds); // For reading output
        FD_SET(tail_entry->stdout_pipe[0], &exceptfds); // For detecting closed pipe
        int max_fd = fileno(stdin);
        max_fd = MAX(max_fd, tail_entry->stdout_pipe[0]);

        // Set timeout to avoid blocking forever
        tv.tv_sec = 5;
        tv.tv_usec = 0; // 1000000; // 100ms

        int ret = select(max_fd + 1, &readfds, NULL, &exceptfds, &tv);

        if (ret > 0) {
            // Select loop while task is running
            char read_buffer[READ_BUFFER_SIZE];

            // Check stdout from last task in pipeline
            if (FD_ISSET(tail_entry->stdout_pipe[0], &readfds)) {
                ssize_t n = read(tail_entry->stdout_pipe[0], read_buffer, READ_BUFFER_SIZE - 1);
                if (n > 0) {
                    if (output_file) {
                        // Redirect to file
                        fwrite(read_buffer, 1, n, output_file);
                        fflush(output_file);
                    } else {
                        // Normal output to stdout
                        fwrite(read_buffer, 1, n, stdout);
                        fflush(stdout);
                    }
                }
            }

            // Standard input is buffered before sent to first task, because we need to detect Ctrl+D
            if (FD_ISSET(fileno(stdin), &readfds)) {
                // Skip if stdin pipe on first task is already closed
                if (stdin_pipe[1] != -1) {
                    // Read from stdin and write to command's stdin
                    ssize_t n = read(fileno(stdin), read_buffer, READ_BUFFER_SIZE - 1);
                    if (n > 0) {
                        // Check for Ctrl+D (EOF character, ASCII 0x04)
                        bool ctrl_d_found = false;
                        for (ssize_t i = 0; i < n; i++) {
                            if (read_buffer[i] == 0x04) {
                                ctrl_d_found = true;
                                // Write everything up to Ctrl+D
                                if (i > 0) {
                                    write(stdin_pipe[1], read_buffer, i);
                                }
                                break;
                            }
                        }

                        if (ctrl_d_found) {
                            printf("Ctrl+D detected, sending EOF to command input\n");
                            // Close write end of stdin pipe to signal EOF to the command
                            close(stdin_pipe[1]);
                            stdin_pipe[1] = -1; // Mark as closed
                        } else {
                            // Write the full buffer if no Ctrl+D
                            write(stdin_pipe[1], read_buffer, n);
                        }
                    } else if (n == 0) {
                        // EOF on stdin (should not happen in terminal, but handle it)
                        close(stdin_pipe[1]);
                        stdin_pipe[1] = -1;
                    }
                }

            }
        } else if (ret < 0 && errno != EINTR) {
            fprintf(stderr, "select() error: %s\n", strerror(errno));
            break;
        }
    }

abort_pipe:
    if (stdin_pipe[0] != -1) {
        close(stdin_pipe[0]);
    }
    if (stdin_pipe[1] != -1) {
        close(stdin_pipe[1]);
    }

    return ESP_OK;
}

/*
 * This function runs a single command line, which may be part of a pipeline.
 * It sets up output redirection and delegates pipeline execution to esp_shell_run_pipeline.
 */
static esp_err_t esp_shell_run_single(char *command_line, int *cmd_ret)
{
    // Check for output redirection (> or >> filename) first
    char *redirect_pos = NULL;
    char *append_pos = strstr(command_line, ">>");
    char *write_pos = strstr(command_line, ">");
    bool append_mode = false;
    FILE *output_file = NULL;
    char *filename = NULL;

    // Check for >> first (longer pattern), then >
    if (append_pos) {
        redirect_pos = append_pos;
        append_mode = true;
    } else if (write_pos) {
        redirect_pos = write_pos;
        append_mode = false;
    }

    if (redirect_pos) {
        // Null-terminate the command part before '>' or '>>'
        *redirect_pos = '\0';

        // Find the filename after '>' or '>>'
        filename = redirect_pos + (append_mode ? 2 : 1);
        while (*filename == ' ') {
            filename++;    // Skip spaces
        }

        // Remove trailing spaces/newlines from filename
        char *end = filename + strlen(filename) - 1;
        while (end > filename && (*end == ' ' || *end == '\n' || *end == '\r')) {
            *end-- = '\0';
        }

        if (*filename != '\0') {
            output_file = fopen(filename, append_mode ? "a" : "w");
            if (!output_file) {
                fprintf(stderr, "Failed to open file '%s' for %s: %s\n", filename,
                        append_mode ? "appending" : "writing", strerror(errno));
                return ESP_FAIL;
            }
        } else {
            fprintf(stderr, "No filename specified after '%s'\n", append_mode ? ">>" : ">");
            return ESP_FAIL;
        }
    }

    // Run the pipeline with the specified output file (or NULL for stdout)
    esp_err_t result = esp_shell_run_pipeline(command_line, output_file, cmd_ret);

    // Close output file if it was opened
    if (output_file) {
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
