/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#if CONFIG_ESP_WIFI_FTM_ENABLE
/********************************************** 2.4 GHz Values *******************************************************/
#if CONFIG_IDF_TARGET_ESP32S2
// 20 MHz FTM in 20MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ         367 // Connected Initiator     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_DIS     364 // Disconnected Initiator  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ         374 // Connected Initiator     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_DIS     373 // Disconnected Initiator  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ         366 // Connected Responder     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_DIS     366 // Disconnected Responder  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ         359 // Connected Responder     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_DIS     359 // Disconnected Responder  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ         229 // Connected Initiator     in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_DIS     221 // Disconnected Initiator  in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ         234 // Connected Initiator     in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_DIS     228 // Disconnected Initiator  in 40MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ         363 //  Connected Responder    in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_DIS     222 //  Disconnected Responder in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ         356 //  Connected Responder    in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_DIS     216 //  Disconnected Responder in 40MHz (Ch 11) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ         403 // Connected Initiator     in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_DIS     403 // Disconnected Initiator  in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ         403 // Connected Initiator     in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_DIS     402 // Disconnected Initiator  in 40MHz (Ch 11) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ         42 //  Connected Responder    in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_DIS     42 //  Disconnected Responder in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ         41 //  Connected Responder    in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_DIS     41 //  Disconnected Responder in 40MHz (Ch 11) using 40MHz FTM
#elif CONFIG_IDF_TARGET_ESP32C3
// 20 MHz FTM in 20MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ         449 // Connected Initiator     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_DIS     448 // Disconnected Initiator  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ         450 // Connected Initiator     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_DIS     452 // Disconnected Initiator  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ         441 // Connected Responder     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_DIS     441 // Disconnected Responder  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ         433 // Connected Responder     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_DIS     433 // Disconnected Responder  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ         267 // Connected Initiator     in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_DIS     267 // Disconnected Initiator  in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ         270 // Connected Initiator     in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_DIS     270 // Disconnected Initiator  in 40MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ         260 //  Connected Responder    in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_DIS     260 //  Disconnected Responder in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ         255 //  Connected Responder    in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_DIS     253 //  Disconnected Responder in 40MHz (Ch 11) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ         437 // Connected Initiator     in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_DIS     437 // Disconnected Initiator  in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ         438 // Connected Initiator     in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_DIS     438 // Disconnected Initiator  in 40MHz (Ch 11) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ         86 //  Connected Responder    in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_DIS     86 //  Disconnected Responder in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ         83 //  Connected Responder    in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_DIS     83 //  Disconnected Responder in 40MHz (Ch 11) using 40MHz FTM
#elif CONFIG_IDF_TARGET_ESP32C2
// 20 MHz FTM in 20MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ         443 // Connected Initiator     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_DIS     443 // Disconnected Initiator  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ         451 // Connected Initiator     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_DIS     452 // Disconnected Initiator  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ         441 // Connected Responder     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_DIS     441 // Disconnected Responder  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ         432 // Connected Responder     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_DIS     432 // Disconnected Responder  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ         1510 // Connected Initiator     in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_DIS     1349 // Disconnected Initiator  in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ         1429 // Connected Initiator     in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_DIS     1313 // Disconnected Initiator  in 40MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ         1354 //  Connected Responder    in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_DIS     1347 //  Disconnected Responder in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ         1295 //  Connected Responder    in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_DIS     1293 //  Disconnected Responder in 40MHz (Ch 11) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ         1626 // Connected Initiator     in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_DIS     1626 // Disconnected Initiator  in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ         1626 // Connected Initiator     in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_DIS     1626 // Disconnected Initiator  in 40MHz (Ch 11) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ         1626 //  Connected Responder    in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_DIS     1626 //  Disconnected Responder in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ         1626 //  Connected Responder    in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_DIS     1626 //  Disconnected Responder in 40MHz (Ch 11) using 40MHz FTM
#elif CONFIG_IDF_TARGET_ESP32S3
// 20 MHz FTM in 20MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ         453 // Connected Initiator     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_DIS     453 // Disconnected Initiator  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ         453 // Connected Initiator     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_DIS     454 // Disconnected Initiator  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ         444 // Connected Responder     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_DIS     444 // Disconnected Responder  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ         440 // Connected Responder     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_DIS     440 // Disconnected Responder  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ         271 // Connected Initiator     in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_DIS     269 // Disconnected Initiator  in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ         270 // Connected Initiator     in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_DIS     270 // Disconnected Initiator  in 40MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ         262 //  Connected Responder    in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_DIS     262 //  Disconnected Responder in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ         261 //  Connected Responder    in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_DIS     258 //  Disconnected Responder in 40MHz (Ch 11) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ         443 // Connected Initiator     in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_DIS     442 // Disconnected Initiator  in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ         443 // Connected Initiator     in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_DIS     445 // Disconnected Initiator  in 40MHz (Ch 11) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ         88 //  Connected Responder    in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_DIS     89 //  Disconnected Responder in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ         84 //  Connected Responder    in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_DIS     82 //  Disconnected Responder in 40MHz (Ch 11) using 40MHz FTM
#elif CONFIG_IDF_TARGET_ESP32C6
// 20 MHz FTM in 20MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ         716 // Connected Initiator     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_DIS     716 // Disconnected Initiator  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ         717 // Connected Initiator     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_DIS     717 // Disconnected Initiator  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ         703 // Connected Responder     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_DIS     703 // Disconnected Responder  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ         702 // Connected Responder     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_DIS     702 // Disconnected Responder  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ         617 // Connected Initiator     in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_DIS     617 // Disconnected Initiator  in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ         617 // Connected Initiator     in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_DIS     617 // Disconnected Initiator  in 40MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ         611 //  Connected Responder    in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_DIS     611 //  Disconnected Responder in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ         609 //  Connected Responder    in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_DIS     609 //  Disconnected Responder in 40MHz (Ch 11) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ         790 // Connected Initiator     in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_DIS     790 // Disconnected Initiator  in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ         796 // Connected Initiator     in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_DIS     796 // Disconnected Initiator  in 40MHz (Ch 11) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ         436 //  Connected Responder    in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_DIS     436 //  Disconnected Responder in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ         433 //  Connected Responder    in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_DIS     433 //  Disconnected Responder in 40MHz (Ch 11) using 40MHz FTM
#elif CONFIG_IDF_TARGET_ESP32C61
// 20 MHz FTM in 20MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ         870 // Connected Initiator     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_DIS     870 // Disconnected Initiator  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ         870 // Connected Initiator     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_DIS     870 // Disconnected Initiator  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ         852 // Connected Responder     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_DIS     852 // Disconnected Responder  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ         849 // Connected Responder     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_DIS     849 // Disconnected Responder  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ         758 // Connected Initiator     in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_DIS     758 // Disconnected Initiator  in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ         774 // Connected Initiator     in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_DIS     774 // Disconnected Initiator  in 40MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ         734 //  Connected Responder    in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_DIS     734 //  Disconnected Responder in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ         737 //  Connected Responder    in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_DIS     737 //  Disconnected Responder in 40MHz (Ch 11) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ         929 // Connected Initiator     in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_DIS     929 // Disconnected Initiator  in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ         928 // Connected Initiator     in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_DIS     928 // Disconnected Initiator  in 40MHz (Ch 11) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ         574 //  Connected Responder    in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_DIS     574 //  Disconnected Responder in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ         575 //  Connected Responder    in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_DIS     575 //  Disconnected Responder in 40MHz (Ch 11) using 40MHz FTM
#elif CONFIG_IDF_TARGET_ESP32C5
// 20 MHz FTM in 20MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ         929 // Connected Initiator     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_DIS     972 // Disconnected Initiator  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ         934 // Connected Initiator     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_DIS     934 // Disconnected Initiator  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ         974 // Connected Responder     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_DIS     974 // Disconnected Responder  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ         922 // Connected Responder     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_DIS     922 // Disconnected Responder  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ         814 // Connected Initiator     in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_DIS     814 // Disconnected Initiator  in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ         820 // Connected Initiator     in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_DIS     820 // Disconnected Initiator  in 40MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ         860 //  Connected Responder    in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_DIS     817 //  Disconnected Responder in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ         809 //  Connected Responder    in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_DIS     807 //  Disconnected Responder in 40MHz (Ch 11) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ         986 // Connected Initiator     in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_DIS     988 // Disconnected Initiator  in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ         992 // Connected Initiator     in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_DIS     992 // Disconnected Initiator  in 40MHz (Ch 11) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ         640 //  Connected Responder    in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_DIS     640 //  Disconnected Responder in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ         631 //  Connected Responder    in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_DIS     631 //  Disconnected Responder in 40MHz (Ch 11) using 40MHz FTM
#elif CONFIG_IDF_TARGET_ESP32S31
// 20 MHz FTM in 20MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ         1151667 // Connected Initiator     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_DIS     1150667 // Disconnected Initiator  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ         1157667 // Connected Initiator     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_DIS     1157667 // Disconnected Initiator  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ         1143333 // Connected Responder     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_DIS     1143333 // Disconnected Responder  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ         1137333 // Connected Responder     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_DIS     1137333 // Disconnected Responder  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ         980667  // Connected Initiator     in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_DIS     974667  // Disconnected Initiator  in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ         983667  // Connected Initiator     in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_DIS     980667  // Disconnected Initiator  in 40MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ         971333  //  Connected Responder    in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_DIS     963333  //  Disconnected Responder in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ         964333  //  Connected Responder    in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_DIS     957333  //  Disconnected Responder in 40MHz (Ch 11) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ         1304667 // Connected Initiator     in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_DIS     1304667 // Disconnected Initiator  in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ         1309333 // Connected Initiator     in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_DIS     1310333 // Disconnected Initiator  in 40MHz (Ch 11) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ         683333  //  Connected Responder    in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_DIS     683333  //  Disconnected Responder in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ         678667  //  Connected Responder    in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_DIS     678667  //  Disconnected Responder in 40MHz (Ch 11) using 40MHz FTM
#endif

/********************************************** 5 GHz Values *******************************************************/
#if CONFIG_IDF_TARGET_ESP32C5
// 20 MHz FTM in 20MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_5G      930 // Connected Initiator     in 20MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_5G_DIS  974 // Disconnected Initiator  in 20MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_5G      930 // Connected Initiator     in 20MHz (Ch 149) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_5G_DIS  974 // Disconnected Initiator  in 20MHz (Ch 149) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_5G      964 // Connected Responder     in 20MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_5G_DIS  964 // Disconnected Responder  in 20MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_5G      964 // Connected Responder     in 20MHz (Ch 149) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_5G_DIS  964 // Disconnected Responder  in 20MHz (Ch 149) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_5G      794 // Connected Initiator     in 40MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_5G_DIS  831 // Disconnected Initiator  in 40MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_5G      796 // Connected Initiator     in 40MHz (Ch 149) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_5G_DIS  835 // Disconnected Initiator  in 40MHz (Ch 149) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_5G      825 //  Connected Responder    in 40MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_5G_DIS  830 //  Disconnected Responder in 40MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_5G      829 //  Connected Responder    in 40MHz (Ch 149) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_5G_DIS  830 //  Disconnected Responder in 40MHz (Ch 149) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_5G      780 // Connected Initiator     in 40MHz (Ch 36)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_5G_DIS  818 // Disconnected Initiator  in 40MHz (Ch 36)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_5G      784 // Connected Initiator     in 40MHz (Ch 149) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_5G_DIS  820 // Disconnected Initiator  in 40MHz (Ch 149) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_5G      836 //  Connected Responder    in 40MHz (Ch 36)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_5G_DIS  836 //  Disconnected Responder in 40MHz (Ch 36)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_5G      837 //  Connected Responder    in 40MHz (Ch 149) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_5G_DIS  837 //  Disconnected Responder in 40MHz (Ch 149) using 40MHz FTM
#endif

#endif /* CONFIG_ESP_WIFI_FTM_ENABLE */
