/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp_wifi_types_generic.h"
#include "esp_wifi.h"

#ifdef CONFIG_SOC_WIFI_SUPPORT_5G
typedef enum {
    ESP_WIFI_REGULATORY_TYPE_DEFAULT,
    ESP_WIFI_REGULATORY_TYPE_CE,
    ESP_WIFI_REGULATORY_TYPE_ACMA,
    ESP_WIFI_REGULATORY_TYPE_ANATEL,
    ESP_WIFI_REGULATORY_TYPE_ISED,
    ESP_WIFI_REGULATORY_TYPE_SRRC,
    ESP_WIFI_REGULATORY_TYPE_OFCA,
    ESP_WIFI_REGULATORY_TYPE_WPC,
    ESP_WIFI_REGULATORY_TYPE_MIC,
    ESP_WIFI_REGULATORY_TYPE_KCC,
    ESP_WIFI_REGULATORY_TYPE_IFETEL,
    ESP_WIFI_REGULATORY_TYPE_RCM,
    ESP_WIFI_REGULATORY_TYPE_NCC,
    ESP_WIFI_REGULATORY_TYPE_FCC,
    ESP_WIFI_REGULATORY_TYPE_AF,
    ESP_WIFI_REGULATORY_TYPE_AM,
    ESP_WIFI_REGULATORY_TYPE_AS,
    ESP_WIFI_REGULATORY_TYPE_AZ,
    ESP_WIFI_REGULATORY_TYPE_BB,
    ESP_WIFI_REGULATORY_TYPE_BD,
    ESP_WIFI_REGULATORY_TYPE_BH,
    ESP_WIFI_REGULATORY_TYPE_BN,
    ESP_WIFI_REGULATORY_TYPE_BO,
    ESP_WIFI_REGULATORY_TYPE_BS,
    ESP_WIFI_REGULATORY_TYPE_BW,
    ESP_WIFI_REGULATORY_TYPE_BZ,
    ESP_WIFI_REGULATORY_TYPE_CF,
    ESP_WIFI_REGULATORY_TYPE_CR,
    ESP_WIFI_REGULATORY_TYPE_CU,
    ESP_WIFI_REGULATORY_TYPE_DM,
    ESP_WIFI_REGULATORY_TYPE_DZ,
    ESP_WIFI_REGULATORY_TYPE_EC,
    ESP_WIFI_REGULATORY_TYPE_EG,
    ESP_WIFI_REGULATORY_TYPE_FO,
    ESP_WIFI_REGULATORY_TYPE_GB,
    ESP_WIFI_REGULATORY_TYPE_GD,
    ESP_WIFI_REGULATORY_TYPE_GT,
    ESP_WIFI_REGULATORY_TYPE_GU,
    ESP_WIFI_REGULATORY_TYPE_HR,
    ESP_WIFI_REGULATORY_TYPE_ID,
    ESP_WIFI_REGULATORY_TYPE_IR,
    ESP_WIFI_REGULATORY_TYPE_JO,
    ESP_WIFI_REGULATORY_TYPE_KE,
    ESP_WIFI_REGULATORY_TYPE_KN,
    ESP_WIFI_REGULATORY_TYPE_KP,
    ESP_WIFI_REGULATORY_TYPE_KW,
    ESP_WIFI_REGULATORY_TYPE_KY,
    ESP_WIFI_REGULATORY_TYPE_KZ,
    ESP_WIFI_REGULATORY_TYPE_LK,
    ESP_WIFI_REGULATORY_TYPE_MA,
    ESP_WIFI_REGULATORY_TYPE_MO,
    ESP_WIFI_REGULATORY_TYPE_MV,
    ESP_WIFI_REGULATORY_TYPE_MY,
    ESP_WIFI_REGULATORY_TYPE_NA,
    ESP_WIFI_REGULATORY_TYPE_OM,
    ESP_WIFI_REGULATORY_TYPE_PA,
    ESP_WIFI_REGULATORY_TYPE_PH,
    ESP_WIFI_REGULATORY_TYPE_PK,
    ESP_WIFI_REGULATORY_TYPE_QA,
    ESP_WIFI_REGULATORY_TYPE_RS,
    ESP_WIFI_REGULATORY_TYPE_RU,
    ESP_WIFI_REGULATORY_TYPE_SG,
    ESP_WIFI_REGULATORY_TYPE_SV,
    ESP_WIFI_REGULATORY_TYPE_SX,
    ESP_WIFI_REGULATORY_TYPE_SY,
    ESP_WIFI_REGULATORY_TYPE_TG,
    ESP_WIFI_REGULATORY_TYPE_TR,
    ESP_WIFI_REGULATORY_TYPE_UA,
    ESP_WIFI_REGULATORY_TYPE_VE,
    ESP_WIFI_REGULATORY_TYPE_VN,
    ESP_WIFI_REGULATORY_TYPE_WS,
    ESP_WIFI_REGULATORY_TYPE_YE,
    ESP_WIFI_REGULATORY_TYPE_ZA,
    ESP_WIFI_REGULATORY_TYPE_MAX,
} esp_wifi_regulatory_type_t;

/*
 * regdomain_table: ISO alpha-2 country codes and their regulatory profile indices.
 * Last row is a sentinel (not a real country code).
 */
const wifi_regdomain_t regdomain_table[] = {
    {"01", ESP_WIFI_REGULATORY_TYPE_DEFAULT},
    {"EU", ESP_WIFI_REGULATORY_TYPE_CE},
    {"AD", ESP_WIFI_REGULATORY_TYPE_CE},
    {"AE", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"AF", ESP_WIFI_REGULATORY_TYPE_AF},
    {"AI", ESP_WIFI_REGULATORY_TYPE_AF},
    {"AL", ESP_WIFI_REGULATORY_TYPE_CE},
    {"AM", ESP_WIFI_REGULATORY_TYPE_AM},
    {"AN", ESP_WIFI_REGULATORY_TYPE_AF},
    {"AR", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"AS", ESP_WIFI_REGULATORY_TYPE_AS},
    {"AT", ESP_WIFI_REGULATORY_TYPE_CE},
    {"AU", ESP_WIFI_REGULATORY_TYPE_ACMA},
    {"AW", ESP_WIFI_REGULATORY_TYPE_AF},
    {"AZ", ESP_WIFI_REGULATORY_TYPE_AZ},
    {"BA", ESP_WIFI_REGULATORY_TYPE_CE},
    {"BB", ESP_WIFI_REGULATORY_TYPE_BB},
    {"BD", ESP_WIFI_REGULATORY_TYPE_BD},
    {"BE", ESP_WIFI_REGULATORY_TYPE_CE},
    {"BF", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"BG", ESP_WIFI_REGULATORY_TYPE_CE},
    {"BH", ESP_WIFI_REGULATORY_TYPE_BH},
    {"BL", ESP_WIFI_REGULATORY_TYPE_AF},
    {"BM", ESP_WIFI_REGULATORY_TYPE_AS},
    {"BN", ESP_WIFI_REGULATORY_TYPE_BN},
    {"BO", ESP_WIFI_REGULATORY_TYPE_BO},
    {"BR", ESP_WIFI_REGULATORY_TYPE_ANATEL},
    {"BS", ESP_WIFI_REGULATORY_TYPE_BS},
    {"BT", ESP_WIFI_REGULATORY_TYPE_AF},
    {"BW", ESP_WIFI_REGULATORY_TYPE_BW},
    {"BY", ESP_WIFI_REGULATORY_TYPE_AF},
    {"BZ", ESP_WIFI_REGULATORY_TYPE_BZ},
    {"CA", ESP_WIFI_REGULATORY_TYPE_ISED},
    {"CF", ESP_WIFI_REGULATORY_TYPE_CF},
    {"CH", ESP_WIFI_REGULATORY_TYPE_CE},
    {"CI", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"CL", ESP_WIFI_REGULATORY_TYPE_BN},
    {"CN", ESP_WIFI_REGULATORY_TYPE_SRRC},
    {"CO", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"CR", ESP_WIFI_REGULATORY_TYPE_CR},
    {"CU", ESP_WIFI_REGULATORY_TYPE_CU},
    {"CX", ESP_WIFI_REGULATORY_TYPE_BS},
    {"CY", ESP_WIFI_REGULATORY_TYPE_CE},
    {"CZ", ESP_WIFI_REGULATORY_TYPE_CE},
    {"DE", ESP_WIFI_REGULATORY_TYPE_CE},
    {"DK", ESP_WIFI_REGULATORY_TYPE_CE},
    {"DM", ESP_WIFI_REGULATORY_TYPE_DM},
    {"DO", ESP_WIFI_REGULATORY_TYPE_DM},
    {"DZ", ESP_WIFI_REGULATORY_TYPE_DZ},
    {"EC", ESP_WIFI_REGULATORY_TYPE_EC},
    {"EE", ESP_WIFI_REGULATORY_TYPE_CE},
    {"EG", ESP_WIFI_REGULATORY_TYPE_EG},
    {"ES", ESP_WIFI_REGULATORY_TYPE_CE},
    {"ET", ESP_WIFI_REGULATORY_TYPE_AF},
    {"FI", ESP_WIFI_REGULATORY_TYPE_CE},
    {"FM", ESP_WIFI_REGULATORY_TYPE_AS},
    {"FO", ESP_WIFI_REGULATORY_TYPE_FO},
    {"FR", ESP_WIFI_REGULATORY_TYPE_CE},
    {"GB", ESP_WIFI_REGULATORY_TYPE_GB},
    {"GD", ESP_WIFI_REGULATORY_TYPE_GD},
    {"GE", ESP_WIFI_REGULATORY_TYPE_AZ},
    {"GF", ESP_WIFI_REGULATORY_TYPE_AF},
    {"GH", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"GI", ESP_WIFI_REGULATORY_TYPE_FO},
    {"GL", ESP_WIFI_REGULATORY_TYPE_AF},
    {"GP", ESP_WIFI_REGULATORY_TYPE_AF},
    {"GR", ESP_WIFI_REGULATORY_TYPE_CE},
    {"GT", ESP_WIFI_REGULATORY_TYPE_GT},
    {"GU", ESP_WIFI_REGULATORY_TYPE_GU},
    {"GY", ESP_WIFI_REGULATORY_TYPE_NCC},
    {"HK", ESP_WIFI_REGULATORY_TYPE_OFCA},
    {"HN", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"HR", ESP_WIFI_REGULATORY_TYPE_HR},
    {"HT", ESP_WIFI_REGULATORY_TYPE_AS},
    {"HU", ESP_WIFI_REGULATORY_TYPE_CE},
    {"ID", ESP_WIFI_REGULATORY_TYPE_ID},
    {"IE", ESP_WIFI_REGULATORY_TYPE_CE},
    {"IL", ESP_WIFI_REGULATORY_TYPE_CE},
    {"IM", ESP_WIFI_REGULATORY_TYPE_FO},
    {"IN", ESP_WIFI_REGULATORY_TYPE_WPC},
    {"IR", ESP_WIFI_REGULATORY_TYPE_IR},
    {"IS", ESP_WIFI_REGULATORY_TYPE_CE},
    {"IT", ESP_WIFI_REGULATORY_TYPE_CE},
    {"JM", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"JO", ESP_WIFI_REGULATORY_TYPE_JO},
    {"JP", ESP_WIFI_REGULATORY_TYPE_MIC},
    {"KE", ESP_WIFI_REGULATORY_TYPE_KE},
    {"KH", ESP_WIFI_REGULATORY_TYPE_AF},
    {"KN", ESP_WIFI_REGULATORY_TYPE_KN},
    {"KP", ESP_WIFI_REGULATORY_TYPE_KP},
    {"KR", ESP_WIFI_REGULATORY_TYPE_KCC},
    {"KW", ESP_WIFI_REGULATORY_TYPE_KW},
    {"KY", ESP_WIFI_REGULATORY_TYPE_KY},
    {"KZ", ESP_WIFI_REGULATORY_TYPE_KZ},
    {"LB", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"LC", ESP_WIFI_REGULATORY_TYPE_KN},
    {"LI", ESP_WIFI_REGULATORY_TYPE_CE},
    {"LK", ESP_WIFI_REGULATORY_TYPE_LK},
    {"LS", ESP_WIFI_REGULATORY_TYPE_AF},
    {"LT", ESP_WIFI_REGULATORY_TYPE_CE},
    {"LU", ESP_WIFI_REGULATORY_TYPE_CE},
    {"LV", ESP_WIFI_REGULATORY_TYPE_CE},
    {"MA", ESP_WIFI_REGULATORY_TYPE_MA},
    {"MC", ESP_WIFI_REGULATORY_TYPE_CE},
    {"MD", ESP_WIFI_REGULATORY_TYPE_CE},
    {"ME", ESP_WIFI_REGULATORY_TYPE_CE},
    {"MF", ESP_WIFI_REGULATORY_TYPE_AF},
    {"MH", ESP_WIFI_REGULATORY_TYPE_AS},
    {"MK", ESP_WIFI_REGULATORY_TYPE_CE},
    {"MN", ESP_WIFI_REGULATORY_TYPE_BS},
    {"MO", ESP_WIFI_REGULATORY_TYPE_MO},
    {"MP", ESP_WIFI_REGULATORY_TYPE_AS},
    {"MQ", ESP_WIFI_REGULATORY_TYPE_AF},
    {"MR", ESP_WIFI_REGULATORY_TYPE_AF},
    {"MT", ESP_WIFI_REGULATORY_TYPE_CE},
    {"MU", ESP_WIFI_REGULATORY_TYPE_BS},
    {"MV", ESP_WIFI_REGULATORY_TYPE_MV},
    {"MW", ESP_WIFI_REGULATORY_TYPE_AF},
    {"MX", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"MY", ESP_WIFI_REGULATORY_TYPE_MY},
    {"NA", ESP_WIFI_REGULATORY_TYPE_NA},
    {"NG", ESP_WIFI_REGULATORY_TYPE_BO},
    {"NI", ESP_WIFI_REGULATORY_TYPE_AS},
    {"NL", ESP_WIFI_REGULATORY_TYPE_CE},
    {"NO", ESP_WIFI_REGULATORY_TYPE_CE},
    {"NP", ESP_WIFI_REGULATORY_TYPE_BN},
    {"NZ", ESP_WIFI_REGULATORY_TYPE_RCM},
    {"OM", ESP_WIFI_REGULATORY_TYPE_OM},
    {"PA", ESP_WIFI_REGULATORY_TYPE_PA},
    {"PE", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"PF", ESP_WIFI_REGULATORY_TYPE_AF},
    {"PG", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"PH", ESP_WIFI_REGULATORY_TYPE_PH},
    {"PK", ESP_WIFI_REGULATORY_TYPE_PK},
    {"PL", ESP_WIFI_REGULATORY_TYPE_CE},
    {"PM", ESP_WIFI_REGULATORY_TYPE_AF},
    {"PR", ESP_WIFI_REGULATORY_TYPE_GD},
    {"PT", ESP_WIFI_REGULATORY_TYPE_CE},
    {"PW", ESP_WIFI_REGULATORY_TYPE_AS},
    {"PY", ESP_WIFI_REGULATORY_TYPE_BS},
    {"QA", ESP_WIFI_REGULATORY_TYPE_QA},
    {"RE", ESP_WIFI_REGULATORY_TYPE_AF},
    {"RO", ESP_WIFI_REGULATORY_TYPE_CE},
    {"RS", ESP_WIFI_REGULATORY_TYPE_RS},
    {"RU", ESP_WIFI_REGULATORY_TYPE_RU},
    {"RW", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"SA", ESP_WIFI_REGULATORY_TYPE_AF},
    {"SE", ESP_WIFI_REGULATORY_TYPE_CE},
    {"SG", ESP_WIFI_REGULATORY_TYPE_SG},
    {"SI", ESP_WIFI_REGULATORY_TYPE_CE},
    {"SK", ESP_WIFI_REGULATORY_TYPE_CE},
    {"SM", ESP_WIFI_REGULATORY_TYPE_FO},
    {"SN", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"SR", ESP_WIFI_REGULATORY_TYPE_AF},
    {"SV", ESP_WIFI_REGULATORY_TYPE_SV},
    {"SX", ESP_WIFI_REGULATORY_TYPE_SX},
    {"SY", ESP_WIFI_REGULATORY_TYPE_SY},
    {"TC", ESP_WIFI_REGULATORY_TYPE_BS},
    {"TD", ESP_WIFI_REGULATORY_TYPE_AF},
    {"TG", ESP_WIFI_REGULATORY_TYPE_TG},
    {"TH", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"TN", ESP_WIFI_REGULATORY_TYPE_MA},
    {"TR", ESP_WIFI_REGULATORY_TYPE_TR},
    {"TT", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"TW", ESP_WIFI_REGULATORY_TYPE_NCC},
    {"TZ", ESP_WIFI_REGULATORY_TYPE_BW},
    {"UA", ESP_WIFI_REGULATORY_TYPE_UA},
    {"UG", ESP_WIFI_REGULATORY_TYPE_BS},
    {"US", ESP_WIFI_REGULATORY_TYPE_FCC},
    {"UY", ESP_WIFI_REGULATORY_TYPE_BB},
    {"UZ", ESP_WIFI_REGULATORY_TYPE_MA},
    {"VA", ESP_WIFI_REGULATORY_TYPE_FO},
    {"VC", ESP_WIFI_REGULATORY_TYPE_AF},
    {"VE", ESP_WIFI_REGULATORY_TYPE_VE},
    {"VI", ESP_WIFI_REGULATORY_TYPE_AS},
    {"VN", ESP_WIFI_REGULATORY_TYPE_VN},
    {"VU", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"WF", ESP_WIFI_REGULATORY_TYPE_AF},
    {"WS", ESP_WIFI_REGULATORY_TYPE_WS},
    {"YE", ESP_WIFI_REGULATORY_TYPE_YE},
    {"YT", ESP_WIFI_REGULATORY_TYPE_AF},
    {"ZA", ESP_WIFI_REGULATORY_TYPE_ZA},
    {"ZW", ESP_WIFI_REGULATORY_TYPE_AF},
    {"##", ESP_WIFI_REGULATORY_TYPE_MAX}, /* Sentinel: end of table; not a real country code — do not use as wifi_country_t.cc. */
};

const wifi_regulatory_t regulatory_data[] = {
    /* ESP_WIFI_REGULATORY_TYPE_DEFAULT */
    {
        3,  /* Number of rules */
        {
            { 1, 11, 2, 20, 0, 0 },  /* chan:1~11, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 20, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:20(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_CE */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 26, 1, 0 },  /* chan:100~140, max_bw:160M, max_power:26(dBm), dfs:1, reserved.*/
            { 149, 173, 3, 13, 0, 0 },  /* chan:149~173, max_bw:80M, max_power:13(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_ACMA */
    {
        7,  /* Number of rules */
        {
            { 1, 13, 2, 36, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:36(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
            { 100, 116, 3, 26, 1, 0 },  /* chan:100~116, max_bw:80M, max_power:26(dBm), dfs:1, reserved.*/
            { 132, 144, 3, 26, 1, 0 },  /* chan:132~144, max_bw:80M, max_power:26(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 36, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:36(dBm), dfs:0, reserved.*/
            { 173, 173, 1, 13, 0, 0 },  /* chan:173~173, max_bw:20M, max_power:13(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_ANATEL */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 30, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:30(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 27, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:27(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 27, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:27(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 27, 1, 0 },  /* chan:100~140, max_bw:160M, max_power:27(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 30, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_ISED */
    {
        6,  /* Number of rules */
        {
            { 1, 13, 2, 36, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:36(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 26, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:26(dBm), dfs:1, reserved.*/
            { 100, 144, 4, 26, 1, 0 },  /* chan:100~144, max_bw:160M, max_power:26(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 36, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:36(dBm), dfs:0, reserved.*/
            { 173, 177, 2, 27, 0, 0 },  /* chan:173~177, max_bw:40M, max_power:27(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_SRRC */
    {
        4,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 33, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:33(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_OFCA */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 36, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:36(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 23, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:23(dBm), dfs:1, reserved.*/
            { 100, 144, 4, 27, 1, 0 },  /* chan:100~144, max_bw:160M, max_power:27(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 36, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:36(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_WPC */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 30, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:30(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 30, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 24, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:24(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 24, 1, 0 },  /* chan:100~140, max_bw:160M, max_power:24(dBm), dfs:1, reserved.*/
            { 149, 173, 3, 30, 0, 0 },  /* chan:149~173, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_MIC */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 14, 14, 1, 20, 0, 0 },  /* chan:14~14, max_bw:20M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 20, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:20(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
            { 100, 144, 4, 23, 1, 0 },  /* chan:100~144, max_bw:160M, max_power:23(dBm), dfs:1, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_KCC */
    {
        6,  /* Number of rules */
        {
            { 1, 13, 2, 23, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:23(dBm), dfs:0, reserved.*/
            { 36, 44, 2, 23, 0, 0 },  /* chan:36~44, max_bw:40M, max_power:23(dBm), dfs:0, reserved.*/
            { 48, 48, 1, 17, 0, 0 },  /* chan:48~48, max_bw:20M, max_power:17(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 20, 1, 0 },  /* chan:100~140, max_bw:160M, max_power:20(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 23, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_IFETEL */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 17, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:17(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 24, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:24(dBm), dfs:1, reserved.*/
            { 100, 144, 4, 24, 1, 0 },  /* chan:100~144, max_bw:160M, max_power:24(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 30, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_RCM */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 36, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:36(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 30, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 27, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:27(dBm), dfs:1, reserved.*/
            { 100, 144, 4, 27, 1, 0 },  /* chan:100~144, max_bw:160M, max_power:27(dBm), dfs:1, reserved.*/
            { 149, 173, 3, 36, 0, 0 },  /* chan:149~173, max_bw:80M, max_power:36(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_NCC */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 30, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:30(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 23, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:23(dBm), dfs:1, reserved.*/
            { 100, 144, 4, 23, 1, 0 },  /* chan:100~144, max_bw:160M, max_power:23(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 30, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_FCC */
    {
        6,  /* Number of rules */
        {
            { 1, 11, 2, 30, 0, 0 },  /* chan:1~11, max_bw:40M, max_power:30(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 24, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:24(dBm), dfs:1, reserved.*/
            { 100, 144, 4, 24, 1, 0 },  /* chan:100~144, max_bw:160M, max_power:24(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 30, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
            { 173, 177, 2, 27, 0, 0 },  /* chan:173~177, max_bw:40M, max_power:27(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_AF */
    {
        4,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 20, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:20(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 27, 1, 0 },  /* chan:100~140, max_bw:160M, max_power:27(dBm), dfs:1, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_AM */
    {
        3,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 64, 4, 17, 1, 0 },  /* chan:36~64, max_bw:160M, max_power:17(dBm), dfs:1, reserved.*/
            { 100, 173, 4, 17, 1, 0 },  /* chan:100~173, max_bw:160M, max_power:17(dBm), dfs:1, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_AS */
    {
        5,  /* Number of rules */
        {
            { 1, 11, 2, 30, 0, 0 },  /* chan:1~11, max_bw:40M, max_power:30(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 24, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:24(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 24, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:24(dBm), dfs:1, reserved.*/
            { 100, 144, 4, 24, 1, 0 },  /* chan:100~144, max_bw:160M, max_power:24(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 30, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_AZ */
    {
        3,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 18, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:18(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 18, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:18(dBm), dfs:1, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_BB */
    {
        4,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 23, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:23(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 30, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_BD */
    {
        2,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 149, 165, 3, 30, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_BH */
    {
        4,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 64, 3, 23, 1, 0 },  /* chan:36~64, max_bw:80M, max_power:23(dBm), dfs:1, reserved.*/
            { 100, 140, 3, 27, 1, 0 },  /* chan:100~140, max_bw:80M, max_power:27(dBm), dfs:1, reserved.*/
            { 149, 173, 3, 24, 1, 0 },  /* chan:149~173, max_bw:80M, max_power:24(dBm), dfs:1, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_BN */
    {
        4,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 20, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:20(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 20, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:20(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_BO */
    {
        3,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 30, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:30(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 30, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_BS */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 24, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:24(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 24, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:24(dBm), dfs:1, reserved.*/
            { 100, 144, 4, 24, 1, 0 },  /* chan:100~144, max_bw:160M, max_power:24(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 30, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_BW */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 23, 1, 0 },  /* chan:100~140, max_bw:160M, max_power:23(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 24, 1, 0 },  /* chan:149~165, max_bw:80M, max_power:24(dBm), dfs:1, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_BZ */
    {
        2,  /* Number of rules */
        {
            { 1, 13, 2, 30, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:30(dBm), dfs:0, reserved.*/
            { 149, 165, 3, 30, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_CF */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 2, 17, 0, 0 },  /* chan:36~48, max_bw:40M, max_power:17(dBm), dfs:0, reserved.*/
            { 52, 64, 2, 24, 1, 0 },  /* chan:52~64, max_bw:40M, max_power:24(dBm), dfs:1, reserved.*/
            { 100, 144, 2, 24, 1, 0 },  /* chan:100~144, max_bw:40M, max_power:24(dBm), dfs:1, reserved.*/
            { 149, 165, 2, 30, 0, 0 },  /* chan:149~165, max_bw:40M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_CR */
    {
        6,  /* Number of rules */
        {
            { 1, 13, 2, 36, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:36(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 30, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 30, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:30(dBm), dfs:1, reserved.*/
            { 100, 144, 4, 30, 1, 0 },  /* chan:100~144, max_bw:160M, max_power:30(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 36, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:36(dBm), dfs:0, reserved.*/
            { 173, 177, 2, 36, 0, 0 },  /* chan:173~177, max_bw:40M, max_power:36(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_CU */
    {
        4,  /* Number of rules */
        {
            { 1, 13, 2, 23, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:23(dBm), dfs:0, reserved.*/
            { 36, 64, 3, 23, 0, 0 },  /* chan:36~64, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 100, 140, 3, 23, 0, 0 },  /* chan:100~140, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 149, 165, 3, 23, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_DM */
    {
        4,  /* Number of rules */
        {
            { 1, 11, 2, 30, 0, 0 },  /* chan:1~11, max_bw:40M, max_power:30(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 17, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:17(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 23, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:23(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 30, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_DZ */
    {
        4,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 23, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:23(dBm), dfs:1, reserved.*/
            { 100, 132, 4, 23, 1, 0 },  /* chan:100~132, max_bw:160M, max_power:23(dBm), dfs:1, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_EC */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 30, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:30(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 16, 1, 0 },  /* chan:36~48, max_bw:80M, max_power:16(dBm), dfs:1, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 20, 1, 0 },  /* chan:100~140, max_bw:160M, max_power:20(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 30, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_EG */
    {
        3,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_FO */
    {
        4,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 26, 1, 0 },  /* chan:100~140, max_bw:160M, max_power:26(dBm), dfs:1, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_GB */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
            { 100, 144, 4, 26, 1, 0 },  /* chan:100~144, max_bw:160M, max_power:26(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 23, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_GD */
    {
        5,  /* Number of rules */
        {
            { 1, 11, 2, 30, 0, 0 },  /* chan:1~11, max_bw:40M, max_power:30(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 17, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:17(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 24, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:24(dBm), dfs:1, reserved.*/
            { 100, 144, 4, 24, 1, 0 },  /* chan:100~144, max_bw:160M, max_power:24(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 30, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_GT */
    {
        4,  /* Number of rules */
        {
            { 1, 13, 2, 26, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:26(dBm), dfs:0, reserved.*/
            { 36, 64, 3, 23, 0, 0 },  /* chan:36~64, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 100, 140, 4, 23, 0, 0 },  /* chan:100~140, max_bw:160M, max_power:23(dBm), dfs:0, reserved.*/
            { 149, 165, 3, 26, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:26(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_GU */
    {
        5,  /* Number of rules */
        {
            { 1, 11, 2, 30, 0, 0 },  /* chan:1~11, max_bw:40M, max_power:30(dBm), dfs:0, reserved.*/
            { 36, 48, 1, 17, 0, 0 },  /* chan:36~48, max_bw:20M, max_power:17(dBm), dfs:0, reserved.*/
            { 52, 64, 1, 24, 1, 0 },  /* chan:52~64, max_bw:20M, max_power:24(dBm), dfs:1, reserved.*/
            { 100, 144, 1, 24, 1, 0 },  /* chan:100~144, max_bw:20M, max_power:24(dBm), dfs:1, reserved.*/
            { 149, 165, 1, 30, 0, 0 },  /* chan:149~165, max_bw:20M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_HR */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 23, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:23(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 26, 1, 0 },  /* chan:100~140, max_bw:160M, max_power:26(dBm), dfs:1, reserved.*/
            { 149, 173, 3, 13, 0, 0 },  /* chan:149~173, max_bw:80M, max_power:13(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_ID */
    {
        3,  /* Number of rules */
        {
            { 1, 13, 2, 26, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:26(dBm), dfs:0, reserved.*/
            { 36, 64, 4, 23, 0, 0 },  /* chan:36~64, max_bw:160M, max_power:23(dBm), dfs:0, reserved.*/
            { 149, 161, 3, 23, 0, 0 },  /* chan:149~161, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_IR */
    {
        4,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 23, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:23(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 27, 1, 0 },  /* chan:100~140, max_bw:160M, max_power:27(dBm), dfs:1, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_JO */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 23, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:23(dBm), dfs:1, reserved.*/
            { 100, 140, 3, 27, 1, 0 },  /* chan:100~140, max_bw:80M, max_power:27(dBm), dfs:1, reserved.*/
            { 149, 173, 3, 23, 0, 0 },  /* chan:149~173, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_KE */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 33, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:33(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 17, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:17(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 17, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:17(dBm), dfs:1, reserved.*/
            { 100, 140, 3, 24, 1, 0 },  /* chan:100~140, max_bw:80M, max_power:24(dBm), dfs:1, reserved.*/
            { 149, 173, 2, 24, 1, 0 },  /* chan:149~173, max_bw:40M, max_power:24(dBm), dfs:1, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_KN */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 20, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:20(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 30, 1, 0 },  /* chan:100~140, max_bw:160M, max_power:30(dBm), dfs:1, reserved.*/
            { 149, 161, 3, 30, 0, 0 },  /* chan:149~161, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_KP */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 1, 20, 0, 0 },  /* chan:1~13, max_bw:20M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 1, 20, 0, 0 },  /* chan:36~48, max_bw:20M, max_power:20(dBm), dfs:0, reserved.*/
            { 52, 64, 1, 20, 1, 0 },  /* chan:52~64, max_bw:20M, max_power:20(dBm), dfs:1, reserved.*/
            { 100, 124, 1, 30, 1, 0 },  /* chan:100~124, max_bw:20M, max_power:30(dBm), dfs:1, reserved.*/
            { 149, 161, 1, 30, 0, 0 },  /* chan:149~161, max_bw:20M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_KW */
    {
        4,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 17, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:17(dBm), dfs:1, reserved.*/
            { 100, 161, 4, 24, 1, 0 },  /* chan:100~161, max_bw:160M, max_power:24(dBm), dfs:1, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_KY */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 30, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:30(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 23, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:23(dBm), dfs:1, reserved.*/
            { 100, 144, 4, 23, 1, 0 },  /* chan:100~144, max_bw:160M, max_power:23(dBm), dfs:1, reserved.*/
            { 149, 173, 3, 30, 0, 0 },  /* chan:149~173, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_KZ */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 20, 1, 0 },  /* chan:100~140, max_bw:160M, max_power:20(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 20, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:20(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_LK */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 1, 17, 0, 0 },  /* chan:36~48, max_bw:20M, max_power:17(dBm), dfs:0, reserved.*/
            { 52, 64, 1, 24, 1, 0 },  /* chan:52~64, max_bw:20M, max_power:24(dBm), dfs:1, reserved.*/
            { 100, 144, 1, 24, 1, 0 },  /* chan:100~144, max_bw:20M, max_power:24(dBm), dfs:1, reserved.*/
            { 149, 165, 1, 30, 0, 0 },  /* chan:149~165, max_bw:20M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_MA */
    {
        3,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 20, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:20(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_MO */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 23, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:23(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 23, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:23(dBm), dfs:1, reserved.*/
            { 100, 144, 4, 30, 1, 0 },  /* chan:100~144, max_bw:160M, max_power:30(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 30, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_MV */
    {
        4,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 20, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:20(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_MY */
    {
        5,  /* Number of rules */
        {
            { 1, 14, 2, 26, 0, 0 },  /* chan:1~14, max_bw:40M, max_power:26(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 30, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 23, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:23(dBm), dfs:1, reserved.*/
            { 100, 128, 4, 23, 1, 0 },  /* chan:100~128, max_bw:160M, max_power:23(dBm), dfs:1, reserved.*/
            { 149, 173, 3, 30, 0, 0 },  /* chan:149~173, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_NA */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 20, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:20(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 21, 1, 0 },  /* chan:100~140, max_bw:160M, max_power:21(dBm), dfs:1, reserved.*/
            { 149, 173, 3, 24, 1, 0 },  /* chan:149~173, max_bw:80M, max_power:24(dBm), dfs:1, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_OM */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 27, 1, 0 },  /* chan:100~140, max_bw:160M, max_power:27(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 28, 1, 0 },  /* chan:149~165, max_bw:80M, max_power:28(dBm), dfs:1, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_PA */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 36, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:36(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 36, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:36(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 30, 0, 0 },  /* chan:52~64, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
            { 100, 140, 4, 30, 0, 0 },  /* chan:100~140, max_bw:160M, max_power:30(dBm), dfs:0, reserved.*/
            { 149, 165, 3, 36, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:36(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_PH */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 23, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:23(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 24, 1, 0 },  /* chan:100~140, max_bw:160M, max_power:24(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 24, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:24(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_PK */
    {
        2,  /* Number of rules */
        {
            { 1, 14, 2, 30, 0, 0 },  /* chan:1~14, max_bw:40M, max_power:30(dBm), dfs:0, reserved.*/
            { 149, 173, 3, 30, 0, 0 },  /* chan:149~173, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_QA */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 23, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:23(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 20, 1, 0 },  /* chan:100~140, max_bw:160M, max_power:20(dBm), dfs:1, reserved.*/
            { 149, 173, 3, 20, 1, 0 },  /* chan:149~173, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_RS */
    {
        6,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 23, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:23(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 27, 1, 0 },  /* chan:100~140, max_bw:160M, max_power:27(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 24, 1, 0 },  /* chan:149~165, max_bw:80M, max_power:24(dBm), dfs:1, reserved.*/
            { 173, 173, 1, 24, 0, 0 },  /* chan:173~173, max_bw:20M, max_power:24(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_RU */
    {
        3,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 64, 4, 20, 0, 0 },  /* chan:36~64, max_bw:160M, max_power:20(dBm), dfs:0, reserved.*/
            { 132, 165, 4, 23, 0, 0 },  /* chan:132~165, max_bw:160M, max_power:23(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_SG */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 23, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:23(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
            { 100, 144, 4, 26, 1, 0 },  /* chan:100~144, max_bw:160M, max_power:26(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 30, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_SV */
    {
        4,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 1, 17, 0, 0 },  /* chan:36~48, max_bw:20M, max_power:17(dBm), dfs:0, reserved.*/
            { 52, 64, 1, 23, 1, 0 },  /* chan:52~64, max_bw:20M, max_power:23(dBm), dfs:1, reserved.*/
            { 149, 165, 1, 30, 0, 0 },  /* chan:149~165, max_bw:20M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_SX */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 16, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:16(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 21, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:21(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 23, 1, 0 },  /* chan:100~140, max_bw:160M, max_power:23(dBm), dfs:1, reserved.*/
            { 149, 173, 3, 24, 1, 0 },  /* chan:149~173, max_bw:80M, max_power:24(dBm), dfs:1, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_SY */
    {
        6,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 23, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:23(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 23, 1, 0 },  /* chan:100~140, max_bw:160M, max_power:23(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 23, 1, 0 },  /* chan:149~165, max_bw:80M, max_power:23(dBm), dfs:1, reserved.*/
            { 173, 173, 1, 23, 0, 0 },  /* chan:173~173, max_bw:20M, max_power:23(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_TG */
    {
        3,  /* Number of rules */
        {
            { 1, 13, 2, 20, 1, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:1, reserved.*/
            { 36, 64, 3, 20, 1, 0 },  /* chan:36~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
            { 100, 165, 4, 27, 1, 0 },  /* chan:100~165, max_bw:160M, max_power:27(dBm), dfs:1, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_TR */
    {
        4,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 27, 1, 0 },  /* chan:100~140, max_bw:160M, max_power:27(dBm), dfs:1, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_UA */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 20, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:20(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 20, 1, 0 },  /* chan:100~140, max_bw:160M, max_power:20(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 20, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:20(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_VE */
    {
        4,  /* Number of rules */
        {
            { 1, 13, 2, 30, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:30(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 23, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:23(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 30, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_VN */
    {
        5,  /* Number of rules */
        {
            { 1, 13, 2, 23, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:23(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 26, 1, 0 },  /* chan:100~140, max_bw:160M, max_power:26(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 30, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_WS */
    {
        4,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 2, 20, 0, 0 },  /* chan:36~48, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 52, 64, 2, 20, 1, 0 },  /* chan:52~64, max_bw:40M, max_power:20(dBm), dfs:1, reserved.*/
            { 100, 140, 2, 27, 1, 0 },  /* chan:100~140, max_bw:40M, max_power:27(dBm), dfs:1, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_YE */
    {
        1,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_ZA */
    {
        4,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 20, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:20(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 20, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:20(dBm), dfs:1, reserved.*/
            { 100, 140, 4, 30, 0, 0 },  /* chan:100~140, max_bw:160M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
};
#else
typedef enum {
    ESP_WIFI_REGULATORY_TYPE_DEFAULT,
    ESP_WIFI_REGULATORY_TYPE_CE,
    ESP_WIFI_REGULATORY_TYPE_ACMA,
    ESP_WIFI_REGULATORY_TYPE_ANATEL,
    ESP_WIFI_REGULATORY_TYPE_ISED,
    ESP_WIFI_REGULATORY_TYPE_SRRC,
    ESP_WIFI_REGULATORY_TYPE_OFCA,
    ESP_WIFI_REGULATORY_TYPE_WPC,
    ESP_WIFI_REGULATORY_TYPE_MIC,
    ESP_WIFI_REGULATORY_TYPE_KCC,
    ESP_WIFI_REGULATORY_TYPE_IFETEL,
    ESP_WIFI_REGULATORY_TYPE_RCM,
    ESP_WIFI_REGULATORY_TYPE_NCC,
    ESP_WIFI_REGULATORY_TYPE_FCC,
    ESP_WIFI_REGULATORY_TYPE_GT,
    ESP_WIFI_REGULATORY_TYPE_KE,
    ESP_WIFI_REGULATORY_TYPE_KP,
    ESP_WIFI_REGULATORY_TYPE_MY,
    ESP_WIFI_REGULATORY_TYPE_PK,
    ESP_WIFI_REGULATORY_TYPE_TG,
    ESP_WIFI_REGULATORY_TYPE_MAX,
} esp_wifi_regulatory_type_t;

/*
 * regdomain_table: ISO alpha-2 country codes and their regulatory profile indices.
 * Last row is a sentinel (not a real country code).
 */
const wifi_regdomain_t regdomain_table[] = {
    {"01", ESP_WIFI_REGULATORY_TYPE_DEFAULT},
    {"EU", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"AD", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"AE", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"AF", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"AI", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"AL", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"AM", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"AN", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"AR", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"AS", ESP_WIFI_REGULATORY_TYPE_FCC},
    {"AT", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"AU", ESP_WIFI_REGULATORY_TYPE_RCM},
    {"AW", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"AZ", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"BA", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"BB", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"BD", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"BE", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"BF", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"BG", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"BH", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"BL", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"BM", ESP_WIFI_REGULATORY_TYPE_FCC},
    {"BN", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"BO", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"BR", ESP_WIFI_REGULATORY_TYPE_NCC},
    {"BS", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"BT", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"BW", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"BY", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"BZ", ESP_WIFI_REGULATORY_TYPE_NCC},
    {"CA", ESP_WIFI_REGULATORY_TYPE_RCM},
    {"CF", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"CH", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"CI", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"CL", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"CN", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"CO", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"CR", ESP_WIFI_REGULATORY_TYPE_RCM},
    {"CU", ESP_WIFI_REGULATORY_TYPE_KCC},
    {"CX", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"CY", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"CZ", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"DE", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"DK", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"DM", ESP_WIFI_REGULATORY_TYPE_FCC},
    {"DO", ESP_WIFI_REGULATORY_TYPE_FCC},
    {"DZ", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"EC", ESP_WIFI_REGULATORY_TYPE_NCC},
    {"EE", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"EG", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"ES", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"ET", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"FI", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"FM", ESP_WIFI_REGULATORY_TYPE_FCC},
    {"FO", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"FR", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"GB", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"GD", ESP_WIFI_REGULATORY_TYPE_FCC},
    {"GE", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"GF", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"GH", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"GI", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"GL", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"GP", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"GR", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"GT", ESP_WIFI_REGULATORY_TYPE_GT},
    {"GU", ESP_WIFI_REGULATORY_TYPE_FCC},
    {"GY", ESP_WIFI_REGULATORY_TYPE_NCC},
    {"HK", ESP_WIFI_REGULATORY_TYPE_RCM},
    {"HN", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"HR", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"HT", ESP_WIFI_REGULATORY_TYPE_FCC},
    {"HU", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"ID", ESP_WIFI_REGULATORY_TYPE_GT},
    {"IE", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"IL", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"IM", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"IN", ESP_WIFI_REGULATORY_TYPE_NCC},
    {"IR", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"IS", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"IT", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"JM", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"JO", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"JP", ESP_WIFI_REGULATORY_TYPE_MIC},
    {"KE", ESP_WIFI_REGULATORY_TYPE_KE},
    {"KH", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"KN", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"KP", ESP_WIFI_REGULATORY_TYPE_KP},
    {"KR", ESP_WIFI_REGULATORY_TYPE_KCC},
    {"KW", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"KY", ESP_WIFI_REGULATORY_TYPE_NCC},
    {"KZ", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"LB", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"LC", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"LI", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"LK", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"LS", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"LT", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"LU", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"LV", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"MA", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"MC", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"MD", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"ME", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"MF", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"MH", ESP_WIFI_REGULATORY_TYPE_FCC},
    {"MK", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"MN", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"MO", ESP_WIFI_REGULATORY_TYPE_KCC},
    {"MP", ESP_WIFI_REGULATORY_TYPE_FCC},
    {"MQ", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"MR", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"MT", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"MU", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"MV", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"MW", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"MX", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"MY", ESP_WIFI_REGULATORY_TYPE_MY},
    {"NA", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"NG", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"NI", ESP_WIFI_REGULATORY_TYPE_FCC},
    {"NL", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"NO", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"NP", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"NZ", ESP_WIFI_REGULATORY_TYPE_RCM},
    {"OM", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"PA", ESP_WIFI_REGULATORY_TYPE_RCM},
    {"PE", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"PF", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"PG", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"PH", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"PK", ESP_WIFI_REGULATORY_TYPE_PK},
    {"PL", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"PM", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"PR", ESP_WIFI_REGULATORY_TYPE_FCC},
    {"PT", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"PW", ESP_WIFI_REGULATORY_TYPE_FCC},
    {"PY", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"QA", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"RE", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"RO", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"RS", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"RU", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"RW", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"SA", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"SE", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"SG", ESP_WIFI_REGULATORY_TYPE_KCC},
    {"SI", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"SK", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"SM", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"SN", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"SR", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"SV", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"SX", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"SY", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"TC", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"TD", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"TG", ESP_WIFI_REGULATORY_TYPE_TG},
    {"TH", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"TN", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"TR", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"TT", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"TW", ESP_WIFI_REGULATORY_TYPE_NCC},
    {"TZ", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"UA", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"UG", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"US", ESP_WIFI_REGULATORY_TYPE_FCC},
    {"UY", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"UZ", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"VA", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"VC", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"VE", ESP_WIFI_REGULATORY_TYPE_NCC},
    {"VI", ESP_WIFI_REGULATORY_TYPE_FCC},
    {"VN", ESP_WIFI_REGULATORY_TYPE_KCC},
    {"VU", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"WF", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"WS", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"YE", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"YT", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"ZA", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"ZW", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"##", ESP_WIFI_REGULATORY_TYPE_MAX}, /* Sentinel: end of table; not a real country code — do not use as wifi_country_t.cc. */
};

const wifi_regulatory_t regulatory_data[] = {
    /* ESP_WIFI_REGULATORY_TYPE_DEFAULT */
    {
        1,  /* Number of rules */
        {
            { 1, 11, 2, 20, 0, 0 },  /* chan:1~11, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_CE */
    {
        1,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_ACMA */
    {
        1,  /* Number of rules */
        {
            { 1, 13, 2, 36, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:36(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_ANATEL */
    {
        1,  /* Number of rules */
        {
            { 1, 13, 2, 30, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_ISED */
    {
        1,  /* Number of rules */
        {
            { 1, 13, 2, 36, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:36(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_SRRC */
    {
        1,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_OFCA */
    {
        1,  /* Number of rules */
        {
            { 1, 13, 2, 36, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:36(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_WPC */
    {
        1,  /* Number of rules */
        {
            { 1, 13, 2, 30, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_MIC */
    {
        2,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 14, 14, 1, 20, 0, 0 },  /* chan:14~14, max_bw:20M, max_power:20(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_KCC */
    {
        1,  /* Number of rules */
        {
            { 1, 13, 2, 23, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:23(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_IFETEL */
    {
        1,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_RCM */
    {
        1,  /* Number of rules */
        {
            { 1, 13, 2, 36, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:36(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_NCC */
    {
        1,  /* Number of rules */
        {
            { 1, 13, 2, 30, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_FCC */
    {
        1,  /* Number of rules */
        {
            { 1, 11, 2, 30, 0, 0 },  /* chan:1~11, max_bw:40M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_GT */
    {
        1,  /* Number of rules */
        {
            { 1, 13, 2, 26, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:26(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_KE */
    {
        1,  /* Number of rules */
        {
            { 1, 13, 2, 33, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:33(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_KP */
    {
        1,  /* Number of rules */
        {
            { 1, 13, 1, 20, 0, 0 },  /* chan:1~13, max_bw:20M, max_power:20(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_MY */
    {
        1,  /* Number of rules */
        {
            { 1, 14, 2, 26, 0, 0 },  /* chan:1~14, max_bw:40M, max_power:26(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_PK */
    {
        1,  /* Number of rules */
        {
            { 1, 14, 2, 30, 0, 0 },  /* chan:1~14, max_bw:40M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_TG */
    {
        1,  /* Number of rules */
        {
            { 1, 13, 2, 20, 1, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:1, reserved.*/
        }
    },
};
#endif // CONFIG_SOC_WIFI_SUPPORT_5G
