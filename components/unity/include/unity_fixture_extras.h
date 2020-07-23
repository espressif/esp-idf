/* IDF-specific additions to "Unity Fixture" */
#pragma once

#ifndef CONFIG_IDF_TARGET

/* A shorthand for running one test group from the main function */
#define UNITY_MAIN(group_) do { \
    const char* argv[] = { "test", "-v" }; \
    const int argc = sizeof(argv)/sizeof(argv[0]); \
    int rc = UnityMain(argc, argv, TEST_ ## group_ ## _GROUP_RUNNER); \
    printf("\nTests finished, rc=%d\n", rc); \
    exit(rc); \
} while(0)

#else // CONFIG_IDF_TARGET

/* A shorthand for running one test group from the main function */
#define UNITY_MAIN(group_) do { \
    const char* argv[] = { "test", "-v" }; \
    const int argc = sizeof(argv)/sizeof(argv[0]); \
    int rc = UnityMain(argc, argv, TEST_ ## group_ ## _GROUP_RUNNER); \
    printf("\nTests finished, rc=%d\n", rc); \
} while(0)

#endif // CONFIG_IDF_TARGET
