#ifndef _OPENSSL_DEMO_H_
#define _OPENSSL_DEMO_H_

const static unsigned char key_ctx[] =
        "-----BEGIN PRIVATE KEY-----\r\n"
        "MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDQyyF0WBb2XbkL\r\n"
        "wYFgyoPOanYvbb/qwbAkGf1zSPX35xruZmjszjcverMoyF6x2MBxD3gP1ijBR0Rr\r\n"
        "0J0CfluABDLkzqpF5smOVX9k8W7ePClm91NhcASuF+CaZOe6B+6vOYShYjhe6eFG\r\n"
        "AGk8SP4zSrG2XHNKXlR3w8duK9fyOOZLWRjL3T6+++HEaly1p4ujKZhrm5wHzywA\r\n"
        "DvjDdvIWBCW1Z+8j7Q9qUITjlsDWHjrXCpyEfclE1WQxTP/W7rBLxNVxTfwbrdcD\r\n"
        "HNrKTOXtN+oDmCruvmBnTkz9x4Te6wJuvtFd0fBtW1kWsMzomvOlKmvHo0gmpqfh\r\n"
        "CwEPoKCNAgMBAAECggEBAIVr+LHXBL1kQLZhbiKC5t1FrMY8FLKYGM3JCevciMlk\r\n"
        "lhIXwR7p29vsRYZfHBv7zWWyI9/C04JG31MiGOMaCbBxfnA2HBrVEqFwwxqnKVi8\r\n"
        "CxzwGQkfwu3Y9j7TEj0PipfQYo+aKzmDrN/FrXnHjyEJryxAQbAZPVLW+Z7OR41R\r\n"
        "ZOwtZLeVqmbeARGpu2Xd9SKAhbjdLSz96IdUcNrwbP/lzUgrKaiUioBMVFfIG5ce\r\n"
        "4Mm2seCwaWxI8k24q0keSjsjV+5IxatVUNtJ9vYv6Tzo+3oqGvPeUBO7w9xhbLKf\r\n"
        "jw1uEykcs0wcftWb1iB7r78bMPZ/KYhnSFsjT+vnIOECgYEA9LM5p63sn6OWrULd\r\n"
        "doBBLaIw1ZK9rDUFLLRwv0e6+7tuuB08ok6D7PSfOpx0bb10lIx7epOE8ogGSmLS\r\n"
        "w0rMbmcKAlTLAJ/0nX1ierZkb+Ay21F398tKL3igEfnaB9CzuOHF8XhbsTqeGFDJ\r\n"
        "HFBMXxTbo4kfkUmZNYxwTombzkkCgYEA2m9teqjEOVZItqghOicWZ68JhWxBJFme\r\n"
        "oSfzJKLd8atfUOnBLqAhPzvg1PvdIxjLWBqy28tEJf+jdSQCNe9BmhlptOwbFrJy\r\n"
        "IyCXj6QTApSKTxyzIjMvzQkv1m8CxeCq5T64hvJ2++i7dlhumh21c7oL8aLeTnoe\r\n"
        "AG1dBLJ9UCUCgYAhSlDJsyvB/Ct/nt0qRgCYCLzEEZakWwI9Nr8wBr41iSZi/fdF\r\n"
        "zZC9J/qRqr9cPq4hl4sk/fTUWhUhAZjS4NY3HuWJs6d6ikhpNKm1MCMx5TqGA+ti\r\n"
        "VtHc63g7edZjwczxliWr2EgBMIxZmoQByhrZxKis8vbMeUrSsiyFQstjoQKBgD3k\r\n"
        "2Paqn39Hra7kERYzQSv78wy1UfgE1OgBM+orpAv4bTe2JKEbipAqXVi8TTrGqce7\r\n"
        "OPcCr7q8pwpoO6AgvUv263byd/KEecbuU0PGUASpJk+oaDHGo0LL2Zw/NF/xezsd\r\n"
        "/JdwWLqkhYnRIPXWeTXjf8LmTWubOqkQVA0irlNpAoGAJ+9N/NF3XAW0BroiVYLZ\r\n"
        "p0Btcgt+b4LWrBlm0XqHhzKUlqhfibAr3OtUkFjo/509ncYPiuOzVSNosyjXFJ86\r\n"
        "2kQ88fB3eeLnBAcbBXQKiOBPU2y6bCCfgdo+JEOK/cxVslaxMAyKSnFi9gdgzScd\r\n"
        "k+hOlkflXQVkic3W358kFto=\r\n"
        "-----END PRIVATE KEY-----\r\n"
        ;
static int key_bytes = sizeof(key_ctx);

const static unsigned char cert_ctx[] =
        "-----BEGIN CERTIFICATE-----\r\n"
        "MIID7jCCAtYCAQEwDQYJKoZIhvcNAQELBQAwgbwxCzAJBgNVBAYTAkNOMRAwDgYD\r\n"
        "VQQIDAdKaWFuZ3N1MQ0wCwYDVQQHDARXdXhpMSYwJAYDVQQKDB1Fc3ByZXNzaWYg\r\n"
        "Um9vdCBSU0EyMDQ4IHNoYTI1NjEcMBoGA1UECwwTUm9vdCBSU0EyMDQ4IHNoYTI1\r\n"
        "NjEfMB0GA1UEAwwWcm9vdGNlcnQuZXNwcmVzc2lmLmNvbTElMCMGCSqGSIb3DQEJ\r\n"
        "ARYWcm9vdGNlcnRAZXNwcmVzc2lmLmNvbTAeFw0xNjA2MjgwMjMxMjlaFw0yNjA2\r\n"
        "MjYwMjMxMjlaMIG8MQswCQYDVQQGEwJDTjEQMA4GA1UECAwHSmlhbmdzdTENMAsG\r\n"
        "A1UEBwwEV3V4aTEmMCQGA1UECgwdRXNwcmVzc2lmIFJvb3QgUlNBMjA0OCBzaGEy\r\n"
        "NTYxHDAaBgNVBAsME1Jvb3QgUlNBMjA0OCBzaGEyNTYxHzAdBgNVBAMMFnJvb3Rj\r\n"
        "ZXJ0LmVzcHJlc3NpZi5jb20xJTAjBgkqhkiG9w0BCQEWFnJvb3RjZXJ0QGVzcHJl\r\n"
        "c3NpZi5jb20wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDQyyF0WBb2\r\n"
        "XbkLwYFgyoPOanYvbb/qwbAkGf1zSPX35xruZmjszjcverMoyF6x2MBxD3gP1ijB\r\n"
        "R0Rr0J0CfluABDLkzqpF5smOVX9k8W7ePClm91NhcASuF+CaZOe6B+6vOYShYjhe\r\n"
        "6eFGAGk8SP4zSrG2XHNKXlR3w8duK9fyOOZLWRjL3T6+++HEaly1p4ujKZhrm5wH\r\n"
        "zywADvjDdvIWBCW1Z+8j7Q9qUITjlsDWHjrXCpyEfclE1WQxTP/W7rBLxNVxTfwb\r\n"
        "rdcDHNrKTOXtN+oDmCruvmBnTkz9x4Te6wJuvtFd0fBtW1kWsMzomvOlKmvHo0gm\r\n"
        "pqfhCwEPoKCNAgMBAAEwDQYJKoZIhvcNAQELBQADggEBABTYZLiFHq51lqaa0nHI\r\n"
        "aDMAb29DfO93fqp+oHZYO4xKyEeLr8EhD39GjnQmhz710wO0TBCYV7nD+xnJ1h5F\r\n"
        "IbQUAQZO9NIy3ns4mYVRUWjnWYAo+evGeKgRrxvh7sjNLPBPzs9tg/u7XjBp/nor\r\n"
        "8JnnFFT0wXPyi/qg8J3QutqJvWRQGRRx2AP93F44+Zcj7ReFMVSmOXyzT4aNJL0+\r\n"
        "Ls+baKwA4pnyVRoAaKbs/JYDgd0/DunuktVKuhyvK/qOGjJSRLPhdrXbvSAegpiM\r\n"
        "4xIm6ZWKtTv8VvkGgXUVQ7RpruP6nV6506gDcUgecbEq7H2VDhEzUYcMmGCUQZlG\r\n"
        "sJ8=\r\n"
        "-----END CERTIFICATE-----\r\n"
        ;
static int cert_bytes = sizeof(cert_ctx);



#endif
