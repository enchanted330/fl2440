#!/bin/sh

mkdir -p certs
mkdir -p private
rm -f serial* index.txt* 
rm -f certs/*
rm -f private/*
echo '01' > serial 
touch index.txt
chmod 666 serial index.txt

#Create CA
CA_CERT_FILE=certs/ca_crt.pem
CA_KEY_FILE=private/ca_key.pem

sed -i 's/= client/= server/g' conf/openssl.cnf
echo "====================================================================="
echo "= Generate CA certficate and private key need input pass phrase:"
echo "====================================================================="
openssl req -x509 -newkey rsa:2048 -out $CA_CERT_FILE -outform PEM -days 1825 -config conf/openssl.cnf
echo "====================================================================="
echo "= Remove CA private key pass phrase:"
echo "====================================================================="
openssl rsa -in $CA_KEY_FILE  -out $CA_KEY_FILE

#Create Server Certificate 
SERVER_KEY_FILE=private/server_key.pem
SERVER_CERT_FILE=certs/server_crt.pem
echo "====================================================================="
echo "= Generate Server certficate and private key need input pass phrase:"
echo "====================================================================="
openssl req -newkey rsa:1024 -keyout $SERVER_KEY_FILE -keyform PEM -out tempreq.pem -outform PEM -config conf/server.cnf 

echo "====================================================================="
echo "= Remove server private key pass phrase:"
echo "====================================================================="
openssl rsa -in $SERVER_KEY_FILE  -out $SERVER_KEY_FILE

echo "====================================================================="
echo "= Signature Server certficate by CA need input CA key pass phrase:"
echo "====================================================================="
sed -i 's/= client/= server/g' conf/openssl.cnf
openssl ca -in tempreq.pem -out $SERVER_CERT_FILE -config conf/openssl.cnf
rm -f tempreq.pem
rm -f certs/01.pem

#Create Client Certificate 
CLIENT_KEY_FILE=private/client_key.pem
CLIENT_CERT_FILE=certs/client_crt.pem
echo "====================================================================="
echo "= Generate client certficate and private key need input pass phrase:"
echo "====================================================================="
openssl req -newkey rsa:1024 -keyout $CLIENT_KEY_FILE -keyform PEM -out tempreq.pem -outform PEM -config conf/client.cnf 

echo "====================================================================="
echo "= Remove client private key pass phrase:"
echo "====================================================================="
openssl rsa -in $CLIENT_KEY_FILE  -out $CLIENT_KEY_FILE

echo "====================================================================="
echo "= Signature client certficate by CA need input CA key pass phrase:"
echo "====================================================================="
sed -i 's/= server/= client/g' conf/openssl.cnf
openssl ca -in tempreq.pem -out $CLIENT_CERT_FILE -config conf/openssl.cnf
rm -f tempreq.pem
rm -f certs/02.pem
rm -f index.txt.* serial.old 



echo "====================================================================="
echo "= Convert all the certificate and key from PEM formate to DER formate"
echo "====================================================================="

cd certs/
openssl x509 -in ca_crt.pem -inform PEM -out ca_crt.der -outform DER
openssl x509 -in client_crt.pem -inform PEM -out client_crt.der -outform DER
openssl x509 -in server_crt.pem -inform PEM -out server_crt.der -outform DER  

cd ../private
openssl rsa -in ca_key.pem -outform DER -out ca_key.der    
openssl rsa -in server_key.pem -outform DER -out server_key.der
openssl rsa -in client_key.pem -outform DER -out client_key.der


