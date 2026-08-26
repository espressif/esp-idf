/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include "plugins_api.h"

/**
 * This is an example function implemented by the plugin.
 */
static void plugin_hello_greet(const char* arg)
{
    if (arg == NULL) {
        return;
    }
    printf("Hello, %s!\n", arg);
}

/* The code below demonstates both static and dynamic registration approaches. */

/**
 * Static registration of this plugin can be achieved by defining the plugin description
 * structure and placing it into .plugins_desc section.
 * The name of the section and its placement is determined by linker.lf file in 'plugins' component.
 */
static const example_plugin_desc_t __attribute__((section(".plugins_desc"),used)) PLUGIN = {
    .name = "Hello",
    .greet = &plugin_hello_greet
};

/**
 * Dynamic registration of this plugin can be achieved by calling plugin registration function
 * ('example_plugin_register') from a "constructor" function. Constructor function is called automatically
 * during application startup.
 */
static void __attribute__((constructor)) plugin_hello_self_register(void)
{
    printf("Hello plugin performing self-registration...\n");
    example_plugin_register(&(example_plugin_desc_t){
        .name = "Hello",
        .greet = &plugin_hello_greet
    });
}
