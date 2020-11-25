/*
 * https://dsp.stackexchange.com/questions/2990/how-to-change-volume-of-a-pcm-16-bit-signed-audio
 * 
 * In order to adjust the volume, the input signal is multiplied by some constant in dB.
 * constant = min_dB + (volume_level * (max_dB - min_dB)) / volume_level_max;
 * constant = (pow(10.0, constant / 20.0);
 * In order to avoit the floating point calculation, the constant is multiplied by some value 
 * and then during input signal proccessing, the constant is restored back. 
 * So the last equation will look like below:
 * constant = (pow(10.0, constant / 20.0) * (double) 1 << VOLUME_SCALE_VAL;
 * 
 * input_signal = (input_signal * constant) >> VOLUME_SCALE_VAL
 */

#include <stdint.h>
#include "bt_app_av.h"
#include "bt_app_volume_control.h"
#include "esp_log.h"

#define VOLUME_LEVELS 128 // volume is adjusted from 0 to 127 levels
#define VOLUME_SCALE_VAL 16

// precalculated constants.
// min_db = -51, max_db= 0; volume_level_max = 127, VOLUME_SCALE_VAL = 16
// for some reason, vol_level = 0 does not mute the sound, 
// so the 0x0000 is added at pos 0 and all indices are shifted by 1;

static const uint16_t gain_presets[VOLUME_LEVELS] = {
0x0000/*was 0x00b8*/, 0x00b8, 0x00c1, 0x00ca, 0x00d4, 0x00de, 0x00e8, 0x00f3,
0x00ff, 0x010b, 0x0118, 0x0125, 0x0133, 0x0141, 0x0150, 0x0160,
0x0171, 0x0183, 0x0195, 0x01a8, 0x01bc, 0x01d1, 0x01e7, 0x01fe,
0x0216, 0x0230, 0x024a, 0x0266, 0x0283, 0x02a2, 0x02c1, 0x02e3,
0x0306, 0x032a, 0x0351, 0x0379, 0x03a3, 0x03cf, 0x03fd, 0x042e,
0x0460, 0x0495, 0x04cd, 0x0507, 0x0544, 0x0584, 0x05c7, 0x060d,
0x0656, 0x06a3, 0x06f3, 0x0747, 0x07a0, 0x07fc, 0x085d, 0x08c2,
0x092c, 0x099b, 0x0a10, 0x0a8a, 0x0b09, 0x0b8f, 0x0c1b, 0x0cae,
0x0d47, 0x0de8, 0x0e91, 0x0f41, 0x0ffa, 0x10bb, 0x1186, 0x125a,
0x1339, 0x1422, 0x1515, 0x1615, 0x1720, 0x1839, 0x195e, 0x1a91,
0x1bd3, 0x1d24, 0x1e85, 0x1ff7, 0x217a, 0x2310, 0x24b8, 0x2675,
0x2847, 0x2a2f, 0x2c2e, 0x2e45, 0x3076, 0x32c1, 0x3528, 0x37ac,
0x3a4e, 0x3d10, 0x3ff4, 0x42fb, 0x4626, 0x4978, 0x4cf2, 0x5096,
0x5466, 0x5865, 0x5c93, 0x60f5, 0x658b, 0x6a5a, 0x6f62, 0x74a7,
0x7a2c, 0x7ff4, 0x8602, 0x8c59, 0x92fe, 0x99f2, 0xa13b, 0xa8dc,
0xb0da, 0xb938, 0xc1fc, 0xcb29, 0xd4c6, 0xded8, 0xe963, 0xffff,
};

int32_t volume = VOLUME_LEVELS - 1; // if the volume is not set by host, 
                                    // the input strem will not be modified.  

void bt_app_set_volume(uint32_t level) {
    if(level < VOLUME_LEVELS) {
        volume = level;
    }
}

void bt_app_adjust_volume(uint8_t *data, size_t size) {
    size_t items = size / (BT_SBC_BITS_PER_SAMPLE / 8); // 8 is bits per byte.
    int16_t* ptr = (int16_t*) data;
    while(items--) {
        int32_t fraction = (int32_t)(*ptr * gain_presets[volume]) >> VOLUME_SCALE_VAL;
        *ptr = (int16_t)fraction;
        ptr++;
    }
}