/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @cond */
// ANSI Color Codes:
// Macros for defining foreground colors (text).
#define LOG_ANSI_COLOR_BLACK                                        "30"
#define LOG_ANSI_COLOR_RED                                          "31"
#define LOG_ANSI_COLOR_GREEN                                        "32"
#define LOG_ANSI_COLOR_YELLOW                                       "33"
#define LOG_ANSI_COLOR_BLUE                                         "34"
#define LOG_ANSI_COLOR_MAGENTA                                      "35"
#define LOG_ANSI_COLOR_CYAN                                         "36"
#define LOG_ANSI_COLOR_WHITE                                        "37"
#define LOG_ANSI_COLOR_DEFAULT                                      "39"
// Macros for defining background colors.
#define LOG_ANSI_COLOR_BG_BLACK                                     "40"
#define LOG_ANSI_COLOR_BG_RED                                       "41"
#define LOG_ANSI_COLOR_BG_GREEN                                     "42"
#define LOG_ANSI_COLOR_BG_YELLOW                                    "43"
#define LOG_ANSI_COLOR_BG_BLUE                                      "44"
#define LOG_ANSI_COLOR_BG_MAGENTA                                   "45"
#define LOG_ANSI_COLOR_BG_CYAN                                      "46"
#define LOG_ANSI_COLOR_BG_WHITE                                     "47"
#define LOG_ANSI_COLOR_BG_DEFAULT                                   "49"
// Macros for defining text styles like bold, italic, and underline.
#define LOG_ANSI_COLOR_STYLE_RESET                                  "0"
#define LOG_ANSI_COLOR_STYLE_BOLD                                   "1"
#define LOG_ANSI_COLOR_STYLE_ITALIC                                 "3"
#define LOG_ANSI_COLOR_STYLE_UNDERLINE                              "4"
// Macros that form the starting sequence for setting the text color, background color, and reset all.
#define LOG_ANSI_COLOR(TEXT_COLOR)                                  "\033[" TEXT_COLOR "m"
#define LOG_ANSI_COLOR_BG(BG_COLOR)                                 "\033[" BG_COLOR "m"
#define LOG_ANSI_COLOR_RESET                                        "\033[" LOG_ANSI_COLOR_STYLE_RESET "m"
// Macros that form the starting sequence for text color + style + background colors
#define LOG_ANSI_COLOR_REGULAR(COLOR)                               LOG_ANSI_COLOR(LOG_ANSI_COLOR_STYLE_RESET ";" COLOR)
#define LOG_ANSI_COLOR_BOLD(COLOR)                                  LOG_ANSI_COLOR(LOG_ANSI_COLOR_STYLE_BOLD ";" COLOR)
#define LOG_ANSI_COLOR_ITALIC(COLOR)                                LOG_ANSI_COLOR(LOG_ANSI_COLOR_STYLE_ITALIC ";" COLOR)
#define LOG_ANSI_COLOR_UNDERLINE(COLOR)                             LOG_ANSI_COLOR(LOG_ANSI_COLOR_STYLE_UNDERLINE ";" COLOR)
#define LOG_ANSI_COLOR_BACKGROUND(TEXT_COLOR, BG_COLOR)             LOG_ANSI_COLOR(TEXT_COLOR ";" BG_COLOR)
#define LOG_ANSI_COLOR_REGULAR_BACKGROUND(TEXT_COLOR, BG_COLOR)     LOG_ANSI_COLOR_REGULAR(TEXT_COLOR ";" BG_COLOR)
#define LOG_ANSI_COLOR_BOLD_BACKGROUND(TEXT_COLOR, BG_COLOR)        LOG_ANSI_COLOR_BOLD(TEXT_COLOR ";" BG_COLOR)
#define LOG_ANSI_COLOR_ITALIC_BACKGROUND(TEXT_COLOR, BG_COLOR)      LOG_ANSI_COLOR_ITALIC(TEXT_COLOR ";" BG_COLOR)
#define LOG_ANSI_COLOR_UNDERLINE_BACKGROUND(TEXT_COLOR, BG_COLOR)   LOG_ANSI_COLOR_UNDERLINE(TEXT_COLOR ";" BG_COLOR)
#define LOG_ANSI_COLOR_FORMAT(TEXT_STYLE, TEXT_COLOR, BG_COLOR)     LOG_ANSI_COLOR(TEXT_STYLE ";" TEXT_COLOR ";" BG_COLOR)

/**
 * Usage example of ANSI color for logs:
 *
 * The text is yellow and no style.
 * printf(LOG_ANSI_COLOR(LOG_ANSI_COLOR_YELLOW) "%s" LOG_ANSI_COLOR_RESET "\n", text_str);
 *
 * The text is red and no style on a green background.
 * printf(LOG_ANSI_COLOR_REGULAR_BACKGROUND(LOG_ANSI_COLOR_RED, LOG_ANSI_COLOR_BG_GREEN) "%s" LOG_ANSI_COLOR_RESET "\n", text_str);
 *
 * Note that if LOG_ANSI_COLOR_RESET is not sent, the settings are retained until the following setting.
 *
 * The LOG_ANSI_COLOR_FORMAT macro is more flexible than others. It does not use the reset command, which means that
 * this macro can combine the style with the previous settings.
 * The text is white and bold on a blue background.
 * printf(LOG_ANSI_COLOR_FORMAT(LOG_ANSI_COLOR_STYLE_BOLD, LOG_ANSI_COLOR_WHITE, LOG_ANSI_COLOR_BG_BLUE) "%s" LOG_ANSI_COLOR_RESET "\n", text_str);
 */

#if (!BOOTLOADER_BUILD && CONFIG_LOG_COLORS) || (BOOTLOADER_BUILD && CONFIG_BOOTLOADER_LOG_COLORS)
#define LOG_COLOR_BLACK       LOG_ANSI_COLOR_BLACK
#define LOG_COLOR_RED         LOG_ANSI_COLOR_RED
#define LOG_COLOR_GREEN       LOG_ANSI_COLOR_GREEN
#define LOG_COLOR_BROWN       LOG_ANSI_COLOR_YELLOW
#define LOG_COLOR_YELLOW      LOG_ANSI_COLOR_YELLOW
#define LOG_COLOR_BLUE        LOG_ANSI_COLOR_BLUE
#define LOG_COLOR_PURPLE      LOG_ANSI_COLOR_MAGENTA
#define LOG_COLOR_MAGENTA     LOG_ANSI_COLOR_MAGENTA
#define LOG_COLOR_CYAN        LOG_ANSI_COLOR_CYAN
#define LOG_COLOR_WHITE       LOG_ANSI_COLOR_WHITE
#define LOG_COLOR(COLOR)      LOG_ANSI_COLOR_REGULAR(COLOR)
#define LOG_BOLD(COLOR)       LOG_ANSI_COLOR_BOLD(COLOR)
#define LOG_ITALIC(COLOR)     LOG_ANSI_COLOR_ITALIC(COLOR)
#define LOG_UNDERLINE(COLOR)  LOG_ANSI_COLOR_UNDERLINE(COLOR)
#define LOG_RESET_COLOR       LOG_ANSI_COLOR_RESET
#define LOG_COLOR_E           LOG_ANSI_COLOR_REGULAR(LOG_COLOR_RED)
#define LOG_COLOR_W           LOG_ANSI_COLOR_REGULAR(LOG_COLOR_YELLOW)
#define LOG_COLOR_I           LOG_ANSI_COLOR_REGULAR(LOG_COLOR_GREEN)
#define LOG_COLOR_D           ""
#define LOG_COLOR_V           ""
#else
#define LOG_RESET_COLOR       ""
#define LOG_COLOR_E           ""
#define LOG_COLOR_W           ""
#define LOG_COLOR_I           ""
#define LOG_COLOR_D           ""
#define LOG_COLOR_V           ""
#endif
/** @endcond */

#ifdef __cplusplus
}
#endif
