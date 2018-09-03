#include <stdbool.h>
#include "unity.h"

static bool setup_runned = false;
static bool teardown_runned = false;

void setUp()
{
    setup_runned = true;
}

void tearDown(void)
{
    setup_runned = false;
    teardown_runned = true;
}

TEST_CASE("setUp runned", "[unity]")
{
    TEST_ASSERT_TRUE(setup_runned);
}

TEST_CASE("tearDown Runned", "[unity]")
{
    TEST_ASSERT_TRUE(teardown_runned);
}