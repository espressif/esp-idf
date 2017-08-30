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

/**
 * Mean error of 219 modules: 3.756418mV
 * Max error of 219 modules: 26.314087mV
 * Mean of max errors of 219 modules: 7.387282mV
 */ 
const esp_adc_cal_lookup_table_t esp_adc_cal_table_atten_0 = {
	.gain_m = 56,
	.gain_c = 59928,
	.offset_m = 91,
	.offset_c = 52798,
	.bit_shift = 7,
	.voltage = {
		54, 90, 120, 150, 180, 209, 241, 271,
		301, 330, 360, 391, 421, 450, 480, 511,
		541, 571, 601, 630, 660, 690, 720, 750,
		780, 809, 839, 870, 900, 929, 959, 988,
		1018
	}
};

/**
 * Mean error of 219 modules: 4.952441mV
 * Max error of 219 modules: 38.235321mV
 * Mean of max errors of 219 modules: 9.718749mV
 */ 
const esp_adc_cal_lookup_table_t esp_adc_cal_table_atten_1 = {
	.gain_m = 57,
	.gain_c = 59834,
	.offset_m = 108,
	.offset_c = 54733,
	.bit_shift = 7,
	.voltage = {
		60, 102, 143, 184, 223, 262, 303, 343,
		383, 423, 463, 503, 543, 583, 623, 663,
		703, 742, 782, 823, 862, 901, 942, 981,
		1022, 1060, 1101, 1141, 1180, 1219, 1259, 1298,
		1338
	}
};

/**
 * Mean error of 219 modules: 6.793558mV
 * Max error of 219 modules: 51.435440mV
 * Mean of max errors of 219 modules: 13.083121mV
 */ 
const esp_adc_cal_lookup_table_t esp_adc_cal_table_atten_2 = {
	.gain_m = 56,
	.gain_c = 59927,
	.offset_m = 154,
	.offset_c = 71995,
	.bit_shift = 7,
	.voltage = {
		82, 138, 194, 250, 305, 360, 417, 473,
		529, 584, 639, 696, 751, 806, 861, 917,
		971, 1026, 1081, 1136, 1192, 1246, 1301, 1356,
		1411, 1466, 1522, 1577, 1632, 1687, 1743, 1799,
		1855
	}
};

/**
 * Mean error of 219 modules: 13.149460mV
 * Max error of 219 modules: 97.102951mV
 * Mean of max errors of 219 modules: 35.538924mV
 */ 
const esp_adc_cal_lookup_table_t esp_adc_cal_table_atten_3 = {
	.gain_m = 33,
	.gain_c = 62214,
	.offset_m = 610,
	.offset_c = 108422,
	.bit_shift = 7,
	.voltage = {
		110, 221, 325, 430, 534, 637, 741, 845,
		947, 1049, 1153, 1256, 1358, 1461, 1565, 1670,
		1774, 1878, 1983, 2088, 2192, 2293, 2393, 2490,
		2580, 2665, 2746, 2820, 2885, 2947, 3007, 3060,
		3107
	}
};

