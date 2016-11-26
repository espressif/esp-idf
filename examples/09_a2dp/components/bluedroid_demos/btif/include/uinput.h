/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#ifndef __UINPUT_H
#define __UINPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
// #include <sys/time.h>
// #include <sys/ioctl.h>

/*******************************************************************************
**  Constants & Macros
********************************************************************************/

/* Events */

#define EV_SYN          0x00
#define EV_KEY          0x01
#define EV_REL          0x02
#define EV_ABS          0x03
#define EV_MSC          0x04
#define EV_LED          0x11
#define EV_SND          0x12
#define EV_REP          0x14
#define EV_FF           0x15
#define EV_PWR          0x16
#define EV_FF_STATUS        0x17
#define EV_MAX          0x1f

/* Synchronization events */

#define SYN_REPORT      0
#define SYN_CONFIG      1

/* Keys and buttons */

#define KEY_RESERVED        0
#define KEY_ESC         1
#define KEY_1           2
#define KEY_2           3
#define KEY_3           4
#define KEY_4           5
#define KEY_5           6
#define KEY_6           7
#define KEY_7           8
#define KEY_8           9
#define KEY_9           10
#define KEY_0           11
#define KEY_MINUS       12
#define KEY_EQUAL       13
#define KEY_BACKSPACE       14
#define KEY_TAB         15
#define KEY_Q           16
#define KEY_W           17
#define KEY_E           18
#define KEY_R           19
#define KEY_T           20
#define KEY_Y           21
#define KEY_U           22
#define KEY_I           23
#define KEY_O           24
#define KEY_P           25
#define KEY_LEFTBRACE       26
#define KEY_RIGHTBRACE      27
#define KEY_ENTER       28
#define KEY_LEFTCTRL        29
#define KEY_A           30
#define KEY_S           31
#define KEY_D           32
#define KEY_F           33
#define KEY_G           34
#define KEY_H           35
#define KEY_J           36
#define KEY_K           37
#define KEY_L           38
#define KEY_SEMICOLON       39
#define KEY_APOSTROPHE      40
#define KEY_GRAVE       41
#define KEY_LEFTSHIFT       42
#define KEY_BACKSLASH       43
#define KEY_Z           44
#define KEY_X           45
#define KEY_C           46
#define KEY_V           47
#define KEY_B           48
#define KEY_N           49
#define KEY_M           50
#define KEY_COMMA       51
#define KEY_DOT         52
#define KEY_SLASH       53
#define KEY_RIGHTSHIFT      54
#define KEY_KPASTERISK      55
#define KEY_LEFTALT     56
#define KEY_SPACE       57
#define KEY_CAPSLOCK        58
#define KEY_F1          59
#define KEY_F2          60
#define KEY_F3          61
#define KEY_F4          62
#define KEY_F5          63
#define KEY_F6          64
#define KEY_F7          65
#define KEY_F8          66
#define KEY_F9          67
#define KEY_F10         68
#define KEY_NUMLOCK     69
#define KEY_SCROLLLOCK      70
#define KEY_KP7         71
#define KEY_KP8         72
#define KEY_KP9         73
#define KEY_KPMINUS     74
#define KEY_KP4         75
#define KEY_KP5         76
#define KEY_KP6         77
#define KEY_KPPLUS      78
#define KEY_KP1         79
#define KEY_KP2         80
#define KEY_KP3         81
#define KEY_KP0         82
#define KEY_KPDOT       83
#define KEY_103RD       84
#define KEY_F13         85
#define KEY_102ND       86
#define KEY_F11         87
#define KEY_F12         88
#define KEY_F14         89
#define KEY_F15         90
#define KEY_F16         91
#define KEY_F17         92
#define KEY_F18         93
#define KEY_F19         94
#define KEY_F20         95
#define KEY_KPENTER     96
#define KEY_RIGHTCTRL       97
#define KEY_KPSLASH     98
#define KEY_SYSRQ       99
#define KEY_RIGHTALT        100
#define KEY_LINEFEED        101
#define KEY_HOME        102
#define KEY_UP          103
#define KEY_PAGEUP      104
#define KEY_LEFT        105
#define KEY_RIGHT       106
#define KEY_END         107
#define KEY_DOWN        108
#define KEY_PAGEDOWN        109
#define KEY_INSERT      110
#define KEY_DELETE      111
#define KEY_MACRO       112
#define KEY_MUTE        113
#define KEY_VOLUMEDOWN      114
#define KEY_VOLUMEUP        115
#define KEY_POWER       116
#define KEY_KPEQUAL     117
#define KEY_KPPLUSMINUS     118
#define KEY_PAUSE       119
#define KEY_F21         120
#define KEY_F22         121
#define KEY_F23         122
#define KEY_F24         123
#define KEY_KPCOMMA     124
#define KEY_LEFTMETA        125
#define KEY_RIGHTMETA       126
#define KEY_COMPOSE     127

#define KEY_STOP        128
#define KEY_AGAIN       129
#define KEY_PROPS       130
#define KEY_UNDO        131
#define KEY_FRONT       132
#define KEY_COPY        133
#define KEY_OPEN        134
#define KEY_PASTE       135
#define KEY_FIND        136
#define KEY_CUT         137
#define KEY_HELP        138
#define KEY_MENU        139
#define KEY_CALC        140
#define KEY_SETUP       141
#define KEY_SLEEP       142
#define KEY_WAKEUP      143
#define KEY_FILE        144
#define KEY_SENDFILE        145
#define KEY_DELETEFILE      146
#define KEY_XFER        147
#define KEY_PROG1       148
#define KEY_PROG2       149
#define KEY_WWW         150
#define KEY_MSDOS       151
#define KEY_COFFEE      152
#define KEY_DIRECTION       153
#define KEY_CYCLEWINDOWS    154
#define KEY_MAIL        155
#define KEY_BOOKMARKS       156
#define KEY_COMPUTER        157
#define KEY_BACK        158
#define KEY_FORWARD     159
#define KEY_CLOSECD     160
#define KEY_EJECTCD     161
#define KEY_EJECTCLOSECD    162
#define KEY_NEXTSONG        163
#define KEY_PLAYPAUSE       164
#define KEY_PREVIOUSSONG    165
#define KEY_STOPCD      166
#define KEY_RECORD      167
#define KEY_REWIND      168
#define KEY_PHONE       169
#define KEY_ISO         170
#define KEY_CONFIG      171
#define KEY_HOMEPAGE        172
#define KEY_REFRESH     173
#define KEY_EXIT        174
#define KEY_MOVE        175
#define KEY_EDIT        176
#define KEY_SCROLLUP        177
#define KEY_SCROLLDOWN      178
#define KEY_KPLEFTPAREN     179
#define KEY_KPRIGHTPAREN    180

#define KEY_INTL1       181
#define KEY_INTL2       182
#define KEY_INTL3       183
#define KEY_INTL4       184
#define KEY_INTL5       185
#define KEY_INTL6       186
#define KEY_INTL7       187
#define KEY_INTL8       188
#define KEY_INTL9       189
#define KEY_LANG1       190
#define KEY_LANG2       191
#define KEY_LANG3       192
#define KEY_LANG4       193
#define KEY_LANG5       194
#define KEY_LANG6       195
#define KEY_LANG7       196
#define KEY_LANG8       197
#define KEY_LANG9       198

#define KEY_PLAYCD      200
#define KEY_PAUSECD     201
#define KEY_PROG3       202
#define KEY_PROG4       203
#define KEY_SUSPEND     205
#define KEY_CLOSE       206
#define KEY_PLAY        207
#define KEY_FAST_FORWARD  208

#define KEY_UNKNOWN     220

#define KEY_BRIGHTNESSDOWN  224
#define KEY_BRIGHTNESSUP    225

#define BTN_MISC        0x100
#define BTN_0           0x100
#define BTN_1           0x101
#define BTN_2           0x102
#define BTN_3           0x103
#define BTN_4           0x104
#define BTN_5           0x105
#define BTN_6           0x106
#define BTN_7           0x107
#define BTN_8           0x108
#define BTN_9           0x109

#define BTN_MOUSE       0x110
#define BTN_LEFT        0x110
#define BTN_RIGHT       0x111
#define BTN_MIDDLE      0x112
#define BTN_SIDE        0x113
#define BTN_EXTRA       0x114
#define BTN_FORWARD     0x115
#define BTN_BACK        0x116
#define BTN_TASK        0x117

#define BTN_JOYSTICK        0x120
#define BTN_TRIGGER     0x120
#define BTN_THUMB       0x121
#define BTN_THUMB2      0x122
#define BTN_TOP         0x123
#define BTN_TOP2        0x124
#define BTN_PINKIE      0x125
#define BTN_BASE        0x126
#define BTN_BASE2       0x127
#define BTN_BASE3       0x128
#define BTN_BASE4       0x129
#define BTN_BASE5       0x12a
#define BTN_BASE6       0x12b
#define BTN_DEAD        0x12f

#define BTN_GAMEPAD     0x130
#define BTN_A           0x130
#define BTN_B           0x131
#define BTN_C           0x132
#define BTN_X           0x133
#define BTN_Y           0x134
#define BTN_Z           0x135
#define BTN_TL          0x136
#define BTN_TR          0x137
#define BTN_TL2         0x138
#define BTN_TR2         0x139
#define BTN_SELECT      0x13a
#define BTN_START       0x13b
#define BTN_MODE        0x13c
#define BTN_THUMBL      0x13d
#define BTN_THUMBR      0x13e

#define BTN_DIGI        0x140
#define BTN_TOOL_PEN        0x140
#define BTN_TOOL_RUBBER     0x141
#define BTN_TOOL_BRUSH      0x142
#define BTN_TOOL_PENCIL     0x143
#define BTN_TOOL_AIRBRUSH   0x144
#define BTN_TOOL_FINGER     0x145
#define BTN_TOOL_MOUSE      0x146
#define BTN_TOOL_LENS       0x147
#define BTN_TOUCH       0x14a
#define BTN_STYLUS      0x14b
#define BTN_STYLUS2     0x14c
#define BTN_TOOL_DOUBLETAP  0x14d
#define BTN_TOOL_TRIPLETAP  0x14e

#define BTN_WHEEL       0x150
#define BTN_GEAR_DOWN       0x150
#define BTN_GEAR_UP     0x151

#define KEY_OK          0x160
#define KEY_SELECT      0x161
#define KEY_GOTO        0x162
#define KEY_CLEAR       0x163
#define KEY_POWER2      0x164
#define KEY_OPTION      0x165
#define KEY_INFO        0x166
#define KEY_TIME        0x167
#define KEY_VENDOR      0x168
#define KEY_ARCHIVE     0x169
#define KEY_PROGRAM     0x16a
#define KEY_CHANNEL     0x16b
#define KEY_FAVORITES       0x16c
#define KEY_EPG         0x16d
#define KEY_PVR         0x16e
#define KEY_MHP         0x16f
#define KEY_LANGUAGE        0x170
#define KEY_TITLE       0x171
#define KEY_SUBTITLE        0x172
#define KEY_ANGLE       0x173
#define KEY_ZOOM        0x174
#define KEY_MODE        0x175
#define KEY_KEYBOARD        0x176
#define KEY_SCREEN      0x177
#define KEY_PC          0x178
#define KEY_TV          0x179
#define KEY_TV2         0x17a
#define KEY_VCR         0x17b
#define KEY_VCR2        0x17c
#define KEY_SAT         0x17d
#define KEY_SAT2        0x17e
#define KEY_CD          0x17f
#define KEY_TAPE        0x180
#define KEY_RADIO       0x181
#define KEY_TUNER       0x182
#define KEY_PLAYER      0x183
#define KEY_TEXT        0x184
#define KEY_DVD         0x185
#define KEY_AUX         0x186
#define KEY_MP3         0x187
#define KEY_AUDIO       0x188
#define KEY_VIDEO       0x189
#define KEY_DIRECTORY       0x18a
#define KEY_LIST        0x18b
#define KEY_MEMO        0x18c
#define KEY_CALENDAR        0x18d
#define KEY_RED         0x18e
#define KEY_GREEN       0x18f
#define KEY_YELLOW      0x190
#define KEY_BLUE        0x191
#define KEY_CHANNELUP       0x192
#define KEY_CHANNELDOWN     0x193
#define KEY_FIRST       0x194
#define KEY_LAST        0x195
#define KEY_AB          0x196
#define KEY_NEXT        0x197
#define KEY_RESTART     0x198
#define KEY_SLOW        0x199
#define KEY_SHUFFLE     0x19a
#define KEY_BREAK       0x19b
#define KEY_PREVIOUS        0x19c
#define KEY_DIGITS      0x19d
#define KEY_TEEN        0x19e
#define KEY_TWEN        0x19f

#define KEY_FRAMEBACK       0x1b2
#define KEY_FRAMEFORWARD    0x1b3
#define KEY_CONTEXT_MENU    0x1fb

#define KEY_MAX         0x1ff

/* Relative axes */

#define REL_X           0x00
#define REL_Y           0x01
#define REL_Z           0x02
#define REL_RX          0x03
#define REL_RY          0x04
#define REL_RZ          0x05
#define REL_HWHEEL      0x06
#define REL_DIAL        0x07
#define REL_WHEEL       0x08
#define REL_MISC        0x09
#define REL_MAX         0x0f

/* Absolute axes */

#define ABS_X           0x00
#define ABS_Y           0x01
#define ABS_Z           0x02
#define ABS_RX          0x03
#define ABS_RY          0x04
#define ABS_RZ          0x05
#define ABS_THROTTLE        0x06
#define ABS_RUDDER      0x07
#define ABS_WHEEL       0x08
#define ABS_GAS         0x09
#define ABS_BRAKE       0x0a
#define ABS_HAT0X       0x10
#define ABS_HAT0Y       0x11
#define ABS_HAT1X       0x12
#define ABS_HAT1Y       0x13
#define ABS_HAT2X       0x14
#define ABS_HAT2Y       0x15
#define ABS_HAT3X       0x16
#define ABS_HAT3Y       0x17
#define ABS_PRESSURE        0x18
#define ABS_DISTANCE        0x19
#define ABS_TILT_X      0x1a
#define ABS_TILT_Y      0x1b
#define ABS_TOOL_WIDTH      0x1c
#define ABS_VOLUME      0x20
#define ABS_MISC        0x28
#define ABS_MAX         0x3f

/* Switch events */

#define SW_0            0x00
#define SW_1            0x01
#define SW_2            0x02
#define SW_3            0x03
#define SW_4            0x04
#define SW_5            0x05
#define SW_6            0x06
#define SW_7            0x07
#define SW_MAX          0x0f

/* Misc events */

#define MSC_SERIAL      0x00
#define MSC_PULSELED        0x01
#define MSC_GESTURE     0x02
#define MSC_RAW         0x03
#define MSC_SCAN        0x04
#define MSC_MAX         0x07

/* LEDs */

#define LED_NUML        0x00
#define LED_CAPSL       0x01
#define LED_SCROLLL     0x02
#define LED_COMPOSE     0x03
#define LED_KANA        0x04
#define LED_SLEEP       0x05
#define LED_SUSPEND     0x06
#define LED_MUTE        0x07
#define LED_MISC        0x08
#define LED_MAIL        0x09
#define LED_CHARGING        0x0a
#define LED_MAX         0x0f

/* Autorepeat values */

#define REP_DELAY       0x00
#define REP_PERIOD      0x01
#define REP_MAX         0x01

/* Sounds */

#define SND_CLICK       0x00
#define SND_BELL        0x01
#define SND_TONE        0x02
#define SND_MAX         0x07

/* Identifiers */

#define ID_BUS          0
#define ID_VENDOR       1
#define ID_PRODUCT      2
#define ID_VERSION      3

#define BUS_PCI         0x01
#define BUS_ISAPNP      0x02
#define BUS_USB         0x03
#define BUS_HIL         0x04
#define BUS_BLUETOOTH       0x05

#define BUS_ISA         0x10
#define BUS_I8042       0x11
#define BUS_XTKBD       0x12
#define BUS_RS232       0x13
#define BUS_GAMEPORT        0x14
#define BUS_PARPORT     0x15
#define BUS_AMIGA       0x16
#define BUS_ADB         0x17
#define BUS_I2C         0x18
#define BUS_HOST        0x19
#define BUS_GSC         0x1A

/* User input interface */
#define _IO(a, b)           (0)  // temporary hack
#define _IOW(a, b, c)       (1)  // temporary hack

#define UINPUT_IOCTL_BASE   'U'

#define UI_DEV_CREATE       _IO(UINPUT_IOCTL_BASE, 1)
#define UI_DEV_DESTROY      _IO(UINPUT_IOCTL_BASE, 2)

#define UI_SET_EVBIT        _IOW(UINPUT_IOCTL_BASE, 100, int)
#define UI_SET_KEYBIT       _IOW(UINPUT_IOCTL_BASE, 101, int)
#define UI_SET_RELBIT       _IOW(UINPUT_IOCTL_BASE, 102, int)
#define UI_SET_ABSBIT       _IOW(UINPUT_IOCTL_BASE, 103, int)
#define UI_SET_MSCBIT       _IOW(UINPUT_IOCTL_BASE, 104, int)
#define UI_SET_LEDBIT       _IOW(UINPUT_IOCTL_BASE, 105, int)
#define UI_SET_SNDBIT       _IOW(UINPUT_IOCTL_BASE, 106, int)
#define UI_SET_FFBIT        _IOW(UINPUT_IOCTL_BASE, 107, int)
#define UI_SET_PHYS     _IOW(UINPUT_IOCTL_BASE, 108, char*)
#define UI_SET_SWBIT        _IOW(UINPUT_IOCTL_BASE, 109, int)

#ifndef NBITS
#define NBITS(x) ((((x) - 1) / (sizeof(long) * 8)) + 1)
#endif

#define UINPUT_MAX_NAME_SIZE    80


/*******************************************************************************
**  Type definitions and return values
********************************************************************************/

struct uinput_id {
    uint16_t bustype;
    uint16_t vendor;
    uint16_t product;
    uint16_t version;
};

struct uinput_dev {
    char name[UINPUT_MAX_NAME_SIZE];
    struct uinput_id id;
    int ff_effects_max;
    int absmax[ABS_MAX + 1];
    int absmin[ABS_MAX + 1];
    int absfuzz[ABS_MAX + 1];
    int absflat[ABS_MAX + 1];
};

struct uinput_event {
    // struct timeval time;  // temporarily remove this part;
    uint16_t type;
    uint16_t code;
    int32_t value;
};

#ifdef __cplusplus
}
#endif

#endif /* __UINPUT_H */

