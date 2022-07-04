echo NOTICE: This used the experimental sparse checkout
echo See https://git-scm.com/docs/git-sparse-checkout/2.37.0
echo and https://github.blog/2020-01-17-bring-your-monorepo-down-to-size-with-sparse-checkout/
echo ""
echo  intialize...
git init

echo remote origin https://github.com/wolfSSL/wolfssl.git
git remote add origin https://github.com/wolfSSL/wolfssl.git

echo setting fetch depth = 1
git fetch --depth 1

echo enabling sparse checkout to limit only files for this component
git config core.sparsecheckout true

echo  it seems only when all parameters are on the git sparse-checkout command does this do what is desired to include and exclude proprly:
git sparse-checkout set  /src/ /test/ /wolfssl/ /wolfcrypt/ !/wolfcrypt/src/*.S !/wolfcrypt/src/*.asm /wolfcrypt/src/port/Espressif/ /wolfcrypt/src/port/atmel/  !/wolfcrypt/src/port/Renesas/  !/wolfcrypt/src/port/af_alg/  !/wolfcrypt/src/port/arm/ !/wolfcrypt/src/port/caam/ !/wolfcrypt/src/port/cavium/ !/wolfcrypt/src/port/cypress/ !/wolfcrypt/src/port/devcrypto/  !/wolfcrypt/src/port/intel/ !/wolfcrypt/src/port/iotsafe/  !/wolfcrypt/src/port/kcapi/ !/wolfcrypt/src/port/mynewt/ !/wolfcrypt/src/port/nxp/ !/wolfcrypt/src/port/pic32/ !/wolfcrypt/src/port/psa/ !/wolfcrypt/src/port/silabs/ !/wolfcrypt/src/port/st/ !/wolfcrypt/src/port/ti/ !/wolfcrypt/src/port/xilinx/ !/wolfssl/wolfcrypt/port/Renesas/  !/wolfssl/wolfcrypt/port/af_alg/  !/wolfssl/wolfcrypt/port/arm/ !/wolfssl/wolfcrypt/port/caam/ !/wolfssl/wolfcrypt/port/cavium/ !/wolfssl/wolfcrypt/port/cypress/ !/wolfssl/wolfcrypt/port/devcrypto/  !/wolfssl/wolfcrypt/port/intel/ !/wolfssl/wolfcrypt/port/iotsafe/  !/wolfssl/wolfcrypt/port/kcapi/ !/wolfssl/wolfcrypt/port/mynewt/ !/wolfssl/wolfcrypt/port/nxp/ !/wolfssl/wolfcrypt/port/pic32/ !/wolfssl/wolfcrypt/port/psa/ !/wolfssl/wolfcrypt/port/silabs/ !/wolfssl/wolfcrypt/port/st/ !/wolfssl/wolfcrypt/port/ti/ !/wolfssl/wolfcrypt/port/xilinx/

echo checking out master branch....
git checkout -b master

echo  by the time we get here, we should be excluding everything expect those items
echo  of interest for the wolfSSL component
git pull origin master

mkdir include
git show master:IDE/Espressif/ESP-IDF/user_settings.h     > include/user_settings.h
git show master:IDE/Espressif/ESP-IDF/libs/CMakeLists.txt > CMakeLists.txt
git show master:IDE/Espressif/ESP-IDF/libs/component.mk   > component.mk

echo wolfSSH installed!
