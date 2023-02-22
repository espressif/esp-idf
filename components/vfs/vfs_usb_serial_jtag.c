/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

//This is a simple non-blocking (well, tx may spin for a bit if the buffer
//is full) USB-serial-jtag driver. Select etc is not supported yet.

#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <sys/errno.h>
#include <sys/lock.h>
#include <sys/fcntl.h>
#include <sys/param.h>
#include "esp_timer.h"
#include "esp_vfs.h"
#include "esp_vfs_dev.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "hal/usb_serial_jtag_ll.h"
#include "esp_vfs_usb_serial_jtag.h"
#include "driver/usb_serial_jtag.h"

// Token signifying that no character is available
#define NONE -1

#if CONFIG_NEWLIB_STDOUT_LINE_ENDING_CRLF
#   define DEFAULT_TX_MODE ESP_LINE_ENDINGS_CRLF
#elif CONFIG_NEWLIB_STDOUT_LINE_ENDING_CR
#   define DEFAULT_TX_MODE ESP_LINE_ENDINGS_CR
#else
#   define DEFAULT_TX_MODE ESP_LINE_ENDINGS_LF
#endif

#if CONFIG_NEWLIB_STDIN_LINE_ENDING_CRLF
#   define DEFAULT_RX_MODE ESP_LINE_ENDINGS_CRLF
#elif CONFIG_NEWLIB_STDIN_LINE_ENDING_CR
#   define DEFAULT_RX_MODE ESP_LINE_ENDINGS_CR
#else
#   define DEFAULT_RX_MODE ESP_LINE_ENDINGS_LF
#endif

// write bytes function type
typedef void (*tx_func_t)(int, int);
// read bytes function type
typedef int (*rx_func_t)(int);

// Basic functions for sending and receiving bytes
static void usb_serial_jtag_tx_char(int fd, int c);
static int usb_serial_jtag_rx_char(int fd);


//If no host is listening to the CDCACM port, the TX buffer
//will never be able to flush to the host. Instead of the Tx
//routines waiting forever, if the buffer hasn't been flushed
//to the host the tx routine will fail fast. (Note that as
//soon as something starts listening, the CDCACM port will
//start working as normal again.)
#define TX_FLUSH_TIMEOUT_US (50*1000LL)
//(As a reference, you'd expect an unloaded usb bus to try to
//pick up tx data once every USB frame, aka every 1ms. We take a
//longer timeout to allow for a loaded bus.)

typedef struct {
    // One-character buffer used for newline conversion code
    int peek_char;
    // Read and write locks, lazily initialized
    _lock_t read_lock;
    _lock_t write_lock;
    // Non-blocking flag.
    // The default implementation does not honor this flag, all reads
    // are non-blocking.
    // When the driver is used (via esp_vfs_usb_serial_jtag_use_driver),
    // reads are either blocking or non-blocking depending on this flag.
    bool non_blocking;
    // TX has already tried a blocking send.
    bool tx_tried_blocking;
    // Newline conversion mode when transmitting
    esp_line_endings_t tx_mode;
    // Newline conversion mode when receiving
    esp_line_endings_t rx_mode;
    // Functions used to write bytes to port. Default to "basic" functions.
    tx_func_t tx_func;
    // Functions used to read bytes from port. Default to "basic" functions.
    rx_func_t rx_func;
    // Timestamp of last time we managed to write something to the tx buffer
    int64_t last_tx_ts;
} vfs_usb_serial_jtag_context_t;

//If the context should be dynamically initialized, remove this structure
//and point s_ctx to allocated data.
static vfs_usb_serial_jtag_context_t s_ctx = {
    .peek_char = NONE,
    .tx_mode = DEFAULT_TX_MODE,
    .rx_mode = DEFAULT_RX_MODE,
    .tx_func = usb_serial_jtag_tx_char,
    .rx_func = usb_serial_jtag_rx_char
};

static int usb_serial_jtag_open(const char * path, int flags, int mode)
{
    s_ctx.non_blocking = ((flags & O_NONBLOCK) == O_NONBLOCK);
    return 0;
}

static void usb_serial_jtag_tx_char(int fd, int c)
{
    uint8_t cc=(uint8_t)c;
    // Try to write to the buffer as long as we still expect the buffer to have
    // a chance of being emptied by an active host. Just drop the data if there's
    // no chance anymore.
    // When we first try to send a character and the buffer is not accessible yet,
    // we wait until the time has been more than TX_FLUSH_TIMEOUT_US since we successfully
    // sent the last byte. If it takes longer than TX_FLUSH_TIMEOUT_US, we drop every
    // byte until the buffer can be accessible again.
    do {
        if (usb_serial_jtag_ll_txfifo_writable()) {
            usb_serial_jtag_ll_write_txfifo(&cc, 1);
            s_ctx.last_tx_ts = esp_timer_get_time();
            break;
        }
    } while ((esp_timer_get_time() - s_ctx.last_tx_ts) < TX_FLUSH_TIMEOUT_US);

}

static int usb_serial_jtag_rx_char(int fd)
{
    uint8_t c;
    int l = usb_serial_jtag_ll_read_rxfifo(&c, 1);
    if (l == 0) {
        return NONE;
    }
    return c;
}

static ssize_t usb_serial_jtag_write(int fd, const void * data, size_t size)
{
    const char *data_c = (const char *)data;
    /*  Even though newlib does stream locking on each individual stream, we need
     *  a dedicated lock if two streams (stdout and stderr) point to the
     *  same port.
     */
    _lock_acquire_recursive(&s_ctx.write_lock);
    for (size_t i = 0; i < size; i++) {
        int c = data_c[i];
        if (c == '\n' && s_ctx.tx_mode != ESP_LINE_ENDINGS_LF) {
            s_ctx.tx_func(fd, '\r');
            if (s_ctx.tx_mode == ESP_LINE_ENDINGS_CR) {
                continue;
            }
        }
        s_ctx.tx_func(fd, c);
        if (c == '\n') {
            //Make sure line doesn't linger in fifo
            usb_serial_jtag_ll_txfifo_flush();
        }
    }
    _lock_release_recursive(&s_ctx.write_lock);
    return size;
}


/* Helper function which returns a previous character or reads a new one from
 * the port. Previous character can be returned ("pushed back") using
 * usb_serial_jtag_return_char function.
 */
static int usb_serial_jtag_read_char(int fd)
{
    /* return character from peek buffer, if it is there */
    if (s_ctx.peek_char != NONE) {
        int c = s_ctx.peek_char;
        s_ctx.peek_char = NONE;
        return c;
    }
    return s_ctx.rx_func(fd);
}

/* Push back a character; it will be returned by next call to usb_serial_jtag_read_char */
static void usb_serial_jtag_return_char(int fd, int c)
{
    assert(s_ctx.peek_char == NONE);
    s_ctx.peek_char = c;
}

static ssize_t usb_serial_jtag_read(int fd, void* data, size_t size)
{
    char *data_c = (char *) data;
    size_t received = 0;
    _lock_acquire_recursive(&s_ctx.read_lock);
    while (received < size) {
        int c = usb_serial_jtag_read_char(fd);
        if (c == '\r') {
            if (s_ctx.rx_mode == ESP_LINE_ENDINGS_CR) {
                c = '\n';
            } else if (s_ctx.rx_mode == ESP_LINE_ENDINGS_CRLF) {
                /* look ahead */
                int c2 = usb_serial_jtag_read_char(fd);
                if (c2 == NONE) {
                    /* could not look ahead, put the current character back */
                    usb_serial_jtag_return_char(fd, c);
                    break;
                }
                if (c2 == '\n') {
                    /* this was \r\n sequence. discard \r, return \n */
                    c = '\n';
                } else {
                    /* \r followed by something else. put the second char back,
                     * it will be processed on next iteration. return \r now.
                     */
                    usb_serial_jtag_return_char(fd, c2);
                }
            }
        } else if (c == NONE) {
            break;
        }
        data_c[received] = (char) c;
        ++received;
        if (c == '\n') {
            break;
        }
    }
    _lock_release_recursive(&s_ctx.read_lock);
    if (received > 0) {
        return received;
    }
    errno = EWOULDBLOCK;
    return -1;
}

static int usb_serial_jtag_fstat(int fd, struct stat * st)
{
    memset(st, 0, sizeof(*st));
    st->st_mode = S_IFCHR;
    return 0;
}

static int usb_serial_jtag_close(int fd)
{
    return 0;
}

static int usb_serial_jtag_fcntl(int fd, int cmd, int arg)
{
    int result = 0;
    if (cmd == F_GETFL) {
        result |= O_RDWR;
        if (s_ctx.non_blocking) {
            result |= O_NONBLOCK;
        }
    } else if (cmd == F_SETFL) {
        s_ctx.non_blocking = (arg & O_NONBLOCK) != 0;
    } else {
        // unsupported operation
        result = -1;
        errno = ENOSYS;
    }
    return result;
}

static int usb_serial_jtag_fsync(int fd)
{
    _lock_acquire_recursive(&s_ctx.write_lock);
    usb_serial_jtag_ll_txfifo_flush();
    //Wait for the host to have picked up the buffer, but honour the timeout in
    //case the host is not listening.
    while ((esp_timer_get_time() - s_ctx.last_tx_ts) < TX_FLUSH_TIMEOUT_US) {
        if (usb_serial_jtag_ll_txfifo_writable()) {
            s_ctx.last_tx_ts = esp_timer_get_time();
            break;
        }
    }
    _lock_release_recursive(&s_ctx.write_lock);
    return 0;
}


#ifdef CONFIG_VFS_SUPPORT_TERMIOS
static int usb_serial_jtag_tcsetattr(int fd, int optional_actions, const struct termios *p)
{
    if (p == NULL) {
        errno = EINVAL;
        return -1;
    }

    switch (optional_actions) {
        case TCSANOW:
            // nothing to do
            break;
        case TCSADRAIN:
            usb_serial_jtag_fsync(fd);
            break;
        case TCSAFLUSH:
            // Not applicable.
            break;
        default:
            errno = EINVAL;
            return -1;
    }

    if (p->c_iflag & IGNCR) {
        s_ctx.rx_mode = ESP_LINE_ENDINGS_CRLF;
    } else if (p->c_iflag & ICRNL) {
        s_ctx.rx_mode = ESP_LINE_ENDINGS_CR;
    } else {
        s_ctx.rx_mode = ESP_LINE_ENDINGS_LF;
    }
    return 0;
}

static int usb_serial_jtag_tcgetattr(int fd, struct termios *p)
{
    if (p == NULL) {
        errno = EINVAL;
        return -1;
    }

    memset(p, 0, sizeof(struct termios));

    if (s_ctx.rx_mode == ESP_LINE_ENDINGS_CRLF) {
        p->c_iflag |= IGNCR;
    } else if (s_ctx.rx_mode == ESP_LINE_ENDINGS_CR) {
        p->c_iflag |= ICRNL;
    }

    //Dummy values that vaguely make sense on a not-actually-rs232 port.
    //Should be good enough to keep software that expects an actual
    //serial port happy.
    p->c_cflag &= (~CSIZE);
    p->c_cflag |= CS8;
    p->c_ispeed = p->c_ospeed = 1000000;

    return 0;
}

static int usb_serial_jtag_tcdrain(int fd)
{
    usb_serial_jtag_fsync(fd);
    return 0;
}

static int usb_serial_jtag_tcflush(int fd, int select)
{
    //Flushing is not supported.
    errno = EINVAL;
    return -1;
}
#endif // CONFIG_VFS_SUPPORT_TERMIOS

void esp_vfs_dev_usb_serial_jtag_set_tx_line_endings(esp_line_endings_t mode)
{
    s_ctx.tx_mode = mode;
}

void esp_vfs_dev_usb_serial_jtag_set_rx_line_endings(esp_line_endings_t mode)
{
    s_ctx.rx_mode = mode;
}

static const esp_vfs_t vfs = {
    .flags = ESP_VFS_FLAG_DEFAULT,
    .write = &usb_serial_jtag_write,
    .open = &usb_serial_jtag_open,
    .fstat = &usb_serial_jtag_fstat,
    .close = &usb_serial_jtag_close,
    .read = &usb_serial_jtag_read,
    .fcntl = &usb_serial_jtag_fcntl,
    .fsync = &usb_serial_jtag_fsync,
#ifdef CONFIG_VFS_SUPPORT_TERMIOS
    .tcsetattr = &usb_serial_jtag_tcsetattr,
    .tcgetattr = &usb_serial_jtag_tcgetattr,
    .tcdrain = &usb_serial_jtag_tcdrain,
    .tcflush = &usb_serial_jtag_tcflush,
#endif // CONFIG_VFS_SUPPORT_TERMIOS
};

const esp_vfs_t* esp_vfs_usb_serial_jtag_get_vfs(void)
{
    return &vfs;
}

esp_err_t esp_vfs_dev_usb_serial_jtag_register(void)
{
    // "/dev/usb_serial_jtag" unfortunately is too long for vfs
    return esp_vfs_register("/dev/usbserjtag", &vfs, NULL);
}

/***********************************************************
 * VFS uses USB-SERIAL-JTAG driver part.
 **********************************************************/

static int usbjtag_rx_char_via_driver(int fd)
{
    uint8_t c;
    TickType_t timeout = s_ctx.non_blocking ? 0 : portMAX_DELAY;
    int n = usb_serial_jtag_read_bytes(&c, 1, timeout);
    if (n <= 0) {
        return NONE;
    }
    return c;
}

static void usbjtag_tx_char_via_driver(int fd, int c)
{
    char ch = (char) c;
    TickType_t ticks = (TX_FLUSH_TIMEOUT_US / 1000) / portTICK_PERIOD_MS;
    if (usb_serial_jtag_write_bytes(&ch, 1, 0) != 0) {
        s_ctx.tx_tried_blocking = false;
        return;
    }

    if (s_ctx.tx_tried_blocking == false) {
        if (usb_serial_jtag_write_bytes(&ch, 1, ticks) != 0) {
            return;
        } else {
            s_ctx.tx_tried_blocking = true;
        }
    }
}

void esp_vfs_usb_serial_jtag_use_nonblocking(void)
{
    _lock_acquire_recursive(&s_ctx.read_lock);
    _lock_acquire_recursive(&s_ctx.write_lock);
    s_ctx.tx_func = usb_serial_jtag_tx_char;
    s_ctx.rx_func = usb_serial_jtag_rx_char;
    _lock_release_recursive(&s_ctx.write_lock);
    _lock_release_recursive(&s_ctx.read_lock);
}

void esp_vfs_usb_serial_jtag_use_driver(void)
{
    _lock_acquire_recursive(&s_ctx.read_lock);
    _lock_acquire_recursive(&s_ctx.write_lock);
    s_ctx.tx_func = usbjtag_tx_char_via_driver;
    s_ctx.rx_func = usbjtag_rx_char_via_driver;
    _lock_release_recursive(&s_ctx.write_lock);
    _lock_release_recursive(&s_ctx.read_lock);
}
