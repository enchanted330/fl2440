#!/bin/bash

if [ $# !=  2 ] ; then
    echo "Usage: $0 [username] [password]"
    exit
fi

if [ `id -u` != 0 ] ; then
    echo "Only root can create new account, exit now..."
    exit
fi

username=$1
password=$2

echo "Create $username with default password: $2"
useradd -g trainning $username
echo $password | passwd $username --stdin
