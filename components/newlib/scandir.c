/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "esp_check.h"
#include "esp_log.h"

static const char *TAG = "scandir";

int alphasort(const struct dirent **lhs, const struct dirent **rhs)
{
    return strcoll((*lhs)->d_name, (*rhs)->d_name);
}

int scandir(const char *dirname, struct dirent ***out_dirlist,
            int (*select_func)(const struct dirent *),
            int (*cmp_func)(const struct dirent **, const struct dirent **))
{
    DIR *dir_ptr = NULL;
    struct dirent *entry;
    size_t num_entries = 0;
    size_t array_size = 8;  /* initial estimate */
    struct dirent **entries = NULL;
    int ret = -1;

    entries = malloc(array_size * sizeof(struct dirent *));
    ESP_RETURN_ON_FALSE(entries, -1, TAG, "Malloc failed for entries");

    dir_ptr = opendir(dirname);
    ESP_GOTO_ON_FALSE(dir_ptr, -1, out, TAG, "Failed to open directory: %s", dirname);

    while ((entry = readdir(dir_ptr)) != NULL) {
        /* skip entries that don't match the filter function */
        if (select_func != NULL && !select_func(entry)) {
            continue;
        }

        struct dirent *entry_copy = malloc(sizeof(struct dirent));
        ESP_GOTO_ON_FALSE(entry_copy, -1, out, TAG, "Malloc failed for entry_copy");

        *entry_copy = *entry;
        entries[num_entries++] = entry_copy;

        /* grow the array size if it's full */
        if (num_entries >= array_size) {
            array_size *= 2;
            struct dirent **new_entries = realloc(entries, array_size * sizeof(struct dirent *));
            ESP_GOTO_ON_FALSE(new_entries, -1, out, TAG, "Realloc failed for entries");
            entries = new_entries;
        }
    }

    /* sort the entries if a comparison function is provided */
    if (num_entries && cmp_func) {
        qsort(entries, num_entries, sizeof(struct dirent *),
              (int (*)(const void *, const void *))cmp_func);
    }

    *out_dirlist = entries;
    ret = num_entries;

out:
    if (ret < 0) {
        while (num_entries > 0) {
            free(entries[--num_entries]);
        }
        free(entries);
    }
    if (dir_ptr) {
        closedir(dir_ptr);
    }
    return ret;
}
