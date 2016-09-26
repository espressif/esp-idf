

class SSLCapability(object):
    CAPABILITY_TYPE = ["version", "cipher_suite", "fragment_size",  # for hello capability negotiation
                       "verify_server", "verify_client",  # if support verify server/client
                       "key_algorithm", "key_encoding", "pem_encryption",  # what kind of private it supports
                       "certificate_encoding", "certificate_digest",  # what kind of certificate it supports
                       ]
    SSL_TYPE = ("TargetClient", "TargetServer", "PCClient", "PCServer")

    def __init__(self, typ, **kwargs):
        assert typ in self.SSL_TYPE
        self.type = typ
        self.capability = dict.fromkeys(self.CAPABILITY_TYPE, None)
        for kw in kwargs:
            self.capability[kw] = kwargs[kw]
        for kw in self.capability:
            assert self.capability[kw] is not None
        pass

    def get(self, kw):
        return self.capability[kw]

    def set(self, **kwargs):
        for kw in kwargs:
            self.capability[kw] = kwargs[kw]
    pass


class TargetSSLCapability(SSLCapability):
    DEFAULT_CAPABILITY = {
        "version": ["SSLv23_2"],
        "cipher_suite": ["TLS_RSA_WITH_AES_128_CBC_SHA",
                         "TLS_RSA_WITH_AES_256_CBC_SHA",
                         "TLS_RSA_WITH_RC4_128_SHA",
                         "TLS_RSA_WITH_RC4_128_MD5"],
        "fragment_size": [2048, 4096, 8192],
        "verify_server": True,
        "verify_client": False,
        "key_algorithm": ["RSA512", "RSA1024", "RSA2048", "RSA4096"],
        "key_encoding": ["PEM", "DER"],
        "pem_encryption": [None, "aes128", "aes256"],
        "certificate_encoding": ["PEM", "DER"],
        "certificate_digest": ["md5", "sha1", "sha256", "sha384", "sha512"],
    }

    def __init__(self, typ, **kwargs):
        assert typ == "TargetClient" or typ == "TargetServer"
        capability = dict(self.DEFAULT_CAPABILITY)
        for kw in kwargs:
            capability[kw] = kwargs[kw]
        SSLCapability.__init__(self, typ, **capability)
        pass
    pass


class PCSSLCapability(SSLCapability):
    DEFAULT_CAPABILITY = {
        "version": ["SSLv23", "SSLv20", "SSLv30", "TLSv10", "TLSv11", "TLSv12"],
        "cipher_suite": ["TLS_RSA_WITH_AES_128_CBC_SHA",
                         "TLS_RSA_WITH_AES_256_CBC_SHA",
                         "TLS_RSA_WITH_RC4_128_SHA",
                         "TLS_RSA_WITH_RC4_128_MD5",
                         "TLS_DH_DSS_WITH_AES_128_CBC_SHA",
                         "TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256"],
        "fragment_size": [16384],
        "verify_server": True,
        "verify_client": True,
        "key_algorithm": ["RSA512", "RSA1024", "RSA2048", "RSA4096"],
        "key_encoding": ["PEM"],
        "pem_encryption": [None],
        "certificate_encoding": ["PEM"],
        "certificate_digest": ["md5", "sha1", "sha256", "sha384", "sha512"],
    }

    def __init__(self, typ):
        assert typ == "PCClient" or typ == "PCServer"
        SSLCapability.__init__(self, typ, **self.DEFAULT_CAPABILITY)
        pass
    pass


def main():
    pc = PCSSLCapability("PCClient")
    target = TargetSSLCapability("TargetClient")
    pass


if __name__ == '__main__':
    main()
