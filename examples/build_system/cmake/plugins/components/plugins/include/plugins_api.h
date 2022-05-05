/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* This stucture describes the plugin to the rest of the application */
typedef struct {
    /* A pointer to the plugin name */
    const char* name;

    /* A function which the plugin provides to the application.
     * In this example, this function prints something to the console
     * depending on the value of the argument 'arg'.
     */
    void (*greet)(const char* arg);
} example_plugin_desc_t;

/**
 * @brief Register the plugin with the application
 * This function is called from each plugin's "constructor" function.
 * It adds the plugin to the list.
 * @param plugin_desc  Pointer to the structure which describes the given plugin.
 */
void example_plugin_register(const example_plugin_desc_t* plugin_desc);

/**
 * @brief Print the list of registered plugins to the console.
 * This function is called from the application.
 */
void example_plugins_list(void);

/**
 * @brief Invoke 'greet' function of each registered plugin with the given argument.
 * This function is called from the application.
 * @param arg  argument to pass to plugins' greet functions.
 */
void example_plugins_greet(const char* arg);


#ifdef __cplusplus
}
#endif
