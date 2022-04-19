#!/bin/bash

make
sudo insmod seqfile.ko
lsmod | grep seqfile

sudo dmesg | grep fortune

echo "12345" > /proc/fortuneFile
echo "dkfjaijf" > /proc/fortuneFile
echo "m k e j" > /proc/fortuneFile

cat /proc/fortuneFile
cat /proc/fortuneFile
cat /proc/fortuneFile

sudo rmmod seqfile

sudo dmesg | grep fortune

make disclean
