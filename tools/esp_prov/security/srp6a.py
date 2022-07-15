# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

# N     A large safe prime (N = 2q+1, where q is prime) [All arithmetic is done modulo N]
# g     A generator modulo N
# k     Multiplier parameter (k = H(N, g) in SRP-6a, k = 3 for legacy SRP-6)
# s     User's salt
# Iu    Username
# p     Cleartext Password
# H()   One-way hash function
# ^     (Modular) Exponentiation
# u     Random scrambling parameter
# a, b  Secret ephemeral values
# A, B  Public ephemeral values
# x     Private key (derived from p and s)
# v     Password verifier

import hashlib
import os
from typing import Any, Callable, Optional, Tuple

from utils import bytes_to_long, long_to_bytes

SHA1 = 0
SHA224 = 1
SHA256 = 2
SHA384 = 3
SHA512 = 4

NG_1024 = 0
NG_2048 = 1
NG_3072 = 2
NG_4096 = 3
NG_8192 = 4

_hash_map = {SHA1: hashlib.sha1,
             SHA224: hashlib.sha224,
             SHA256: hashlib.sha256,
             SHA384: hashlib.sha384,
             SHA512: hashlib.sha512}


_ng_const = (
    # 1024-bit
    ('''\
EEAF0AB9ADB38DD69C33F80AFA8FC5E86072618775FF3C0B9EA2314C9C256576D674DF7496\
EA81D3383B4813D692C6E0E0D5D8E250B98BE48E495C1D6089DAD15DC7D7B46154D6B6CE8E\
F4AD69B15D4982559B297BCF1885C529F566660E57EC68EDBC3C05726CC02FD4CBF4976EAA\
9AFD5138FE8376435B9FC61D2FC0EB06E3''',
     '2'),
    # 2048
    ('''\
AC6BDB41324A9A9BF166DE5E1389582FAF72B6651987EE07FC3192943DB56050A37329CBB4\
A099ED8193E0757767A13DD52312AB4B03310DCD7F48A9DA04FD50E8083969EDB767B0CF60\
95179A163AB3661A05FBD5FAAAE82918A9962F0B93B855F97993EC975EEAA80D740ADBF4FF\
747359D041D5C33EA71D281E446B14773BCA97B43A23FB801676BD207A436C6481F1D2B907\
8717461A5B9D32E688F87748544523B524B0D57D5EA77A2775D2ECFA032CFBDBF52FB37861\
60279004E57AE6AF874E7303CE53299CCC041C7BC308D82A5698F3A8D0C38271AE35F8E9DB\
FBB694B5C803D89F7AE435DE236D525F54759B65E372FCD68EF20FA7111F9E4AFF73''',
     '2'),
    # 3072
    ('''\
FFFFFFFFFFFFFFFFC90FDAA22168C2\
34C4C6628B80DC1CD129024E088A67CC74020BBEA63B139B22514A08798E\
3404DDEF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245E485B5\
76625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7EDEE386BFB5A899FA5AE\
9F24117C4B1FE649286651ECE45B3DC2007CB8A163BF0598DA48361C55D3\
9A69163FA8FD24CF5F83655D23DCA3AD961C62F356208552BB9ED5290770\
96966D670C354E4ABC9804F1746C08CA18217C32905E462E36CE3BE39E77\
2C180E86039B2783A2EC07A28FB5C55DF06F4C52C9DE2BCBF69558171839\
95497CEA956AE515D2261898FA051015728E5A8AAAC42DAD33170D04507A\
33A85521ABDF1CBA64ECFB850458DBEF0A8AEA71575D060C7DB3970F85A6\
E1E4C7ABF5AE8CDB0933D71E8C94E04A25619DCEE3D2261AD2EE6BF12FFA\
06D98A0864D87602733EC86A64521F2B18177B200CBBE117577A615D6C77\
0988C0BAD946E208E24FA074E5AB3143DB5BFCE0FD108E4B82D120A93AD2\
CAFFFFFFFFFFFFFFFF''',
     '5'),
    # 4096
    ('''\
FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD129024E08\
8A67CC74020BBEA63B139B22514A08798E3404DDEF9519B3CD3A431B\
302B0A6DF25F14374FE1356D6D51C245E485B576625E7EC6F44C42E9\
A637ED6B0BFF5CB6F406B7EDEE386BFB5A899FA5AE9F24117C4B1FE6\
49286651ECE45B3DC2007CB8A163BF0598DA48361C55D39A69163FA8\
FD24CF5F83655D23DCA3AD961C62F356208552BB9ED529077096966D\
670C354E4ABC9804F1746C08CA18217C32905E462E36CE3BE39E772C\
180E86039B2783A2EC07A28FB5C55DF06F4C52C9DE2BCBF695581718\
3995497CEA956AE515D2261898FA051015728E5A8AAAC42DAD33170D\
04507A33A85521ABDF1CBA64ECFB850458DBEF0A8AEA71575D060C7D\
B3970F85A6E1E4C7ABF5AE8CDB0933D71E8C94E04A25619DCEE3D226\
1AD2EE6BF12FFA06D98A0864D87602733EC86A64521F2B18177B200C\
BBE117577A615D6C770988C0BAD946E208E24FA074E5AB3143DB5BFC\
E0FD108E4B82D120A92108011A723C12A787E6D788719A10BDBA5B26\
99C327186AF4E23C1A946834B6150BDA2583E9CA2AD44CE8DBBBC2DB\
04DE8EF92E8EFC141FBECAA6287C59474E6BC05D99B2964FA090C3A2\
233BA186515BE7ED1F612970CEE2D7AFB81BDD762170481CD0069127\
D5B05AA993B4EA988D8FDDC186FFB7DC90A6C08F4DF435C934063199\
FFFFFFFFFFFFFFFF''',
     '5'),
    # 8192
    ('''\
FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD129024E08\
8A67CC74020BBEA63B139B22514A08798E3404DDEF9519B3CD3A431B\
302B0A6DF25F14374FE1356D6D51C245E485B576625E7EC6F44C42E9\
A637ED6B0BFF5CB6F406B7EDEE386BFB5A899FA5AE9F24117C4B1FE6\
49286651ECE45B3DC2007CB8A163BF0598DA48361C55D39A69163FA8\
FD24CF5F83655D23DCA3AD961C62F356208552BB9ED529077096966D\
670C354E4ABC9804F1746C08CA18217C32905E462E36CE3BE39E772C\
180E86039B2783A2EC07A28FB5C55DF06F4C52C9DE2BCBF695581718\
3995497CEA956AE515D2261898FA051015728E5A8AAAC42DAD33170D\
04507A33A85521ABDF1CBA64ECFB850458DBEF0A8AEA71575D060C7D\
B3970F85A6E1E4C7ABF5AE8CDB0933D71E8C94E04A25619DCEE3D226\
1AD2EE6BF12FFA06D98A0864D87602733EC86A64521F2B18177B200C\
BBE117577A615D6C770988C0BAD946E208E24FA074E5AB3143DB5BFC\
E0FD108E4B82D120A92108011A723C12A787E6D788719A10BDBA5B26\
99C327186AF4E23C1A946834B6150BDA2583E9CA2AD44CE8DBBBC2DB\
04DE8EF92E8EFC141FBECAA6287C59474E6BC05D99B2964FA090C3A2\
233BA186515BE7ED1F612970CEE2D7AFB81BDD762170481CD0069127\
D5B05AA993B4EA988D8FDDC186FFB7DC90A6C08F4DF435C934028492\
36C3FAB4D27C7026C1D4DCB2602646DEC9751E763DBA37BDF8FF9406\
AD9E530EE5DB382F413001AEB06A53ED9027D831179727B0865A8918\
DA3EDBEBCF9B14ED44CE6CBACED4BB1BDB7F1447E6CC254B33205151\
2BD7AF426FB8F401378CD2BF5983CA01C64B92ECF032EA15D1721D03\
F482D7CE6E74FEF6D55E702F46980C82B5A84031900B1C9E59E7C97F\
BEC7E8F323A97A7E36CC88BE0F1D45B7FF585AC54BD407B22B4154AA\
CC8F6D7EBF48E1D814CC5ED20F8037E0A79715EEF29BE32806A1D58B\
B7C5DA76F550AA3D8A1FBFF0EB19CCB1A313D55CDA56C9EC2EF29632\
387FE8D76E3C0468043E8F663F4860EE12BF2D5B0B7474D6E694F91E\
6DBE115974A3926F12FEE5E438777CB6A932DF8CD8BEC4D073B931BA\
3BC832B68D9DD300741FA7BF8AFC47ED2576F6936BA424663AAB639C\
5AE4F5683423B4742BF1C978238F16CBE39D652DE3FDB8BEFC848AD9\
22222E04A4037C0713EB57A81A23F0C73473FC646CEA306B4BCBC886\
2F8385DDFA9D4B7FA2C087E879683303ED5BDD3A062B3CF5B3A278A6\
6D2A13F83F44F82DDF310EE074AB6A364597E899A0255DC164F31CC5\
0846851DF9AB48195DED7EA1B1D510BD7EE74D73FAF36BC31ECFA268\
359046F4EB879F924009438B481C6CD7889A002ED5EE382BC9190DA6\
FC026E479558E4475677E9AA9E3050E2765694DFC81F56E880B96E71\
60C980DD98EDD3DFFFFFFFFFFFFFFFFF''',
     '0x13')
)


def get_ng(ng_type: int) -> Tuple[int, int]:
    n_hex, g_hex = _ng_const[ng_type]
    return int(n_hex, 16), int(g_hex, 16)


def get_random(nbytes: int) -> Any:
    return bytes_to_long(os.urandom(nbytes))


def get_random_of_length(nbytes: int) -> Any:
    offset = (nbytes * 8) - 1
    return get_random(nbytes) | (1 << offset)


def H(hash_class: Callable, *args: Any, **kwargs: Any) -> int:
    width = kwargs.get('width', None)

    h = hash_class()

    for s in args:
        if s is not None:
            data = long_to_bytes(s) if isinstance(s, int) else s
            if width is not None:
                h.update(bytes(width - len(data)))
            h.update(data)

    return int(h.hexdigest(), 16)


def H_N_xor_g(hash_class: Callable, N: int, g: int) -> bytes:
    bin_N = long_to_bytes(N)
    bin_g = long_to_bytes(g)

    padding = len(bin_N) - len(bin_g)

    hN = hash_class(bin_N).digest()
    hg = hash_class(b''.join([b'\0' * padding, bin_g])).digest()

    return b''.join(long_to_bytes(hN[i] ^ hg[i]) for i in range(0, len(hN)))


def calculate_x(hash_class: Callable, s: Any, Iu: str, p: str) -> int:
    _Iu = Iu.encode()
    _p = p.encode()

    return H(hash_class, s, H(hash_class, _Iu + b':' + _p))


def generate_salt_and_verifier(Iu: str, p: str, len_s: int, hash_alg: int = SHA512, ng_type: int = NG_3072) -> Tuple[bytes, bytes]:
    hash_class = _hash_map[hash_alg]
    N, g = get_ng(ng_type)

    _s = long_to_bytes(get_random(len_s))
    _v = long_to_bytes(pow(g, calculate_x(hash_class, _s, Iu, p), N))

    return _s, _v


def calculate_M(hash_class: Callable, N: int, g: int, Iu: str, s: int, A: int, B: int, K: bytes) -> Any:
    _Iu = Iu.encode()
    h = hash_class()
    h.update(H_N_xor_g(hash_class, N, g))
    h.update(hash_class(_Iu).digest())
    h.update(long_to_bytes(s))
    h.update(long_to_bytes(A))
    h.update(long_to_bytes(B))
    h.update(K)
    return h.digest()


def calculate_H_AMK(hash_class: Callable, A: int, M: bytes, K: bytes) -> Any:
    h = hash_class()
    h.update(long_to_bytes(A))
    h.update(M)
    h.update(K)
    return h.digest()


class Srp6a (object):
    def __init__(self, username: str, password: str, hash_alg: int = SHA512, ng_type: int = NG_3072):
        hash_class = _hash_map[hash_alg]

        N, g = get_ng(ng_type)
        k = H(hash_class, N, g, width=len(long_to_bytes(N)))

        self.Iu = username
        self.p = password

        self.a = get_random_of_length(32)
        self.A = pow(g, self.a, N)

        self.v: Optional[int] = None
        self.K: Optional[bytes] = None
        self.H_AMK = None
        self._authenticated = False

        self.hash_class = hash_class
        self.N = N
        self.g = g
        self.k = k

    def authenticated(self) -> bool:
        return self._authenticated

    def get_username(self) -> str:
        return self.Iu

    def get_ephemeral_secret(self) -> Any:
        return long_to_bytes(self.a)

    def get_session_key(self) -> Any:
        return self.K if self._authenticated else None

    def start_authentication(self) -> Tuple[str, bytes]:
        return (self.Iu, long_to_bytes(self.A))

    # Returns M or None if SRP-6a safety check is violated
    def process_challenge(self, bytes_s: bytes, bytes_B: bytes) -> Any:
        s = bytes_to_long(bytes_s)
        B = bytes_to_long(bytes_B)

        N = self.N
        g = self.g
        k = self.k

        hash_class = self.hash_class

        # SRP-6a safety check
        if (B % N) == 0:
            return None

        u = H(hash_class, self.A, B, width=len(long_to_bytes(N)))
        if u == 0:  # SRP-6a safety check
            return None

        x = calculate_x(hash_class, s, self.Iu, self.p)

        v = pow(g, x, N)

        S = pow((B - k * v), (self.a + u * x), N)

        self.K = hash_class(long_to_bytes(S)).digest()

        M = calculate_M(hash_class, N, g, self.Iu, s, self.A, B, self.K)
        if not M:
            return None

        self.H_AMK = calculate_H_AMK(hash_class, self.A, M, self.K)

        return M

    def verify_session(self, host_HAMK: bytes) -> None:
        if self.H_AMK == host_HAMK:
            self._authenticated = True


class AuthenticationFailed (Exception):
    pass
