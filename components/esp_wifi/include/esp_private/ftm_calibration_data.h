/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ         442 // Connected Initiator     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_DIS     442 // Disconnected Initiator  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ         452 // Connected Initiator     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_DIS     447 // Disconnected Initiator  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ         439 // Connected Responder     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_DIS     439 // Disconnected Responder  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ         431 // Connected Responder     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_DIS     431 // Disconnected Responder  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ         266 // Connected Initiator     in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_DIS     263 // Disconnected Initiator  in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ         276 // Connected Initiator     in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_DIS     267 // Disconnected Initiator  in 40MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ         274 //  Connected Responder    in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_DIS     260 //  Disconnected Responder in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ         247 //  Connected Responder    in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_DIS     251 //  Disconnected Responder in 40MHz (Ch 11) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ         440 // Connected Initiator     in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_DIS     440 // Disconnected Initiator  in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ         438 // Connected Initiator     in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_DIS     442 // Disconnected Initiator  in 40MHz (Ch 11) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ         82 //  Connected Responder    in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_DIS     82 //  Disconnected Responder in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ         80 //  Connected Responder    in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_DIS     75 //  Disconnected Responder in 40MHz (Ch 11) using 40MHz FTM
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
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ         712 // Connected Initiator     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_DIS     711 // Disconnected Initiator  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ         715 // Connected Initiator     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_DIS     714 // Disconnected Initiator  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ         714 // Connected Responder     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_DIS     714 // Disconnected Responder  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ         712 // Connected Responder     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_DIS     712 // Disconnected Responder  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ         611 // Connected Initiator     in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_DIS     610 // Disconnected Initiator  in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ         611 // Connected Initiator     in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_DIS     613 // Disconnected Initiator  in 40MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ         610 //  Connected Responder    in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_DIS     612 //  Disconnected Responder in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ         613 //  Connected Responder    in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_DIS     613 //  Disconnected Responder in 40MHz (Ch 11) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ         787 // Connected Initiator     in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_DIS     787 // Disconnected Initiator  in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ         790 // Connected Initiator     in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_DIS     790 // Disconnected Initiator  in 40MHz (Ch 11) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ         436 //  Connected Responder    in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_DIS     436 //  Disconnected Responder in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ         436 //  Connected Responder    in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_DIS     436 //  Disconnected Responder in 40MHz (Ch 11) using 40MHz FTM
#elif CONFIG_IDF_TARGET_ESP32C61
// 20 MHz FTM in 20MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ         859 // Connected Initiator     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_DIS     859 // Disconnected Initiator  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ         869 // Connected Initiator     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_DIS     869 // Disconnected Initiator  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ         867 // Connected Responder     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_DIS     867 // Disconnected Responder  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ         857 // Connected Responder     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_DIS     857 // Disconnected Responder  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ         746 // Connected Initiator     in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_DIS     744 // Disconnected Initiator  in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ         758 // Connected Initiator     in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_DIS     755 // Disconnected Initiator  in 40MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ         754 //  Connected Responder    in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_DIS     753 //  Disconnected Responder in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ         744 //  Connected Responder    in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_DIS     744 //  Disconnected Responder in 40MHz (Ch 11) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ         931 // Connected Initiator     in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_DIS     931 // Disconnected Initiator  in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ         931 // Connected Initiator     in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_DIS     931 // Disconnected Initiator  in 40MHz (Ch 11) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ         566 //  Connected Responder    in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_DIS     566 //  Disconnected Responder in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ         567 //  Connected Responder    in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_DIS     567 //  Disconnected Responder in 40MHz (Ch 11) using 40MHz FTM
#elif CONFIG_IDF_TARGET_ESP32C5
// 20 MHz FTM in 20MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ         929 // Connected Initiator     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_DIS     970 // Disconnected Initiator  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ         938 // Connected Initiator     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_DIS     938 // Disconnected Initiator  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ         972 // Connected Responder     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_DIS     976 // Disconnected Responder  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ         918 // Connected Responder     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_DIS     918 // Disconnected Responder  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ         817 // Connected Initiator     in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_DIS     813 // Disconnected Initiator  in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ         822 // Connected Initiator     in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_DIS     822 // Disconnected Initiator  in 40MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ         815 //  Connected Responder    in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_DIS     819 //  Disconnected Responder in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ         803 //  Connected Responder    in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_DIS     803 //  Disconnected Responder in 40MHz (Ch 11) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ         994 // Connected Initiator     in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_DIS     994 // Disconnected Initiator  in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ         993 // Connected Initiator     in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_DIS     992 // Disconnected Initiator  in 40MHz (Ch 11) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ         626 //  Connected Responder    in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_DIS     626 //  Disconnected Responder in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ         629 //  Connected Responder    in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_DIS     629 //  Disconnected Responder in 40MHz (Ch 11) using 40MHz FTM
#elif CONFIG_IDF_TARGET_ESP32S31
//TODO: Dummy values, remove or update when FTM loopback and auto-calibration is verified
// 20 MHz FTM in 20MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ         859 // Connected Initiator     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_DIS     859 // Disconnected Initiator  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ         869 // Connected Initiator     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_DIS     869 // Disconnected Initiator  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ         867 // Connected Responder     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_DIS     867 // Disconnected Responder  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ         857 // Connected Responder     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_DIS     857 // Disconnected Responder  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ         746 // Connected Initiator     in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_DIS     744 // Disconnected Initiator  in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ         758 // Connected Initiator     in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_DIS     755 // Disconnected Initiator  in 40MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ         754 //  Connected Responder    in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_DIS     753 //  Disconnected Responder in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ         744 //  Connected Responder    in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_DIS     744 //  Disconnected Responder in 40MHz (Ch 11) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ         931 // Connected Initiator     in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_DIS     931 // Disconnected Initiator  in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ         931 // Connected Initiator     in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_DIS     931 // Disconnected Initiator  in 40MHz (Ch 11) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ         566 //  Connected Responder    in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_DIS     566 //  Disconnected Responder in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ         567 //  Connected Responder    in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_DIS     567 //  Disconnected Responder in 40MHz (Ch 11) using 40MHz FTM
#endif

/********************************************** 5 GHz Values *******************************************************/
#if CONFIG_IDF_TARGET_ESP32C5
// 20 MHz FTM in 20MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_5G      968 // Connected Initiator     in 20MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_5G_DIS  968 // Disconnected Initiator  in 20MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_5G      969 // Connected Initiator     in 20MHz (Ch 149) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_5G_DIS  973 // Disconnected Initiator  in 20MHz (Ch 149) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_5G      966 // Connected Responder     in 20MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_5G_DIS  966 // Disconnected Responder  in 20MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_5G      966 // Connected Responder     in 20MHz (Ch 149) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_5G_DIS  962 // Disconnected Responder  in 20MHz (Ch 149) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_5G      798 // Connected Initiator     in 40MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_5G_DIS  831 // Disconnected Initiator  in 40MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_5G      798 // Connected Initiator     in 40MHz (Ch 149) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_5G_DIS  836 // Disconnected Initiator  in 40MHz (Ch 149) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_5G      828 //  Connected Responder    in 40MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_5G_DIS  828 //  Disconnected Responder in 40MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_5G      828 //  Connected Responder    in 40MHz (Ch 149) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_5G_DIS  824 //  Disconnected Responder in 40MHz (Ch 149) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_5G      816 // Connected Initiator     in 40MHz (Ch 36)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_5G_DIS  816 // Disconnected Initiator  in 40MHz (Ch 36)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_5G      818 // Connected Initiator     in 40MHz (Ch 149) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_5G_DIS  818 // Disconnected Initiator  in 40MHz (Ch 149) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_5G      840 //  Connected Responder    in 40MHz (Ch 36)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_5G_DIS  840 //  Disconnected Responder in 40MHz (Ch 36)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_5G      840 //  Connected Responder    in 40MHz (Ch 149) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_5G_DIS  840 //  Disconnected Responder in 40MHz (Ch 149) using 40MHz FTM
#endif

#endif /* CONFIG_ESP_WIFI_FTM_ENABLE */
