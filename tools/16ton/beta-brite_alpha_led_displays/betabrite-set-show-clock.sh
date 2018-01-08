#!/bin/bash
echo "/dev/ttyX needs chmod aou+wrx or something similiar to work sudo wont pass correctly"
echo "settime in 24hr format no colon use all 4 charactors so 130am = 0130 130pm = 1330"
 printf '\001'"Z""00"'\002'"E"" "$1'\004' | tee -a /dev/ttyUSB0
sleep 1.5
 printf '\001'"Z""00"'\002'"E"" "$1'\004' | tee -a /dev/ttyUSB0
sleep 1.5
 printf '\001'"Z""00"'\002'"AA"'\x1B'" "'\x6f''\x1C\x43''\x13''\004' | tee -a /dev/ttyUSB0
sleep 1.5
 printf '\001'"Z""00"'\002'"AA"'\x1B'" "'\x6f''\x1C\x43''\x13''\004' | tee -a /dev/ttyUSB0
