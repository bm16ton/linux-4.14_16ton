#!/bin/bash
echo "/dev/ttyX needs chmod aou+wrx or something similiar to work sudo wont pass correctly"
 printf '\001'"Z""00"'\002'"AA"'\x1B'" "'\x6f''\x1C\x43''\x13''\004' | tee -a /dev/ttyUSB0
sleep 1.5
 printf '\001'"Z""00"'\002'"AA"'\x1B'" "'\x6f''\x1C\x43''\x13''\004' | tee -a /dev/ttyUSB0
