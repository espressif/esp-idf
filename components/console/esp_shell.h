/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>

#ifdef CONFIG_CONSOLE_SHELL_ENABLE

/**
 * @brief Run a shell command line with advanced features
 *
 * This function provides a Unix-like shell interface with support for:
 * - Command pipelines using '|' operator
 * - Sequential execution using ';' operator
 * - Conditional execution using '&&' (AND) and '||' (OR) operators
 * - Input redirection using '<' operator
 * - Output redirection using '>' and '>>' operators
 *
 * @param command_line Null-terminated string containing the command(s) to execute.
 *                     The string will be modified during parsing (tokens will be null-terminated).
 * @param cmd_ret Pointer to integer where the combined exit code will be stored.
 *                For multiple commands, non-zero exit codes are accumulated.
 *
 * @return
 *     - ESP_OK: Commands executed successfully (individual command exit codes in cmd_ret)
 *     - ESP_FAIL: Shell parsing or execution error
 *     - ESP_ERR_NO_MEM: Memory allocation failure
 *     - ESP_ERR_NOT_FOUND: Command not found
 *     - ESP_ERR_INVALID_ARG: Invalid command line arguments
 *
 * @note The command_line string is modified during execution and should not be reused.
 *
 * Example usage:
 * @code{c}
 * int exit_code;
 * esp_err_t ret;
 *
 * // Simple command
 * ret = esp_shell_run("help", &exit_code);
 *
 * // Pipeline
 * ret = esp_shell_run("tasks | grep running", &exit_code);
 *
 * // Input/Output redirection
 * ret = esp_shell_run("cat < /data/input.txt > /data/output.txt", &exit_code);
 *
 * // Conditional execution
 * ret = esp_shell_run("mkdir /data/logs && echo 'Directory created'", &exit_code);
 *
 * // Sequential commands
 * ret = esp_shell_run("echo 'Starting'; sleep 1; echo 'Done'", &exit_code);
 * @endcode
 */
esp_err_t esp_shell_run(char *command_line, int *cmd_ret);

#endif
