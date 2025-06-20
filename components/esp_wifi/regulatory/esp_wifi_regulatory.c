/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
    ESP_WIFI_REGULATORY_TYPE_AI,
    ESP_WIFI_REGULATORY_TYPE_AM,
    ESP_WIFI_REGULATORY_TYPE_AS,
    ESP_WIFI_REGULATORY_TYPE_AZ,
    ESP_WIFI_REGULATORY_TYPE_BB,
    ESP_WIFI_REGULATORY_TYPE_BD,
    ESP_WIFI_REGULATORY_TYPE_BH,
    ESP_WIFI_REGULATORY_TYPE_BN,
    ESP_WIFI_REGULATORY_TYPE_BO,
    ESP_WIFI_REGULATORY_TYPE_BS,
    ESP_WIFI_REGULATORY_TYPE_BZ,
    ESP_WIFI_REGULATORY_TYPE_CF,
    ESP_WIFI_REGULATORY_TYPE_CR,
    ESP_WIFI_REGULATORY_TYPE_CU,
    ESP_WIFI_REGULATORY_TYPE_DM,
    ESP_WIFI_REGULATORY_TYPE_DZ,
    ESP_WIFI_REGULATORY_TYPE_EC,
    ESP_WIFI_REGULATORY_TYPE_EG,
    ESP_WIFI_REGULATORY_TYPE_GB,
    ESP_WIFI_REGULATORY_TYPE_GD,
    ESP_WIFI_REGULATORY_TYPE_GT,
    ESP_WIFI_REGULATORY_TYPE_GU,
    ESP_WIFI_REGULATORY_TYPE_HR,
    ESP_WIFI_REGULATORY_TYPE_ID,
    ESP_WIFI_REGULATORY_TYPE_IL,
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
    ESP_WIFI_REGULATORY_TYPE_SY,
    ESP_WIFI_REGULATORY_TYPE_TC,
    ESP_WIFI_REGULATORY_TYPE_TG,
    ESP_WIFI_REGULATORY_TYPE_TR,
    ESP_WIFI_REGULATORY_TYPE_TZ,
    ESP_WIFI_REGULATORY_TYPE_UA,
    ESP_WIFI_REGULATORY_TYPE_VE,
    ESP_WIFI_REGULATORY_TYPE_VN,
    ESP_WIFI_REGULATORY_TYPE_WS,
    ESP_WIFI_REGULATORY_TYPE_YE,
    ESP_WIFI_REGULATORY_TYPE_ZA,
    ESP_WIFI_REGULATORY_TYPE_MAX,
} esp_wifi_regulatory_type_t;

const wifi_regdomain_t regdomain_table[WIFI_MAX_SUPPORT_COUNTRY_NUM] = {
    {"01", ESP_WIFI_REGULATORY_TYPE_DEFAULT},
    {"EU", ESP_WIFI_REGULATORY_TYPE_CE},
    {"AD", ESP_WIFI_REGULATORY_TYPE_CE},
    {"AE", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"AF", ESP_WIFI_REGULATORY_TYPE_AF},
    {"AI", ESP_WIFI_REGULATORY_TYPE_AI},
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
    {"FR", ESP_WIFI_REGULATORY_TYPE_CE},
    {"GB", ESP_WIFI_REGULATORY_TYPE_GB},
    {"GD", ESP_WIFI_REGULATORY_TYPE_GD},
    {"GE", ESP_WIFI_REGULATORY_TYPE_AZ},
    {"GF", ESP_WIFI_REGULATORY_TYPE_AI},
    {"GH", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"GL", ESP_WIFI_REGULATORY_TYPE_AI},
    {"GP", ESP_WIFI_REGULATORY_TYPE_AI},
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
    {"IL", ESP_WIFI_REGULATORY_TYPE_IL},
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
    {"MF", ESP_WIFI_REGULATORY_TYPE_AI},
    {"MH", ESP_WIFI_REGULATORY_TYPE_AS},
    {"MK", ESP_WIFI_REGULATORY_TYPE_CE},
    {"MN", ESP_WIFI_REGULATORY_TYPE_BS},
    {"MO", ESP_WIFI_REGULATORY_TYPE_MO},
    {"MP", ESP_WIFI_REGULATORY_TYPE_AS},
    {"MQ", ESP_WIFI_REGULATORY_TYPE_AI},
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
    {"PF", ESP_WIFI_REGULATORY_TYPE_AI},
    {"PG", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"PH", ESP_WIFI_REGULATORY_TYPE_PH},
    {"PK", ESP_WIFI_REGULATORY_TYPE_PK},
    {"PL", ESP_WIFI_REGULATORY_TYPE_CE},
    {"PM", ESP_WIFI_REGULATORY_TYPE_AI},
    {"PR", ESP_WIFI_REGULATORY_TYPE_GD},
    {"PT", ESP_WIFI_REGULATORY_TYPE_CE},
    {"PW", ESP_WIFI_REGULATORY_TYPE_AS},
    {"PY", ESP_WIFI_REGULATORY_TYPE_BS},
    {"QA", ESP_WIFI_REGULATORY_TYPE_QA},
    {"RE", ESP_WIFI_REGULATORY_TYPE_AI},
    {"RO", ESP_WIFI_REGULATORY_TYPE_CE},
    {"RS", ESP_WIFI_REGULATORY_TYPE_RS},
    {"RU", ESP_WIFI_REGULATORY_TYPE_RU},
    {"RW", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"SA", ESP_WIFI_REGULATORY_TYPE_AF},
    {"SE", ESP_WIFI_REGULATORY_TYPE_CE},
    {"SG", ESP_WIFI_REGULATORY_TYPE_SG},
    {"SI", ESP_WIFI_REGULATORY_TYPE_CE},
    {"SK", ESP_WIFI_REGULATORY_TYPE_CE},
    {"SN", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"SR", ESP_WIFI_REGULATORY_TYPE_AF},
    {"SV", ESP_WIFI_REGULATORY_TYPE_SV},
    {"SY", ESP_WIFI_REGULATORY_TYPE_SY},
    {"TC", ESP_WIFI_REGULATORY_TYPE_TC},
    {"TD", ESP_WIFI_REGULATORY_TYPE_AF},
    {"TG", ESP_WIFI_REGULATORY_TYPE_TG},
    {"TH", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"TN", ESP_WIFI_REGULATORY_TYPE_MA},
    {"TR", ESP_WIFI_REGULATORY_TYPE_TR},
    {"TT", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"TW", ESP_WIFI_REGULATORY_TYPE_NCC},
    {"TZ", ESP_WIFI_REGULATORY_TYPE_TZ},
    {"UA", ESP_WIFI_REGULATORY_TYPE_UA},
    {"UG", ESP_WIFI_REGULATORY_TYPE_BS},
    {"US", ESP_WIFI_REGULATORY_TYPE_FCC},
    {"UY", ESP_WIFI_REGULATORY_TYPE_BB},
    {"UZ", ESP_WIFI_REGULATORY_TYPE_MA},
    {"VC", ESP_WIFI_REGULATORY_TYPE_AF},
    {"VE", ESP_WIFI_REGULATORY_TYPE_VE},
    {"VI", ESP_WIFI_REGULATORY_TYPE_AS},
    {"VN", ESP_WIFI_REGULATORY_TYPE_VN},
    {"VU", ESP_WIFI_REGULATORY_TYPE_IFETEL},
    {"WF", ESP_WIFI_REGULATORY_TYPE_AI},
    {"WS", ESP_WIFI_REGULATORY_TYPE_WS},
    {"YE", ESP_WIFI_REGULATORY_TYPE_YE},
    {"YT", ESP_WIFI_REGULATORY_TYPE_AI},
    {"ZA", ESP_WIFI_REGULATORY_TYPE_ZA},
    {"ZW", ESP_WIFI_REGULATORY_TYPE_AF},
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
            { 1, 11, 2, 30, 0, 0 },  /* chan:1~11, max_bw:40M, max_power:30(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 23, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:23(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 24, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:24(dBm), dfs:1, reserved.*/
            { 100, 116, 3, 24, 1, 0 },  /* chan:100~116, max_bw:80M, max_power:24(dBm), dfs:1, reserved.*/
            { 132, 144, 3, 24, 1, 0 },  /* chan:132~144, max_bw:80M, max_power:24(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 30, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
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
            { 149, 169, 3, 27, 0, 0 },  /* chan:149~169, max_bw:80M, max_power:27(dBm), dfs:0, reserved.*/
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
    /* ESP_WIFI_REGULATORY_TYPE_AI */
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
        5,  /* Number of rules */
        {
            { 1, 13, 2, 36, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:36(dBm), dfs:0, reserved.*/
            { 36, 48, 1, 30, 0, 0 },  /* chan:36~48, max_bw:20M, max_power:30(dBm), dfs:0, reserved.*/
            { 52, 64, 1, 30, 1, 0 },  /* chan:52~64, max_bw:20M, max_power:30(dBm), dfs:1, reserved.*/
            { 100, 144, 1, 30, 1, 0 },  /* chan:100~144, max_bw:20M, max_power:30(dBm), dfs:1, reserved.*/
            { 149, 165, 1, 36, 0, 0 },  /* chan:149~165, max_bw:20M, max_power:36(dBm), dfs:0, reserved.*/
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
    /* ESP_WIFI_REGULATORY_TYPE_IL */
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
            { 1, 13, 2, 26, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:26(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 30, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 30, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:30(dBm), dfs:1, reserved.*/
            { 100, 128, 4, 30, 1, 0 },  /* chan:100~128, max_bw:160M, max_power:30(dBm), dfs:1, reserved.*/
            { 149, 165, 3, 30, 0, 0 },  /* chan:149~165, max_bw:80M, max_power:30(dBm), dfs:0, reserved.*/
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
        1,  /* Number of rules */
        {
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
            { 132, 165, 4, 20, 0, 0 },  /* chan:132~165, max_bw:160M, max_power:20(dBm), dfs:0, reserved.*/
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
    /* ESP_WIFI_REGULATORY_TYPE_TC */
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
    /* ESP_WIFI_REGULATORY_TYPE_TZ */
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
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
            { 36, 48, 3, 17, 0, 0 },  /* chan:36~48, max_bw:80M, max_power:17(dBm), dfs:0, reserved.*/
            { 52, 64, 3, 24, 1, 0 },  /* chan:52~64, max_bw:80M, max_power:24(dBm), dfs:1, reserved.*/
            { 100, 144, 3, 24, 1, 0 },  /* chan:100~144, max_bw:80M, max_power:24(dBm), dfs:1, reserved.*/
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
    ESP_WIFI_REGULATORY_TYPE_CU,
    ESP_WIFI_REGULATORY_TYPE_GT,
    ESP_WIFI_REGULATORY_TYPE_ID,
    ESP_WIFI_REGULATORY_TYPE_KE,
    ESP_WIFI_REGULATORY_TYPE_KP,
    ESP_WIFI_REGULATORY_TYPE_OM,
    ESP_WIFI_REGULATORY_TYPE_PK,
    ESP_WIFI_REGULATORY_TYPE_TG,
    ESP_WIFI_REGULATORY_TYPE_MAX,
} esp_wifi_regulatory_type_t;

const wifi_regdomain_t regdomain_table[WIFI_MAX_SUPPORT_COUNTRY_NUM] = {
    {"01", ESP_WIFI_REGULATORY_TYPE_DEFAULT},
    {"EU", ESP_WIFI_REGULATORY_TYPE_CE},
    {"AD", ESP_WIFI_REGULATORY_TYPE_CE},
    {"AE", ESP_WIFI_REGULATORY_TYPE_CE},
    {"AF", ESP_WIFI_REGULATORY_TYPE_CE},
    {"AI", ESP_WIFI_REGULATORY_TYPE_CE},
    {"AL", ESP_WIFI_REGULATORY_TYPE_CE},
    {"AM", ESP_WIFI_REGULATORY_TYPE_CE},
    {"AN", ESP_WIFI_REGULATORY_TYPE_CE},
    {"AR", ESP_WIFI_REGULATORY_TYPE_CE},
    {"AS", ESP_WIFI_REGULATORY_TYPE_ISED},
    {"AT", ESP_WIFI_REGULATORY_TYPE_CE},
    {"AU", ESP_WIFI_REGULATORY_TYPE_ACMA},
    {"AW", ESP_WIFI_REGULATORY_TYPE_CE},
    {"AZ", ESP_WIFI_REGULATORY_TYPE_CE},
    {"BA", ESP_WIFI_REGULATORY_TYPE_CE},
    {"BB", ESP_WIFI_REGULATORY_TYPE_CE},
    {"BD", ESP_WIFI_REGULATORY_TYPE_CE},
    {"BE", ESP_WIFI_REGULATORY_TYPE_CE},
    {"BF", ESP_WIFI_REGULATORY_TYPE_CE},
    {"BG", ESP_WIFI_REGULATORY_TYPE_CE},
    {"BH", ESP_WIFI_REGULATORY_TYPE_CE},
    {"BL", ESP_WIFI_REGULATORY_TYPE_CE},
    {"BM", ESP_WIFI_REGULATORY_TYPE_ISED},
    {"BN", ESP_WIFI_REGULATORY_TYPE_CE},
    {"BO", ESP_WIFI_REGULATORY_TYPE_CE},
    {"BR", ESP_WIFI_REGULATORY_TYPE_ANATEL},
    {"BS", ESP_WIFI_REGULATORY_TYPE_CE},
    {"BT", ESP_WIFI_REGULATORY_TYPE_CE},
    {"BY", ESP_WIFI_REGULATORY_TYPE_CE},
    {"BZ", ESP_WIFI_REGULATORY_TYPE_ANATEL},
    {"CA", ESP_WIFI_REGULATORY_TYPE_ISED},
    {"CF", ESP_WIFI_REGULATORY_TYPE_CE},
    {"CH", ESP_WIFI_REGULATORY_TYPE_CE},
    {"CI", ESP_WIFI_REGULATORY_TYPE_CE},
    {"CL", ESP_WIFI_REGULATORY_TYPE_CE},
    {"CN", ESP_WIFI_REGULATORY_TYPE_CE},
    {"CO", ESP_WIFI_REGULATORY_TYPE_CE},
    {"CR", ESP_WIFI_REGULATORY_TYPE_OFCA},
    {"CU", ESP_WIFI_REGULATORY_TYPE_CU},
    {"CX", ESP_WIFI_REGULATORY_TYPE_CE},
    {"CY", ESP_WIFI_REGULATORY_TYPE_CE},
    {"CZ", ESP_WIFI_REGULATORY_TYPE_CE},
    {"DE", ESP_WIFI_REGULATORY_TYPE_CE},
    {"DK", ESP_WIFI_REGULATORY_TYPE_CE},
    {"DM", ESP_WIFI_REGULATORY_TYPE_ISED},
    {"DO", ESP_WIFI_REGULATORY_TYPE_ISED},
    {"DZ", ESP_WIFI_REGULATORY_TYPE_CE},
    {"EC", ESP_WIFI_REGULATORY_TYPE_ANATEL},
    {"EE", ESP_WIFI_REGULATORY_TYPE_CE},
    {"EG", ESP_WIFI_REGULATORY_TYPE_CE},
    {"ES", ESP_WIFI_REGULATORY_TYPE_CE},
    {"ET", ESP_WIFI_REGULATORY_TYPE_CE},
    {"FI", ESP_WIFI_REGULATORY_TYPE_CE},
    {"FM", ESP_WIFI_REGULATORY_TYPE_ISED},
    {"FR", ESP_WIFI_REGULATORY_TYPE_CE},
    {"GB", ESP_WIFI_REGULATORY_TYPE_CE},
    {"GD", ESP_WIFI_REGULATORY_TYPE_ISED},
    {"GE", ESP_WIFI_REGULATORY_TYPE_CE},
    {"GF", ESP_WIFI_REGULATORY_TYPE_CE},
    {"GH", ESP_WIFI_REGULATORY_TYPE_CE},
    {"GL", ESP_WIFI_REGULATORY_TYPE_CE},
    {"GP", ESP_WIFI_REGULATORY_TYPE_CE},
    {"GR", ESP_WIFI_REGULATORY_TYPE_CE},
    {"GT", ESP_WIFI_REGULATORY_TYPE_GT},
    {"GU", ESP_WIFI_REGULATORY_TYPE_ISED},
    {"GY", ESP_WIFI_REGULATORY_TYPE_ANATEL},
    {"HK", ESP_WIFI_REGULATORY_TYPE_OFCA},
    {"HN", ESP_WIFI_REGULATORY_TYPE_CE},
    {"HR", ESP_WIFI_REGULATORY_TYPE_CE},
    {"HT", ESP_WIFI_REGULATORY_TYPE_ISED},
    {"HU", ESP_WIFI_REGULATORY_TYPE_CE},
    {"ID", ESP_WIFI_REGULATORY_TYPE_ID},
    {"IE", ESP_WIFI_REGULATORY_TYPE_CE},
    {"IL", ESP_WIFI_REGULATORY_TYPE_CE},
    {"IN", ESP_WIFI_REGULATORY_TYPE_ANATEL},
    {"IR", ESP_WIFI_REGULATORY_TYPE_CE},
    {"IS", ESP_WIFI_REGULATORY_TYPE_CE},
    {"IT", ESP_WIFI_REGULATORY_TYPE_CE},
    {"JM", ESP_WIFI_REGULATORY_TYPE_CE},
    {"JO", ESP_WIFI_REGULATORY_TYPE_CE},
    {"JP", ESP_WIFI_REGULATORY_TYPE_MIC},
    {"KE", ESP_WIFI_REGULATORY_TYPE_KE},
    {"KH", ESP_WIFI_REGULATORY_TYPE_CE},
    {"KN", ESP_WIFI_REGULATORY_TYPE_CE},
    {"KP", ESP_WIFI_REGULATORY_TYPE_KP},
    {"KR", ESP_WIFI_REGULATORY_TYPE_KCC},
    {"KW", ESP_WIFI_REGULATORY_TYPE_CE},
    {"KY", ESP_WIFI_REGULATORY_TYPE_ANATEL},
    {"KZ", ESP_WIFI_REGULATORY_TYPE_CE},
    {"LB", ESP_WIFI_REGULATORY_TYPE_CE},
    {"LC", ESP_WIFI_REGULATORY_TYPE_CE},
    {"LI", ESP_WIFI_REGULATORY_TYPE_CE},
    {"LK", ESP_WIFI_REGULATORY_TYPE_CE},
    {"LS", ESP_WIFI_REGULATORY_TYPE_CE},
    {"LT", ESP_WIFI_REGULATORY_TYPE_CE},
    {"LU", ESP_WIFI_REGULATORY_TYPE_CE},
    {"LV", ESP_WIFI_REGULATORY_TYPE_CE},
    {"MA", ESP_WIFI_REGULATORY_TYPE_CE},
    {"MC", ESP_WIFI_REGULATORY_TYPE_CE},
    {"MD", ESP_WIFI_REGULATORY_TYPE_CE},
    {"ME", ESP_WIFI_REGULATORY_TYPE_CE},
    {"MF", ESP_WIFI_REGULATORY_TYPE_CE},
    {"MH", ESP_WIFI_REGULATORY_TYPE_ISED},
    {"MK", ESP_WIFI_REGULATORY_TYPE_CE},
    {"MN", ESP_WIFI_REGULATORY_TYPE_CE},
    {"MO", ESP_WIFI_REGULATORY_TYPE_CU},
    {"MP", ESP_WIFI_REGULATORY_TYPE_ISED},
    {"MQ", ESP_WIFI_REGULATORY_TYPE_CE},
    {"MR", ESP_WIFI_REGULATORY_TYPE_CE},
    {"MT", ESP_WIFI_REGULATORY_TYPE_CE},
    {"MU", ESP_WIFI_REGULATORY_TYPE_CE},
    {"MV", ESP_WIFI_REGULATORY_TYPE_CE},
    {"MW", ESP_WIFI_REGULATORY_TYPE_CE},
    {"MX", ESP_WIFI_REGULATORY_TYPE_CE},
    {"MY", ESP_WIFI_REGULATORY_TYPE_GT},
    {"NA", ESP_WIFI_REGULATORY_TYPE_CE},
    {"NG", ESP_WIFI_REGULATORY_TYPE_CE},
    {"NI", ESP_WIFI_REGULATORY_TYPE_ISED},
    {"NL", ESP_WIFI_REGULATORY_TYPE_CE},
    {"NO", ESP_WIFI_REGULATORY_TYPE_CE},
    {"NP", ESP_WIFI_REGULATORY_TYPE_CE},
    {"NZ", ESP_WIFI_REGULATORY_TYPE_OFCA},
    {"OM", ESP_WIFI_REGULATORY_TYPE_OM},
    {"PA", ESP_WIFI_REGULATORY_TYPE_OFCA},
    {"PE", ESP_WIFI_REGULATORY_TYPE_CE},
    {"PF", ESP_WIFI_REGULATORY_TYPE_CE},
    {"PG", ESP_WIFI_REGULATORY_TYPE_CE},
    {"PH", ESP_WIFI_REGULATORY_TYPE_CE},
    {"PK", ESP_WIFI_REGULATORY_TYPE_PK},
    {"PL", ESP_WIFI_REGULATORY_TYPE_CE},
    {"PM", ESP_WIFI_REGULATORY_TYPE_CE},
    {"PR", ESP_WIFI_REGULATORY_TYPE_ISED},
    {"PT", ESP_WIFI_REGULATORY_TYPE_CE},
    {"PW", ESP_WIFI_REGULATORY_TYPE_ISED},
    {"PY", ESP_WIFI_REGULATORY_TYPE_CE},
    {"QA", ESP_WIFI_REGULATORY_TYPE_OM},
    {"RE", ESP_WIFI_REGULATORY_TYPE_CE},
    {"RO", ESP_WIFI_REGULATORY_TYPE_CE},
    {"RS", ESP_WIFI_REGULATORY_TYPE_CE},
    {"RU", ESP_WIFI_REGULATORY_TYPE_CE},
    {"RW", ESP_WIFI_REGULATORY_TYPE_CE},
    {"SA", ESP_WIFI_REGULATORY_TYPE_CE},
    {"SE", ESP_WIFI_REGULATORY_TYPE_CE},
    {"SG", ESP_WIFI_REGULATORY_TYPE_CU},
    {"SI", ESP_WIFI_REGULATORY_TYPE_CE},
    {"SK", ESP_WIFI_REGULATORY_TYPE_CE},
    {"SN", ESP_WIFI_REGULATORY_TYPE_CE},
    {"SR", ESP_WIFI_REGULATORY_TYPE_CE},
    {"SV", ESP_WIFI_REGULATORY_TYPE_CE},
    {"SY", ESP_WIFI_REGULATORY_TYPE_OM},
    {"TC", ESP_WIFI_REGULATORY_TYPE_CE},
    {"TD", ESP_WIFI_REGULATORY_TYPE_CE},
    {"TG", ESP_WIFI_REGULATORY_TYPE_TG},
    {"TH", ESP_WIFI_REGULATORY_TYPE_CE},
    {"TN", ESP_WIFI_REGULATORY_TYPE_CE},
    {"TR", ESP_WIFI_REGULATORY_TYPE_CE},
    {"TT", ESP_WIFI_REGULATORY_TYPE_CE},
    {"TW", ESP_WIFI_REGULATORY_TYPE_ANATEL},
    {"TZ", ESP_WIFI_REGULATORY_TYPE_CE},
    {"UA", ESP_WIFI_REGULATORY_TYPE_OM},
    {"UG", ESP_WIFI_REGULATORY_TYPE_CE},
    {"US", ESP_WIFI_REGULATORY_TYPE_ISED},
    {"UY", ESP_WIFI_REGULATORY_TYPE_CE},
    {"UZ", ESP_WIFI_REGULATORY_TYPE_CE},
    {"VC", ESP_WIFI_REGULATORY_TYPE_CE},
    {"VE", ESP_WIFI_REGULATORY_TYPE_ANATEL},
    {"VI", ESP_WIFI_REGULATORY_TYPE_ISED},
    {"VN", ESP_WIFI_REGULATORY_TYPE_CE},
    {"VU", ESP_WIFI_REGULATORY_TYPE_CE},
    {"WF", ESP_WIFI_REGULATORY_TYPE_CE},
    {"WS", ESP_WIFI_REGULATORY_TYPE_CE},
    {"YE", ESP_WIFI_REGULATORY_TYPE_CE},
    {"YT", ESP_WIFI_REGULATORY_TYPE_CE},
    {"ZA", ESP_WIFI_REGULATORY_TYPE_CE},
    {"ZW", ESP_WIFI_REGULATORY_TYPE_CE},
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
            { 1, 11, 2, 30, 0, 0 },  /* chan:1~11, max_bw:40M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_CE */
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
    /* ESP_WIFI_REGULATORY_TYPE_ANATEL */
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
    /* ESP_WIFI_REGULATORY_TYPE_CE */
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
            { 1, 11, 2, 30, 0, 0 },  /* chan:1~11, max_bw:40M, max_power:30(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_CU */
    {
        1,  /* Number of rules */
        {
            { 1, 13, 2, 23, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:23(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_GT */
    {
        1,  /* Number of rules */
        {
            { 1, 13, 2, 26, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:26(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_ID */
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
    /* ESP_WIFI_REGULATORY_TYPE_OM */
    {
        1,  /* Number of rules */
        {
            { 1, 13, 2, 20, 0, 0 },  /* chan:1~13, max_bw:40M, max_power:20(dBm), dfs:0, reserved.*/
        }
    },
    /* ESP_WIFI_REGULATORY_TYPE_PK */
    {
        0,  /* Number of rules */
        {
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
