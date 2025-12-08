/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef FTM_CALIBRATION_DATA_H
#define FTM_CALIBRATION_DATA_H

#include "sdkconfig.h"

#if CONFIG_ESP_WIFI_FTM_ENABLE
/********************************************** 2.4 GHz Values *******************************************************/
#if CONFIG_IDF_TARGET_ESP32S2
// 20 MHz FTM in 20MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ         728 // Connected Initiator     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_DIS     732 // Disconnected Initiator  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ         733 // Connected Initiator     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_DIS     366 // Disconnected Initiator  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ         366 // Connected Responder     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_DIS     366 // Disconnected Responder  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ         225 // Connected Responder     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_DIS     225 // Disconnected Responder  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ         1798 // Connected Initiator     in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_DIS     589 // Disconnected Initiator  in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ         1745 // Connected Initiator     in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_DIS     1034 // Disconnected Initiator  in 40MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ         822 //  Connected Responder    in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_DIS     588 //  Disconnected Responder in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ         699 //  Connected Responder    in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_DIS     587 //  Disconnected Responder in 40MHz (Ch 11) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ         591 // Connected Initiator     in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_DIS     446 // Disconnected Initiator  in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ         442 // Connected Initiator     in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_DIS     443 // Disconnected Initiator  in 40MHz (Ch 11) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ         359 //  Connected Responder    in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_DIS     359 //  Disconnected Responder in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ         215 //  Connected Responder    in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_DIS     215 //  Disconnected Responder in 40MHz (Ch 11) using 40MHz FTM
#elif CONFIG_IDF_TARGET_ESP32C3
// 20 MHz FTM in 20MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ         975 // Connected Initiator     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_DIS     975 // Disconnected Initiator  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ         1046 // Connected Initiator     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_DIS     1046 // Disconnected Initiator  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ         439 // Connected Responder     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_DIS     439 // Disconnected Responder  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ         264 // Connected Responder     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_DIS     264 // Disconnected Responder  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ         975 // Connected Initiator     in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_DIS     814 // Disconnected Initiator  in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ         1046 // Connected Initiator     in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_DIS     930 // Disconnected Initiator  in 40MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ         822 //  Connected Responder    in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_DIS     815 //  Disconnected Responder in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ         699 //  Connected Responder    in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_DIS     699 //  Disconnected Responder in 40MHz (Ch 11) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ         701 // Connected Initiator     in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_DIS     701 // Disconnected Initiator  in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ         815 // Connected Initiator     in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_DIS     815 // Disconnected Initiator  in 40MHz (Ch 11) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ         431 //  Connected Responder    in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_DIS     431 //  Disconnected Responder in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ         253 //  Connected Responder    in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_DIS     253 //  Disconnected Responder in 40MHz (Ch 11) using 40MHz FTM
#elif CONFIG_IDF_TARGET_ESP32C2
// 20 MHz FTM in 20MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ         975 // Connected Initiator     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_DIS     975 // Disconnected Initiator  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ         1046 // Connected Initiator     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_DIS     1046 // Disconnected Initiator  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ         1951 // Connected Responder     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_DIS     1951 // Disconnected Responder  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ         1861 // Connected Responder     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_DIS     1861 // Disconnected Responder  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ         975 // Connected Initiator     in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_DIS     814 // Disconnected Initiator  in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ         1046 // Connected Initiator     in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_DIS     930 // Disconnected Initiator  in 40MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ         822 //  Connected Responder    in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_DIS     815 //  Disconnected Responder in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ         699 //  Connected Responder    in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_DIS     699 //  Disconnected Responder in 40MHz (Ch 11) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ         701 // Connected Initiator     in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_DIS     701 // Disconnected Initiator  in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ         815 // Connected Initiator     in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_DIS     815 // Disconnected Initiator  in 40MHz (Ch 11) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ         1626 //  Connected Responder    in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_DIS     1626 //  Disconnected Responder in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ         1626 //  Connected Responder    in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_DIS     1626 //  Disconnected Responder in 40MHz (Ch 11) using 40MHz FTM
#elif CONFIG_IDF_TARGET_ESP32S3
// 20 MHz FTM in 20MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ         975 // Connected Initiator     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_DIS     975 // Disconnected Initiator  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ         1046 // Connected Initiator     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_DIS     1046 // Disconnected Initiator  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ         444 // Connected Responder     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_DIS     444 // Disconnected Responder  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ         268 // Connected Responder     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_DIS     268 // Disconnected Responder  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ         975 // Connected Initiator     in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_DIS     814 // Disconnected Initiator  in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ         1046 // Connected Initiator     in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_DIS     930 // Disconnected Initiator  in 40MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ         822 //  Connected Responder    in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_DIS     815 //  Disconnected Responder in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ         699 //  Connected Responder    in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_DIS     699 //  Disconnected Responder in 40MHz (Ch 11) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ         701 // Connected Initiator     in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_DIS     701 // Disconnected Initiator  in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ         815 // Connected Initiator     in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_DIS     815 // Disconnected Initiator  in 40MHz (Ch 11) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ         440 //  Connected Responder    in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_DIS     440 //  Disconnected Responder in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ         261 //  Connected Responder    in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_DIS     261 //  Disconnected Responder in 40MHz (Ch 11) using 40MHz FTM
#elif CONFIG_IDF_TARGET_ESP32C6
// 20 MHz FTM in 20MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ         975 // Connected Initiator     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_DIS     975 // Disconnected Initiator  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ         1046 // Connected Initiator     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_DIS     1046 // Disconnected Initiator  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ         714 // Connected Responder     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_DIS     714 // Disconnected Responder  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ         617 // Connected Responder     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_DIS     617 // Disconnected Responder  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ         975 // Connected Initiator     in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_DIS     814 // Disconnected Initiator  in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ         1046 // Connected Initiator     in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_DIS     930 // Disconnected Initiator  in 40MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ         822 //  Connected Responder    in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_DIS     815 //  Disconnected Responder in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ         699 //  Connected Responder    in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_DIS     699 //  Disconnected Responder in 40MHz (Ch 11) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ         701 // Connected Initiator     in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_DIS     701 // Disconnected Initiator  in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ         815 // Connected Initiator     in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_DIS     815 // Disconnected Initiator  in 40MHz (Ch 11) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ         712 //  Connected Responder    in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_DIS     712 //  Disconnected Responder in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ         611 //  Connected Responder    in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_DIS     611 //  Disconnected Responder in 40MHz (Ch 11) using 40MHz FTM
#elif CONFIG_IDF_TARGET_ESP32C61
// 20 MHz FTM in 20MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ         749 // Connected Initiator     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_DIS     749 // Disconnected Initiator  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ         910 // Connected Initiator     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_DIS     910 // Disconnected Initiator  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ         867 // Connected Responder     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_DIS     867 // Disconnected Responder  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ         753 // Connected Responder     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_DIS     753 // Disconnected Responder  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ         749 // Connected Initiator     in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_DIS     636 // Disconnected Initiator  in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ         911 // Connected Initiator     in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_DIS     797 // Disconnected Initiator  in 40MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ         822 //  Connected Responder    in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_DIS     634 //  Disconnected Responder in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ         699 //  Connected Responder    in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_DIS     563 //  Disconnected Responder in 40MHz (Ch 11) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ         569 // Connected Initiator     in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_DIS     569 // Disconnected Initiator  in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ         685 // Connected Initiator     in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_DIS     684 // Disconnected Initiator  in 40MHz (Ch 11) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ         863 //  Connected Responder    in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_DIS     863 //  Disconnected Responder in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ         747 //  Connected Responder    in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_DIS     747 //  Disconnected Responder in 40MHz (Ch 11) using 40MHz FTM
#elif CONFIG_IDF_TARGET_ESP32C5
// 20 MHz FTM in 20MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ         975 // Connected Initiator     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_DIS     975 // Disconnected Initiator  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ         1046 // Connected Initiator     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_DIS     1046 // Disconnected Initiator  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ         976 // Connected Responder     in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_DIS     976 // Disconnected Responder  in 20MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ         815 // Connected Responder     in 20MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_DIS     815 // Disconnected Responder  in 20MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ         975 // Connected Initiator     in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_DIS     814 // Disconnected Initiator  in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ         1046 // Connected Initiator     in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_DIS     930 // Disconnected Initiator  in 40MHz (Ch 11) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ         822 //  Connected Responder    in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_DIS     815 //  Disconnected Responder in 40MHz (Ch 1)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ         699 //  Connected Responder    in 40MHz (Ch 11) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_DIS     699 //  Disconnected Responder in 40MHz (Ch 11) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ         701 // Connected Initiator     in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_DIS     701 // Disconnected Initiator  in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ         815 // Connected Initiator     in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_DIS     815 // Disconnected Initiator  in 40MHz (Ch 11) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ         925 //  Connected Responder    in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_DIS     925 //  Disconnected Responder in 40MHz (Ch 1)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ         811 //  Connected Responder    in 40MHz (Ch 11) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_DIS     811 //  Disconnected Responder in 40MHz (Ch 11) using 40MHz FTM
#endif

/********************************************** 5 GHz Values *******************************************************/
#if CONFIG_IDF_TARGET_ESP32C5
// 20 MHz FTM in 20MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_5G      975 // Connected Initiator     in 20MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20U_MHZ_5G_DIS  975 // Disconnected Initiator  in 20MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_5G      1046 // Connected Initiator     in 20MHz (Ch 149) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_20D_MHZ_5G_DIS  1046 // Disconnected Initiator  in 20MHz (Ch 149) using 20MHz FTM
// 20 MHz FTM in 20MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_5G      976 // Connected Responder     in 20MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20U_MHZ_5G_DIS  976 // Disconnected Responder  in 20MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_5G      815 // Connected Responder     in 20MHz (Ch 149) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_20D_MHZ_5G_DIS  815 // Disconnected Responder  in 20MHz (Ch 149) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_5G      822 // Connected Initiator     in 40MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40U_MHZ_5G_DIS  822 // Disconnected Initiator  in 40MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_5G      926 // Connected Initiator     in 40MHz (Ch 149) using 20MHz FTM
#define EST_PHY_INIT_FTM_COMP_20_40D_MHZ_5G_DIS  926 // Disconnected Initiator  in 40MHz (Ch 149) using 20MHz FTM
// 20 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_5G      822 //  Connected Responder    in 40MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40U_MHZ_5G_DIS  822 //  Disconnected Responder in 40MHz (Ch 36)  using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_5G      699 //  Connected Responder    in 40MHz (Ch 149) using 20MHz FTM
#define EST_PHY_RESP_FTM_COMP_20_40D_MHZ_5G_DIS  699 //  Disconnected Responder in 40MHz (Ch 149) using 20MHz FTM
// 40 MHz FTM in 40MHz PHY - Initiator Values
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_5G      703 // Connected Initiator     in 40MHz (Ch 36)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40U_MHZ_5G_DIS  703 // Disconnected Initiator  in 40MHz (Ch 36)  using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_5G      815 // Connected Initiator     in 40MHz (Ch 149) using 40MHz FTM
#define EST_PHY_INIT_FTM_COMP_40_40D_MHZ_5G_DIS  815 // Disconnected Initiator  in 40MHz (Ch 149) using 40MHz FTM
// 40 MHz FTM in 40MHz PHY - Responder Values
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_5G      925 //  Connected Responder    in 40MHz (Ch 36)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40U_MHZ_5G_DIS  925 //  Disconnected Responder in 40MHz (Ch 36)  using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_5G      811 //  Connected Responder    in 40MHz (Ch 149) using 40MHz FTM
#define EST_PHY_RESP_FTM_COMP_40_40D_MHZ_5G_DIS  811 //  Disconnected Responder in 40MHz (Ch 149) using 40MHz FTM
#endif

#endif /* CONFIG_ESP_WIFI_FTM_ENABLE */
#endif // FTM_CALIBRATION_DATA_H
