/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "../esp_linenoise.h"
#include "linenoise/linenoise.h"

////////////////////////////////////////////////////////////////////
// Linenoise Compatability
//

void linenoiseSetCompletionCallback(linenoiseCompletionCallback *cb) {
    esp_linenoise_set_completions_callback(0, cb);
}

void linenoiseSetHintsCallback(linenoiseHintsCallback *cb) {
    esp_linenoise_set_hints_callback(0, cb);
}

void linenoiseSetFreeHintsCallback(linenoiseFreeHintsCallback *cb) {
    esp_linenoise_set_free_hints_callback(0, cb);
}

void linenoiseAddCompletion(linenoiseCompletions *cmpl, const char * str) {
    esp_linenoise_add_completion(cmpl, str);
}

char* linenoise(const char *prompt) {
    return esp_linenoise(0, prompt);
}

int linenoiseProbe(void) {
    return esp_linenoise_probe(0) == ESP_OK ? 0 : -1;
}

void linenoiseFree(void *ptr) {
    esp_linenoise_free(ptr);
}

int linenoiseHistoryAdd(const char *line) {
    return esp_linenoise_history_add(0, line) == ESP_OK ? 1 : 0;
}

int linenoiseHistorySetMaxLen(int len) {
    return esp_linenoise_history_set_max_length(0, len) == ESP_OK ? 1 : 0;
}

int linenoiseHistorySave(const char *filename) {
    return esp_linenoise_history_save(0, filename) == ESP_OK ? 0 : -1;
}

int linenoiseHistoryLoad(const char *filename) {
    return esp_linenoise_history_load(0, filename) == ESP_OK ? 0 : -1;
}

void linenoiseHistoryFree(void) {
    esp_linenoise_history_free(0);
}

void linenoiseClearScreen(void) {
    esp_linenoise_clear_screen(0);
}

void linenoiseSetMultiLine(int mlmode) {
    esp_linenoise_set_multiline(0, mlmode);
}

void linenoiseSetDumbMode(int dumbmode) {
    esp_linenoise_set_dumb_mode(0, dumbmode);
}

bool linenoiseIsDumbMode(void) {
    bool dumb = false; 
    esp_linenoise_is_dumb_mode(0, &dumb);
    return dumb;
}

void linenoiseAllowEmpty(bool allow) {
    esp_linenoise_set_allow_empty(0, allow);
}

int linenoiseSetMaxLineLen(size_t len) {
    return esp_linenoise_set_max_line_length(0, len) == ESP_OK ? 0 : -1;
}