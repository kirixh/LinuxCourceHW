#!/bin/bash

make clean
sudo rmmod phoneBook.ko
sudo rm /dev/phonebook
sudo mknod /dev/phonebook c 222 0
sudo chmod a+rw /dev/phonebook
make
/usr/src/linux-headers-6.2.0-39-generic/scripts/sign-file sha256     ~/signing_key.priv     ~/signing_key.x509     ./phoneBook.ko
sudo insmod phoneBook.ko
