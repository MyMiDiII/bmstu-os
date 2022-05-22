#!/bin/bash

sudo umount ./dir
sudo umount ./dir1

sudo rmmod myvfs

echo
sudo dmesg | grep MyVFS

rm -rf ./dir
rm ./image
rm -rf ./dir1
rm ./image1

make disclean

sudo dmesg -c > /dev/null
