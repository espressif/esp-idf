/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

// throughput performance by ethernet iperf
#ifndef IDF_PERFORMANCE_MIN_TCP_RX_ETH_THROUGHPUT
#define IDF_PERFORMANCE_MIN_TCP_RX_ETH_THROUGHPUT                                   20
#endif
#ifndef IDF_PERFORMANCE_MIN_TCP_TX_ETH_THROUGHPUT
#define IDF_PERFORMANCE_MIN_TCP_TX_ETH_THROUGHPUT                                   30
#endif
#ifndef IDF_PERFORMANCE_MIN_UDP_RX_ETH_THROUGHPUT
#define IDF_PERFORMANCE_MIN_UDP_RX_ETH_THROUGHPUT                                   50
#endif
#ifndef IDF_PERFORMANCE_MIN_UDP_TX_ETH_THROUGHPUT
#define IDF_PERFORMANCE_MIN_UDP_TX_ETH_THROUGHPUT                                   70
#endif

#ifndef IDF_PERFORMANCE_MIN_TCP_RX_ETH_THROUGHPUT_SPI_ETH
#define IDF_PERFORMANCE_MIN_TCP_RX_ETH_THROUGHPUT_SPI_ETH                           6
#endif
#ifndef IDF_PERFORMANCE_MIN_TCP_TX_ETH_THROUGHPUT_SPI_ETH
#define IDF_PERFORMANCE_MIN_TCP_TX_ETH_THROUGHPUT_SPI_ETH                           8
#endif
#ifndef IDF_PERFORMANCE_MIN_UDP_RX_ETH_THROUGHPUT_SPI_ETH
#define IDF_PERFORMANCE_MIN_UDP_RX_ETH_THROUGHPUT_SPI_ETH                           8
#endif
#ifndef IDF_PERFORMANCE_MIN_UDP_TX_ETH_THROUGHPUT_SPI_ETH
#define IDF_PERFORMANCE_MIN_UDP_TX_ETH_THROUGHPUT_SPI_ETH                           10
#endif
