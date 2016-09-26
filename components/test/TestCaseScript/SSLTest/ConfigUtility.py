from PKI import PKIDict, PKIItem
import Parameter


def multiply_2_lists(list1, list2):
    def make_list(li):
        if not isinstance(li, list):
            li = [li]
        return li
    list1 = make_list(list1)
    list2 = make_list(list2)
    ret = []
    for a in list1:
        for b in list2:
            ret.append(make_list(a) + make_list(b))
    return ret


def list_multiply(list1, *args):
    ret = list1
    for arg in args:
        ret = multiply_2_lists(ret, arg)
    return ret


def list_and(list1, list2):
    ret = []
    for item in list1:
        if item in list2:
            ret.append(item)
    return ret


class ComputeResult(object):
    NEGOTIATION_CONFIG = ["client_version", "client_cipher_suite", "client_fragment_size",
                          "server_version", "server_cipher_suite", "server_fragment_size"]
    CERT_KEY_CONFIG = ["verify_server", "verify_client",
                       "client_trust_anchor", "client_certificate", "client_key",
                       "server_trust_anchor", "server_certificate", "server_key"]

    TYPE_CONTEXT = "context"
    TYPE_NEGOTIATION = "negotiation"
    TYPE_CERT_KEY = "cert_key"
    TYPE_SEND_PARAM = "send_param"

    # results
    SUCCEED = 0
    CREATE_CONTEXT_FAIL = 1
    HANDSHAKE_FAIL = 2
    CERT_KEY_FAIL = 3

    def __init__(self, client_capability, server_capability):
        self.client_capability = client_capability
        self.server_capability = server_capability
        pass

    @staticmethod
    def __check_cert(cert, capability, check_encoding=True):
        ret = True
        if cert.name is not None:
            if check_encoding is True:
                if cert.digest not in capability.get("certificate_digest") \
                        or cert.key_algorithm not in capability.get("key_algorithm") \
                        or cert.file_encoding not in capability.get("certificate_encoding"):
                    ret = False
            else:
                if cert.digest not in capability.get("certificate_digest") \
                        or cert.key_algorithm not in capability.get("key_algorithm"):
                    ret = False
        return ret

    @staticmethod
    def __check_key(key, capability, check_encoding=True):
        ret = True
        if key.name is not None:
            if check_encoding is True:
                if key.algorithm not in capability.get("key_algorithm") \
                        or key.file_encoding not in capability.get("key_encoding") \
                        or key.file_encryption not in capability.get("pem_encryption"):
                    ret = False
            else:
                if key.algorithm not in capability.get("key_algorithm") \
                        or key.file_encryption not in capability.get("pem_encryption"):
                    ret = False
        return ret

    # compute result functions
    def check_context(self, config):
        result = self.SUCCEED
        check_list = [(self.__check_cert, PKIItem.Certificate(config["client_trust_anchor"]),
                       self.client_capability),
                      (self.__check_cert, PKIItem.Certificate(config["client_certificate"]),
                       self.client_capability),
                      (self.__check_key, PKIItem.PrivateKey(config["client_key"]),
                       self.client_capability),
                      (self.__check_cert, PKIItem.Certificate(config["server_trust_anchor"]),
                       self.server_capability),
                      (self.__check_cert, PKIItem.Certificate(config["server_certificate"]),
                       self.server_capability),
                      (self.__check_key, PKIItem.PrivateKey(config["server_key"]),
                       self.server_capability)]
        for _check in check_list:
            if _check[0](_check[1], _check[2]) is False:
                result = self.CREATE_CONTEXT_FAIL
                break
        return result

    def check_negotiation_param(self, config):
        result = self.SUCCEED
        # first check version
        while True:
            if Parameter.VERSION[config["client_version"]]\
                    & Parameter.VERSION[config["server_version"]] == 0:
                result = self.HANDSHAKE_FAIL
                break
            # check cipher suite
            supported_cipher_suite = list_and(self.client_capability.get("cipher_suite"),
                                              self.server_capability.get("cipher_suite"))
            if config["client_cipher_suite"] not in supported_cipher_suite\
                    or config["server_cipher_suite"] not in supported_cipher_suite\
                    or config["client_cipher_suite"] != config["server_cipher_suite"]:
                result = self.HANDSHAKE_FAIL
                break
            break
        return result

    # check cert key, if it can be supported by both client and server, if it matches
    def __check_cert_key_content(self, cert, key):
        if self.__check_cert(cert, self.client_capability, check_encoding=False) is True\
                and self.__check_cert(cert, self.server_capability, check_encoding=False) is True \
                and self.__check_key(key, self.client_capability, check_encoding=False) is True \
                and self.__check_key(key, self.server_capability, check_encoding=False) is True \
                and key.name.find(cert.private_key) != -1:
            result = True
        else:
            result = False
        return result

    def __verify_ca(self, ca, cert, capability):
        result = True
        while True:
            # if ca supported
            if self.__check_cert(ca, capability) is False:
                result = False
                break
            # check if ca in cert chain
            try:
                index = cert.cert_chain.index(ca.name)
            except StandardError:
                result = False
                break

            # for pem cert, it contains cert chain to issuer, any cert in chain works
            # der cert do not contain cert chain
            # only der root cert verify L1 cert and root cert works
            if cert.file_encoding == "DER":
                if len(cert.cert_chain) > 2 and index != len(cert.cert_chain) - 1:
                    result = False
                    break
            # check if all certs in before trust anchor supported
            for cert_name in cert.cert_chain[1:index]:
                _cert = PKIItem.Certificate(cert_name + ".pem")
                if self.__check_cert(_cert, capability) is False:
                    result = False
                    break
            break
        return result

    def __check_verify_client(self, client_cert, client_key, server_ca):
        result = self.__check_cert_key_content(client_cert, client_key)
        if result is True:
            result = self.__verify_ca(server_ca, client_cert, self.server_capability)
        return result

    def __check_verify_server(self, client_ca, server_cert):
        return self.__verify_ca(client_ca, server_cert, self.client_capability)

    def check_cert_key(self, config):
        result = self.SUCCEED
        while True:  # break if when anything failed
            if (config["verify_server"] is True and self.client_capability.get("verify_server") is False) \
                    or (config["verify_client"] is True and
                        (self.server_capability.get("verify_client") is False or
                            self.client_capability.get("verify_client") is False)):
                result = self.CERT_KEY_FAIL
                break

            server_cert = PKIItem.Certificate(config["server_certificate"])
            server_key = PKIItem.PrivateKey(config["server_key"])
            server_ca = PKIItem.Certificate(config["server_trust_anchor"])
            client_cert = PKIItem.Certificate(config["client_certificate"])
            client_key = PKIItem.PrivateKey(config["client_key"])
            client_ca = PKIItem.Certificate(config["client_trust_anchor"])
            # always check server cert key
            if self.__check_cert_key_content(server_cert, server_key) is False:
                result = self.CERT_KEY_FAIL
                break
            # if require to verify server
            if config["verify_server"] is True:
                if self.__check_verify_server(client_ca, server_cert) is False:
                    result = self.CERT_KEY_FAIL
                    break
            # if require to verify client
            if config["verify_client"] is True:
                if self.__check_verify_client(client_cert, client_key, server_ca) is False:
                    result = self.CERT_KEY_FAIL
                    break
            break
        return result

    CHECK_FUNC = {
        TYPE_CONTEXT: check_context,
        TYPE_NEGOTIATION: check_negotiation_param,
        TYPE_CERT_KEY: check_cert_key,
    }
    CONFIG_KEY = {
        TYPE_CONTEXT: CERT_KEY_CONFIG,
        TYPE_NEGOTIATION: NEGOTIATION_CONFIG,
        TYPE_CERT_KEY: CERT_KEY_CONFIG,
    }

    def compute_result(self, typ, config_list):
        succeed_list = []
        fail_list = []
        for config in config_list:
            if self.CHECK_FUNC[typ](self, dict(zip(self.CONFIG_KEY[typ], config))) != self.SUCCEED:
                fail_list.append(config)
            else:
                succeed_list.append(config)
        return succeed_list, fail_list
    pass


class GenerateTestConfig(ComputeResult):
    TEST_CONFIG = ComputeResult.NEGOTIATION_CONFIG + \
                  ComputeResult.CERT_KEY_CONFIG

    def __init__(self, client_capability, server_capability):
        ComputeResult.__init__(self, client_capability, server_capability)
        self.key_dict = PKIDict.PKIDict.KEY_DICT
        self.cert_dict = PKIDict.PKIDict.CERT_DICT
        pass

    def generate_negotiation_config(self):
        _config = list_multiply(self.client_capability.get("version"),
                                self.client_capability.get("cipher_suite"),
                                self.client_capability.get("fragment_size"),
                                self.server_capability.get("version"),
                                self.server_capability.get("cipher_suite"),
                                self.server_capability.get("fragment_size"))
        return self.compute_result(self.TYPE_NEGOTIATION, _config)

    def __choose_cert_key(self, verify_server, verify_client,
                          client_ca_opt, client_cert_key_opt,
                          server_ca_opt, server_cert_key_opt):
        pass

    # CERT_KEY_CONFIG = ["verify_server", "verify_client",
    #                    "client_trust_anchor", "client_certificate", "client_key",
    #                    "server_trust_anchor", "server_certificate", "server_key"]
    def generate_cert_key_config(self):
        # first handle not verify certificate case
        _config_list = []
        for cert in PKIDict.PKIDict.CERT_DICT:
            for key in PKIDict.PKIDict.KEY_DICT:
                _config_list.append([False, False, None, None, None, None, cert, key])
        cert_key_succeed, context_fail = self.compute_result(self.TYPE_CONTEXT, _config_list)
        cert_key_succeed, cert_key_fail = self.compute_result(self.TYPE_CERT_KEY, cert_key_succeed)
        key_cert_pair = [[x[6], x[7]] for x in cert_key_succeed]
        # for succeed config, do server cert verify
        _config_list = []
        for _config in cert_key_succeed:
            for cert in PKIDict.PKIDict.CERT_DICT:
                _config_list.append([True, False, cert, None, None,
                                     None, _config[6], _config[7]])
        _cert_key_succeed, _context_fail = self.compute_result(self.TYPE_CONTEXT, _config_list)
        context_fail += _context_fail
        _cert_key_succeed, _cert_key_fail = self.compute_result(self.TYPE_CERT_KEY, _cert_key_succeed)
        cert_key_fail += _cert_key_fail
        cert_key_succeed += _cert_key_succeed
        # for succeed config, do client verify
        _config_list = []
        for _config in _cert_key_succeed:
            for key_cert in key_cert_pair:
                _config_list.append([True, True, _config[2], key_cert[0], key_cert[1],
                                     key_cert[0], _config[6], _config[7]])
        _cert_key_succeed, _context_fail = self.compute_result(self.TYPE_CONTEXT, _config_list)
        context_fail += _context_fail
        _cert_key_succeed, _cert_key_fail = self.compute_result(self.TYPE_CERT_KEY, _cert_key_succeed)
        cert_key_fail += _cert_key_fail
        cert_key_succeed += _cert_key_succeed
        # only verify client not verify server
        _config_list = []
        for _config in _cert_key_succeed:
            _config_list.append([False, True, None,
                                 _config[3], _config[4], _config[5], _config[6], _config[7]])
        _cert_key_succeed, _context_fail = self.compute_result(self.TYPE_CONTEXT, _config_list)
        context_fail += _context_fail
        _cert_key_succeed, _cert_key_fail = self.compute_result(self.TYPE_CERT_KEY, _cert_key_succeed)
        cert_key_fail += _cert_key_fail
        cert_key_succeed += _cert_key_succeed
        return cert_key_succeed, context_fail, cert_key_fail


class ConfigUtility(GenerateTestConfig):
    # test config
    _TEST_CONFIG_DICT_KEY = ("config", "result")

    def __init__(self, client_capability, server_capability):
        GenerateTestConfig.__init__(self, client_capability, server_capability)
        pass

    def get_all_test_config(self):
        negotiation_succeed, negotiation_fail = self.generate_negotiation_config()
        cert_key_succeed, context_fail, cert_key_fail = self.generate_cert_key_config()
        succeed_config = list_multiply(negotiation_succeed, cert_key_succeed)
        context_fail_config = list_multiply([negotiation_succeed[0]], context_fail)
        negotiation_fail_config = list_multiply(negotiation_fail, [cert_key_succeed[0]])
        cert_key_fail_config = list_multiply([negotiation_succeed[0]], cert_key_fail)
        return dict(zip(["succeed", "context_fail", "negotiation_fail", "cert_key_fail"],
                        [[dict(zip(self.TEST_CONFIG, x)) for x in succeed_config],
                         [dict(zip(self.TEST_CONFIG, x)) for x in context_fail_config],
                         [dict(zip(self.TEST_CONFIG, x)) for x in negotiation_fail_config],
                         [dict(zip(self.TEST_CONFIG, x)) for x in cert_key_fail_config]]))
    pass


def main():
    pass


if __name__ == '__main__':
    main()
