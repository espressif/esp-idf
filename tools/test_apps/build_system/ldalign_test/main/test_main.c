#include <stdio.h>

const static uint32_t __attribute__ ((aligned (64))) testTab[] =
  {
   0xff445566, 0x44556677, 0x33221100,
   0x88997755, 0x99887755, 0x88997755,
   0x99546327, 0x7946fa9e, 0xa6b5f8ee,
   0x12345678
  };

void app_main(void)
{
  /* Do something with the array, in order to avoid it being discarded. */
  for (uint32_t i = 0; i < 10; i++)
    printf ("%x\n", testTab[i]);
}
