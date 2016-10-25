#include <string.h>
#include "esp_vfs.h"
#include "esp_attr.h"
#include "sys/errno.h"
#include "sys/lock.h"
#include "soc/uart_struct.h"

static uart_dev_t* s_uarts[3] = {&UART0, &UART1, &UART2};

static int IRAM_ATTR uart_open(const char * path, int flags, int mode)
{
    // this is fairly primitive, we should check if file is opened read only,
    // and error out if write is requested
    if (strcmp(path, "/0") == 0) {
        return 0;
    } else if (strcmp(path, "/1") == 0) {
        return 1;
    } else if (strcmp(path, "/2") == 0) {
        return 2;
    }
    errno = ENOENT;
    return -1;
}

static void IRAM_ATTR uart_tx_char(uart_dev_t* uart, int c)
{
    while (uart->status.txfifo_cnt >= 127) {
        ;
    }
    uart->fifo.rw_byte = c;
}


static size_t IRAM_ATTR uart_write(int fd, const void * data, size_t size)
{
    assert(fd >=0 && fd < 3);
    const char *data_c = (const char *)data;
    uart_dev_t* uart = s_uarts[fd];
    static _lock_t stdout_lock; /* lazily initialised */
    /* Even though newlib does stream locking on stdout, we need
       a dedicated stdout UART lock...

       This is because each task has its own _reent structure with
       unique FILEs for stdin/stdout/stderr, so these are
       per-thread (lazily initialised by __sinit the first time a
       stdio function is used, see findfp.c:235.

       It seems like overkill to allocate a FILE-per-task and lock
       a thread-local stream, but I see no easy way to fix this
       (pre-__sinit_, tasks have "fake" FILEs ie __sf_fake_stdout
       which aren't fully valid.)
    */
    _lock_acquire_recursive(&stdout_lock);
    for (size_t i = 0; i < size; i++) {
#if CONFIG_NEWLIB_STDOUT_ADDCR
        if (data_c[i]=='\n') {
            uart_tx_char(uart, '\r');
        }
#endif
        uart_tx_char(uart, data_c[i]);
    }
    _lock_release_recursive(&stdout_lock);
    return size;
}

static int IRAM_ATTR uart_fstat(int fd, struct stat * st)
{
    assert(fd >=0 && fd < 3);
    st->st_mode = S_IFCHR;
    return 0;
}

static int IRAM_ATTR uart_close(int fd)
{
    assert(fd >=0 && fd < 3);
    return 0;
}

void esp_vfs_dev_uart_register()
{
    esp_vfs_t vfs = {
        .fd_offset = 0,
        .flags = ESP_VFS_FLAG_DEFAULT,
        .write = &uart_write,
        .open = &uart_open,
        .fstat = &uart_fstat,
        .close = &uart_close,
        .read = NULL, // TODO: implement reading from UART
        .stat = NULL,
        .link = NULL,
        .unlink = NULL,
        .rename = NULL
    };
    ESP_ERROR_CHECK(esp_vfs_register("/dev/uart", &vfs, NULL));
}
