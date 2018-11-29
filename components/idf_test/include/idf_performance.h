#pragma once

/* declare the performance here */
#define IDF_PERFORMANCE_MAX_HTTPS_REQUEST_BIN_SIZE                              800
#define IDF_PERFORMANCE_MAX_FREERTOS_SPINLOCK_CYCLES_PER_OP                     200
#define IDF_PERFORMANCE_MAX_FREERTOS_SPINLOCK_CYCLES_PER_OP_PSRAM               300
#define IDF_PERFORMANCE_MAX_FREERTOS_SPINLOCK_CYCLES_PER_OP_UNICORE             130
#define IDF_PERFORMANCE_MAX_ESP_TIMER_GET_TIME_PER_CALL                         1000
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING                            30
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING_NO_DMA                     27
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_POLLING                               15
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_POLLING_NO_DMA                        15
/* Due to code size & linker layout differences interacting with cache, VFS
   microbenchmark currently runs slower with PSRAM enabled. */
#define IDF_PERFORMANCE_MAX_VFS_OPEN_WRITE_CLOSE_TIME                           20000
#define IDF_PERFORMANCE_MAX_VFS_OPEN_WRITE_CLOSE_TIME_PSRAM                     25000
// throughput performance by iperf
#define IDF_PERFORMANCE_MIN_TCP_RX_THROUGHPUT                                   50
#define IDF_PERFORMANCE_MIN_TCP_TX_THROUGHPUT                                   40
#define IDF_PERFORMANCE_MIN_UDP_RX_THROUGHPUT                                   80
#define IDF_PERFORMANCE_MIN_UDP_TX_THROUGHPUT                                   50
// events dispatched per second by event loop library
#define IDF_PERFORMANCE_MIN_EVENT_DISPATCH                                      25000
#define IDF_PERFORMANCE_MIN_EVENT_DISPATCH_PSRAM                                21000
