/* A simple program for testing the CPU trace feature.
 * Doesn't matter what it does, as long as it runs for a sufficient amount of time.
 * The program is linked with the linker script of 2nd stage bootloader, and gets
 * run from the boot ROM.
 */

int fib(int n)
{
    if (n <= 1) {
        return n;
    }
    return fib(n - 1) + fib(n - 2);
}

void done()
{
    /* serves as a breakpoint target for the debugger */
}

void entry(void)
{
    for (int i = 0; i < 10; ++ i) {
        fib(10);
    }
    done();
}
