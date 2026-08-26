/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <assert.h>
#include "sdkconfig.h"
#include "esp_stdio_linux.h"

static struct termios s_orig_termios;

/**
 * This function restores the original terminal settings.
 */
static void disable_raw_mode(void)
{
    assert(tcsetattr(STDIN_FILENO, TCSAFLUSH, &s_orig_termios) == 0);
}

void linux_vfs_dev_port_deinit(linux_port_config_t *config)
{
    (void)config;
}

esp_err_t linux_vfs_dev_port_init(linux_port_config_t *config)
{
    (void)config;

    // Set stdin to unbuffered
    setvbuf(stdin, NULL, _IONBF, 0);

    const int stdin_fileno = fileno(stdin);

    if (isatty(stdin_fileno)) {
        // Use Termios driver to activate CR-NL translation and deactivate echo and canonical mode
        assert(tcgetattr(stdin_fileno, &s_orig_termios) == 0);
        struct termios raw = s_orig_termios;
        raw.c_iflag |= ICRNL; // we translate to NL because linenoise expects NL
        raw.c_lflag &= ~(ECHO | ICANON); // turn off echo and canonical mode
        assert(tcsetattr(stdin_fileno, TCSAFLUSH, &raw) == 0);

        // Make sure user does not end up with a broken terminal
        assert(atexit(disable_raw_mode) == 0);
    } else {
        // Flush input
        assert(fflush(stdin) == 0);
    }

    return ESP_OK;
}
