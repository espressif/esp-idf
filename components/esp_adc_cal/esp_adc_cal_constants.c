// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "esp_adc_cal.h"
#include "esp_adc_cal_constants.h"

/* ------------------------- Lookup Tables ---------------------------------- */

/**
 * Mean error of 70 modules: 2.674297mV
 * Max error of 70 modules: 12.176238mV
 * Mean of max errors of 70 modules: 7.079913mV
 */
const esp_adc_cal_lookup_table_t lut_atten0_adc1 = {
    .low_vref_curve = {
        58, 84, 114, 142, 168, 196, 224, 252,
        281, 308, 335, 363, 390, 417, 445, 473,
        501, 528, 556, 583, 611, 637, 665, 692,
        720, 747, 775, 802, 830, 856, 883, 910,
        937
    },
    .high_vref_curve = {
        86, 106, 152, 184, 218, 251, 283, 316,
        348, 381, 415, 447, 480, 513, 546, 579,
        612, 644, 677, 710, 742, 775, 808, 841,
        874, 906, 938, 971, 1003, 1035, 1068, 1100,
        1133
    }
};

/**
 * Mean error of 70 modules: 3.950325mV
 * Max error of 70 modules: 20.975788mV
 * Mean of max errors of 70 modules: 6.629054mV
 */
const esp_adc_cal_lookup_table_t lut_atten0_adc2 = {
    .low_vref_curve = {
        49, 75, 105, 132, 160, 187, 214, 241,
        268, 296, 324, 352, 380, 407, 434, 461,
        488, 516, 545, 572, 600, 627, 654, 681,
        708, 735, 763, 791, 818, 845, 872, 899,
        927
    },
    .high_vref_curve = {
        72, 103, 137, 170, 202, 235, 268, 302,
        335, 367, 399, 432, 464, 497, 530, 563,
        596, 628, 661, 693, 725, 758, 791, 824,
        857, 889, 921, 954, 986, 1018, 1051, 1084,
        1116
    }
};

/**
 * Mean error of 70 modules: 3.339671mV
 * Max error of 70 modules: 13.476428mV
 * Mean of max errors of 70 modules: 5.818235mV
 */
const esp_adc_cal_lookup_table_t lut_atten1_adc1 = {
    .low_vref_curve = {
        57, 87, 130, 168, 204, 240, 277, 313,
        349, 387, 424, 461, 498, 534, 571, 607,
        644, 680, 717, 753, 789, 826, 862, 898,
        934, 970, 1007, 1043, 1078, 1115, 1151, 1187,
        1223
    },
    .high_vref_curve = {
        89, 128, 178, 221, 264, 309, 353, 397,
        442, 484, 528, 572, 616, 659, 703, 747,
        790, 833, 877, 921, 964, 1006, 1050, 1094,
        1137, 1179, 1222, 1266, 1309, 1351, 1394, 1437,
        1481
    }
};

/**
 * Mean error of 70 modules: 5.319836mV
 * Max error of 70 modules: 29.034477mV
 * Mean of max errors of 70 modules: 9.226072mV
 */
const esp_adc_cal_lookup_table_t lut_atten1_adc2 = {
    .low_vref_curve = {
        47, 86, 122, 159, 196, 232, 268, 305,
        340, 377, 415, 452, 487, 525, 562, 598,
        635, 671, 708, 745, 782, 817, 853, 890,
        926, 962, 999, 1035, 1071, 1107, 1144, 1180,
        1215
    },
    .high_vref_curve = {
        78, 113, 165, 209, 251, 295, 339, 383,
        427, 470, 514, 558, 602, 644, 688, 732,
        775, 818, 862, 905, 948, 992, 1036, 1079,
        1122, 1164, 1208, 1252, 1295, 1338, 1381, 1424,
        1468
    }
};

/**
 * Mean error of 70 modules: 4.943406mV
 * Max error of 70 modules: 19.203104mV
 * Mean of max errors of 70 modules: 9.462435mV
 */
const esp_adc_cal_lookup_table_t lut_atten2_adc1 = {
    .low_vref_curve = {
        72, 117, 177, 228, 281, 330, 382, 434,
        483, 536, 587, 639, 690, 740, 791, 842,
        892, 942, 992, 1042, 1092, 1141, 1191, 1241,
        1290, 1339, 1391, 1441, 1490, 1540, 1591, 1642,
        1692
    },
    .high_vref_curve = {
        120, 175, 243, 305, 364, 426, 488, 550,
        612, 671, 732, 793, 852, 912, 973, 1033,
        1092, 1151, 1211, 1272, 1331, 1390, 1450, 1511,
        1571, 1631, 1691, 1752, 1812, 1872, 1933, 1994,
        2056
    }
};

/**
 * Mean error of 70 modules: 7.782408mV
 * Max error of 70 modules: 45.327423mV
 * Mean of max errors of 70 modules: 13.569581mV
 */
const esp_adc_cal_lookup_table_t lut_atten2_adc2 = {
    .low_vref_curve = {
        61, 119, 164, 216, 267, 317, 369, 420,
        471, 522, 574, 625, 676, 726, 777, 828,
        878, 928, 979, 1030, 1080, 1130, 1180, 1230,
        1280, 1330, 1381, 1432, 1483, 1532, 1583, 1634,
        1685
    },
    .high_vref_curve = {
        102, 152, 225, 286, 347, 407, 469, 530,
        590, 651, 712, 773, 832, 892, 953, 1013,
        1073, 1132, 1192, 1253, 1313, 1372, 1432, 1494,
        1554, 1613, 1675, 1735, 1795, 1856, 1917, 1978,
        2039
    }
};

/**
 * Mean error of 70 modules: 9.568297mV
 * Max error of 70 modules: 44.480817mV
 * Mean of max errors of 70 modules: 20.165069mV
 */
const esp_adc_cal_lookup_table_t lut_atten3_adc1 = {
    .low_vref_curve = {
        88, 185, 285, 380, 473, 568, 664, 759,
        853, 945, 1039, 1133, 1224, 1316, 1410, 1505,
        1599, 1692, 1788, 1886, 1983, 2081, 2186, 2297,
        2405, 2511, 2615, 2710, 2793, 2868, 2937, 3000,
        3061
    },
    .high_vref_curve = {
        173, 271, 399, 515, 628, 740, 853, 965,
        1075, 1186, 1299, 1411, 1523, 1634, 1749, 1863,
        1975, 2087, 2201, 2314, 2422, 2525, 2622, 2707,
        2779, 2844, 2901, 2956, 3008, 3059, 3110, 3160,
        3210
    }
};

/**
 * Mean error of 70 modules: 12.799173mV
 * Max error of 70 modules: 60.129227mV
 * Mean of max errors of 70 modules: 23.334659mV
 */
const esp_adc_cal_lookup_table_t lut_atten3_adc2 = {
    .low_vref_curve = {
        83, 177, 274, 370, 465, 559, 655, 751,
        844, 937, 1031, 1126, 1218, 1312, 1407, 1502,
        1597, 1691, 1787, 1885, 1983, 2081, 2185, 2293,
        2400, 2506, 2612, 2709, 2793, 2868, 2937, 2999,
        3059
    },
    .high_vref_curve = {
        155, 257, 383, 498, 610, 722, 836, 947,
        1058, 1170, 1283, 1396, 1507, 1619, 1733, 1848,
        1960, 2073, 2188, 2300, 2408, 2513, 2611, 2698,
        2773, 2838, 2895, 2946, 2996, 3044, 3092, 3139,
        3186
    }
};

/* ---------------- Constants for linear calibration using TP --------------- */

// (0.999518539509928, 1.32714033039721, 1.83698541693337, 3.42269475397192) * 65536
const uint32_t adc1_lin_tp_atten_scale[4] = {65504, 86975, 120389, 224310};

// (0.99895306551877194, 1.3253993908658808, 1.8373986067502703, 3.4287787961634124) * 65536
const uint32_t adc2_lin_tp_atten_scale[4] = {65467, 86861, 120416, 224708};

// (0.369882210218334, 1.33352056600476, 26.5266258898132, 54.2706443679156)
const uint32_t adc1_lin_tp_atten_offset[4] = {0, 1, 27, 54};

// (0.0, 3.59737421, 26.3951927, 64.738429)
const uint32_t adc2_lin_tp_atten_offset[4] = {0, 9, 26, 66};

/* ------------- Constants for linear calibration using Vref ---------------- */

// (0.87632707, 1.16327136, 1.60951523, 2.99991113) * 65536
const uint32_t adc1_lin_vref_atten_scale[4] = {57431, 76236, 105481, 196602};

// (0.87335934, 1.16233322, 1.61251264, 3.00858015) * 65536
const uint32_t adc2_lin_vref_atten_scale[4] = {57236, 76175, 105678, 197170};

// (74.97079, 77.67434785, 106.58843993, 142.18959459)
const uint32_t adc1_lin_vref_atten_offset[4] = {75, 78, 107, 142};

// (62.73368574, 66.33498527, 89.23584218, 127.69820652)
const uint32_t adc2_lin_vref_atten_offset[4] = {63, 66, 89, 128};
