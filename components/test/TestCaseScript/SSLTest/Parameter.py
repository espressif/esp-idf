

VERSION = {
    "SSLv23": 0x1F,
    "SSLv23_2": 0x1C,  # current target ssl implementation do not support SSLv20 and TLSv12
    "SSLv20": 0x01,
    "SSLv30": 0x02,
    "TLSv10": 0x04,
    "TLSv11": 0x08,
    "TLSv12": 0x10,
}


CIPHER_SUITE = {
    # supported algorithm
    "TLS_RSA_WITH_AES_128_CBC_SHA": "AES128-SHA",
    "TLS_RSA_WITH_AES_256_CBC_SHA": "AES256-SHA",
    "TLS_RSA_WITH_RC4_128_SHA": "RC4-SHA",
    "TLS_RSA_WITH_RC4_128_MD5": "RC4-MD5",
    "TLS_DH_DSS_WITH_AES_128_CBC_SHA": "DH-DSS-AES128-SHA",
    "TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256": "ECDHE-RSA-AES128-GCM-SHA256",
}


FRAGMENT_SIZE = {
    "SIZE_DEFAULT": 0,
    "SIZE_512": 512,
    "SIZE_1024": 1024,
    "SIZE_2048": 2048,
    "SIZE_4096": 4096,
    "SIZE_8192": 8192,
}


VERIFY_OPTION = {
    "NOT_VERIFY": "NOT_VERIFY",
    "VERIFY": "VERIFY",
}


SEND_OPTION = {
    "MAX_SEND_SIZE_512": 512,
    "MAX_SEND_SIZE_1K": 1024,
    "MAX_SEND_SIZE_2K": 2048,
    "MAX_SEND_SIZE_4K": 4096,
    "MAX_SEND_SIZE_8K": 8192,
    "MAX_SEND_SIZE_16K": 16384,
}


def main():
    pass


if __name__ == '__main__':
    main()
