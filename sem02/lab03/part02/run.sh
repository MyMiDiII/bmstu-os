#!/bin/bash
set -e

make
echo ">>>>> Загрузка md1.ko..."
sleep 0.8
insmod md1.ko

echo
echo ">>>>> Загрузка md2.ko..."
sleep 0.8
insmod md2.ko

echo
echo ">>>>> Модули md1 и md2 загружены:"

lsmod | grep 'md\|Module'
echo
sleep 1

echo ">>>>> Последние сообщения загруженных модулей"
dmesg | tail -10 | grep md | grep -v audit
echo

echo ">>>>> Удаление md2..."
rmmod md2
echo
sleep 0.8

echo ">>>>> Загрузка md3.ko..."
echo
insmod md3.ko
sleep 0.8

echo ">>>>> Последние сообщения загруженных модулей"
dmesg | tail -10 | grep md | grep -v audit
echo

echo ">>>>> Remove md3..."
rmmod md3
echo
sleep 0.8

echo ">>>>> Remove md1..."
rmmod md1
echo
sleep 0.8

echo ">>>>> Последние сообщения модулей"
dmesg | tail -10 | grep md | grep -v audit

