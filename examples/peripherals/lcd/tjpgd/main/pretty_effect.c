/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <math.h>
#include "pretty_effect.h"
#include "sdkconfig.h"
#include "decode_image.h"

uint16_t *pixels;

//Grab a rgb16 pixel from the esp32_tiles image
static inline uint16_t get_bgnd_pixel(int x, int y)
{
    //Get color of the pixel on x,y coords
    return (uint16_t) * (pixels + (y * IMAGE_W) + x);
}

//This variable is used to detect the next frame.
static int prev_frame = -1;

//Instead of calculating the offsets for each pixel we grab, we pre-calculate the valueswhenever a frame changes, then re-use
//these as we go through all the pixels in the frame. This is much, much faster.
static int8_t xofs[320], yofs[240];
static int8_t xcomp[320], ycomp[240];

//Calculate the pixel data for a set of lines (with implied line size of 320). Pixels go in dest, line is the Y-coordinate of the
//first line to be calculated, linect is the amount of lines to calculate. Frame increases by one every time the entire image
//is displayed; this is used to go to the next frame of animation.
void pretty_effect_calc_lines(uint16_t *dest, int line, int frame, int linect)
{
    if (frame != prev_frame) {
        //We need to calculate a new set of offset coefficients. Take some random sines as offsets to make everything
        //look pretty and fluid-y.
        for (int x = 0; x < 320; x++) {
            xofs[x] = sin(frame * 0.15 + x * 0.06) * 4;
        }
        for (int y = 0; y < 240; y++) {
            yofs[y] = sin(frame * 0.1 + y * 0.05) * 4;
        }
        for (int x = 0; x < 320; x++) {
            xcomp[x] = sin(frame * 0.11 + x * 0.12) * 4;
        }
        for (int y = 0; y < 240; y++) {
            ycomp[y] = sin(frame * 0.07 + y * 0.15) * 4;
        }
        prev_frame = frame;
    }
    for (int y = line; y < line + linect; y++) {
        for (int x = 0; x < 320; x++) {
            *dest++ = get_bgnd_pixel(x + yofs[y] + xcomp[x], y + xofs[x] + ycomp[y]);
        }
    }
}

esp_err_t pretty_effect_init(void)
{
    return decode_image(&pixels);
}
