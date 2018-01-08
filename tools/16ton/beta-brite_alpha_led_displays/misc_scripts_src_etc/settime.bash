#!/bin/bash
#stty -F /dev/ttyUSB0 9600 cs8 -cstopb -parenb
# echo "/dev/ttyX needs chmod aou+wrx or something similiar to work sudo wont pass correctly"
#printf '\0 \0 \0 \0 \0 \0 \0 \0 \0 \0 \0 \0 \0 \0 \0 \0 \0 \0 \0 \0 \0 \0 \0 \0 \0 ' | tee -a /dev/ttyUSB0
#sleep .2
 printf '\001'"Z""00"'\002'"E"" "$1'\004' | tee -a /dev/ttyUSB0
sleep 1.5
 printf '\001'"Z""00"'\002'"E"" "$1'\004' | tee -a /dev/ttyUSB0
