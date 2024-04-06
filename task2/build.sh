#!/bin/bash

make clean
sudo rmmod keyboardPressCounter.ko
make
/usr/src/linux-headers-6.2.0-39-generic/scripts/sign-file sha256     ~/signing_key.priv     ~/signing_key.x509     ./keyboardPressCounter.ko
sudo insmod keyboardPressCounter.ko
