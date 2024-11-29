/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
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
#include "esp_vfs_dev.h" // Old headers for the aliasing functions
#include "esp_vfs_usb_serial_jtag.h" // Old headers for the aliasing functions
#include "esp_attr.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "hal/usb_serial_jtag_ll.h"
#include "driver/usb_serial_jtag_select.h"
#include "driver/usb_serial_jtag_vfs.h"
#include "driver/usb_serial_jtag.h"
#include "esp_private/startup_internal.h"
#include "esp_heap_caps.h"

// local file descriptor value for the USJ
#define USJ_LOCAL_FD 0

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

#if CONFIG_VFS_SELECT_IN_RAM
#define USJ_VFS_MALLOC_FLAGS (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define USJ_VFS_MALLOC_FLAGS MALLOC_CAP_DEFAULT
#endif

// write bytes function type
typedef void (*tx_func_t)(int, int);
// read bytes function type
typedef int (*rx_func_t)(int);
// fsync bytes function type
typedef int (*fsync_func_t)(int);

// Basic functions for sending and receiving bytes and fsync
static void usb_serial_jtag_tx_char_no_driver(int fd, int c);
static int usb_serial_jtag_rx_char_no_driver(int fd);
static int usb_serial_jtag_wait_tx_done_no_driver(int fd);

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
    // Function used to write bytes to port. Default to "basic" functions.
    tx_func_t tx_func;
    // Function used to read bytes from port. Default to "basic" functions.
    rx_func_t rx_func;
    // Function used to make sure all data is sent to the host.
    fsync_func_t fsync_func;
    // Timestamp of last time we managed to write something to the tx buffer
    int64_t last_tx_ts;
} usb_serial_jtag_vfs_context_t;

//If the context should be dynamically initialized, remove this structure
//and point s_ctx to allocated data.
static usb_serial_jtag_vfs_context_t s_ctx = {
    .peek_char = NONE,
    .tx_mode = DEFAULT_TX_MODE,
    .rx_mode = DEFAULT_RX_MODE,
    .tx_func = usb_serial_jtag_tx_char_no_driver,
    .rx_func = usb_serial_jtag_rx_char_no_driver,
    .fsync_func = usb_serial_jtag_wait_tx_done_no_driver
};

#ifdef CONFIG_VFS_SUPPORT_SELECT

typedef struct {
    esp_vfs_select_sem_t select_sem;
    fd_set *readfds;
    fd_set *writefds;
    fd_set *errorfds;
    fd_set readfds_orig;
    fd_set writefds_orig;
    fd_set errorfds_orig;
} usb_serial_jtag_select_args_t;

static usb_serial_jtag_select_args_t **s_registered_selects = NULL;
static int s_registered_select_num = 0;
static portMUX_TYPE s_registered_select_lock = portMUX_INITIALIZER_UNLOCKED;

static esp_err_t usb_serial_jtag_end_select(void *end_select_args);

#endif // CONFIG_VFS_SUPPORT_SELECT

static int usb_serial_jtag_open(const char * path, int flags, int mode)
{
    s_ctx.non_blocking = ((flags & O_NONBLOCK) == O_NONBLOCK);
    return USJ_LOCAL_FD;
}

static void usb_serial_jtag_tx_char_no_driver(int fd, int c)
{
    uint8_t cc = (uint8_t)c;
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
            if (c == '\n') {
                //Make sure line doesn't linger in fifo
                usb_serial_jtag_ll_txfifo_flush();
            }
            //update time of last successful tx to now.
            s_ctx.last_tx_ts = esp_timer_get_time();
            break;
        }
    } while ((esp_timer_get_time() - s_ctx.last_tx_ts) < TX_FLUSH_TIMEOUT_US);

}

static int usb_serial_jtag_rx_char_no_driver(int fd)
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

static int usb_serial_jtag_wait_tx_done_no_driver(int fd)
{
    usb_serial_jtag_ll_txfifo_flush();
    //Wait for the host to have picked up the buffer, but honour the timeout in
    //case the host is not listening.
    while ((esp_timer_get_time() - s_ctx.last_tx_ts) < TX_FLUSH_TIMEOUT_US) {
        if (usb_serial_jtag_ll_txfifo_writable()) {
            s_ctx.last_tx_ts = esp_timer_get_time();
            //The last transfer may have been a 64-byte one. Flush again in order to
            //send a 0-byte packet to indicate the end of the USB transfer, otherwise
            //those 64 bytes will get stuck in the hosts buffer.
            usb_serial_jtag_ll_txfifo_flush();
            return 0;
        }
    }
    //Timeout. Host probably isn't listening.
    return EIO;
}

static int usb_serial_jtag_fsync(int fd)
{
    _lock_acquire_recursive(&s_ctx.write_lock);
    int r = s_ctx.fsync_func(fd);
    _lock_release_recursive(&s_ctx.write_lock);
    if (r == 0) {
        return 0;
    } else {
        errno = r;
        return -1;
    }
}

#ifdef CONFIG_VFS_SUPPORT_SELECT

static void select_notif_callback_isr(usj_select_notif_t usj_select_notif, BaseType_t *task_woken)
{
    portENTER_CRITICAL_ISR(&s_registered_select_lock);
    for (int i = 0; i < s_registered_select_num; ++i) {
        usb_serial_jtag_select_args_t *args = s_registered_selects[i];
        if (args) {
            switch (usj_select_notif) {
            case USJ_SELECT_READ_NOTIF:
                if (FD_ISSET(USJ_LOCAL_FD, &args->readfds_orig)) {
                    FD_SET(USJ_LOCAL_FD, args->readfds);
                    esp_vfs_select_triggered_isr(args->select_sem, task_woken);
                }
                break;
            case USJ_SELECT_WRITE_NOTIF:
                if (FD_ISSET(USJ_LOCAL_FD, &args->writefds_orig)) {
                    FD_SET(USJ_LOCAL_FD, args->writefds);
                    esp_vfs_select_triggered_isr(args->select_sem, task_woken);
                }
                break;
            case USJ_SELECT_ERROR_NOTIF:
                if (FD_ISSET(USJ_LOCAL_FD, &args->errorfds_orig)) {
                    FD_SET(USJ_LOCAL_FD, args->errorfds);
                    esp_vfs_select_triggered_isr(args->select_sem, task_woken);
                }
                break;
            }
        }
    }
    portEXIT_CRITICAL_ISR(&s_registered_select_lock);
}

static esp_err_t register_select(usb_serial_jtag_select_args_t *args)
{
    esp_err_t ret = ESP_ERR_INVALID_ARG;

    if (args) {
        portENTER_CRITICAL(&s_registered_select_lock);
        const int new_size = s_registered_select_num + 1;
        usb_serial_jtag_select_args_t **new_selects;
        if ((new_selects = heap_caps_realloc(s_registered_selects, new_size * sizeof(usb_serial_jtag_select_args_t *), USJ_VFS_MALLOC_FLAGS)) == NULL) {
            ret = ESP_ERR_NO_MEM;
        } else {
            /* on first select registration register the callback  */
            if (s_registered_select_num == 0) {
                usb_serial_jtag_set_select_notif_callback(select_notif_callback_isr);
            }

            s_registered_selects = new_selects;
            s_registered_selects[s_registered_select_num] = args;
            s_registered_select_num = new_size;
            ret = ESP_OK;
        }
        portEXIT_CRITICAL(&s_registered_select_lock);
    }

    return ret;
}

static esp_err_t unregister_select(usb_serial_jtag_select_args_t *args)
{
    esp_err_t ret = ESP_OK;
    if (args) {
        ret = ESP_ERR_INVALID_STATE;
        portENTER_CRITICAL(&s_registered_select_lock);
        for (int i = 0; i < s_registered_select_num; ++i) {
            if (s_registered_selects[i] == args) {
                const int new_size = s_registered_select_num - 1;
                // The item is removed by overwriting it with the last item. The subsequent rellocation will drop the
                // last item.
                s_registered_selects[i] = s_registered_selects[new_size];
                s_registered_selects = heap_caps_realloc(s_registered_selects, new_size * sizeof(usb_serial_jtag_select_args_t *), USJ_VFS_MALLOC_FLAGS);
                // Shrinking a buffer with realloc is guaranteed to succeed.
                s_registered_select_num = new_size;

                /* when the last select is unregistered, also unregister the callback  */
                if (s_registered_select_num == 0) {
                    usb_serial_jtag_set_select_notif_callback(NULL);
                }

                ret = ESP_OK;
                break;
            }
        }
        portEXIT_CRITICAL(&s_registered_select_lock);
    }
    return ret;
}

static esp_err_t usb_serial_jtag_start_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
                                              esp_vfs_select_sem_t select_sem, void **end_select_args)
{
    (void)nfds; /* Since there is only 1 usb serial jtag port, this parameter is useless */
    *end_select_args = NULL;
    if (!usb_serial_jtag_is_driver_installed()) {
        return ESP_ERR_INVALID_STATE;
    }

    usb_serial_jtag_select_args_t *args = heap_caps_malloc(sizeof(usb_serial_jtag_select_args_t), USJ_VFS_MALLOC_FLAGS);

    if (args == NULL) {
        return ESP_ERR_NO_MEM;
    }
    args->select_sem = select_sem;
    args->readfds = readfds;
    args->writefds = writefds;
    args->errorfds = exceptfds;
    args->readfds_orig = *readfds; // store the original values because they will be set to zero
    args->writefds_orig = *writefds;
    args->errorfds_orig = *exceptfds;
    FD_ZERO(readfds);
    FD_ZERO(writefds);
    FD_ZERO(exceptfds);

    esp_err_t ret = register_select(args);
    if (ret != ESP_OK) {
        free(args);
        return ret;
    }

    bool trigger_select = false;

    // check if the select should return instantly if the bus is read ready
    if (FD_ISSET(USJ_LOCAL_FD, &args->readfds_orig) && usb_serial_jtag_read_ready()) {
        // signal immediately when data is buffered
        FD_SET(USJ_LOCAL_FD, readfds);
        trigger_select = true;
    }

    // check if the select should return instantly if the bus is write ready
    if (FD_ISSET(USJ_LOCAL_FD, &args->writefds_orig) && usb_serial_jtag_write_ready()) {
        // signal immediately when data can be written
        FD_SET(USJ_LOCAL_FD, writefds);
        trigger_select = true;
    }

    if (trigger_select) {
        esp_vfs_select_triggered(args->select_sem);
    }

    *end_select_args = args;
    return ESP_OK;
}

static esp_err_t usb_serial_jtag_end_select(void *end_select_args)
{
    usb_serial_jtag_select_args_t *args = end_select_args;
    esp_err_t ret = unregister_select(args);
    if (args) {
        free(args);
    }

    return ret;
}

#endif // CONFIG_VFS_SUPPORT_SELECT

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

void usb_serial_jtag_vfs_set_tx_line_endings(esp_line_endings_t mode)
{
    s_ctx.tx_mode = mode;
}

void usb_serial_jtag_vfs_set_rx_line_endings(esp_line_endings_t mode)
{
    s_ctx.rx_mode = mode;
}

#ifdef CONFIG_VFS_SUPPORT_SELECT
static const esp_vfs_select_ops_t s_vfs_jtag_select = {
    .start_select = &usb_serial_jtag_start_select,
    .end_select = &usb_serial_jtag_end_select,
};
#endif // CONFIG_VFS_SUPPORT_SELECT
#ifdef CONFIG_VFS_SUPPORT_TERMIOS
static const esp_vfs_termios_ops_t s_vfs_jtag_termios = {
    .tcsetattr = &usb_serial_jtag_tcsetattr,
    .tcgetattr = &usb_serial_jtag_tcgetattr,
    .tcdrain = &usb_serial_jtag_tcdrain,
    .tcflush = &usb_serial_jtag_tcflush,
};
#endif // CONFIG_VFS_SUPPORT_TERMIOS

static const esp_vfs_fs_ops_t s_vfs_jtag = {
    .write = &usb_serial_jtag_write,
    .open = &usb_serial_jtag_open,
    .fstat = &usb_serial_jtag_fstat,
    .close = &usb_serial_jtag_close,
    .read = &usb_serial_jtag_read,
    .fcntl = &usb_serial_jtag_fcntl,
    .fsync = &usb_serial_jtag_fsync,

#ifdef CONFIG_VFS_SUPPORT_SELECT
    .select = &s_vfs_jtag_select,
#endif // CONFIG_VFS_SUPPORT_SELECT

#ifdef CONFIG_VFS_SUPPORT_TERMIOS
    .termios = &s_vfs_jtag_termios,
#endif // CONFIG_VFS_SUPPORT_TERMIOS
};

const esp_vfs_fs_ops_t* esp_vfs_usb_serial_jtag_get_vfs(void)
{
    return &s_vfs_jtag;
}

esp_err_t usb_serial_jtag_vfs_register(void)
{
    // "/dev/usb_serial_jtag" unfortunately is too long for vfs
    return esp_vfs_register_fs("/dev/usbserjtag", &s_vfs_jtag, ESP_VFS_FLAG_STATIC, NULL);
}

#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
ESP_SYSTEM_INIT_FN(init_vfs_usj, CORE, BIT(0), 111)
{
    usb_serial_jtag_vfs_register();
    return ESP_OK;
}
#endif

#if CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG
ESP_SYSTEM_INIT_FN(init_vfs_usj_sec, CORE, BIT(0), 112)
{
    // "/dev/seccondary_usb_serial_jtag" unfortunately is too long for vfs
    esp_vfs_register_fs("/dev/secondary", &s_vfs_jtag, ESP_VFS_FLAG_STATIC, NULL);
    return ESP_OK;
}
#endif

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

static int usbjtag_wait_tx_done_via_driver(int fd)
{
    TickType_t ticks = (TX_FLUSH_TIMEOUT_US / 1000) / portTICK_PERIOD_MS;
    esp_err_t r = usb_serial_jtag_wait_tx_done(ticks);
    return (r == ESP_OK) ? 0 : EIO;
}

void usb_serial_jtag_vfs_use_nonblocking(void)
{
    _lock_acquire_recursive(&s_ctx.read_lock);
    _lock_acquire_recursive(&s_ctx.write_lock);
    s_ctx.tx_func = usb_serial_jtag_tx_char_no_driver;
    s_ctx.rx_func = usb_serial_jtag_rx_char_no_driver;
    s_ctx.fsync_func = usb_serial_jtag_wait_tx_done_no_driver;
    _lock_release_recursive(&s_ctx.write_lock);
    _lock_release_recursive(&s_ctx.read_lock);
}

void usb_serial_jtag_vfs_use_driver(void)
{
    _lock_acquire_recursive(&s_ctx.read_lock);
    _lock_acquire_recursive(&s_ctx.write_lock);
    s_ctx.tx_func = usbjtag_tx_char_via_driver;
    s_ctx.rx_func = usbjtag_rx_char_via_driver;
    s_ctx.fsync_func = usbjtag_wait_tx_done_via_driver;
    _lock_release_recursive(&s_ctx.write_lock);
    _lock_release_recursive(&s_ctx.read_lock);
}

void usb_serial_jtag_vfs_include_dev_init(void)
{
    // Linker hook function, exists to make the linker examine this file
}

// -------------------------- esp_vfs_usb_serial_jtag_xxx ALIAS (deprecated) ----------------------------

esp_err_t esp_vfs_dev_usb_serial_jtag_register(void) __attribute__((alias("usb_serial_jtag_vfs_register")));

void esp_vfs_dev_usb_serial_jtag_set_rx_line_endings(esp_line_endings_t mode) __attribute__((alias("usb_serial_jtag_vfs_set_rx_line_endings")));

void esp_vfs_dev_usb_serial_jtag_set_tx_line_endings(esp_line_endings_t mode) __attribute__((alias("usb_serial_jtag_vfs_set_tx_line_endings")));

void esp_vfs_usb_serial_jtag_use_nonblocking(void) __attribute__((alias("usb_serial_jtag_vfs_use_nonblocking")));

void esp_vfs_usb_serial_jtag_use_driver(void) __attribute__((alias("usb_serial_jtag_vfs_use_driver")));
