/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

extern void bootloader_extra_dir_function(void);

/* Function used to tell the linker to include this file
 * with all its symbols.
 */
void bootloader_hooks_include(void){
}

void bootloader_after_init(void) {
    bootloader_extra_dir_function();
}
