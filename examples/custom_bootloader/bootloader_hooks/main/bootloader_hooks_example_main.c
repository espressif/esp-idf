#include <stdio.h>

void app_main(void)
{
    /**
     * Nothing special is done here, everything interesting in this example
     * is done in the custom bootloader code, located in:
     * `bootloader_components/my_boot_hooks/hooks.c`
     */
    printf("User application is loaded and running.\n");
}
