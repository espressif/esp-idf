import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_examples_partition_ops(env, extra_data):

    dut = env.get_dut('partition_ops', 'examples/storage/partition_api/partition_ops')
    dut.start_app()

    # ESP_ERROR_CHECK or assert will cause abort on error and "Example end" won't be received
    dut.expect_all('Written data: ESP-IDF Partition Operations Example (Read, Erase, Write)',
                   'Read data: ESP-IDF Partition Operations Example (Read, Erase, Write)',
                   'Erased data',
                   'Example end',
                   timeout=20)


if __name__ == '__main__':
    test_examples_partition_ops()
