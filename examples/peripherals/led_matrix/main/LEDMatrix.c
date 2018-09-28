/* led_matrix - Example
   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#include "driver/gpio.h"
#include <stdint.h>
#include "LEDMatrix.h"



uint8_t a,b,c,d,oe,r1,r2,stb,clk ;
uint16_t width , height;

uint8_t *displaybuf;
uint8_t mask = 0x00 ;
uint8_t enable = 0xff ;



void configure(uint8_t ap, uint8_t bp, uint8_t cp, uint8_t dp, uint8_t oep, uint8_t r1p, uint8_t r2p, uint8_t stbp, uint8_t clkp)
{
    clk = clkp;
    r1 = r1p;
    r2 = r2p;
    stb = stbp;
    oe = oep;
    a = ap;
    b = bp;
    c = cp;
    d = dp;


}

void begin(uint8_t *displaybufp, uint16_t widthp, uint16_t heightp)
{


    displaybuf = displaybufp;
    width = widthp;
    height = heightp;
    gpio_set_direction(a, GPIO_MODE_OUTPUT);
    gpio_set_direction(b, GPIO_MODE_OUTPUT);
    gpio_set_direction(c, GPIO_MODE_OUTPUT);
    gpio_set_direction(d, GPIO_MODE_OUTPUT);
	gpio_set_direction(oe, GPIO_MODE_OUTPUT);
	gpio_set_direction(clk, GPIO_MODE_OUTPUT);
	gpio_set_direction(r1, GPIO_MODE_OUTPUT);
	gpio_set_direction(r2, GPIO_MODE_OUTPUT);
	gpio_set_direction(stb, GPIO_MODE_OUTPUT);


}

void clear()
{
    uint8_t *ptr = displaybuf;
    for (uint16_t i = 0; i < 256; i++) {
        *ptr = 0x00;
        ptr++;
    }
}





void scan()
{

    for(int row=0;row<16;row++){

        uint8_t *head = displaybuf + row * (width / 8);
        uint8_t *ptr = head;
        uint8_t *ptr1 = head+16*(width/8);

        for (uint8_t byte = 0; byte < (width / 8); byte++) {
            
            uint8_t pixels = *ptr;
            uint8_t pixels1 = *ptr1;
            pixels = pixels ^ mask;     // reverse: mask = 0xff, normal: mask =0x00
            pixels1 = pixels1 ^ mask;
            ptr++;
            ptr1++;
            
            for (uint8_t bit = 0; bit < 8; bit++) {

                gpio_set_level(clk, 0);
                gpio_set_level(r1, pixels & (0x80 >> bit)&enable);
                gpio_set_level(r2, pixels1 & (0x80 >> bit)&enable);
                gpio_set_level(clk, 1);

			}

		}

        gpio_set_level(oe, 1);
        gpio_set_level(a, (row & 0x01));
        gpio_set_level(b, (row & 0x02));
        gpio_set_level(c, (row & 0x04));
        gpio_set_level(d, (row & 0x08));
        gpio_set_level(stb, 0);
        gpio_set_level(stb, 1);
        gpio_set_level(stb, 0);
        gpio_set_level(oe, 0);

    }

}


