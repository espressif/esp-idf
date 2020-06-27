import re
import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_examples_partition_mmap(env, extra_data):

    dut = env.get_dut('partition_mmap', 'examples/storage/partition_api/partition_mmap')
    dut.start_app()

    # ESP_ERROR_CHECK or assert will cause abort on error and "Example end" won't be received
    dut.expect_all('Written sample data to partition: ESP-IDF Partition Memory Map Example',
                   re.compile(r'Mapped partition to data memory address \S+'),
                   'Read sample data from partition using mapped memory: ESP-IDF Partition Memory Map Example',
                   'Data matches',
                   'Unmapped partition from data memory',
                   'Example end',
                   timeout=20)


if __name__ == '__main__':
    test_examples_partition_mmap()
