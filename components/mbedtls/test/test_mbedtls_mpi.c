/* mbedTLS bignum (MPI) self-tests as unit tests
*/
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <esp_system.h>
#include "mbedtls/bignum.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "sdkconfig.h"

#define MBEDTLS_OK 0

/* Debugging function to print an MPI number to stdout. Happens to
   print output that can be copy-pasted directly into a Python shell.
*/
void mbedtls_mpi_printf(const char *name, const mbedtls_mpi *X)
{
    static char buf[1024];
    size_t n;
    memset(buf, 0, sizeof(buf));
    mbedtls_mpi_write_string(X, 16, buf, sizeof(buf)-1, &n);
    if(n) {
        printf("%s = 0x%s\n", name, buf);
    } else {
        printf("%s = TOOLONG\n", name);
    }
}

/* Assert E = A * B */
static void test_bignum_mult(const char *a_str, const char *b_str, const char *e_str, size_t mod_bits)
{
    mbedtls_mpi A, B, X, E, M;
    char x_buf[1024] = { 0 };
    size_t x_buf_len = 0;

    mbedtls_mpi_init(&A);
    mbedtls_mpi_init(&B);
    mbedtls_mpi_init(&X);
    mbedtls_mpi_init(&E);

    TEST_ASSERT_FALSE(mbedtls_mpi_read_string(&A, 16, a_str));
    TEST_ASSERT_FALSE(mbedtls_mpi_read_string(&B, 16, b_str));

    /* E = A * B */
    TEST_ASSERT_FALSE(mbedtls_mpi_read_string(&E, 16, e_str));
    TEST_ASSERT_FALSE(mbedtls_mpi_mul_mpi(&X, &A, &B));

    mbedtls_mpi_write_string(&X, 16, x_buf, sizeof(x_buf)-1, &x_buf_len);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(e_str, x_buf, "mbedtls_mpi_mul_mpi result wrong");

    /* if mod_bits arg is set, also do a esp_mpi_mul_mod() call */
    if (mod_bits > 0) {
        mbedtls_mpi_init(&M);
        for(int i = 0; i < mod_bits; i++) {
            mbedtls_mpi_set_bit(&M, i, 1);
        }

        TEST_ASSERT_FALSE(esp_mpi_mul_mpi_mod(&X, &A, &B, &M));

        mbedtls_mpi_write_string(&X, 16, x_buf, sizeof(x_buf)-1, &x_buf_len);
        TEST_ASSERT_EQUAL_STRING_MESSAGE(e_str, x_buf, "esp_mpi_mul_mpi_mod result wrong");

        mbedtls_mpi_free(&M);
    }


    mbedtls_mpi_free(&A);
    mbedtls_mpi_free(&B);
    mbedtls_mpi_free(&X);
    mbedtls_mpi_free(&E);
}

TEST_CASE("test MPI multiplication", "[bignum]")
{
    /* Run some trivial numbers tests w/ various high modulo bit counts,
     should make no difference to the result
    */
    for(int i = 512; i <= 4096; i+= 512) {
        test_bignum_mult("10", "100", "1000",
                         i);
    }

    test_bignum_mult("60006FA8D3E3BD746BE39B860FFAADB4F108E15CF2ED8F685FB0E86CC4CB107A488720B41C3F1E18550F00619CD3CA8442296ECB54D2F52ECEE5346D310195700000000",
                     "BF474CA7",
                     "047BB102CAF58A48D3D97E4231BC0B753051D8232B9B939A2A4E310F88E65FEFD7762FC2DE0E2BAD6AA51A391DFFABD120653A312E4998F42E2C03AA404EE63B67275BC100000000",
                     1024);

    test_bignum_mult("49493AC229831EC01EEB01EAF3BBEBC44768EADF9ABC30C87D1791F5E04245756ED4965361EC0599626884DF079B6B5738985CE76BD66FAA67E3AAAD60775D5C9D44C09FDF9E27C033696C007BE1C540D718CA148BA01FFA4A358541E9E9F02F72BE37AFAB037DAEA5E3669A770400D2F4A5DBBD83A83919D05E3DD64787BC80000000",
                     "B878CC29",
                     "34CF37013066D5BDA2C86CF1FE7BDA66604E0D55DAFF9864B6E727BFF5871012B0AB73D28D4E100BA1E4607AA2A247C912FDBC435C6BF7C5F8E00278AE1381B1E5F6E3D52D2CBE819F0D65CB37370666D156E7A7B1FD4698D8C9D3165FC8A83F9293C839521993619CCF8180E521300C4306206C9121D629754F1FCC7839BF6DFAF33080000000",
                     3072);

    test_bignum_mult("24BF6185468786FDD303083D25E64EFC66CA472BC44D253102F8B4A9D3BFA75091386C0077937FE33FA3252D28855837AE1B484A8A9A45F7EE8C0C634F9E8CDDF79C5CE07EE72C7F123142198164234CABB724CF78B8173B9F880FC86322407AF1FEDFDDE2BEB674CA15F3E81A1521E071513A1E85B5DFA031F21ECAE9A34D",
                     "010001",
                     "24BF8644A80CCD855A00DB402E2374E2B5C6ADF60B78E97E2829B7A288697B103888FD38E393F776BF8664D04DB280BD0652F665D2E4D0923483FAEF5C01DC7C847A547CDBC7AB663EB0544AC37DA4B0CF03D0869D878FF3B6C3AF5072EAA39D3279D1DCC29C9933808ABDFE0DFD3BF59331AB6FBFD46556119250BD086E36A34D",
        1536);

    test_bignum_mult("-5D88B669C417EDD02213723546A906B7E9DA7683780E9B54856A2147467ADA316F8819D69486FC8056FE1E8EA7DEC5D5EF12340B95C4FC966F4B348D35893620",
                     "9AE7FBC99546432DF71896FC239EADAEF38D18D2B2F0E2DD275AA977E2BF4411F5A3B2A5D33605AEBBCCBA7FEB9F2D2FA74206CEC169D74BF5A8C50D6F48EA08",
                     "-38990016EB21810E3B5E6AEE339AEE72BB7CD629C4C9270A3D832701A2949BC82B2BE5A7F900C0C9937464699862821976095187D646884E8FBF01DE8C3442F3BC97B670AF573EFB74A9BBEBE4432EE74B0A83BBCDF59485D332B1FF49EB461A3A8B12C38FD72C7772D75EC6EBA5633199540C47678BD2F4ADEEA40830C2F100",
        2048);


    /* 1 << 2050 * 0X1234 */
    test_bignum_mult("400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
                     "1234",
                     "48D000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
                     3072);

    /* multiply a 1178 bit number by a 2050 bit number */
    test_bignum_mult("AAAAAAAAAA75124938ABBECD0EEEEE333333333333333333333FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFAAAAAAABBBBBBBBBBBBBBBBBBBB000000000000000000000000000000000004988A5293848932948872398400000000000FFFFFFFFFFF0000000000000EDFABC0204048975876873487387478327482374871327482347328742837483247283748234723874238",
                     "390587293875124938ABBECD0EEEEE3333333333333333333333333333333399999888000AAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBBB00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000EDFABC0204048975876873487387478327482374871327482347328742837483247283748234723874238478327400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003012111111111111111100000000000000000000000111111111111111111111111",
                     "02603AF70D0421C1AD82CE623F28F70B128118D06D00C27D433EC25BA86E6105C3890A0B1973B8BE068CA68E159A21078785DDB37F94216FBF4AEC939958AF4B8CEA2A48895CECA87562FC846EAAE0C866AF9D41EEABFB1D579F5828E9666A15E2AF946F16A189B5C645872FDCA247D309AB0BCAFB0D112881186FCFFEDC87061B4AE4A375E9BBCF579A7BC87A8EAC8C6F66E107986FC603F920F5E1A0FD8C619D88D90066FFFC8F4DB77437EBD7E3BD7E398C4C01F93426E347E039DCA7B0A73C0C90A9C4271BB761ADFF88971D190CE5DA98EFC5D7390D33BC034908AF81D784A4D7F32D0902E0C5DABC706635D5A28FC0E3A364EDEB21E8E117041D0E4B51CA6F9684F434057E7FCF2AF6BD050334B1D11E043B0967154E57354B681161D3C618974D5A7E0385755B80B931AE9B59DD4402BAEC206F04B8440741B3C4CA6D9F7DAF0AE6B3BF1B24B76C2F12B9E9A7C50D32E2093608FC9A30CBD852329E64A9AE0BC3F513899EBFA28629C1DF38081FB8C6630408F70D7B9A37701ABA4176C8B7DCB8CC78BD7783B861A7FC50862E75191DB8",
                     4096);
}

static bool test_bignum_modexp(const char *z_str, const char *x_str, const char *y_str, const char *m_str, int ret_error)
{
    mbedtls_mpi Z, X, Y, M;
    char z_buf[400] = { 0 };
    size_t z_buf_len = 0;
    bool fail = false;

    printf("%s = (%s ^ %s) mod %s  ret=%d ... ", z_str, x_str, y_str, m_str, ret_error);

    mbedtls_mpi_init(&Z);
    mbedtls_mpi_init(&X);
    mbedtls_mpi_init(&Y);
    mbedtls_mpi_init(&M);

    TEST_ASSERT_FALSE(mbedtls_mpi_read_string(&X, 16, x_str));
    TEST_ASSERT_FALSE(mbedtls_mpi_read_string(&Y, 16, y_str));
    TEST_ASSERT_FALSE(mbedtls_mpi_read_string(&M, 16, m_str));

    //mbedtls_mpi_printf("X", &X);
    //mbedtls_mpi_printf("X", &Y);
    //mbedtls_mpi_printf("M", &M);

    /* Z = (X ^ Y) mod M */
    if (ret_error != mbedtls_mpi_exp_mod(&Z, &X, &Y, &M, NULL)) {
        fail = true;
    }
    
    if (ret_error == MBEDTLS_OK) {
        mbedtls_mpi_write_string(&Z, 16, z_buf, sizeof(z_buf)-1, &z_buf_len);
        if (memcmp(z_str, z_buf, strlen(z_str)) != 0) {
            printf("\n Expected '%s' Was '%s' \n", z_str, z_buf);
            fail = true;
        }
    }

    mbedtls_mpi_free(&Z);
    mbedtls_mpi_free(&X);
    mbedtls_mpi_free(&Y);
    mbedtls_mpi_free(&M);

    if (fail == true) {
        printf(" FAIL\n");
    } else {
        printf(" PASS\n");
    }
    return fail;
}

TEST_CASE("test MPI modexp", "[bignum]")
{
    bool test_error = false;
    printf("Z = (X ^ Y) mod M \n");
    // test_bignum_modexp(Z, X, Y, M, ret_error);
    test_error |= test_bignum_modexp("01000000", "1000", "2", "FFFFFFFF", MBEDTLS_OK);
    test_error |= test_bignum_modexp("014B5A90", "1234", "2", "FFFFFFF", MBEDTLS_OK);
    test_error |= test_bignum_modexp("01234321", "1111", "2", "FFFFFFFF", MBEDTLS_OK);
    test_error |= test_bignum_modexp("02", "5", "1", "3", MBEDTLS_OK);
    test_error |= test_bignum_modexp("22", "55", "1", "33", MBEDTLS_OK);
    test_error |= test_bignum_modexp("0222", "555", "1", "333", MBEDTLS_OK);
    test_error |= test_bignum_modexp("2222", "5555", "1", "3333", MBEDTLS_OK);
    test_error |= test_bignum_modexp("11", "5555", "1", "33", MBEDTLS_OK);
    test_error |= test_bignum_modexp("55", "1111", "1", "77", MBEDTLS_OK);
    test_error |= test_bignum_modexp("88", "1111", "2", "BB", MBEDTLS_OK);
    test_error |= test_bignum_modexp("01000000", "2", "128", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", MBEDTLS_OK);
    test_error |= test_bignum_modexp("0ABCDEF12345", "ABCDEF12345", "1", "FFFFFFFFFFFF", MBEDTLS_OK);
    test_error |= test_bignum_modexp("0ABCDE", "ABCDE", "1", "FFFFF", MBEDTLS_OK);

    test_error |= test_bignum_modexp("04",  "2",  "2",  "9",  MBEDTLS_OK);
    test_error |= test_bignum_modexp("04",  "2", "-2",  "9",  MBEDTLS_ERR_MPI_BAD_INPUT_DATA);
    test_error |= test_bignum_modexp("04",  "2",  "2", "-9",  MBEDTLS_ERR_MPI_BAD_INPUT_DATA);
    test_error |= test_bignum_modexp("04",  "2", "-2", "-9",  MBEDTLS_ERR_MPI_BAD_INPUT_DATA);

    test_error |= test_bignum_modexp("01",  "2",  "0",  "9",  MBEDTLS_OK);
    test_error |= test_bignum_modexp("04",  "2",  "0",  "0",  MBEDTLS_ERR_MPI_BAD_INPUT_DATA);
    test_error |= test_bignum_modexp("04",  "2",  "2",  "0",  MBEDTLS_ERR_MPI_BAD_INPUT_DATA);
    test_error |= test_bignum_modexp("00",  "0",  "2",  "9",  MBEDTLS_OK);
    test_error |= test_bignum_modexp("01",  "0",  "0",  "9",  MBEDTLS_OK);

    test_error |= test_bignum_modexp("04", "-2",  "2",  "9",  MBEDTLS_OK);
    test_error |= test_bignum_modexp("01", "-2",  "0",  "9",  MBEDTLS_OK);
    test_error |= test_bignum_modexp("07", "-2",  "7",  "9",  MBEDTLS_OK);
    test_error |= test_bignum_modexp("07", "-2",  "1",  "9",  MBEDTLS_OK);
    test_error |= test_bignum_modexp("02",  "2",  "1",  "9",  MBEDTLS_OK);
    test_error |= test_bignum_modexp("01",  "2",  "0",  "9",  MBEDTLS_OK);

    test_error |= test_bignum_modexp("05",  "5",  "7",  "7",  MBEDTLS_OK);
    test_error |= test_bignum_modexp("02", "-5",  "7",  "7",  MBEDTLS_OK);
    test_error |= test_bignum_modexp("01", "-5",  "7",  "3",  MBEDTLS_OK);

    TEST_ASSERT_FALSE_MESSAGE(test_error, "mbedtls_mpi_exp_mod incorrect for some tests\n");
}

