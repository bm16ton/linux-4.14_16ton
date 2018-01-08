#!/usr/bin/perl
print "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
$message = "FINALLY!!"
# print "\001" . "Z" . "00" . "\002" . "AA" . "\x1B" . " b" . $message . "\004";
print "\001" . "Z" . "00" . "\002" . "AA" . "\x1B" . "\x6F" . "\x11" . "\x1C\x41" . $message . "\004";
