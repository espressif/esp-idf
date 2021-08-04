/* mbedTLS RSA functionality tests

   Focus on testing functionality where we use ESP32 hardware
   accelerated crypto features.

*/
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <esp_system.h>
#include "mbedtls/rsa.h"
#include "mbedtls/pk.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/entropy_poll.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "sdkconfig.h"
#include "test_utils.h"
#include "ccomp_timer.h"

#define PRINT_DEBUG_INFO

/* Taken from openssl s_client -connect api.gigafive.com:443 -showcerts
 */
static const char *rsa4096_cert = "-----BEGIN CERTIFICATE-----\n"\
    "MIIExzCCA6+gAwIBAgIBAzANBgkqhkiG9w0BAQsFADCBkjELMAkGA1UEBhMCVVMx\n"\
    "CzAJBgNVBAgMAkNBMRQwEgYDVQQHDAtTYW50YSBDbGFyYTElMCMGA1UECgwcR2ln\n"\
    "YWZpdmUgVGVjaG5vbG9neSBQYXJ0bmVyczEZMBcGA1UEAwwQR2lnYWZpdmUgUm9v\n"\
    "dCBDQTEeMBwGCSqGSIb3DQEJARYPY2FAZ2lnYWZpdmUuY29tMB4XDTE2MDgyNzE2\n"\
    "NDYyM1oXDTI2MDgyNTE2NDYyM1owgZcxCzAJBgNVBAYTAlVTMQswCQYDVQQIDAJD\n"\
    "QTEUMBIGA1UEBwwLU2FudGEgQ2xhcmExKTAnBgNVBAoMIEdpZ2FmaXZlIFRlY2hu\n"\
    "b2xvZ3kgUGFydG5lcnMgTExDMRkwFwYDVQQDDBBhcGkuZ2lnYWZpdmUuY29tMR8w\n"\
    "HQYJKoZIhvcNAQkBFhBjcmxAZ2lnYWZpdmUuY29tMIICIjANBgkqhkiG9w0BAQEF\n"\
    "AAOCAg8AMIICCgKCAgEAof82VrEpXMpsI/ddW6RLeTeSYtxiXZZkRbDKN6otYgEk\n"\
    "vA8yRbzei2cO2A/8+Erhe9beYLAMXWF+bjoUAFwnuIcbmufgHprOYzX/7CYXCsrH\n"\
    "LrJfVF6kvjCXy2W3xSvgh8ZgHNWnBGzl13tq19Fz8x0AhK5GQ9608oJCbnQjpVSI\n"\
    "lZDl3JVOifCeXf2c7nMhVOC/reTeto0Gbchs8Ox50WyojmfYbVjOQcA7f8p1eI+D\n"\
    "XUJK01cUGVu6/KarVArGHh5LsiyXOadbyeyOXPmjyrgarG3IIBeQSNECfJZPc/OW\n"\
    "lFszjU4YLDckI4x+tReiuFQbQPN5sDplcEldmZZm/8XD36ddvAaDds+SYlPXxDK7\n"\
    "7L8RBVUG2Ylc9YZf7RE6IMDmdQmsCZDX0VxySYEmzv5lnAx4mzzaXcgS+kHMOLyK\n"\
    "n9UxmpzwQoqqC9tMZqwRaeKW1njR1dSwQLqirBPfGCWKkpkpm7C3HEfeeLrasral\n"\
    "aPf6LAwN3A4ZKHa5Jmne7W+1eYS1aTXOAOLIPcXRAh1B80H+SusIdM9d6vk2YTIg\n"\
    "khwGQV3sgM6nIO5+T/8z141UEjWbtP7pb/u0+G9Cg7TwvRoO2UukxdvOwNto1G2e\n"\
    "J3rKB/JSYsYWnPHvvh9XR+55PZ4iCf9Rqw/IP82uyGipR9gxlHqN8WhMTj9tNEkC\n"\
    "AwEAAaMhMB8wHQYDVR0OBBYEFISCemcSriz1HFhRXluw9H+Bv9lEMA0GCSqGSIb3\n"\
    "DQEBCwUAA4IBAQCMetK0xe6Y/uZpb1ARh+hHYcHI3xI+IG4opWJeoB1gDh/xpNAW\n"\
    "j6t5MGbLoqNMBXbqL26hnKVspyvCxw7ebI5ZJgjtbrD1t+0D8yrgIZzr7AWGA9Hj\n"\
    "WIHqDHGDxwkmfjVVPmuO3l5RtJmL6KV6kVL2bOvVI6gECpFLddmOTtg+iXDfSw3x\n"\
    "0+ueMYKr8QLF+TCxfzQTHvTHvOJtcZHecc1n7PYbRmI2p7tV6RoBpV69oM6NAVUV\n"\
    "i2QoSxm0pYzDzavOaxwhEPHT34Tpg6fwXy1QokFD9OtxRFtdpTjL3bMWpatZE+ba\n"\
    "cjvvf0utMW5fNjTTxu1nnpuxZM3ifTCqZJ+9\n"\
    "-----END CERTIFICATE-----\n";

/* Root cert from openssl s_client -connect google.com:443 -showcerts
 */
static const char *rsa2048_cert = "-----BEGIN CERTIFICATE-----\n"\
    "MIIDfTCCAuagAwIBAgIDErvmMA0GCSqGSIb3DQEBBQUAME4xCzAJBgNVBAYTAlVT\n"\
    "MRAwDgYDVQQKEwdFcXVpZmF4MS0wKwYDVQQLEyRFcXVpZmF4IFNlY3VyZSBDZXJ0\n"\
    "aWZpY2F0ZSBBdXRob3JpdHkwHhcNMDIwNTIxMDQwMDAwWhcNMTgwODIxMDQwMDAw\n"\
    "WjBCMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNR2VvVHJ1c3QgSW5jLjEbMBkGA1UE\n"\
    "AxMSR2VvVHJ1c3QgR2xvYmFsIENBMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIB\n"\
    "CgKCAQEA2swYYzD99BcjGlZ+W988bDjkcbd4kdS8odhM+KhDtgPpTSEHCIjaWC9m\n"\
    "OSm9BXiLnTjoBbdqfnGk5sRgprDvgOSJKA+eJdbtg/OtppHHmMlCGDUUna2YRpIu\n"\
    "T8rxh0PBFpVXLVDviS2Aelet8u5fa9IAjbkU+BQVNdnARqN7csiRv8lVK83Qlz6c\n"\
    "JmTM386DGXHKTubU1XupGc1V3sjs0l44U+VcT4wt/lAjNvxm5suOpDkZALeVAjmR\n"\
    "Cw7+OC7RHQWa9k0+bw8HHa8sHo9gOeL6NlMTOdReJivbPagUvTLrGAMoUgRx5asz\n"\
    "PeE4uwc2hGKceeoWMPRfwCvocWvk+QIDAQABo4HwMIHtMB8GA1UdIwQYMBaAFEjm\n"\
    "aPkr0rKV10fYIyAQTzOYkJ/UMB0GA1UdDgQWBBTAephojYn7qwVkDBF9qn1luMrM\n"\
    "TjAPBgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBBjA6BgNVHR8EMzAxMC+g\n"\
    "LaArhilodHRwOi8vY3JsLmdlb3RydXN0LmNvbS9jcmxzL3NlY3VyZWNhLmNybDBO\n"\
    "BgNVHSAERzBFMEMGBFUdIAAwOzA5BggrBgEFBQcCARYtaHR0cHM6Ly93d3cuZ2Vv\n"\
    "dHJ1c3QuY29tL3Jlc291cmNlcy9yZXBvc2l0b3J5MA0GCSqGSIb3DQEBBQUAA4GB\n"\
    "AHbhEm5OSxYShjAGsoEIz/AIx8dxfmbuwu3UOx//8PDITtZDOLC5MH0Y0FWDomrL\n"\
    "NhGc6Ehmo21/uBPUR/6LWlxz/K7ZGzIZOKuXNBSqltLroxwUCEm2u+WR74M26x1W\n"\
    "b8ravHNjkOR/ez4iyz0H7V84dJzjA1BOoa+Y7mHyhD8S\n"\
    "-----END CERTIFICATE-----\n";

/* Some random input bytes to public key encrypt */
static const uint8_t pki_input[4096/8] = {
    0, 1, 4, 6, 7, 9, 33, 103, 49, 11, 56, 211, 67, 92 };

/* Result of an RSA4096 operation using cert's public key
   (raw PKI, no padding/etc) */
static const uint8_t pki_rsa4096_output[] = {
    0x91, 0x87, 0xcd, 0x04, 0x80, 0x7c, 0x8b, 0x0b,
    0x0c, 0xc0, 0x38, 0x37, 0x7a, 0xe3, 0x2c, 0x94,
    0xea, 0xc4, 0xcb, 0x83, 0x2c, 0x77, 0x71, 0x14,
    0x11, 0x85, 0x16, 0x61, 0xd3, 0x64, 0x2a, 0x0f,
    0xf9, 0x6b, 0x45, 0x04, 0x66, 0x5d, 0x15, 0xf1,
    0xcf, 0x69, 0x77, 0x90, 0xb9, 0x41, 0x68, 0xa9,
    0xa6, 0xfd, 0x94, 0xdc, 0x6a, 0xce, 0xc7, 0xb6,
    0x41, 0xd9, 0x44, 0x3c, 0x02, 0xb6, 0xc7, 0x26,
    0xce, 0xec, 0x66, 0x21, 0xa8, 0xe8, 0xf4, 0xa9,
    0x33, 0x4a, 0x6c, 0x28, 0x0f, 0x50, 0x30, 0x32,
    0x28, 0x00, 0xbb, 0x2c, 0xc3, 0x44, 0x72, 0x31,
    0x93, 0xd4, 0xde, 0x29, 0x6b, 0xfa, 0x31, 0xfd,
    0x3a, 0x05, 0xc6, 0xb1, 0x28, 0x43, 0x57, 0x20,
    0xf7, 0xf8, 0x13, 0x0c, 0x4a, 0x80, 0x00, 0xab,
    0x1f, 0xe8, 0x88, 0xad, 0x56, 0xf2, 0xda, 0x5a,
    0x50, 0xe9, 0x02, 0x09, 0x21, 0x2a, 0xfc, 0x82,
    0x68, 0x34, 0xf9, 0x04, 0xa3, 0x25, 0xe1, 0x0f,
    0xa8, 0x77, 0x29, 0x94, 0xb6, 0x9d, 0x5a, 0x08,
    0x33, 0x8d, 0x27, 0x6a, 0xc0, 0x3b, 0xad, 0x91,
    0x8a, 0x83, 0xa9, 0x2e, 0x48, 0xcd, 0x67, 0xa3,
    0x3a, 0x35, 0x41, 0x85, 0xfa, 0x3f, 0x61, 0x1f,
    0x80, 0xeb, 0xcd, 0x5a, 0xc5, 0x14, 0x7b, 0xab,
    0x9c, 0x45, 0x11, 0xd2, 0x25, 0x9a, 0x16, 0xeb,
    0x9c, 0xfa, 0xbe, 0x73, 0x18, 0xbd, 0x25, 0x8e,
    0x99, 0x6d, 0xb3, 0xbc, 0xac, 0x2d, 0xa2, 0x53,
    0xe8, 0x7c, 0x38, 0x1b, 0x7a, 0x75, 0xff, 0x76,
    0x4f, 0x48, 0x5b, 0x39, 0x20, 0x5a, 0x7b, 0x82,
    0xd3, 0x33, 0x33, 0x2a, 0xab, 0x6a, 0x7a, 0x42,
    0x1d, 0x1f, 0xd1, 0x61, 0x58, 0xd7, 0x38, 0x52,
    0xdf, 0xb0, 0x61, 0x98, 0x63, 0xb7, 0xa1, 0x4e,
    0xdb, 0x9b, 0xcb, 0xb7, 0x85, 0xc4, 0x3e, 0x03,
    0xe5, 0x59, 0x50, 0x28, 0x5a, 0x4d, 0x7f, 0x53,
    0x2e, 0x99, 0x1d, 0x6d, 0x85, 0x27, 0x78, 0x34,
    0x5e, 0xae, 0xc9, 0x1b, 0x37, 0x96, 0xde, 0x40,
    0x87, 0x35, 0x3c, 0x1f, 0xe0, 0x8f, 0xfb, 0x3a,
    0x58, 0x0e, 0x60, 0xe9, 0x06, 0xbd, 0x83, 0x03,
    0x92, 0xde, 0x5e, 0x69, 0x28, 0xb1, 0x00, 0xeb,
    0x44, 0xca, 0x3c, 0x49, 0x03, 0x10, 0xa8, 0x84,
    0xa6, 0xbb, 0xd5, 0xda, 0x98, 0x8c, 0x6f, 0xa3,
    0x0f, 0x39, 0xf3, 0xa7, 0x7d, 0xd5, 0x3b, 0xe2,
    0x85, 0x12, 0xda, 0xa4, 0x4d, 0x80, 0x97, 0xcb,
    0x11, 0xe0, 0x89, 0x90, 0xff, 0x5b, 0x72, 0x19,
    0x59, 0xd1, 0x39, 0x23, 0x9f, 0xb0, 0x00, 0xe2,
    0x45, 0x72, 0xc6, 0x9a, 0xbc, 0xe1, 0xd1, 0x51,
    0x6b, 0x35, 0xd2, 0x49, 0xbf, 0xb6, 0xfe, 0xab,
    0x09, 0xf7, 0x9d, 0xa4, 0x6e, 0x69, 0xb6, 0xf9,
    0xde, 0xe3, 0x57, 0x0c, 0x1a, 0x96, 0xf1, 0xcc,
    0x1c, 0x92, 0xdb, 0x44, 0xf4, 0x45, 0xfa, 0x8f,
    0x87, 0xcf, 0xf4, 0xd2, 0xa1, 0xf8, 0x69, 0x18,
    0xcf, 0xdc, 0xa0, 0x1f, 0xb0, 0x26, 0xad, 0x81,
    0xab, 0xdf, 0x78, 0x18, 0xa2, 0x74, 0xba, 0x2f,
    0xec, 0x70, 0xa2, 0x1f, 0x56, 0xee, 0xff, 0xc9,
    0xfe, 0xb1, 0xe1, 0x9b, 0xea, 0x0e, 0x33, 0x14,
    0x5f, 0x6e, 0xca, 0xee, 0x02, 0x56, 0x5a, 0x67,
    0x42, 0x9a, 0xbf, 0x55, 0xc0, 0x0f, 0x8e, 0x01,
    0x67, 0x63, 0x6e, 0xd1, 0x57, 0xf7, 0xf1, 0xc6,
    0x92, 0x9e, 0xb5, 0x45, 0xe1, 0x50, 0x58, 0x94,
    0x20, 0x90, 0x6a, 0x29, 0x2d, 0x4b, 0xd1, 0xb5,
    0x68, 0x63, 0xb5, 0xe6, 0xd8, 0x6e, 0x84, 0x80,
    0xad, 0xe6, 0x03, 0x1e, 0x51, 0xc2, 0xa8, 0x6d,
    0x84, 0xec, 0x2d, 0x7c, 0x61, 0x02, 0xd1, 0xda,
    0xf5, 0x94, 0xfa, 0x2d, 0xa6, 0xed, 0x89, 0x6a,
    0x6a, 0xda, 0x07, 0x5d, 0x83, 0xfc, 0x43, 0x76,
    0x7c, 0xca, 0x8c, 0x00, 0xfc, 0xb9, 0x2c, 0x23,
};

static const uint8_t pki_rsa2048_output[] = {
    0x47, 0x0b, 0xe5, 0x8a, 0xcd, 0x2f, 0x78, 0x07,
    0x69, 0x69, 0x70, 0xff, 0x81, 0xdf, 0x96, 0xf0,
    0xed, 0x82, 0x3a, 0x3d, 0x46, 0xab, 0xe9, 0xc3,
    0xb5, 0xd9, 0xca, 0xa2, 0x05, 0xa9, 0xf6, 0x6e,
    0xad, 0x6c, 0xe0, 0xd1, 0xa2, 0xb4, 0xf2, 0x78,
    0x4a, 0x93, 0xfc, 0x45, 0xe1, 0x9b, 0xdd, 0x62,
    0xf9, 0x66, 0x2a, 0x14, 0x38, 0x12, 0xb6, 0x50,
    0x0b, 0xe3, 0x53, 0x9c, 0x12, 0x56, 0xf1, 0xb7,
    0x83, 0xd5, 0xf3, 0x24, 0x81, 0xcc, 0x5a, 0xeb,
    0xec, 0xac, 0x68, 0xa8, 0x0c, 0xd7, 0x84, 0x7a,
    0xbb, 0x77, 0x7b, 0xd5, 0x5b, 0xcf, 0x7b, 0x25,
    0xd0, 0x75, 0x80, 0x21, 0x12, 0x97, 0x6b, 0xe1,
    0xb6, 0x51, 0x12, 0x52, 0x6e, 0x01, 0x92, 0xb7,
    0xcc, 0x70, 0x4b, 0x46, 0x11, 0x98, 0x5a, 0x84,
    0x1c, 0x90, 0x45, 0x0f, 0x15, 0x77, 0xdb, 0x79,
    0xe8, 0xff, 0x1f, 0xaa, 0x58, 0x95, 0xce, 0x3c,
    0x65, 0x0c, 0x66, 0x29, 0xe1, 0x9c, 0x41, 0xbb,
    0xde, 0x65, 0xb8, 0x29, 0x36, 0x94, 0xbd, 0x87,
    0x93, 0x39, 0xc5, 0xeb, 0x49, 0x21, 0xc1, 0xeb,
    0x48, 0xbd, 0x19, 0x13, 0x4d, 0x40, 0x90, 0x88,
    0xc6, 0x12, 0xd9, 0xf7, 0xdd, 0xc8, 0x4f, 0x89,
    0xc0, 0x91, 0xf8, 0xeb, 0xcf, 0xe3, 0x12, 0x17,
    0x88, 0x9c, 0x88, 0xf4, 0xf5, 0xae, 0xf4, 0x15,
    0xfe, 0x17, 0xf6, 0xa4, 0x74, 0x49, 0x02, 0x05,
    0x11, 0x3b, 0x92, 0x25, 0x39, 0x2c, 0x4b, 0x08,
    0x19, 0x76, 0x13, 0x8d, 0xf9, 0xda, 0xae, 0xdf,
    0x30, 0xda, 0xcc, 0xbb, 0x3f, 0xb9, 0xb0, 0xd6,
    0x5c, 0x78, 0x4b, 0x2b, 0x35, 0x51, 0x17, 0x48,
    0xf5, 0xd4, 0x39, 0x7e, 0x05, 0x83, 0x68, 0x86,
    0x44, 0x5f, 0x56, 0x1d, 0x2c, 0x53, 0xd3, 0x64,
    0x3a, 0xb2, 0x0c, 0x4a, 0x85, 0xd6, 0x5b, 0x7e,
    0xf9, 0xe9, 0x50, 0x29, 0x5d, 0x4f, 0xcc, 0xc9,
};

#ifdef CONFIG_MBEDTLS_HARDWARE_MPI
/* Pregenerated RSA 4096 size keys using openssl */
static const char privkey_4096_buf[] = "-----BEGIN RSA PRIVATE KEY-----\n"
                                  "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\n"
                                  "-----END RSA PRIVATE KEY-----";

static const char privkey_2048_buf[] = "-----BEGIN RSA PRIVATE KEY-----\r\n"
    "MIIEowIBAAKCAQEA8N8hdkemvj6Tpk975/OWhv9BrTsCBCu+ZYfDb5VI7U2meKBg\r\n"
    "3dAkyyhRlY3fNwSRzBUMCzsHjpgnsB40wxOgiwlB9n6PMhq0qUVKAdCpKwFztsKd\r\n"
    "JJAsCUC+Zlwxn4RpH6ZnMl3a/njRYjuDyI32kucMP/lBRo7ks1798Gy/j+x1h5xA\r\n"
    "vZSlFoEXKjCC6S1DWhALePuZnk4m/jGP6g+YfyJXSTqsenKa/DcWndfn/JoElZ0J\r\n"
    "nhud8lBXwVe6mMheE1yqfL+VTU1nwg/TPNZrZsFz2sXig/RQCKt6LuSuzhRpsLp+\r\n"
    "BdwqEs9xrwlhZnp7j4kQBomISd6kAxQfYVROHQIDAQABAoIBAHgtO4rB8QWWPyCJ\r\n"
    "I670r7OnA2OkvzrJgHMzq2SuvPX4+gfRLMM+qDzcXugZIrdWhk+maJ3p07lnXNXY\r\n"
    "HEcAMedstQaA2n0LKfwSX/xL2TtlvBABRVoKvI3ZSaXUdcW60KBD69ULUsoICZ/T\r\n"
    "Rcr4WX+t20TH3bOQc7ayvEwKVgE95xIUpTH9asw8uOPvKxW2j5OLQgZuWrWyUDg0\r\n"
    "MFh92PhWtw3i5zq6OpTTsFJeceKYV/VstIYjZ+FslmhjQxJbr+2DJRbpHXKceqy6\r\n"
    "9yWlSV0EM7neFCHlDa2WPhK8we+6IvMiNVQKj46fHGYNBaW/ZSX7TiG5J0Uqj2e9\r\n"
    "0MUGJ8ECgYEA+frJabhfzW5+JfGjTObeznJZE6fAOjFzaBIwFu8Kz2mIjYpQlwVK\r\n"
    "EepMkv2KkrJuqS4GnI+Nkq7G0BAUyUj9tTJ3HQzvtJrxsnxVi99Yofx1s1P4YAnu\r\n"
    "c8t3ElJoQ4BRoQIs/hIvyYn22IxllBHiGESrnPQ38D82xyXQgd6S8JkCgYEA9qww\r\n"
    "j7jx6Xpy/D1Dq8Dvalm7pz3J+yHnti4w2cqZ67grUoyGnNPtciNDdfi4JzLiKkUu\r\n"
    "SDS3DacvFpFyND0m8sbpMjnR8Rvhj+bfH8KcOAowD+YR/+6vSb/P/aBt6gYXcaBn\r\n"
    "cjepx+sE81mnC7UrHb4TjG4hO5t3ZTc6X28gyCUCgYAMZn9lSisecrO5SCJUp0M4\r\n"
    "NH3stq6XdGqIKBbQnG0J2u9WLh1PUIjbGKdRx1f/bPCGXe0gCRL5yse7/IA7d+51\r\n"
    "9ZnpDAI8EE+bDgXkWWD5MB/alHjGstdsURSICSR47L2f4g6/T8GlGr3vAg/r53My\r\n"
    "xv1IXOkFdu1NtbeBKbxaSQKBgENDmw5mAVmIcXiFAEICn4ahp4EoYT6g9T2BhQKu\r\n"
    "s6BKnU2qUj7Lr5ETOp8dzqGpx3B9Yux/q3cGotmFmd3S2x8SzJ5MlAoqbyy9aRSR\r\n"
    "DeZeKNL9CuV+YcA7lOz1ZWOOe7AZbHwB38NLPBNb3CheI769iTkfAuLtNvabw8go\r\n"
    "VokdAoGBALyvBhW+Squ5tx8NOEgAisakhAVOnT6jcoeKy6FyjcvKaWagmCOCC7Gz\r\n"
    "QB9Yf1tJ+3di+aLtWWdmU494iKJHBtPMhfrYltCpxHHQGlUc/GLPY3Z5bBYYYWpb\r\n"
    "Wzw4ZvDraKlAs7a9CRwS5cpktk5ptK4rc5noSXkvV+yOT75zXat2\r\n"
    "-----END RSA PRIVATE KEY-----\r\n";

#endif

_Static_assert(sizeof(pki_rsa2048_output) == 2048/8, "rsa2048 output is wrong size");
_Static_assert(sizeof(pki_rsa4096_output) == 4096/8, "rsa4096 output is wrong size");

static void test_cert(const char *cert, const uint8_t *expected_output, size_t output_len);
void mbedtls_mpi_printf(const char *name, const mbedtls_mpi *X);

TEST_CASE("mbedtls RSA4096 cert", "[mbedtls]")
{

    test_cert(rsa4096_cert, pki_rsa4096_output, 4096/8);
}

TEST_CASE("mbedtls RSA2048 cert", "[mbedtls]")
{
    test_cert(rsa2048_cert, pki_rsa2048_output, 2048/8);
}

static void test_cert(const char *cert, const uint8_t *expected_output, size_t output_len)
{
    mbedtls_x509_crt crt;
    mbedtls_rsa_context *rsa;
    char buf[output_len];

    bzero(buf, output_len);

    mbedtls_x509_crt_init(&crt);

    TEST_ASSERT_EQUAL_HEX16_MESSAGE(0,
                                    -mbedtls_x509_crt_parse(&crt,
                                                            (const uint8_t *)cert,
                                                            strlen(cert)+1),
                                    "parse cert");

    rsa = mbedtls_pk_rsa(crt.pk);
    TEST_ASSERT_NOT_NULL(rsa);

    TEST_ASSERT_EQUAL_HEX16_MESSAGE(0,
                                    -mbedtls_rsa_check_pubkey(rsa),
                                    "check cert pubkey");

    mbedtls_x509_crt_info(buf, sizeof(buf), "", &crt);
    puts(buf);

    TEST_ASSERT_EQUAL_HEX16_MESSAGE(0,
                                    -mbedtls_rsa_public(rsa, pki_input, (uint8_t *)buf),
                                    "RSA PK operation");

    /*
    // Dump buffer for debugging
    for(int i = 0; i < output_len; i++) {
        printf("0x%02x, ", buf[i]);
    }
    printf("\n");
    */

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_output, buf, output_len);

    mbedtls_x509_crt_free(&crt);
}

#ifdef CONFIG_MBEDTLS_HARDWARE_MPI
static void rsa_key_operations(int keysize, bool check_performance, bool use_blinding, bool generate_new_rsa);

static int myrand(void *rng_state, unsigned char *output, size_t len)
{
    size_t olen;
    return mbedtls_hardware_poll(rng_state, output, len, &olen);
}

#ifdef PRINT_DEBUG_INFO
static void print_rsa_details(mbedtls_rsa_context *rsa)
{
    mbedtls_mpi X[5];
    for (int i=0; i<5; ++i) {
        mbedtls_mpi_init( &X[i] );
    }

    if (0 == mbedtls_rsa_export(rsa, &X[0], &X[1], &X[2], &X[3], &X[4])) {
        for (int i=0; i<5; ++i) {
            mbedtls_mpi_printf((char*)"N\0P\0Q\0D\0E" + 2*i, &X[i]);
            mbedtls_mpi_free( &X[i] );
        }
    }
}
#endif

TEST_CASE("test performance RSA key operations", "[bignum]")
{
    for (int keysize = 2048; keysize <= 4096; keysize += 2048) {
        rsa_key_operations(keysize, true, false, false);
    }
}

TEST_CASE("test RSA-4096 calculations", "[bignum]")
{
    // use pre-genrated keys to make the test run a bit faster
    rsa_key_operations(4096, false, true, false);
}

static void rsa_key_operations(int keysize, bool check_performance, bool use_blinding, bool generate_new_rsa)
{
    mbedtls_rsa_context rsa;
    unsigned char orig_buf[4096 / 8];
    unsigned char encrypted_buf[4096 / 8];
    unsigned char decrypted_buf[4096 / 8];
    int public_perf, private_perf;

    printf("First, orig_buf is encrypted by the public key, and then decrypted by the private key\n");
    printf("keysize=%d check_performance=%d use_blinding=%d generate_new_rsa=%d\n", keysize, check_performance, use_blinding, generate_new_rsa);

    memset(orig_buf, 0xAA, sizeof(orig_buf));
    orig_buf[0] = 0; // Ensure that orig_buf is smaller than rsa.N
    if (generate_new_rsa) {
        mbedtls_rsa_init(&rsa, MBEDTLS_RSA_PRIVATE, 0);
        TEST_ASSERT_EQUAL(0, mbedtls_rsa_gen_key(&rsa, myrand, NULL, keysize, 65537));
    } else if (keysize==4096) {
        mbedtls_pk_context clientkey;
        mbedtls_pk_init(&clientkey);
        TEST_ASSERT_EQUAL(0, mbedtls_pk_parse_key(&clientkey, (const uint8_t *)privkey_4096_buf, sizeof(privkey_4096_buf), NULL, 0));
        memcpy(&rsa, mbedtls_pk_rsa(clientkey), sizeof(mbedtls_rsa_context));
    } else if (keysize==2048) {
        mbedtls_pk_context clientkey;
        mbedtls_pk_init(&clientkey);
        TEST_ASSERT_EQUAL(0, mbedtls_pk_parse_key(&clientkey, (const uint8_t *)privkey_2048_buf, sizeof(privkey_2048_buf), NULL, 0));
        memcpy(&rsa, mbedtls_pk_rsa(clientkey), sizeof(mbedtls_rsa_context));
    } else { // pre-generated private key only available for keysize=4096 and 2048
        printf("Not supported keysize, please use generate_new_rsa=true\n");
        abort();
    }
#ifdef PRINT_DEBUG_INFO
    print_rsa_details(&rsa);
#endif

    TEST_ASSERT_EQUAL(keysize, (int)rsa.len * 8);
    TEST_ASSERT_EQUAL(keysize, (int)rsa.D.n * sizeof(mbedtls_mpi_uint) * 8); // The private exponent

    ccomp_timer_start();;
    TEST_ASSERT_EQUAL(0, mbedtls_rsa_public(&rsa, orig_buf, encrypted_buf));
    public_perf = ccomp_timer_stop();

    ccomp_timer_start();;
    TEST_ASSERT_EQUAL(0, mbedtls_rsa_private(&rsa, use_blinding?myrand:NULL, NULL, encrypted_buf, decrypted_buf));
    private_perf = ccomp_timer_stop();

    if (check_performance && keysize == 2048) {
        TEST_PERFORMANCE_LESS_THAN(RSA_2048KEY_PUBLIC_OP, "%d us", public_perf);
        TEST_PERFORMANCE_LESS_THAN(RSA_2048KEY_PRIVATE_OP, "%d us", private_perf);
    } else if (check_performance && keysize == 4096) {
        TEST_PERFORMANCE_LESS_THAN(RSA_4096KEY_PUBLIC_OP, "%d us", public_perf);
        TEST_PERFORMANCE_LESS_THAN(RSA_4096KEY_PRIVATE_OP, "%d us", private_perf);
    }

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(orig_buf, decrypted_buf, keysize / 8, "RSA operation");

    mbedtls_rsa_free(&rsa);
}

#endif // CONFIG_MBEDTLS_HARDWARE_MPI
