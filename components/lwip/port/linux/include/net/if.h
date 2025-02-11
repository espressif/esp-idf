/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

// Need to provide declarations of if_nametoindex and if_indextoname functions
// as we don't want to bring lwip specific defines
// (since we're on linux target and likely using linux tcp/ip stack)

/**
 * @brief Get the interface index for the given interface name.
 * @param ifname The interface name.
 * @return The interface index.
 */
unsigned int if_nametoindex(const char *ifname);

/**
 * @brief Get the interface name for the given interface index.
 *
 * @param ifindex   The interface index.
 * @param ifname    The buffer to store the interface name.
 * @return char*    The interface name.
 */
char *if_indextoname(unsigned int ifindex, char *ifname);
