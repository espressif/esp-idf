#include <stdbool.h>
#include <setjmp.h>

extern void app_main(void);
jmp_buf buf;

int main(void)
{
    setjmp(buf);
    app_main();
}