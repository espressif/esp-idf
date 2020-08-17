#include <stdint.h>
#include <stdio.h>

extern uint8_t _2file_start[]   asm("_binary_2file_txt_start");
extern uint8_t _2file_end[]     asm("_binary_2file_txt_end");
extern uint8_t file_start[]     asm("_binary_file_txt_start");
extern uint8_t file_end[]       asm("_binary_file_txt_start");
extern uint8_t _file_start[]    asm("_binary__file_txt_start");
extern uint8_t _file_end[]      asm("_binary__file_txt_start");

#define PRINT_ADDR(f)           printf("%s -> start: %p end:%p\n", #f, f ## _start, f ## _end);

void app_main(void)
{
    PRINT_ADDR(_2file);
    PRINT_ADDR(file);
    PRINT_ADDR(_file);
}
