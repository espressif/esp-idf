#include <stdbool.h>
#include <setjmp.h>

extern void app_main();
jmp_buf buf;

int main()
{
    setjmp(buf);
    app_main();
}