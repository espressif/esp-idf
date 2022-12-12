/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Parameterized Test Framework
 *
 * Peripherals like SPI has several parameters like: freq, mode, DMA, etc.
 * This framework helps to test different parameter sets with the same program.
 *
 * Each "parameter set" is a set with different parameters (freq, mode, etc.).
 * A parameter group is a group of several parameter sets. Each unit test performs
 * tests among the sets in a group.
 *
 * The test will be execute in the following sequence:
 *      1. ``pre_test``: initialize the context
 *      2. take a set out of the parameter group
 *      3. ``def_param``: fill in default value for the parameter set if not set
 *      4. ``loop``: execute test program for the set in the initialized context
 *      5. loop executing 2-4 until the last set
 *      6. ``post_test``: free the resources used.
 *
 * Usage example:
 *
 * 1. Define your own parameter set type:
 *      typedef struct {
 *          const char pset_name[PSET_NAME_LEN];
 *          //The test work till the frequency below,
 *          //set the frequency to higher and remove checks in the driver to know how fast the system can run.
 *          const int *freq_list;     // list of tested frequency, terminated by 0
 *          int freq_limit;     //freq larger (not equal) than this will be ignored
 *          spi_dup_t dup;
 *          int mode;
 *          bool length_aligned;
 *          int test_size;
 *
 *          int master_limit;   // the master disable dummy bits and discard readings over this freq
 *          bool master_iomux;
 *          int master_dma_chan;
 *
 *          bool slave_iomux;
 *          int slave_dma_chan;
 *          int slave_tv_ns;
 *          bool slave_unaligned_addr;
 *      } spitest_param_set_t;
 *
 * 2. Define a parameter set:
 *      spitest_param_set_t mode_pgroup[] = {
 *          //non-DMA tests
 *          { .pset_name = "mode 0, no DMA",
 *            .freq_list = test_freq_mode,
 *            .master_limit = FREQ_LIMIT_MODE,
 *            .dup = FULL_DUPLEX,
 *            .master_iomux= true,
 *            .slave_iomux = true,
 *            .slave_tv_ns = TV_WITH_ESP_SLAVE,
 *            .mode = 0,
 *          },
 *          { .pset_name = "mode 1, no DMA",
 *            .freq_list = test_freq_mode,
 *            .master_limit = FREQ_LIMIT_MODE,
 *            .dup = FULL_DUPLEX,
 *            .master_iomux= true,
 *            .slave_iomux = true,
 *            .slave_tv_ns = TV_WITH_ESP_SLAVE,
 *            .mode = 1,
 *          },
 *          // other configurations...
 *      };
 *
 * 3. Define your test functions, and wrap them in the ``param_test_func_t``:
 *      static const param_test_func_t master_test_func = {
 *          .pre_test = test_master_init,
 *          .post_test = test_master_deinit,
 *          .loop = test_master_loop,
 *          .def_param = spitest_def_param,
 *      };
 *
 * 4. Declare the group by PARAM_GROUP_DECLARE right after the param group:
 *      PARAM_GROUP_DECLARE(MODE, mode_pgroup)
 *
 * 5. Declare the test function by TEST_SINGLE_BOARD (for single board test), or TEST_MASTER_SLAVE(for multiboard test)
 *      TEST_MASTER_SLAVE(MODE, mode_pgroup, "[spi][timeout=120]", &master_test_func, &slave_test_func)
 *
 *      or
 *      TEST_SINGLE_BOARD(TIMING, timing_pgroup, "[spi][timeout=120]", &local_test_func)
 *
 * NOTE: suggest to define your own macro to wrap 4 and 5 if your tag and test functions are the same. E.g.:
 *      #define TEST_SPI_MASTER_SLAVE(name, pgroup) (backslash)
 *          PARAM_GROUP_DECLARE(name, pgroup)           (backslash)
 *          TEST_MASTER_SLAVE(name, pgroup, "[spi][timeout=120]", &master_test_func, &slave_test_func)
 *
 * Then declare tests conveniently by:
 *      TEST_SPI_MASTER_SLAVE(TIMING, timing_pgroup)
 *      TEST_SPI_MASTER_SLAVE(MODE, mode_pgroup)
 *
 */

#define PGROUP_NAME_LEN    20               ///< name length of parameter group
#define PGROUP_NAME(name) PGROUP_##name     ///< param group name
#define PTEST_MASTER_NAME(name) PTEST_MASTER_##name ///< test function name of master
#define PTEST_SLAVE_NAME(name) PTEST_SLAVE_##name   ///< test function name of slave

/// Test set structure holding name, param set array pointer, item size and param set num.
typedef struct {
    char name[PGROUP_NAME_LEN];   ///< Name of param group to print
    void *param_group;          ///< Start of the param group array
    int pset_size;              ///< Size of each param set
    int pset_num;               ///< Total number of param sets
} param_group_t;

/// Test functions for the frameowrk
typedef struct {
    void (*pre_test)(void** contxt);    ///< Initialization function called before tests begin. Initial your context here
    void (*post_test)(void* context);   ///< Deinit function called after all tests are done.
    void (*def_param)(void* inout_pset);    ///< Function to fill each pset structure before executed, left NULL if not used.
    void (*loop)(const void* pset, void* context); ///< Function execute each param set
} ptest_func_t;

/**
 * Test framework to execute init, loop and deinit.
 *
 * @param param_group Parameter group holder to test in turns.
 * @param test_func Function set to execute.
 */
void test_serializer(const param_group_t *param_group, const ptest_func_t* test_func);

#define PARAM_GROUP_DECLARE_TYPE(group_name, pset_type, pgroup) \
    static const param_group_t PGROUP_NAME(pgroup) = { \
        .name = #group_name, \
        .param_group = (void*)&pgroup, \
        .pset_size = sizeof(pset_type), \
        .pset_num = sizeof(pgroup)/sizeof(pset_type), \
    };

/**
 * Declare parameter group
 *
 * @param group_name Parameter group name to print in the beginning of the test
 * @param param_group Parameter group structure, should be already defined above, and the size and type is defined.
 */
#define PARAM_GROUP_DECLARE(group_name, param_group) \
    PARAM_GROUP_DECLARE_TYPE(group_name, typeof(param_group[0]), param_group)

/**
 * Test parameter group on one board.
 *
 * @param name Test name to be printed in the menu.
 * @param param_group Parameter group to be tested.
 * @param tag Tag for environment, etc. e.g. [spi][timeout=120]
 * @param test_func ``ptest_func_t`` to be executed.
 */
#define TEST_SINGLE_BOARD(name, param_group, tag, test_func) \
    TEST_CASE("single board test: "#name, tag) { test_serializer(&PGROUP_NAME(param_group), test_func); }

/**
 * Test parameter group for master-slave framework
 *
 * @param name Test name to be printed in the menu.
 * @param param_group Parameter group to be tested.
 * @param tag Tag for environment, etc. e.g. [spi][timeout=120]
 * @param master_func ``ptest_func_t`` to be executed by master.
 * @param slave_func ``ptest_func_t`` to be executed by slave.
 */
#define TEST_MASTER_SLAVE(name, param_group, tag, master_func, slave_func) \
    static void PTEST_MASTER_NAME(name) (void) { test_serializer(&PGROUP_NAME(param_group), master_func); } \
    static void PTEST_SLAVE_NAME(name) (void) { test_serializer(&PGROUP_NAME(param_group), slave_func); } \
    TEST_CASE_MULTIPLE_DEVICES("master slave test: "#name, tag, PTEST_MASTER_NAME(name), PTEST_SLAVE_NAME(name))
