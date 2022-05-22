#!/bin/bash

make
sudo insmod myvfs.ko
lsmod | grep MyVFS

echo
sudo dmesg | grep MyVFS

echo
touch image
mkdir dir
touch image1
mkdir dir1

echo
sudo mount -o loop -t MyVFS ./image ./dir
sudo mount -o loop -t MyVFS ./image1 ./dir1

echo 
sudo cat /proc/slabinfo | grep MyVFSCache

echo
mount | grep MyVFS

