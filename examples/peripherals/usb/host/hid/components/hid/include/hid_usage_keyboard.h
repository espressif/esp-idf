/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------ HID usage keys ---------------------------------------------------------------
/**
 * @brief HID Keys
 *
 */
typedef enum {
    HID_KEY_NO_PRESS                                                              = 0x00,
    HID_KEY_ROLLOVER                                                              = 0x01,
    HID_KEY_POST_FAIL                                                             = 0x02,
    HID_KEY_ERROR_UNDEFINED                                                       = 0x03,
    HID_KEY_A                                                                     = 0x04,
    HID_KEY_B                                                                     = 0x05,
    HID_KEY_C                                                                     = 0x06,
    HID_KEY_D                                                                     = 0x07,
    HID_KEY_E                                                                     = 0x08,
    HID_KEY_F                                                                     = 0x09,
    HID_KEY_G                                                                     = 0x0A,
    HID_KEY_H                                                                     = 0x0B,
    HID_KEY_I                                                                     = 0x0C,
    HID_KEY_J                                                                     = 0x0D,
    HID_KEY_K                                                                     = 0x0E,
    HID_KEY_L                                                                     = 0x0F,
    HID_KEY_M                                                                     = 0x10,
    HID_KEY_N                                                                     = 0x11,
    HID_KEY_O                                                                     = 0x12,
    HID_KEY_P                                                                     = 0x13,
    HID_KEY_Q                                                                     = 0x14,
    HID_KEY_R                                                                     = 0x15,
    HID_KEY_S                                                                     = 0x16,
    HID_KEY_T                                                                     = 0x17,
    HID_KEY_U                                                                     = 0x18,
    HID_KEY_V                                                                     = 0x19,
    HID_KEY_W                                                                     = 0x1A,
    HID_KEY_X                                                                     = 0x1B,
    HID_KEY_Y                                                                     = 0x1C,
    HID_KEY_Z                                                                     = 0x1D,
    HID_KEY_1                                                                     = 0x1E,
    HID_KEY_2                                                                     = 0x1F,
    HID_KEY_3                                                                     = 0x20,
    HID_KEY_4                                                                     = 0x21,
    HID_KEY_5                                                                     = 0x22,
    HID_KEY_6                                                                     = 0x23,
    HID_KEY_7                                                                     = 0x24,
    HID_KEY_8                                                                     = 0x25,
    HID_KEY_9                                                                     = 0x26,
    HID_KEY_0                                                                     = 0x27,
    HID_KEY_ENTER                                                                 = 0x28,
    HID_KEY_ESC                                                                   = 0x29,
    HID_KEY_DEL                                                                   = 0x2A,
    HID_KEY_TAB                                                                   = 0x2B,
    HID_KEY_SPACE                                                                 = 0x2C,
    HID_KEY_MINUS                                                                 = 0x2D,
    HID_KEY_EQUAL                                                                 = 0x2E,
    HID_KEY_OPEN_BRACKET                                                          = 0x2F,
    HID_KEY_CLOSE_BRACKET                                                         = 0x30,
    HID_KEY_BACK_SLASH                                                            = 0x31,
    HID_KEY_SHARP                                                                 = 0x32,
    HID_KEY_COLON                                                                 = 0x33,
    HID_KEY_QUOTE                                                                 = 0x34,
    HID_KEY_TILDE                                                                 = 0x35,
    HID_KEY_LESS                                                                  = 0x36,
    HID_KEY_GREATER                                                               = 0x37,
    HID_KEY_SLASH                                                                 = 0x38,
    HID_KEY_CAPS_LOCK                                                             = 0x39,
    HID_KEY_F1                                                                    = 0x3A,
    HID_KEY_F2                                                                    = 0x3B,
    HID_KEY_F3                                                                    = 0x3C,
    HID_KEY_F4                                                                    = 0x3D,
    HID_KEY_F5                                                                    = 0x3E,
    HID_KEY_F6                                                                    = 0x3F,
    HID_KEY_F7                                                                    = 0x40,
    HID_KEY_F8                                                                    = 0x41,
    HID_KEY_F9                                                                    = 0x42,
    HID_KEY_F10                                                                   = 0x43,
    HID_KEY_F11                                                                   = 0x44,
    HID_KEY_F12                                                                   = 0x45,
    HID_KEY_PRINT_SCREEN                                                          = 0x46,
    HID_KEY_SCROLL_LOCK                                                           = 0x47,
    HID_KEY_PAUSE                                                                 = 0x48,
    HID_KEY_INSERT                                                                = 0x49,
    HID_KEY_HOME                                                                  = 0x4A,
    HID_KEY_PAGEUP                                                                = 0x4B,
    HID_KEY_DELETE                                                                = 0x4C,
    HID_KEY_END                                                                   = 0x4D,
    HID_KEY_PAGEDOWN                                                              = 0x4E,
    HID_KEY_RIGHT                                                                 = 0x4F,
    HID_KEY_LEFT                                                                  = 0x50,
    HID_KEY_DOWN                                                                  = 0x51,
    HID_KEY_UP                                                                    = 0x52,
    HID_KEY_NUM_LOCK                                                              = 0x53,
    HID_KEY_KEYPAD_DIV                                                            = 0x54,
    HID_KEY_KEYPAD_MUL                                                            = 0x55,
    HID_KEY_KEYPAD_SUB                                                            = 0x56,
    HID_KEY_KEYPAD_ADD                                                            = 0x57,
    HID_KEY_KEYPAD_ENTER                                                          = 0x58,
    HID_KEY_KEYPAD_1                                                              = 0x59,
    HID_KEY_KEYPAD_2                                                              = 0x5A,
    HID_KEY_KEYPAD_3                                                              = 0x5B,
    HID_KEY_KEYPAD_4                                                              = 0x5C,
    HID_KEY_KEYPAD_5                                                              = 0x5D,
    HID_KEY_KEYPAD_6                                                              = 0x5E,
    HID_KEY_KEYPAD_7                                                              = 0x5F,
    HID_KEY_KEYPAD_8                                                              = 0x60,
    HID_KEY_KEYPAD_9                                                              = 0x61,
    HID_KEY_KEYPAD_0                                                              = 0x62,
    HID_KEY_KEYPAD_DELETE                                                         = 0x63,
    HID_KEY_KEYPAD_SLASH                                                          = 0x64,
    HID_KEY_APPLICATION                                                           = 0x65,
    HID_KEY_POWER                                                                 = 0x66,
    HID_KEY_KEYPAD_EQUAL                                                          = 0x67,
    HID_KEY_F13                                                                   = 0x68,
    HID_KEY_F14                                                                   = 0x69,
    HID_KEY_F15                                                                   = 0x6A,
    HID_KEY_F16                                                                   = 0x6B,
    HID_KEY_F17                                                                   = 0x6C,
    HID_KEY_F18                                                                   = 0x6D,
    HID_KEY_F19                                                                   = 0x6E,
    HID_KEY_F20                                                                   = 0x6F,
    HID_KEY_F21                                                                   = 0x70,
    HID_KEY_F22                                                                   = 0x71,
    HID_KEY_F23                                                                   = 0x72,
    HID_KEY_F24                                                                   = 0x73,
    HID_KEY_EXECUTE                                                               = 0x74,
    HID_KEY_HELP                                                                  = 0x75,
    HID_KEY_MENU                                                                  = 0x76,
    HID_KEY_SELECT                                                                = 0x77,
    HID_KEY_STOP                                                                  = 0x78,
    HID_KEY_AGAIN                                                                 = 0x79,
    HID_KEY_UNDO                                                                  = 0x7A,
    HID_KEY_CUT                                                                   = 0x7B,
    HID_KEY_COPY                                                                  = 0x7C,
    HID_KEY_PASTE                                                                 = 0x7D,
    HID_KEY_FIND                                                                  = 0x7E,
    HID_KEY_MUTE                                                                  = 0x7F,
    HID_KEY_VOLUME_UP                                                             = 0x80,
    HID_KEY_VOLUME_DOWN                                                           = 0x81,
    HID_KEY_LOCKING_CAPS_LOCK                                                     = 0x82,
    HID_KEY_LOCKING_NUM_LOCK                                                      = 0x83,
    HID_KEY_LOCKING_SCROLL_LOCK                                                   = 0x84,
    HID_KEY_KEYPAD_COMMA                                                          = 0x85,
    HID_KEY_KEYPAD_EQUAL_SIGN                                                     = 0x86,
    HID_KEY_INTERNATIONAL_1                                                       = 0x87,
    HID_KEY_INTERNATIONAL_2                                                       = 0x88,
    HID_KEY_INTERNATIONAL_3                                                       = 0x89,
    HID_KEY_INTERNATIONAL_4                                                       = 0x8A,
    HID_KEY_INTERNATIONAL_5                                                       = 0x8B,
    HID_KEY_INTERNATIONAL_6                                                       = 0x8C,
    HID_KEY_INTERNATIONAL_7                                                       = 0x8D,
    HID_KEY_INTERNATIONAL_8                                                       = 0x8E,
    HID_KEY_INTERNATIONAL_9                                                       = 0x8F,
    HID_KEY_LANG_1                                                                = 0x90,
    HID_KEY_LANG_2                                                                = 0x91,
    HID_KEY_LANG_3                                                                = 0x92,
    HID_KEY_LANG_4                                                                = 0x93,
    HID_KEY_LANG_5                                                                = 0x94,
    HID_KEY_LANG_6                                                                = 0x95,
    HID_KEY_LANG_7                                                                = 0x96,
    HID_KEY_LANG_8                                                                = 0x97,
    HID_KEY_LANG_9                                                                = 0x98,
    HID_KEY_ALTERNATE_ERASE                                                       = 0x99,
    HID_KEY_SYSREQ                                                                = 0x9A,
    HID_KEY_CANCEL                                                                = 0x9B,
    HID_KEY_CLEAR                                                                 = 0x9C,
    HID_KEY_PRIOR                                                                 = 0x9D,
    HID_KEY_RETURN                                                                = 0x9E,
    HID_KEY_SEPARATOR                                                             = 0x9F,
    HID_KEY_OUT                                                                   = 0xA0,
    HID_KEY_OPER                                                                  = 0xA1,
    HID_KEY_CLEAR_AGAIN                                                           = 0xA2,
    HID_KEY_CRSEL                                                                 = 0xA3,
    HID_KEY_EXSEL                                                                 = 0xA4,
    HID_KEY_KEYPAD_00                                                             = 0xB0,
    HID_KEY_KEYPAD_000                                                            = 0xB1,
    HID_KEY_THOUSANDS_SEPARATOR                                                   = 0xB2,
    HID_KEY_DECIMAL_SEPARATOR                                                     = 0xB3,
    HID_KEY_CURRENCY_UNIT                                                         = 0xB4,
    HID_KEY_CURRENCY_SUB_UNIT                                                     = 0xB5,
    HID_KEY_KEYPAD_OPEN_PARENTHESIS                                               = 0xB6,
    HID_KEY_KEYPAD_CLOSE_PARENTHESIS                                              = 0xB7,
    HID_KEY_KEYPAD_OPEN_BRACE                                                     = 0xB8,
    HID_KEY_KEYPAD_CLOSE_BRACE                                                    = 0xB9,
    HID_KEY_KEYPAD_TAB                                                            = 0xBA,
    HID_KEY_KEYPAD_BACKSPACE                                                      = 0xBB,
    HID_KEY_KEYPAD_A                                                              = 0xBC,
    HID_KEY_KEYPAD_B                                                              = 0xBD,
    HID_KEY_KEYPAD_C                                                              = 0xBE,
    HID_KEY_KEYPAD_D                                                              = 0xBF,
    HID_KEY_KEYPAD_E                                                              = 0xC0,
    HID_KEY_KEYPAD_F                                                              = 0xC1,
    HID_KEY_KEYPAD_XOR                                                            = 0xC2,
    HID_KEY_KEYPAD_CARET                                                          = 0xC3,
    HID_KEY_KEYPAD_PERCENT                                                        = 0xC4,
    HID_KEY_KEYPAD_LESSER                                                         = 0xC5,
    HID_KEY_KEYPAD_GREATER                                                        = 0xC6,
    HID_KEY_KEYPAD_AND                                                            = 0xC7,
    HID_KEY_KEYPAD_LOGICAL_AND                                                    = 0xC8,
    HID_KEY_KEYPAD_OR                                                             = 0xC9,
    HID_KEY_KEYPAD_LOGICAL_OR                                                     = 0xCA,
    HID_KEY_KEYPAD_COLON                                                          = 0xCB,
    HID_KEY_KEYPAD_SHARP                                                          = 0xCC,
    HID_KEY_KEYPAD_SPACE                                                          = 0xCD,
    HID_KEY_KEYPAD_AT                                                             = 0xCE,
    HID_KEY_KEYPAD_BANG                                                           = 0xCF,
    HID_KEY_KEYPAD_MEMORY_STORE                                                   = 0xD0,
    HID_KEY_KEYPAD_MEMORY_RECALL                                                  = 0xD1,
    HID_KEY_KEYPAD_MEMORY_CLEAD                                                   = 0xD2,
    HID_KEY_KEYPAD_MEMORY_ADD                                                     = 0xD3,
    HID_KEY_KEYPAD_MEMORY_SUBSTRACT                                               = 0xD4,
    HID_KEY_KEYPAD_MEMORY_MULTIPLY                                                = 0xD5,
    HID_KEY_KEYPAD_MEMORY_DIVIDE                                                  = 0xD6,
    HID_KEY_KEYPAD_SIGN                                                           = 0xD7,
    HID_KEY_KEYPAD_CLEAR                                                          = 0xD8,
    HID_KEY_KEYPAD_CLEAR_ENTRY                                                    = 0xD9,
    HID_KEY_KEYPAD_BINARY                                                         = 0xDA,
    HID_KEY_KEYPAD_OCTAL                                                          = 0xDB,
    HID_KEY_KEYPAD_DECIMAL                                                        = 0xDC,
    HID_KEY_KEYPAD_HEXADECIMAL                                                    = 0xDD,
    HID_KEY_LEFT_CONTROL                                                          = 0xE0,
    HID_KEY_LEFT_SHIFT                                                            = 0xE1,
    HID_KEY_LEFT_ALT                                                              = 0xE2,
    HID_KEY_LEFT_GUI                                                              = 0xE3,
    HID_KEY_RIGHT_CONTROL                                                         = 0xE0,
    HID_KEY_RIGHT_SHIFT                                                           = 0xE1,
    HID_KEY_RIGHT_ALT                                                             = 0xE2,
    HID_KEY_RIGHT_GUI                                                             = 0xE3
} __attribute__((packed)) hid_key_t;

// Modifier bit mask
#define HID_LEFT_CONTROL                                                                (1 << 0)
#define HID_LEFT_SHIFT                                                                  (1 << 1)
#define HID_LEFT_ALT                                                                    (1 << 2)
#define HID_LEFT_GUI                                                                    (1 << 3)
#define HID_RIGHT_CONTROL                                                               (1 << 4)
#define HID_RIGHT_SHIFT                                                                 (1 << 5)
#define HID_RIGHT_ALT                                                                   (1 << 6)
#define HID_RIGHT_GUI                                                                   (1 << 7)

/**
 * @brief HID Keyboard Key number for Boot Interface
 *
 * @see B.1, p.60 of Device Class Definition for Human Interface Devices (HID) Version 1.11
 */
typedef enum {
    HID_KEYBOARD_KEY_NUMBER0 = 0,
    HID_KEYBOARD_KEY_NUMBER1,
    HID_KEYBOARD_KEY_NUMBER2,
    HID_KEYBOARD_KEY_NUMBER3,
    HID_KEYBOARD_KEY_NUMBER4,
    HID_KEYBOARD_KEY_NUMBER5,
    HID_KEYBOARD_KEY_MAX,
} hid_keyboard_key_number_t;

/**
 * @brief HID Keyboard Input Report for Boot Interfaces
 *
 * @see B.1, p.60 of Device Class Definition for Human Interface Devices (HID) Version 1.11
 */
typedef struct {
    union {
        struct {
            uint8_t left_ctr:    1;
            uint8_t left_shift:  1;
            uint8_t left_alt:    1;
            uint8_t left_gui:    1;
            uint8_t rigth_ctr:   1;
            uint8_t right_shift: 1;
            uint8_t right_alt:   1;
            uint8_t right_gui:   1;
        };
        uint8_t val;
    } modifier;
    uint8_t reserved;
    uint8_t key[HID_KEYBOARD_KEY_MAX];
} __attribute__((packed)) hid_keyboard_input_report_boot_t;

#ifdef __cplusplus
}
#endif //__cplusplus
