

#ifndef __LED_MATRIX_H__
#define __LED_MATRIX_H__

#include <stdint.h>



void configure(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t oe, uint8_t r1,uint8_t r2,  uint8_t stb, uint8_t clk);
void begin(uint8_t *displaybuf, uint16_t width, uint16_t height);
void scan();
void clear();

    



#endif
