/*
 * SPDX-FileCopyrightText: 2016-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
  size_t len;
  char **cvec;
} esp_linenoise_completions_t;

// Forward define
struct esp_linenoise_t;

// Linenoise handle is a pointer to an internal structure
typedef struct esp_linenoise_t* esp_linenoise_handle_t;

typedef void(esp_linenoise_completion_callback_t)(const char *buf, esp_linenoise_completions_t *lc);
typedef char*(esp_linenoise_hints_callback_t)(const char *buf, int *color, int *bold);
typedef void(esp_linenoise_free_hints_callback_t)(void *buf);

/**
 * @brief Create a new linenoise instance.
 * 
 * @returns A new linenoise handle.
 */
esp_linenoise_handle_t esp_linenoise_create();

/**
 * @brief The main API of the linenoise library.
 * 
 * @param prompt the command line prompt to show the user when ready to accept input.
 * 
 * @returns the console command the user entered, or NULL if failure.
 */
char* esp_linenoise(esp_linenoise_handle_t handle, const char *prompt);

/**
 * @brief Determine if escape sequences are supported by the terminal by probing it.
 * 
 * @returns - ESP_OK if the terminal supports escape sequences
 *          - ESP_NOT_SUPPORTED if the terminal does not support escape sequences
 *          - ESP_FAIL if probing failed
 *          - ESP_INVALID_ARG if the handle is invalid
 */
esp_err_t  esp_linenoise_probe(esp_linenoise_handle_t handle);

/**
 * @brief Set the FILE* for linenoise to use as standard input (stdin)
 * 
 * @returns - ESP_OK on success
 *          - ESP_INVALID_ARG if the handle is invalid
 */
esp_err_t  esp_linenoise_set_stdin(esp_linenoise_handle_t handle, FILE* fp);

/**
 * @brief Set the FILE* for linenoise to use as standard output (stdout)
 * 
 * @returns - ESP_OK on success
 *          - ESP_INVALID_ARG if the handle is invalid
 */
esp_err_t  esp_linenoise_set_stdout(esp_linenoise_handle_t handle, FILE* fp);

/**
 * @brief Set if multiline mode. 
 * 
 * @returns - ESP_OK on success
 *          - ESP_INVALID_ARG if the handle is invalid
 */
esp_err_t  esp_linenoise_set_multiline(esp_linenoise_handle_t handle, int mlmode);

/**
 * @brief Set if terminal does not recognize escape sequences.
 * 
 * @returns - ESP_OK on success
 *          - ESP_INVALID_ARG if the handle is invalid
 */
esp_err_t  esp_linenoise_set_dumb_mode(esp_linenoise_handle_t handle, int dumbmode);

/**
 * @brief Returns whether the current terminal mode is dumb mode or not.
 * 
 * @returns - ESP_OK on success
 *          - ESP_INVALID_ARG if the handle is invalid
 */
esp_err_t  esp_linenoise_is_dumb_mode(esp_linenoise_handle_t handle, bool *is_dumb_mode);

/**
 * @brief If true, pressing 'enter' without entering any text is considered
 * valid input and will cause esp_linenoise() to return an empty string
 * 
@returns - ESP_OK on success
 *       - ESP_INVALID_ARG if the handle is invalid
 */
esp_err_t  esp_linenoise_set_allow_empty(esp_linenoise_handle_t handle, bool allow_empty);

/**
 * @brief Set line maximum length. 
 * 
 * @returns - ESP_OK on success
 *          - ESP_NOT_SUPPORTED if len arg is smaller than LINENOISE_MINIMAL_MAX_LINE
 *          - ESP_INVALID_ARG if the handle is invalid
 */
esp_err_t  esp_linenoise_set_max_line_length(esp_linenoise_handle_t handle, size_t length);

/**
 * @brief  Clear the screen.
 * 
 * @returns - ESP_OK on success
 *          - ESP_INVALID_ARG if the handle is invalid
 */
esp_err_t  esp_linenoise_clear_screen(esp_linenoise_handle_t handle);

/**
 * @brief Register a callback function to be called for tab-completion.
 * 
 * @returns - ESP_OK on success
 *          - ESP_INVALID_ARG if the handle is invalid
 */
esp_err_t  esp_linenoise_set_completions_callback(esp_linenoise_handle_t handle, esp_linenoise_completion_callback_t *callback);

/**
 * @brief Register a hits function to be called to show hits to the user at the right of the prompt.
 * 
 * @returns - ESP_OK on success
 *          - ESP_INVALID_ARG if the handle is invalid
 */
esp_err_t  esp_linenoise_set_hints_callback(esp_linenoise_handle_t handle, esp_linenoise_hints_callback_t *callback);

/**
 * @brief Register a function to free the hints returned by the hints callback
 * registered with esp_linenoise_set_completions_callback().
 * 
 * @returns - ESP_OK on success
 *          - ESP_INVALID_ARG if the handle is invalid
 */
esp_err_t  esp_linenoise_set_free_hints_callback(esp_linenoise_handle_t handle, esp_linenoise_free_hints_callback_t *callback);

/**
 * @brief This function is used by the callback function registered by the user
 * in order to add completion options given the input string when the
 * user typed <tab>.
 * 
 * @param lc the linenoise completion object that we are augmenting
 * @param str the tab completion to add
 * 
 * @returns - ESP_OK on success
 *          - ESP_NO_MEM if we ran out of memory
 */
esp_err_t  esp_linenoise_add_completion(esp_linenoise_completions_t *lc, const char *str);

/**
 * @brief This is the API call to add a new entry in the linenoise history.
 * It uses a fixed array of char pointers that are shifted (memmoved)
 * when the history max length is reached in order to remove the older
 * entry and make room for the new one, so it is not exactly suitable for huge
 * histories, but will work well for a few hundred of entries.
 * 
 * @returns - ESP_OK on success, or the line is already at the front of the history
 *          - ESP_ERR_NOT_SUPPORTED if the history max length is zero
 *          - ESP_ERR_NO_MEM if we ran out of memory
 *          - ESP_INVALID_ARG if the handle is invalid, or line is NULL
 */
esp_err_t  esp_linenoise_history_add(esp_linenoise_handle_t handle, const char *line);

/**
 * @brief Set the maximum length for the history. This function can be called even
 * if there is already some history, the function will make sure to retain
 * just the latest 'len' elements if the new history length value is smaller
 * than the amount of items already inside the history.
 * 
 * @returns - ESP_OK on success
 *          - ESP_ERR_NO_MEM if we ran out of memory
 *          - ESP_INVALID_ARG if the handle is invalid, or length < 1
 */
esp_err_t  esp_linenoise_history_set_max_length(esp_linenoise_handle_t handle, int length);

/**
 * @brief Save the history in the specified file.
 * 
 * @returns - ESP_OK on success
 *          - ESP_INVALID_ARG if the handle is invalid, or filename is NULL
 */
esp_err_t  esp_linenoise_history_save(esp_linenoise_handle_t handle, const char *filename);

/**
 * @brief Load the history from the specified file.
 * 
 * @returns - ESP_OK on success
 *          - ESP_FAIL if the file does not exist
 *          - ESP_ERR_NO_MEM if we ran out of memory
 *          - ESP_INVALID_ARG if the handle is invalid, or filename is NULL
 */
esp_err_t  esp_linenoise_history_load(esp_linenoise_handle_t handle, const char *filename);

/**
 * @brief Free all history.
 * 
 * @returns - ESP_OK on success
 *          - ESP_INVALID_ARG if the handle is invalid
 */
esp_err_t  esp_linenoise_history_free(esp_linenoise_handle_t handle);


/**
 * @brief Linenoise does not necessarily allocate with malloc. Any memory returned by
 * linenoise should be freed with esp_linenoise_free() in order to make sure 
 * it is freed by the same allocator as esp_linenoise.
 * 
 * @note Use this function to return the string returned by esp_linenoise()
 * 
 * @returns - ESP_OK on success
 */
esp_err_t  esp_linenoise_free(void *ptr);

/**
 * @brief Free a linenoise instance
 * 
 * @returns - ESP_OK on success
 *          - ESP_INVALID_ARG if the handle is invalid
 */
esp_err_t  esp_linenoise_destroy(esp_linenoise_handle_t handle);

#ifdef __cplusplus
}
#endif

