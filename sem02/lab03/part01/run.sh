#!/bin/bash
make
echo ">>>>> Загрузка mymd.ko"
sleep 0.8
insmod mymd.ko
echo
echo ">>>>> Загруженный модуль:"
lsmod | grep 'mymd\|Module'
echo
sleep 1
echo ">>>>> Последние сообщения модуля"
dmesg | tail -5 | grep mymd
echo
echo ">>>>> Удаление модуля md из ядра"
rmmod mymd
sleep 0.8
dmesg | tail -5 | grep mymd
