/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "catch.hpp"
#include "nvs.hpp"
#include "nvs_test_api.h"
#include "sdkconfig.h"
#include "spi_flash_emulation.h"
#include "nvs_partition_manager.hpp"
#include "nvs_partition.hpp"
#include "mbedtls/aes.h"
#include "mbedtls/md.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <string>
#include "test_fixtures.hpp"

#define TEST_ESP_ERR(rc, res) CHECK((rc) == (res))
#define TEST_ESP_OK(rc) CHECK((rc) == ESP_OK)

stringstream s_perf;

bool memeq(void *a, size_t a_len, void *b, size_t b_len)
{
    if (a_len != b_len) {
        return false;
    }
    return memcmp(a, b, a_len) == 0;
}

static void check_nvs_part_gen_args(SpiFlashEmulator *spi_flash_emulator,
                                    char const *part_name,
                                    int size,
                                    char const *filename,
                                    bool is_encr,
                                    nvs_sec_cfg_t *xts_cfg)
{
    nvs_handle_t handle;

    esp_partition_t esp_part;
    esp_part.encrypted = false; // we're not testing generic flash encryption here, only the legacy NVS encryption
    esp_part.address = 0;
    esp_part.size = size * SPI_FLASH_SEC_SIZE;
    strncpy(esp_part.label, part_name, PART_NAME_MAX_SIZE);
    unique_ptr<nvs::Partition> part;

    if (is_encr) {
        nvs::NVSEncryptedPartition *enc_part = new (std::nothrow) nvs::NVSEncryptedPartition(&esp_part);
        REQUIRE(enc_part != nullptr);
        TEST_ESP_OK(enc_part->init(xts_cfg));
        part.reset(enc_part);
    } else {
        part.reset(new PartitionEmulation(spi_flash_emulator, 0, size, part_name));
    }

    TEST_ESP_OK(  nvs::NVSPartitionManager::get_instance()->init_custom(part.get(), 0, size) );

    TEST_ESP_OK( nvs_open_from_partition(part_name, "dummyNamespace", NVS_READONLY, &handle));
    uint8_t u8v;
    TEST_ESP_OK( nvs_get_u8(handle, "dummyU8Key", &u8v));
    CHECK(u8v == 127);
    int8_t i8v;
    TEST_ESP_OK( nvs_get_i8(handle, "dummyI8Key", &i8v));
    CHECK(i8v == -128);
    uint16_t u16v;
    TEST_ESP_OK( nvs_get_u16(handle, "dummyU16Key", &u16v));
    CHECK(u16v == 32768);
    uint32_t u32v;
    TEST_ESP_OK( nvs_get_u32(handle, "dummyU32Key", &u32v));
    CHECK(u32v == 4294967295);
    int32_t i32v;
    TEST_ESP_OK( nvs_get_i32(handle, "dummyI32Key", &i32v));
    CHECK(i32v == -2147483648);

    char string_buf[256];
    const char test_str[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n"
                             "Fusce quis risus justo.\n"
                             "Suspendisse egestas in nisi sit amet auctor.\n"
                             "Pellentesque rhoncus dictum sodales.\n"
                             "In justo erat, viverra at interdum eget, interdum vel dui.";
    size_t str_len = sizeof(test_str);
    TEST_ESP_OK( nvs_get_str(handle, "dummyStringKey", string_buf, &str_len));
    CHECK(strncmp(string_buf, test_str, str_len) == 0);

    char buf[64] = {0};
    uint8_t hexdata[] = {0x01, 0x02, 0x03, 0xab, 0xcd, 0xef};
    size_t buflen = 64;
    int j;
    TEST_ESP_OK( nvs_get_blob(handle, "dummyHex2BinKey", buf, &buflen));
    CHECK(memeq(buf, buflen, hexdata, sizeof(hexdata)));

    uint8_t base64data[] = {'1', '2', '3', 'a', 'b', 'c'};
    TEST_ESP_OK( nvs_get_blob(handle, "dummyBase64Key", buf, &buflen));
    CHECK(memeq(buf, buflen, base64data, sizeof(base64data)));

    buflen = 64;
    uint8_t hexfiledata[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
    TEST_ESP_OK( nvs_get_blob(handle, "hexFileKey", buf, &buflen));
    CHECK(memeq(buf, buflen, hexfiledata, sizeof(hexfiledata)));

    buflen = 64;
    const char strfiledata[64] = "abcdefghijklmnopqrstuvwxyz";
    TEST_ESP_OK( nvs_get_str(handle, "stringFileKey", buf, &buflen));
    CHECK(strcmp(buf, strfiledata) == 0);

    char bin_data[5200];
    size_t bin_len = sizeof(bin_data);
    char binfiledata[5200];
    ifstream file;
    file.open(filename);
    file.read(binfiledata,5200);
    size_t binfile_len = file.gcount();
    TEST_ESP_OK( nvs_get_blob(handle, "binFileKey", bin_data, &bin_len));
    CHECK(memeq(bin_data, bin_len, binfiledata, binfile_len));

    file.close();

    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(part_name));
}

static void check_nvs_part_gen_args_mfg(SpiFlashEmulator *spi_flash_emulator,
                                        char const *part_name,
                                        int size,
                                        char const *filename,
                                        bool is_encr,
                                        nvs_sec_cfg_t *xts_cfg)
{
    nvs_handle_t handle;

    esp_partition_t esp_part;
    esp_part.encrypted = false; // we're not testing generic flash encryption here, only the legacy NVS encryption
    esp_part.address = 0;
    esp_part.size = size * SPI_FLASH_SEC_SIZE;
    strncpy(esp_part.label, part_name, PART_NAME_MAX_SIZE);
    unique_ptr<nvs::Partition> part;

    if (is_encr) {
        nvs::NVSEncryptedPartition *enc_part = new (std::nothrow) nvs::NVSEncryptedPartition(&esp_part);
        REQUIRE(enc_part != nullptr);
        TEST_ESP_OK(enc_part->init(xts_cfg));
        part.reset(enc_part);
    } else {
        part.reset(new PartitionEmulation(spi_flash_emulator, 0, size, part_name));
    }

    TEST_ESP_OK(  nvs::NVSPartitionManager::get_instance()->init_custom(part.get(), 0, size) );

    TEST_ESP_OK( nvs_open_from_partition(part_name, "dummyNamespace", NVS_READONLY, &handle));
    uint8_t u8v;
    TEST_ESP_OK( nvs_get_u8(handle, "dummyU8Key", &u8v));
    CHECK(u8v == 127);
    int8_t i8v;
    TEST_ESP_OK( nvs_get_i8(handle, "dummyI8Key", &i8v));
    CHECK(i8v == -128);
    uint16_t u16v;
    TEST_ESP_OK( nvs_get_u16(handle, "dummyU16Key", &u16v));
    CHECK(u16v == 32768);
    uint32_t u32v;
    TEST_ESP_OK( nvs_get_u32(handle, "dummyU32Key", &u32v));
    CHECK(u32v == 4294967295);
    int32_t i32v;
    TEST_ESP_OK( nvs_get_i32(handle, "dummyI32Key", &i32v));
    CHECK(i32v == -2147483648);

    char buf[64] = {0};
    size_t buflen = 64;
    TEST_ESP_OK( nvs_get_str(handle, "dummyStringKey", buf, &buflen));
    CHECK(strncmp(buf, "0A:0B:0C:0D:0E:0F", buflen) == 0);

    uint8_t hexdata[] = {0x01, 0x02, 0x03, 0xab, 0xcd, 0xef};
    buflen = 64;
    int j;
    TEST_ESP_OK( nvs_get_blob(handle, "dummyHex2BinKey", buf, &buflen));
    CHECK(memeq(buf, buflen, hexdata, sizeof(hexdata)));

    uint8_t base64data[] = {'1', '2', '3', 'a', 'b', 'c'};
    TEST_ESP_OK( nvs_get_blob(handle, "dummyBase64Key", buf, &buflen));
    CHECK(memeq(buf, buflen, base64data, sizeof(base64data)));

    buflen = 64;
    uint8_t hexfiledata[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
    TEST_ESP_OK( nvs_get_blob(handle, "hexFileKey", buf, &buflen));
    CHECK(memeq(buf, buflen, hexfiledata, sizeof(hexfiledata)));

    buflen = 64;
    const char strfiledata[64] = "abcdefghijklmnopqrstuvwxyz";
    TEST_ESP_OK( nvs_get_str(handle, "stringFileKey", buf, &buflen));
    CHECK(strcmp(buf, strfiledata) == 0);

    char bin_data[5200];
    size_t bin_len = sizeof(bin_data);
    char binfiledata[5200];
    ifstream file;
    file.open(filename);
    file.read(binfiledata,5200);
    size_t binfile_len = file.gcount();
    TEST_ESP_OK( nvs_get_blob(handle, "binFileKey", bin_data, &bin_len));
    CHECK(memeq(bin_data, bin_len, binfiledata, binfile_len));

    file.close();

    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(part_name));
}

#if CONFIG_NVS_ENCRYPTION
TEST_CASE("check underlying xts code for 32-byte size sector encryption", "[nvs]")
{
    auto toHex = [](char ch) {
        if(ch >= '0' && ch <= '9')
            return ch - '0';
        else if(ch >= 'a' && ch <= 'f')
            return ch - 'a' + 10;
        else if(ch >= 'A' && ch <= 'F')
            return ch - 'A' + 10;
        else
            return 0;
    };

    auto toHexByte = [toHex](char* c) {
        return 16 * toHex(c[0]) + toHex(c[1]);
    };

    auto toHexStream = [toHexByte](char* src, uint8_t* dest) {
        uint32_t cnt =0;
        char* p = src;
        while(*p != '\0' && *(p + 1) != '\0')
        {
            dest[cnt++] = toHexByte(p); p += 2;
        }
    };

    uint8_t eky_hex[2 * NVS_KEY_SIZE];
    uint8_t ptxt_hex[nvs::Page::ENTRY_SIZE], ctxt_hex[nvs::Page::ENTRY_SIZE], ba_hex[16];
    mbedtls_aes_xts_context ectx[1];
    mbedtls_aes_xts_context dctx[1];

    char eky[][2 * NVS_KEY_SIZE + 1] = {
        "0000000000000000000000000000000000000000000000000000000000000000",
        "1111111111111111111111111111111111111111111111111111111111111111"
    };
    char tky[][2 * NVS_KEY_SIZE + 1] = {
        "0000000000000000000000000000000000000000000000000000000000000000",
        "2222222222222222222222222222222222222222222222222222222222222222"
    };
    char blk_addr[][2*16 + 1]  = {
        "00000000000000000000000000000000",
        "33333333330000000000000000000000"
    };

    char ptxt[][2 * nvs::Page::ENTRY_SIZE + 1] = {
        "0000000000000000000000000000000000000000000000000000000000000000",
        "4444444444444444444444444444444444444444444444444444444444444444"
    };
    char ctxt[][2 * nvs::Page::ENTRY_SIZE + 1] = {
        "d456b4fc2e620bba6ffbed27b956c9543454dd49ebd8d8ee6f94b65cbe158f73",
        "e622334f184bbce129a25b2ac76b3d92abf98e22df5bdd15af471f3db8946a85"
    };

    mbedtls_aes_xts_init(ectx);
    mbedtls_aes_xts_init(dctx);

    for(uint8_t cnt = 0; cnt < sizeof(eky)/sizeof(eky[0]); cnt++) {
        toHexStream(eky[cnt], eky_hex);
        toHexStream(tky[cnt], &eky_hex[NVS_KEY_SIZE]);
        toHexStream(ptxt[cnt], ptxt_hex);
        toHexStream(ctxt[cnt], ctxt_hex);
        toHexStream(blk_addr[cnt], ba_hex);

        CHECK(!mbedtls_aes_xts_setkey_enc(ectx, eky_hex, 2 * NVS_KEY_SIZE * 8));
        CHECK(!mbedtls_aes_xts_setkey_enc(dctx, eky_hex, 2 * NVS_KEY_SIZE * 8));

        CHECK(!mbedtls_aes_crypt_xts(ectx, MBEDTLS_AES_ENCRYPT, nvs::Page::ENTRY_SIZE, ba_hex, ptxt_hex, ptxt_hex));

        CHECK(!memcmp(ptxt_hex, ctxt_hex, nvs::Page::ENTRY_SIZE));
    }
}

TEST_CASE("test nvs apis with encryption enabled", "[nvs]")
{
    nvs_handle_t handle_1;
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;

    nvs_sec_cfg_t xts_cfg;
    for(int count = 0; count < NVS_KEY_SIZE; count++) {
        xts_cfg.eky[count] = 0x11;
        xts_cfg.tky[count] = 0x22;
    }
    EncryptedPartitionFixture fixture(&xts_cfg, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN);
    fixture.emu.randomize(100);
    fixture.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);

    for (uint16_t i = NVS_FLASH_SECTOR; i <NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN; ++i) {
        fixture.emu.erase(i);
    }
    TEST_ESP_OK( nvs::NVSPartitionManager::get_instance()->
            init_custom(&fixture.part, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));

    TEST_ESP_ERR(nvs_open("namespace1", NVS_READONLY, &handle_1), ESP_ERR_NVS_NOT_FOUND);

    TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle_1));
    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x12345678));
    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x23456789));

    nvs_handle_t handle_2;
    TEST_ESP_OK(nvs_open("namespace2", NVS_READWRITE, &handle_2));
    TEST_ESP_OK(nvs_set_i32(handle_2, "foo", 0x3456789a));
    const char* str = "value 0123456789abcdef0123456789abcdef";
    TEST_ESP_OK(nvs_set_str(handle_2, "key", str));

    int32_t v1;
    TEST_ESP_OK(nvs_get_i32(handle_1, "foo", &v1));
    CHECK(0x23456789 == v1);

    int32_t v2;
    TEST_ESP_OK(nvs_get_i32(handle_2, "foo", &v2));
    CHECK(0x3456789a == v2);

    char buf[strlen(str) + 1];
    size_t buf_len = sizeof(buf);

    size_t buf_len_needed;
    TEST_ESP_OK(nvs_get_str(handle_2, "key", NULL, &buf_len_needed));
    CHECK(buf_len_needed == buf_len);

    size_t buf_len_short = buf_len - 1;
    TEST_ESP_ERR(ESP_ERR_NVS_INVALID_LENGTH, nvs_get_str(handle_2, "key", buf, &buf_len_short));
    CHECK(buf_len_short == buf_len);

    size_t buf_len_long = buf_len + 1;
    TEST_ESP_OK(nvs_get_str(handle_2, "key", buf, &buf_len_long));
    CHECK(buf_len_long == buf_len);

    TEST_ESP_OK(nvs_get_str(handle_2, "key", buf, &buf_len));

    CHECK(0 == strcmp(buf, str));
    nvs_close(handle_1);
    nvs_close(handle_2);
    TEST_ESP_OK(nvs_flash_deinit());
}

TEST_CASE("test nvs apis for nvs partition generator utility with encryption enabled", "[nvs_part_gen]")
{
    int status;
    int childpid = fork();
    if (childpid == 0) {
        exit(execlp("cp", " cp",
                    "-rf",
                    "../nvs_partition_generator/testdata",
                    ".", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

        childpid = fork();

        if (childpid == 0) {
            exit(execlp("python", "python",
                        "../nvs_partition_generator/nvs_partition_gen.py",
                        "encrypt",
                        "../nvs_partition_generator/sample_multipage_blob.csv",
                        "partition_encrypted.bin",
                        "0x4000",
                        "--inputkey",
                        "../nvs_partition_generator/testdata/sample_encryption_keys.bin",
                        "--outdir",
                        "../nvs_partition_generator", NULL));
        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);
        }
    }

    SpiFlashEmulator emu("../nvs_partition_generator/partition_encrypted.bin");

    nvs_sec_cfg_t cfg;
    for (int count = 0; count < NVS_KEY_SIZE; count++) {
        cfg.eky[count] = 0x11;
        cfg.tky[count] = 0x22;
    }

    check_nvs_part_gen_args(&emu, NVS_DEFAULT_PART_NAME, 4, "../nvs_partition_generator/testdata/sample_multipage_blob.bin", true, &cfg);

    childpid = fork();
    if (childpid == 0) {
        exit(execlp("rm", " rm",
                    "-rf",
                    "testdata", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

    }

}

TEST_CASE("test decrypt functionality for encrypted data", "[nvs_part_gen]")
{

    //retrieving the temporary test data
    int status = system("cp -rf ../nvs_partition_generator/testdata .");
    CHECK(status == 0);

    //encoding data from sample_multipage_blob.csv
    status = system("python ../nvs_partition_generator/nvs_partition_gen.py generate ../nvs_partition_generator/sample_multipage_blob.csv partition_encoded.bin 0x5000 --outdir ../nvs_partition_generator");
    CHECK(status == 0);

    //encrypting data from sample_multipage_blob.csv
    status = system("python ../nvs_partition_generator/nvs_partition_gen.py encrypt ../nvs_partition_generator/sample_multipage_blob.csv partition_encrypted.bin 0x5000 --inputkey ../nvs_partition_generator/testdata/sample_encryption_keys.bin --outdir ../nvs_partition_generator");
    CHECK(status == 0);

    //encrypting data from sample_multipage_blob.csv (hmac-based scheme)
    status = system("python ../nvs_partition_generator/nvs_partition_gen.py encrypt ../nvs_partition_generator/sample_multipage_blob.csv partition_encrypted_hmac.bin 0x5000 --keygen --key_protect_hmac --kp_hmac_inputkey ../nvs_partition_generator/testdata/sample_hmac_key.bin --outdir ../nvs_partition_generator");
    CHECK(status == 0);

    //decrypting data from partition_encrypted.bin
    status = system("python ../nvs_partition_generator/nvs_partition_gen.py decrypt ../nvs_partition_generator/partition_encrypted.bin ../nvs_partition_generator/testdata/sample_encryption_keys.bin ../nvs_partition_generator/partition_decrypted.bin");
    CHECK(status == 0);

    status = system("diff ../nvs_partition_generator/partition_decrypted.bin ../nvs_partition_generator/partition_encoded.bin");
    CHECK(status == 0);

    //decrypting data from partition_encrypted_hmac.bin
    status = system("python ../nvs_partition_generator/nvs_partition_gen.py decrypt ../nvs_partition_generator/partition_encrypted_hmac.bin ../nvs_partition_generator/testdata/sample_encryption_keys_hmac.bin ../nvs_partition_generator/partition_decrypted_hmac.bin");
    CHECK(status == 0);

    status = system("diff ../nvs_partition_generator/partition_decrypted_hmac.bin ../nvs_partition_generator/partition_encoded.bin");
    CHECK(status == 0);

    CHECK(WEXITSTATUS(status) == 0);


    //cleaning up the temporary test data
    status = system("rm -rf testdata");
    CHECK(status == 0);

}

TEST_CASE("test nvs apis for nvs partition generator utility with encryption enabled using keygen", "[nvs_part_gen]")
{
    int childpid = fork();
    int status;

    if (childpid == 0) {
        exit(execlp("cp", " cp",
                    "-rf",
                    "../nvs_partition_generator/testdata",
                    ".", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

        childpid = fork();

        if (childpid == 0) {
            exit(execlp("rm", " rm",
                        "-rf",
                        "../nvs_partition_generator/keys", NULL));
        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);

            childpid = fork();
            if (childpid == 0) {
                exit(execlp("python", "python",
                            "../nvs_partition_generator/nvs_partition_gen.py",
                            "encrypt",
                            "../nvs_partition_generator/sample_multipage_blob.csv",
                            "partition_encrypted_using_keygen.bin",
                            "0x4000",
                            "--keygen",
                            "--outdir",
                            "../nvs_partition_generator", NULL));

            } else {
                CHECK(childpid > 0);
                waitpid(childpid, &status, 0);
                CHECK(WEXITSTATUS(status) == 0);

            }
        }
    }


    DIR *dir;
    struct dirent *file;
    char *filename;
    char *files;
    char *file_ext;

    dir = opendir("../nvs_partition_generator/keys");
    while ((file = readdir(dir)) != NULL) {
        filename = file->d_name;
        files = strrchr(filename, '.');
        if (files != NULL) {
            file_ext = files + 1;
            if (strncmp(file_ext, "bin", 3) == 0) {
                break;
            }
        }
    }

    std::string encr_file = std::string("../nvs_partition_generator/keys/") + std::string(filename);
    SpiFlashEmulator emu("../nvs_partition_generator/partition_encrypted_using_keygen.bin");

    char buffer[64];
    FILE *fp;

    fp = fopen(encr_file.c_str(), "rb");
    fread(buffer, sizeof(buffer), 1, fp);

    fclose(fp);

    nvs_sec_cfg_t cfg;

    for (int count = 0; count < NVS_KEY_SIZE; count++) {
        cfg.eky[count] = buffer[count] & 255;
        cfg.tky[count] = buffer[count + 32] & 255;
    }

    check_nvs_part_gen_args(&emu, NVS_DEFAULT_PART_NAME, 4, "../nvs_partition_generator/testdata/sample_multipage_blob.bin", true, &cfg);

}

TEST_CASE("test nvs apis for nvs partition generator utility with encryption enabled using inputkey", "[nvs_part_gen]")
{
    int childpid = fork();
    int status;

    DIR *dir;
    struct dirent *file;
    char *filename;
    char *files;
    char *file_ext;

    dir = opendir("../nvs_partition_generator/keys");
    while ((file = readdir(dir)) != NULL) {
        filename = file->d_name;
        files = strrchr(filename, '.');
        if (files != NULL) {
            file_ext = files + 1;
            if (strncmp(file_ext, "bin", 3) == 0) {
                break;
            }
        }
    }

    std::string encr_file = std::string("../nvs_partition_generator/keys/") + std::string(filename);

    if (childpid == 0) {
        exit(execlp("python", "python",
                    "../nvs_partition_generator/nvs_partition_gen.py",
                    "encrypt",
                    "../nvs_partition_generator/sample_multipage_blob.csv",
                    "partition_encrypted_using_keyfile.bin",
                    "0x4000",
                    "--inputkey",
                    encr_file.c_str(),
                    "--outdir",
                    "../nvs_partition_generator", NULL));

    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);
    }

    SpiFlashEmulator emu("../nvs_partition_generator/partition_encrypted_using_keyfile.bin");

    char buffer[64];
    FILE *fp;

    fp = fopen(encr_file.c_str(), "rb");
    fread(buffer, sizeof(buffer), 1, fp);

    fclose(fp);

    nvs_sec_cfg_t cfg;

    for (int count = 0; count < NVS_KEY_SIZE; count++) {
        cfg.eky[count] = buffer[count] & 255;
        cfg.tky[count] = buffer[count + 32] & 255;
    }

    check_nvs_part_gen_args(&emu, NVS_DEFAULT_PART_NAME, 4, "../nvs_partition_generator/testdata/sample_multipage_blob.bin", true, &cfg);

    childpid = fork();
    if (childpid == 0) {
        exit(execlp("rm", " rm",
                    "-rf",
                    "../nvs_partition_generator/keys", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

        childpid = fork();

        if (childpid == 0) {
            exit(execlp("rm", " rm",
                        "-rf",
                        "testdata", NULL));
        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);
        }
    }

}

static void compute_nvs_keys_with_hmac(nvs_sec_cfg_t *cfg, void *hmac_key)
{
    unsigned char key_bytes[32] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
                                    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                                    0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20 };
    if (hmac_key != NULL){
        memcpy(key_bytes, hmac_key, 32);
    }

    unsigned char ekey_seed[32], tkey_seed[32];

    for (unsigned int i = 0; i < sizeof(ekey_seed); i+=4) {
        ekey_seed[i]     = 0x5A;
        ekey_seed[i + 1] = 0x5A;
        ekey_seed[i + 2] = 0xBE;
        ekey_seed[i + 3] = 0xAE;
    }

    for (unsigned int i = 0; i < sizeof(tkey_seed); i+=4) {
        tkey_seed[i]     = 0xA5;
        tkey_seed[i + 1] = 0xA5;
        tkey_seed[i + 2] = 0xDE;
        tkey_seed[i + 3] = 0xCE;
    }

    const mbedtls_md_type_t alg = MBEDTLS_MD_SHA256;

    mbedtls_md_context_t ctx;
    mbedtls_md_init(&ctx);

    const mbedtls_md_info_t *info = mbedtls_md_info_from_type(alg);
    mbedtls_md_setup(&ctx, info, 1);
    mbedtls_md_hmac_starts(&ctx, key_bytes, sizeof(key_bytes));

    mbedtls_md_hmac_update(&ctx, ekey_seed, sizeof(ekey_seed));
    mbedtls_md_hmac_finish(&ctx, cfg->eky);

    mbedtls_md_hmac_reset(&ctx);
    mbedtls_md_hmac_update(&ctx, tkey_seed, sizeof(tkey_seed));
    mbedtls_md_hmac_finish(&ctx, cfg->tky);

    assert(memcmp(cfg->eky, cfg->tky, NVS_KEY_SIZE));

    mbedtls_md_free(&ctx);
}

TEST_CASE("test nvs apis for nvs partition generator utility with encryption enabled using keygen (user-provided HMAC-key)", "[nvs_part_gen]")
{
    int childpid = fork();
    int status;

    if (childpid == 0) {
        exit(execlp("cp", " cp",
                    "-rf",
                    "../nvs_partition_generator/testdata",
                    ".", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

        childpid = fork();

        if (childpid == 0) {
            exit(execlp("rm", " rm",
                        "-rf",
                        "../nvs_partition_generator/keys", NULL));
        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);

            childpid = fork();
            if (childpid == 0) {
                exit(execlp("python", "python",
                            "../nvs_partition_generator/nvs_partition_gen.py",
                            "encrypt",
                            "../nvs_partition_generator/sample_multipage_blob.csv",
                            "partition_encrypted_using_keygen_hmac.bin",
                            "0x4000",
                            "--keygen",
                            "--key_protect_hmac",
                            "--kp_hmac_inputkey",
                            "../nvs_partition_generator/testdata/sample_hmac_key.bin",
                            "--outdir",
                            "../nvs_partition_generator", NULL));

            } else {
                CHECK(childpid > 0);
                waitpid(childpid, &status, 0);
                CHECK(WEXITSTATUS(status) == 0);

            }
        }
    }

    SpiFlashEmulator emu("../nvs_partition_generator/partition_encrypted_using_keygen_hmac.bin");

    nvs_sec_cfg_t cfg;
    compute_nvs_keys_with_hmac(&cfg, NULL);

    check_nvs_part_gen_args(&emu, NVS_DEFAULT_PART_NAME, 4, "../nvs_partition_generator/testdata/sample_multipage_blob.bin", true, &cfg);

}

TEST_CASE("test nvs apis for nvs partition generator utility with encryption enabled using keygen (dynamically generated HMAC-key)", "[nvs_part_gen]")
{
    int childpid = fork();
    int status;

    if (childpid == 0) {
        exit(execlp("cp", " cp",
                    "-rf",
                    "../nvs_partition_generator/testdata",
                    ".", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

        childpid = fork();

        if (childpid == 0) {
            exit(execlp("rm", " rm",
                        "-rf",
                        "../nvs_partition_generator/keys", NULL));
        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);

            childpid = fork();
            if (childpid == 0) {
                exit(execlp("python", "python",
                            "../nvs_partition_generator/nvs_partition_gen.py",
                            "encrypt",
                            "../nvs_partition_generator/sample_multipage_blob.csv",
                            "partition_encrypted_using_keygen_hmac.bin",
                            "0x4000",
                            "--keygen",
                            "--key_protect_hmac",
                            "--kp_hmac_keygen",
                            "--outdir",
                            "../nvs_partition_generator", NULL));

            } else {
                CHECK(childpid > 0);
                waitpid(childpid, &status, 0);
                CHECK(WEXITSTATUS(status) == 0);

            }
        }
    }


    DIR *dir;
    struct dirent *file;
    char *filename;
    char *files;
    char *file_ext;
    char *hmac_key_file;

    dir = opendir("../nvs_partition_generator/keys");
    while ((file = readdir(dir)) != NULL) {
        filename = file->d_name;
        file_ext = NULL;
        files = strrchr(filename, '.');
        if (files != NULL) {
            file_ext = files + 1;
            if (strncmp(file_ext, "bin", 3) != 0) {
                continue;
            }
        }
        if (strstr(filename, "hmac") != NULL) {
            hmac_key_file = filename;
        }
    }

    std::string hmac_key_path = std::string("../nvs_partition_generator/keys/") + std::string(hmac_key_file);
    SpiFlashEmulator emu("../nvs_partition_generator/partition_encrypted_using_keygen_hmac.bin");

    char hmac_key_buf[32];
    FILE *fp;
    fp = fopen(hmac_key_path.c_str(), "rb");
    fread(hmac_key_buf, sizeof(hmac_key_buf), 1, fp);
    fclose(fp);

    nvs_sec_cfg_t cfg;
    compute_nvs_keys_with_hmac(&cfg, hmac_key_buf);

    check_nvs_part_gen_args(&emu, NVS_DEFAULT_PART_NAME, 4, "../nvs_partition_generator/testdata/sample_multipage_blob.bin", true, &cfg);

}

TEST_CASE("check and read data from partition generated via manufacturing utility with encryption enabled using sample inputkey", "[mfg_gen]")
{
    int childpid = fork();
    int status;

    if (childpid == 0) {
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf ../../../tools/mass_mfg/host_test | \
                    cp -rf ../../../tools/mass_mfg/testdata mfg_testdata | \
                    cp -rf ../nvs_partition_generator/testdata . | \
                    mkdir -p ../../../tools/mass_mfg/host_test", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

        childpid = fork();
        if (childpid == 0) {
            exit(execlp("python", "python",
                        "../../../tools/mass_mfg/mfg_gen.py",
                        "generate",
                        "../../../tools/mass_mfg/samples/sample_config.csv",
                        "../../../tools/mass_mfg/samples/sample_values_multipage_blob.csv",
                        "Test",
                        "0x4000",
                        "--outdir",
                        "../../../tools/mass_mfg/host_test",
                        "--version",
                        "2",
                        "--inputkey",
                        "mfg_testdata/sample_encryption_keys.bin", NULL));

        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);

            childpid = fork();
            if (childpid == 0) {
                exit(execlp("python", "python",
                            "../nvs_partition_generator/nvs_partition_gen.py",
                            "encrypt",
                            "../../../tools/mass_mfg/host_test/csv/Test-1.csv",
                            "../nvs_partition_generator/Test-1-partition-encrypted.bin",
                            "0x4000",
                            "--version",
                            "2",
                            "--inputkey",
                            "testdata/sample_encryption_keys.bin", NULL));

            } else {
                CHECK(childpid > 0);
                waitpid(childpid, &status, 0);
                CHECK(WEXITSTATUS(status) == 0);

            }

        }

    }

    SpiFlashEmulator emu1("../../../tools/mass_mfg/host_test/bin/Test-1.bin");

    nvs_sec_cfg_t cfg;
    for (int count = 0; count < NVS_KEY_SIZE; count++) {
        cfg.eky[count] = 0x11;
        cfg.tky[count] = 0x22;
    }

    check_nvs_part_gen_args_mfg(&emu1, NVS_DEFAULT_PART_NAME, 4, "mfg_testdata/sample_multipage_blob.bin", true, &cfg);

    SpiFlashEmulator emu2("../nvs_partition_generator/Test-1-partition-encrypted.bin");

    check_nvs_part_gen_args_mfg(&emu2, NVS_DEFAULT_PART_NAME, 4, "testdata/sample_multipage_blob.bin", true, &cfg);


    childpid = fork();
    if (childpid == 0) {
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf ../../../tools/mass_mfg/host_test | \
                    rm -rf mfg_testdata | \
                    rm -rf testdata", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

    }

}

TEST_CASE("check and read data from partition generated via manufacturing utility with encryption enabled using new generated key", "[mfg_gen]")
{
    int childpid = fork();
    int status;

    if (childpid == 0) {
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf ../../../tools/mass_mfg/host_test | \
                    cp -rf ../../../tools/mass_mfg/testdata mfg_testdata | \
                    cp -rf ../nvs_partition_generator/testdata . | \
                    mkdir -p ../../../tools/mass_mfg/host_test", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

        childpid = fork();
        if (childpid == 0) {
            exit(execlp("python", "python",
                        "../../../tools/mass_mfg/mfg_gen.py",
                        "generate-key",
                        "--outdir",
                        "../../../tools/mass_mfg/host_test",
                        "--keyfile",
                        "encr_keys_host_test.bin", NULL));

        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);

            childpid = fork();
            if (childpid == 0) {
                exit(execlp("python", "python",
                            "../../../tools/mass_mfg/mfg_gen.py",
                            "generate",
                            "../../../tools/mass_mfg/samples/sample_config.csv",
                            "../../../tools/mass_mfg/samples/sample_values_multipage_blob.csv",
                            "Test",
                            "0x4000",
                            "--outdir",
                            "../../../tools/mass_mfg/host_test",
                            "--version",
                            "2",
                            "--inputkey",
                            "../../../tools/mass_mfg/host_test/keys/encr_keys_host_test.bin", NULL));

            } else {
                CHECK(childpid > 0);
                waitpid(childpid, &status, 0);
                CHECK(WEXITSTATUS(status) == 0);

                childpid = fork();
                if (childpid == 0) {
                    exit(execlp("python", "python",
                                "../nvs_partition_generator/nvs_partition_gen.py",
                                "encrypt",
                                "../../../tools/mass_mfg/host_test/csv/Test-1.csv",
                                "../nvs_partition_generator/Test-1-partition-encrypted.bin",
                                "0x4000",
                                "--version",
                                "2",
                                "--inputkey",
                                "../../../tools/mass_mfg/host_test/keys/encr_keys_host_test.bin", NULL));

                } else {
                    CHECK(childpid > 0);
                    waitpid(childpid, &status, 0);
                    CHECK(WEXITSTATUS(status) == 0);

                }

            }

        }

    }

    SpiFlashEmulator emu1("../../../tools/mass_mfg/host_test/bin/Test-1.bin");

    char buffer[64];
    FILE *fp;

    fp = fopen("../../../tools/mass_mfg/host_test/keys/encr_keys_host_test.bin", "rb");
    fread(buffer, sizeof(buffer), 1, fp);

    fclose(fp);

    nvs_sec_cfg_t cfg;

    for (int count = 0; count < NVS_KEY_SIZE; count++) {
        cfg.eky[count] = buffer[count] & 255;
        cfg.tky[count] = buffer[count + 32] & 255;
    }

    check_nvs_part_gen_args_mfg(&emu1, NVS_DEFAULT_PART_NAME, 4, "mfg_testdata/sample_multipage_blob.bin", true, &cfg);

    SpiFlashEmulator emu2("../nvs_partition_generator/Test-1-partition-encrypted.bin");

    check_nvs_part_gen_args_mfg(&emu2, NVS_DEFAULT_PART_NAME, 4, "testdata/sample_multipage_blob.bin", true, &cfg);

    childpid = fork();
    if (childpid == 0) {
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf keys | \
                    rm -rf mfg_testdata | \
                    rm -rf testdata | \
                    rm -rf ../../../tools/mass_mfg/host_test", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

    }

}

TEST_CASE("check and read data from partition generated via manufacturing utility with encryption enabled using new generated key (user-provided HMAC-key)", "[mfg_gen]")
{
    int childpid = fork();
    int status;

    if (childpid == 0) {
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf ../../../tools/mass_mfg/host_test | \
                    cp -rf ../../../tools/mass_mfg/testdata mfg_testdata | \
                    cp -rf ../nvs_partition_generator/testdata . | \
                    mkdir -p ../../../tools/mass_mfg/host_test", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

        childpid = fork();
        if (childpid == 0) {
            exit(execlp("python", "python",
                        "../../../tools/mass_mfg/mfg_gen.py",
                        "generate",
                        "../../../tools/mass_mfg/samples/sample_config.csv",
                        "../../../tools/mass_mfg/samples/sample_values_multipage_blob.csv",
                        "Test",
                        "0x4000",
                        "--version",
                        "2",
                        "--keygen",
                        "--key_protect_hmac",
                        "--kp_hmac_inputkey",
                        "mfg_testdata/sample_hmac_key.bin",
                        "--outdir",
                        "../../../tools/mass_mfg/host_test",NULL));

        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);

            childpid = fork();
            if (childpid == 0) {
                exit(execlp("python", "python",
                            "../nvs_partition_generator/nvs_partition_gen.py",
                            "encrypt",
                            "../../../tools/mass_mfg/host_test/csv/Test-1.csv",
                            "../nvs_partition_generator/Test-1-partition-encrypted-hmac.bin",
                            "0x4000",
                            "--version",
                            "2",
                            "--keygen",
                            "--key_protect_hmac",
                            "--kp_hmac_inputkey",
                            "mfg_testdata/sample_hmac_key.bin", NULL));

            } else {
                CHECK(childpid > 0);
                waitpid(childpid, &status, 0);
                CHECK(WEXITSTATUS(status) == 0);

            }

        }

    }

    SpiFlashEmulator emu1("../../../tools/mass_mfg/host_test/bin/Test-1.bin");

    nvs_sec_cfg_t cfg;
    compute_nvs_keys_with_hmac(&cfg, NULL);

    check_nvs_part_gen_args_mfg(&emu1, NVS_DEFAULT_PART_NAME, 4, "mfg_testdata/sample_multipage_blob.bin", true, &cfg);

    SpiFlashEmulator emu2("../nvs_partition_generator/Test-1-partition-encrypted-hmac.bin");

    check_nvs_part_gen_args_mfg(&emu2, NVS_DEFAULT_PART_NAME, 4, "testdata/sample_multipage_blob.bin", true, &cfg);


    childpid = fork();
    if (childpid == 0) {
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf ../../../tools/mass_mfg/host_test | \
                    rm -rf mfg_testdata | \
                    rm -rf testdata", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

    }

}

TEST_CASE("check and read data from partition generated via manufacturing utility with encryption enabled using new generated key (dynamically generated HMAC-key)", "[mfg_gen]")
{
    int childpid = fork();
    int status;

    if (childpid == 0) {
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf ../../../tools/mass_mfg/host_test | \
                    cp -rf ../../../tools/mass_mfg/testdata mfg_testdata | \
                    cp -rf ../nvs_partition_generator/testdata . | \
                    mkdir -p ../../../tools/mass_mfg/host_test", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

        childpid = fork();
        if (childpid == 0) {
            exit(execlp("python", "python",
                        "../../../tools/mass_mfg/mfg_gen.py",
                        "generate-key",
                        "--outdir",
                        "../../../tools/mass_mfg/host_test",
                        "--key_protect_hmac",
                        "--kp_hmac_keygen",
                        "--kp_hmac_keyfile",
                        "hmac_key_host_test.bin",
                        "--keyfile",
                        "encr_keys_host_test.bin", NULL));

        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);

            childpid = fork();
            if (childpid == 0) {
                exit(execlp("python", "python",
                            "../../../tools/mass_mfg/mfg_gen.py",
                            "generate",
                            "../../../tools/mass_mfg/samples/sample_config.csv",
                            "../../../tools/mass_mfg/samples/sample_values_multipage_blob.csv",
                            "Test",
                            "0x4000",
                            "--outdir",
                            "../../../tools/mass_mfg/host_test",
                            "--version",
                            "2",
                            "--inputkey",
                            "../../../tools/mass_mfg/host_test/keys/encr_keys_host_test.bin", NULL));

            } else {
                CHECK(childpid > 0);
                waitpid(childpid, &status, 0);
                CHECK(WEXITSTATUS(status) == 0);

                childpid = fork();
                if (childpid == 0) {
                    exit(execlp("python", "python",
                                "../nvs_partition_generator/nvs_partition_gen.py",
                                "encrypt",
                                "../../../tools/mass_mfg/host_test/csv/Test-1.csv",
                                "../nvs_partition_generator/Test-1-partition-encrypted-hmac.bin",
                                "0x4000",
                                "--version",
                                "2",
                                "--inputkey",
                                "../../../tools/mass_mfg/host_test/keys/encr_keys_host_test.bin", NULL));

                } else {
                    CHECK(childpid > 0);
                    waitpid(childpid, &status, 0);
                    CHECK(WEXITSTATUS(status) == 0);

                }

            }

        }

    }


    SpiFlashEmulator emu1("../../../tools/mass_mfg/host_test/bin/Test-1.bin");

    char hmac_key_buf[32];
    FILE *fp;

    fp = fopen("../../../tools/mass_mfg/host_test/keys/hmac_key_host_test.bin", "rb");
    fread(hmac_key_buf, sizeof(hmac_key_buf), 1, fp);

    fclose(fp);

    nvs_sec_cfg_t cfg;
    compute_nvs_keys_with_hmac(&cfg, hmac_key_buf);

    check_nvs_part_gen_args_mfg(&emu1, NVS_DEFAULT_PART_NAME, 4, "mfg_testdata/sample_multipage_blob.bin", true, &cfg);

    SpiFlashEmulator emu2("../nvs_partition_generator/Test-1-partition-encrypted-hmac.bin");

    check_nvs_part_gen_args_mfg(&emu2, NVS_DEFAULT_PART_NAME, 4, "testdata/sample_multipage_blob.bin", true, &cfg);

    childpid = fork();
    if (childpid == 0) {
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf keys | \
                    rm -rf mfg_testdata | \
                    rm -rf testdata | \
                    rm -rf ../../../tools/mass_mfg/host_test", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

    }

}

#endif
