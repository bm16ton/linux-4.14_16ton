#!/usr/bin/perl
# Get the attention of the sign
print "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

# Tell the sign to print the message
$message = "Whhhhoooooooaaaaa!";
print "\001" . "Z" . "00" . "\002" . "AA" . "\x1B" . "\x1C" . "\x36" . $message . "\004";



