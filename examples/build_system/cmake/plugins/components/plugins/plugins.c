/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include <sys/queue.h>
#include "plugins_api.h"

/**
 * Demonstration of dynamic registration (self-registration):
 *
 * - example_plugin_register function is called from "constructor" functions of each plugin.
 *   Information about the plugin is passed inside 'example_plugin_desc_t' structure.
 *   This function adds each plugin description into linked list (s_plugins_list).
 *
 * - example_plugins_greet function iterates over the linked list.
 */

struct plugin_record {
    example_plugin_desc_t plugin_desc;
    LIST_ENTRY(plugin_record) list_entry;
};

static LIST_HEAD(plugins_list, plugin_record) s_plugins_list = LIST_HEAD_INITIALIZER(s_plugins_list);

void example_plugin_register(const example_plugin_desc_t* plugin_desc)
{
    struct plugin_record *record = (struct plugin_record *) malloc(sizeof(struct plugin_record));
    if (record == NULL) {
        abort();
    }
    memcpy(&record->plugin_desc, plugin_desc, sizeof(*plugin_desc));

    struct plugin_record *head = LIST_FIRST(&s_plugins_list);
    if (head == NULL) {
        LIST_INSERT_HEAD(&s_plugins_list, record, list_entry);
    } else {
        LIST_INSERT_BEFORE(head, record, list_entry);
    }
    printf("Successfully registered plugin '%s'\n", plugin_desc->name);
}

void example_plugins_greet(const char* arg)
{
    struct plugin_record *it;
    LIST_FOREACH(it, &s_plugins_list, list_entry) {
        printf("Calling greet function of plugin '%s'...\n", it->plugin_desc.name);
        (*it->plugin_desc.greet)(arg);
        printf("Done with greet function of plugin '%s'.\n", it->plugin_desc.name);
    }
}

/**
 * Demonstration of static registration.
 * Symbols '_plugins_array_start' and '_plugins_array_end' mark the beginning and end
 * of the array where 'example_plugin_desc_t' structures are placed by the linker.
 * The names of these variables are determined by linker.lf in 'plugins' component,
 * look for 'SURROUND(plugins_array)'.
 */

void example_plugins_list(void)
{
    printf("List of plugins:\n");
    extern const example_plugin_desc_t _plugins_array_start;
    extern const example_plugin_desc_t _plugins_array_end;
    for (const example_plugin_desc_t* it = &_plugins_array_start; it != &_plugins_array_end; ++it) {
        printf("- Plugin '%s', function greet=%p\n", it->name, it->greet);
    }
}
