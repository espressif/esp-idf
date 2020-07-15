import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_examples_partition_find(env, extra_data):

    dut = env.get_dut('partition_find', 'examples/storage/partition_api/partition_find')
    dut.start_app()

    def expect_partition(name, offset, size):
        dut.expect("found partition '{}' at offset {:#x} with size {:#x}".format(name, offset, size), timeout=5)

    def expect_find_partition(_type, subtype, label, name, offset, size):
        dut.expect('Find partition with type {}, subtype {}, label {}'.format(_type, subtype, label), timeout=5)
        expect_partition(name, offset, size)

    dut.expect('----------------Find partitions---------------', timeout=20)
    expect_find_partition('ESP_PARTITION_TYPE_DATA', 'ESP_PARTITION_SUBTYPE_DATA_NVS', 'NULL',
                          'nvs', 0x9000, 0x6000)
    expect_find_partition('ESP_PARTITION_TYPE_DATA', 'ESP_PARTITION_SUBTYPE_DATA_PHY', 'NULL',
                          'phy_init', 0xf000,  0x1000)
    expect_find_partition('ESP_PARTITION_TYPE_APP', 'ESP_PARTITION_SUBTYPE_APP_FACTORY', 'NULL',
                          'factory', 0x10000, 0x100000)
    expect_find_partition('ESP_PARTITION_TYPE_DATA', 'ESP_PARTITION_SUBTYPE_DATA_FAT', 'NULL',
                          'storage1', 0x110000, 0x40000)

    dut.expect('Find second FAT partition by specifying the label', timeout=5)
    expect_find_partition('ESP_PARTITION_TYPE_DATA', 'ESP_PARTITION_SUBTYPE_DATA_FAT', 'storage2',
                          'storage2', 0x150000, 0x40000)

    dut.expect_all('----------------Iterate through partitions---------------',
                   'Iterating through app partitions...', timeout=5)
    expect_partition('factory', 0x10000, 0x100000)

    dut.expect('Iterating through data partitions...', timeout=5)
    expect_partition('nvs', 0x9000, 0x6000)
    expect_partition('phy_init', 0xf000,  0x1000)
    expect_partition('storage1', 0x110000, 0x40000)
    expect_partition('storage2', 0x150000, 0x40000)

    dut.expect('Example end', timeout=5)


if __name__ == '__main__':
    test_examples_partition_find()
