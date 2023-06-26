#!/bin/bash

help_text="
Usage: generate_certs.sh <cert_type>\n
<cert_type> only support p384, 2048, 3072, 4096\n
example:\n
sh generate_certs.sh p384\n
sh generate_certs.sh 2048\n
sh generate_certs.sh 3072\n
sh generate_certs.sh 4096\n
"

DIGEST="-sha256"
DIGEST_CA="-md sha256"
CERT_TYPE="2048"
CERT="2048-ca"

show_help() {
    echo -e $help_text
}

init_param() {
    if [ $(basename "$(pwd)") != "generate_certs" ]; then
        echo "path is incorrect, please go into generate_certs directory"
        exit
    fi

    CERT_TYPE=$1
    CERT=${CERT_TYPE}-ca

    if [ -d "$CERT" ]; then
        rm -rf "$CERT"
    fi

    if [ $1 = "p384" ] || [ $1 = "3072" ]; then
        DIGEST="-sha384"
        DIGEST_CA="-md sha384"
    elif [ $1 = "2048" ] || [ $1 = "4096" ]; then
        DIGEST="-sha256"
        DIGEST_CA="-md sha256"
    else
        echo "parameter error"
        exit
    fi
}

create_ca() {
    echo
    echo "---[ Root CA ]----------------------------------------------------------"

    if [ -d $CERT ]; then
        rm $CERT
    fi

    mkdir -p $CERT
    cat example-ca-openssl.cnf |
    sed "s/#@CN@/commonName_default = Root CA/" |
    sed s%\./ca$%./$CERT% \
    > ${CERT}-openssl.cnf.tmp
    mkdir -p $CERT/certs $CERT/crl $CERT/newcerts $CERT/private
    case "$CERT_TYPE" in
        "p384") openssl ecparam -out $CERT/ca.key -name secp384r1 -genkey;;
        "2048") openssl genrsa -out $CERT/ca.key 2048;;
        "3072") openssl genrsa -out $CERT/ca.key 3072;;
        "4096") openssl genrsa -out $CERT/ca.key 4096;;
    esac
    openssl req -config ${CERT}-openssl.cnf.tmp -batch -new -x509 -key $CERT/ca.key -out $CERT/ca.pem $DIGEST
    touch $CERT/index.txt
    rm ${CERT}-openssl.cnf.tmp
}

create_certs() {
    echo
    echo "---[ Server ]-----------------------------------------------------------"
    echo

    cat example-ca-openssl.cnf |
    	sed "s/#@CN@/commonName_default = $CERT_TYPE.$1/" |
        sed s%\./ca$%./$CERT% \
        > ${CERT}-openssl.cnf.tmp
    echo "---[ Generate $1 Key]----------------------------------------------"
    case "$CERT_TYPE" in
        "p384") openssl ecparam -out $CERT/$1.key -name secp384r1 -genkey;;
        "2048") openssl genrsa -out $CERT/$1.key 2048;;
        "3072") openssl genrsa -out $CERT/$1.key 3072;;
        "4096") openssl genrsa -out $CERT/$1.key 4096;;
    esac
    echo "---[ Generate $1 Req]----------------------------------------------"
    openssl req -config ${CERT}-openssl.cnf.tmp -batch -new -key $CERT/$1.key -out $CERT/$1.req $DIGEST
    openssl ca -config ${CERT_TYPE}-ca-openssl.cnf.tmp -batch -keyfile $CERT/ca.key -cert $CERT/ca.pem -create_serial -in $CERT/$1.req -out $CERT/$1.pem -extensions ext_$1 ${DIGEST_CA}
    cp $CERT/$1.pem $CERT/$1.crt
    rm ${CERT_TYPE}-ca-openssl.cnf.tmp
}

verify() {
    echo
    echo "---[ Verify ]-----------------------------------------------------------"
    echo

    openssl verify -CAfile $CERT/ca.pem $CERT/server.pem
    openssl verify -CAfile $CERT/ca.pem $CERT/server.crt
    openssl verify -CAfile $CERT/ca.pem $CERT/client.pem
    openssl verify -CAfile $CERT/ca.pem $CERT/client.crt
}

clean() {
    rm $CERT/*.req
    rm $CERT/index*
    rm $CERT/serial*
    rm -rf $CERT/certs
    rm -rf $CERT/newcerts
    rm -rf $CERT/private
    rm -rf $CERT/crl
}

if [ "$1" = "--help" ] || [ "$1" = "-h" ] || [ -z "$1" ]; then
    show_help
    exit 0
else
    init_param $1
    create_ca
    create_certs "server"
    create_certs "client"
    verify
    clean
fi
