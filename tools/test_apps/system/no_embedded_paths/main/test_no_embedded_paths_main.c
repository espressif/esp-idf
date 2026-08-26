/* This test app only exists for the build stage, so doesn't need to do anything at runtime
   apart from embedding an assert to check asserts inside the project dir */
#include <assert.h>

// Declared non-static to avoid the assert being optimized out
int other_function(void)
{
    return 3;
}

void app_main(void)
{
    assert(other_function() == 3);
}
