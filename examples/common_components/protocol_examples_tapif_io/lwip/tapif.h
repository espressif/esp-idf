/*
 * SPDX-FileCopyrightText: 2001-2003 Swedish Institute of Computer Science
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2022-2023 Espressif Systems (Shanghai) CO LTD
 */
#pragma once
#include "lwip/esp_netif_net_stack.h"

/**
 * @brief lwip netif init API
 * @param netif pointer to lwip's netif
 * @return ERR_OK on success
 */
err_t lwip_tapif_init(struct netif *netif);

/**
 * @brief Input data path
 * @param h pointer to network stack handle (stuct netif* in our case)
 * @param buffer Data
 * @param len Data size
 * @param l2_buff Data L2 buffer
 */
void lwip_tapif_input(void *h, void *buffer, size_t len, void *l2_buff);
