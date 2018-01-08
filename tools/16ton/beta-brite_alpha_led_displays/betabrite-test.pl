#!/usr/bin/perl
# Get the attention of the sign
print "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

# Tell the sign to print the message
$message = "   Hello        bWorld!";
#print "\001" . "Z" . "00" . "\002" . "AA" . "\x1B" . " \x6e\x37" . "\x1C\x41" . "\x13" . "\x1B" . "\x0C" . " \x1c\x65" . "\x1C\x31" . "\x13" . "\004";
#print "\001" . "Z" . "00" . "\002" . "AA" . "\x1B" . "\x6F" . " \x69" . "\004";
print "\001" . "Z" . "00" . "\002" . "AA" .  "\x1B" . " \x69" . " \x1C\x32" . $message . "\004";

# USAGE;       ./test.pl > /dev/ttyUSB0

# a = reg font tilted right right to left scroll
# t = small font right to left scroll
# s = just show up
# d = random
# k = wipe right to left
# p = disapears from top down and bottom up at same time

# COLORS
# COLOR_RED "\x1C\x31"
# COLOR_GREEN "\x1C\x32"
# COLOR_AMBER "\x1C\x33"
# COLOR_DIMRED "\x1C\x34"
# COLOR_DIMGREEN "\x1C\x35"
# COLOR_BROWN "\x1C\x36"
# COLOR_ORANGE "\x1C\x37"
# COLOR_YELLOW "\x1C\x38"
# COLOR_RAINBOW1 "\x1C\x39"
# COLOR_RAINBOW2 "\x1C\x41"
# COLOR_MIX "\x1C\x42"
# COLOR_AUTO "\x1C\x43"

# \x0C  NEW PAGE
# \x0D  NEW LINE

# \x13  show time

################# add \x1C before mode if changing from inside txt
# MODES 
# ROTATE "\x61"
# HOLD "\x62"
# FLASH "\x63"
# ROLL_UP "\x65"
# ROLL_DOWN "\x66"
# ROLL_LEFT "\x67"
# ROLL_RIGHT "\x68"
# WIPE_UP "\x69"
# WIPE_DOWN "\x6a"
# WIPE_LEFT "\x6b"
# WIPE_RIGHT "\x6c"
# SCROLL "\x6d"         SAME AS ROLL_UP
# AUTOMODE "\x6f"
# ROLL_IN "\x70"        WORDS APPEAR AT TOP AND BOTTOM AND GROW TILL MEET IN CENTER
# ROLL_OUT "\x71"       WORDS APPEAR IN MIDDLE (of UP/DOWN) and GROW UP AND DOWN
# WIPE_IN "\x72"        OVER WRITES OLD WITH NEW FROM RIGHT TO LEFT
# WIPE_OUT "\x73"       pretty much just shows up
# COMPRESSED_ROTATE "\x74"    SCROLLS RIGHT TO LEFT SMALLER FONT/TXT
# TWINKLE "\x6e\x30"    SPARKLES BY BLINKING THE LEDS
# SPARKLE "\x6e\x31"    FILLS IS EACH LED ON E AT A TIME
# SNOW "\x6e\x32"       JUST WHAT YOU WOULD THINK
# INTERLOCK "\x6e\x33"  DOESNT WORK USE " \x6e\x36" INSTEAD   slides in from right and left!!    DONT FORGET THE SPACE AT BEGGINING
# SWITCH "\x6e\0x34"    DOESNT WORK USE " \x6e\x34" INSTEAD  DONT FORGET THE SPACE AT BEGGINING
# SLIDE "\x63\0x35"    DOESNT WORK USE " \x6e\x35" INSTEAD   DONT FORGET THE SPACE AT BEGGINING
# SPRAY "\x63\0x36"    DOESNT WORK USE " \x6e\x36" INSTEAD   slides in from right!!    DONT FORGET THE SPACE AT BEGGINING
# STARBURST "\x63\0x37" DOESNT WORK USE " \x6e\x37" INSTEAD  DONT FORGET THE SPACE AT BEGGINING
# WELCOME "\x63\0x38"  DOESNT WORK USE " \x6e\x38" INSTEAD   DONT FORGET THE SPACE AT BEGGINING
# SLOT_MACHINE "\x63\0x39" DOESNT WORK USE " \x6e\x39" INSTEAD   DONT FORGET THE SPACE AT BEGGINING

