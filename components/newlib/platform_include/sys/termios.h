/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
//
// This header file is based on the termios header of
// "The Single UNIX (r) Specification, Version 2, Copyright (c)  1997 The Open Group".

#ifndef __ESP_SYS_TERMIOS_H__
#define __ESP_SYS_TERMIOS_H__

// ESP-IDF NOTE: This header provides only a compatibility layer for macros and functions defined in sys/termios.h.
// Not everything has a defined meaning for ESP-IDF (e.g. process leader IDs) and therefore are likely to be stubbed
// in actual implementations.


#include <stdint.h>
#include <sys/types.h>
#include "sdkconfig.h"

#ifdef CONFIG_VFS_SUPPORT_TERMIOS

// subscripts for the array c_cc:
#define VEOF                        0           /** EOF character */
#define VEOL                        1           /** EOL character */
#define VERASE                      2           /** ERASE character */
#define VINTR                       3           /** INTR character */
#define VKILL                       4           /** KILL character */
#define VMIN                        5           /** MIN value */
#define VQUIT                       6           /** QUIT character */
#define VSTART                      7           /** START character */
#define VSTOP                       8           /** STOP character */
#define VSUSP                       9           /** SUSP character */
#define VTIME                      10           /** TIME value */
#define NCCS                       (VTIME + 1)  /** Size of the array c_cc for control characters */

// input modes for use as flags in the c_iflag field
#define BRKINT      (1u << 0)       /** Signal interrupt on break. */
#define ICRNL       (1u << 1)       /** Map CR to NL on input. */
#define IGNBRK      (1u << 2)       /** Ignore break condition. */
#define IGNCR       (1u << 3)       /** Ignore CR. */
#define IGNPAR      (1u << 4)       /** Ignore characters with parity errors. */
#define INLCR       (1u << 5)       /** Map NL to CR on input. */
#define INPCK       (1u << 6)       /** Enable input parity check. */
#define ISTRIP      (1u << 7)       /** Strip character. */
#define IUCLC       (1u << 8)       /** Map upper-case to lower-case on input (LEGACY). */
#define IXANY       (1u << 9)       /** Enable any character to restart output. */
#define IXOFF       (1u << 10)      /** Enable start/stop input control. */
#define IXON        (1u << 11)      /** Enable start/stop output control. */
#define PARMRK      (1u << 12)      /** Mark parity errors. */

// output Modes for use as flags in the c_oflag field
#define OPOST       (1u <<  0)       /** Post-process output */
#define OLCUC       (1u <<  1)       /** Map lower-case to upper-case on output (LEGACY). */
#define ONLCR       (1u <<  2)       /** Map NL to CR-NL on output. */
#define OCRNL       (1u <<  3)       /** Map CR to NL on output. */
#define ONOCR       (1u <<  4)       /** No CR output at column 0. */
#define ONLRET      (1u <<  5)       /** NL performs CR function. */
#define OFILL       (1u <<  6)       /** Use fill characters for delay. */
#define NLDLY       (1u <<  7)       /** Select newline delays: */
#define   NL0       (0u <<  7)       /** Newline character type 0. */
#define   NL1       (1u <<  7)       /** Newline character type 1. */
#define CRDLY       (3u <<  8)       /** Select carriage-return delays: */
#define   CR0       (0u <<  8)       /** Carriage-return delay type 0. */
#define   CR1       (1u <<  8)       /** Carriage-return delay type 1. */
#define   CR2       (2u <<  8)       /** Carriage-return delay type 2. */
#define   CR3       (3u <<  8)       /** Carriage-return delay type 3. */
#define TABDLY      (3u << 10)       /** Select horizontal-tab delays: */
#define   TAB0      (0u << 10)       /** Horizontal-tab delay type 0. */
#define   TAB1      (1u << 10)       /** Horizontal-tab delay type 1. */
#define   TAB2      (2u << 10)       /** Horizontal-tab delay type 2. */
#define   TAB3      (3u << 10)       /** Expand tabs to spaces. */
#define BSDLY       (1u << 12)       /** Select backspace delays: */
#define   BS0       (0u << 12)       /** Backspace-delay type 0. */
#define   BS1       (1u << 12)       /** Backspace-delay type 1. */
#define VTDLY       (1u << 13)       /** Select vertical-tab delays: */
#define   VT0       (0u << 13)       /** Vertical-tab delay type 0. */
#define   VT1       (1u << 13)       /** Vertical-tab delay type 1. */
#define FFDLY       (1u << 14)       /** Select form-feed delays: */
#define   FF0       (0u << 14)       /** Form-feed delay type 0. */
#define   FF1       (1u << 14)       /** Form-feed delay type 1. */

// Baud Rate Selection. Valid values for objects of type speed_t:
// CBAUD range B0 - B38400
#define B0          0   /** Hang up */
#define B50         1
#define B75         2
#define B110        3
#define B134        4
#define B150        5
#define B200        6
#define B300        7
#define B600        8
#define B1200       9
#define B1800      10
#define B2400      11
#define B4800      12
#define B9600      13
#define B19200     14
#define B38400     15
// CBAUDEX range B57600 - B4000000
#define B57600     16
#define B115200    17
#define B230400    18
#define B460800    19
#define B500000    20
#define B576000    21
#define B921600    22
#define B1000000   23
#define B1152000   24
#define B1500000   25
#define B2000000   26
#define B2500000   27
#define B3000000   28
#define B3500000   29
#define B4000000   30

// Control Modes for the c_cflag field:
#define CSIZE      (3u << 0)        /* Character size: */
#define   CS5      (0u << 0)        /** 5 bits. */
#define   CS6      (1u << 0)        /** 6 bits. */
#define   CS7      (2u << 0)        /** 7 bits. */
#define   CS8      (3u << 0)        /** 8 bits. */
#define CSTOPB     (1u << 2)        /** Send two stop bits, else one. */
#define CREAD      (1u << 3)        /** Enable receiver. */
#define PARENB     (1u << 4)        /** Parity enable. */
#define PARODD     (1u << 5)        /** Odd parity, else even. */
#define HUPCL      (1u << 6)        /** Hang up on last close. */
#define CLOCAL     (1u << 7)        /** Ignore modem status lines. */
#define CBAUD      (1u << 8)        /** Use baud rates defined by B0-B38400 macros. */
#define CBAUDEX    (1u << 9)        /** Use baud rates defined by B57600-B4000000 macros. */
#define BOTHER     (1u << 10)       /** Use custom baud rates */

// Local Modes for c_lflag field:
#define ECHO       (1u << 0)        /** Enable echo. */
#define ECHOE      (1u << 1)        /** Echo erase character as error-correcting backspace. */
#define ECHOK      (1u << 2)        /** Echo KILL. */
#define ECHONL     (1u << 3)        /** Echo NL. */
#define ICANON     (1u << 4)        /** Canonical input (erase and kill processing). */
#define IEXTEN     (1u << 5)        /** Enable extended input character processing. */
#define ISIG       (1u << 6)        /** Enable signals. */
#define NOFLSH     (1u << 7)        /** Disable flush after interrupt or quit. */
#define TOSTOP     (1u << 8)        /** Send SIGTTOU for background output. */
#define XCASE      (1u << 9)        /** Canonical upper/lower presentation (LEGACY). */

// Attribute Selection constants for use with tcsetattr():
#define TCSANOW    0                /** Change attributes immediately. */
#define TCSADRAIN  1                /** Change attributes when output has drained. */
#define TCSAFLUSH  2                /** Change attributes when output has drained; also flush pending input. */

// Line Control constants for use with tcflush():
#define TCIFLUSH    0               /** Flush pending input. Flush untransmitted output. */
#define TCIOFLUSH   1               /** Flush both pending input and untransmitted output. */
#define TCOFLUSH    2               /** Flush untransmitted output. */

// constants for use with tcflow():
#define TCIOFF      0               /** Transmit a STOP character, intended to suspend input data. */
#define TCION       1               /** Transmit a START character, intended to restart input data. */
#define TCOOFF      2               /** Suspend output. */
#define TCOON       3               /** Restart output. */

typedef uint8_t cc_t;
typedef uint32_t speed_t;
typedef uint16_t tcflag_t;

struct termios
{
    tcflag_t c_iflag;    /** Input modes */
    tcflag_t c_oflag;    /** Output modes */
    tcflag_t c_cflag;    /** Control modes */
    tcflag_t c_lflag;    /** Local modes */
    cc_t     c_cc[NCCS]; /** Control characters */
    speed_t  c_ispeed;   /** input baud rate */
    speed_t  c_ospeed;   /** output baud rate */
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Extracts the input baud rate from the input structure exactly (without interpretation).
 *
 * @param p input termios structure
 * @return input baud rate
 */
speed_t cfgetispeed(const struct termios *p);

/**
 * @brief Extracts the output baud rate from the input structure exactly (without interpretation).
 *
 * @param p input termios structure
 * @return output baud rate
 */
speed_t cfgetospeed(const struct termios *p);

/**
 * @brief Set input baud rate in the termios structure
 *
 * There is no effect in hardware until a subsequent call of tcsetattr().
 *
 * @param p input termios structure
 * @param sp input baud rate
 * @return 0 when successful, -1 otherwise with errno set
 */
int cfsetispeed(struct termios *p, speed_t sp);

/**
 * @brief Set output baud rate in the termios structure
 *
 * There is no effect in hardware until a subsequent call of tcsetattr().
 *
 * @param p input termios structure
 * @param sp output baud rate
 * @return 0 when successful, -1 otherwise with errno set
 */
int cfsetospeed(struct termios *p, speed_t sp);

/**
 * @brief Wait for transmission of output
 *
 * @param fd file descriptor of the terminal
 * @return 0 when successful, -1 otherwise with errno set
 */
int tcdrain(int fd);

/**
 * @brief Suspend or restart the transmission or reception of data
 *
 * @param fd file descriptor of the terminal
 * @param action selects actions to do
 * @return 0 when successful, -1 otherwise with errno set
 */
int tcflow(int fd, int action);

/**
 * @brief Flush non-transmitted output data and non-read input data
 *
 * @param fd file descriptor of the terminal
 * @param select selects what should be flushed
 * @return 0 when successful, -1 otherwise with errno set
 */
int tcflush(int fd, int select);

/**
 * @brief Gets the parameters of the terminal
 *
 * @param fd file descriptor of the terminal
 * @param p output termios structure
 * @return 0 when successful, -1 otherwise with errno set
 */
int tcgetattr(int fd, struct termios *p);

/**
 * @brief Get process group ID for session leader for controlling terminal
 *
 * @param fd file descriptor of the terminal
 * @return process group ID when successful, -1 otherwise with errno set
 */
pid_t tcgetsid(int fd);

/**
 * @brief Send a break for a specific duration
 *
 * @param fd file descriptor of the terminal
 * @param duration duration of break
 * @return 0 when successful, -1 otherwise with errno set
 */
int tcsendbreak(int fd, int duration);

/**
 * @brief Sets the parameters of the terminal
 *
 * @param fd file descriptor of the terminal
 * @param optional_actions optional actions
 * @param p input termios structure
 * @return 0 when successful, -1 otherwise with errno set
 */
int tcsetattr(int fd, int optional_actions, const struct termios *p);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CONFIG_VFS_SUPPORT_TERMIOS

#endif //__ESP_SYS_TERMIOS_H__
