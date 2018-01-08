/*
   betabrite.h - common declarations for betabrite main program
 
   Copyright (C) 2002 boB Rudis, bob@rudis.net
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA
*/

/* these #defines make it easer to deal with the codes used to 'program' the
   display. The PDF document mentioned in the README provides much more detail
   on the protocol itself. */

/* COMMAND CODES */

#define SOH "\x01"
#define STX "\x02"
#define ETX "\x03"
#define EOT "\x04"
#define ESCAPE "\x1B"

/* ATTRIBUTES */

#define DOUBLE_HEIGHT_OFF "\x05\x30"
#define DOUBLE_HEIGHT_ON "\x05\x31"
#define TRUE_DESCENDERS_OFF "\x06\x30"
#define TRUE_DESCENDERS_ON "\x06\x31"
#define WIDE_CHARS_OFF "\x11"
#define WIDE_CHARS_ON "\x12"

#define CALL_DATE_0 "\x0B\x30"
#define CALL_DATE_1 "\x0B\x31"
#define CALL_DATE_2 "\x0B\x32"
#define CALL_DATE_3 "\x0B\x33"
#define CALL_DATE_4 "\x0B\x34"
#define CALL_DATE_5 "\x0B\x35"
#define CALL_DATE_6 "\x0B\x36"
#define CALL_DATE_7 "\x0B\x37"
#define CALL_DATE_8 "\x0B\x38"
#define CALL_DATE_9 "\x0B\x39"
#define CALL_TIME "\x13"

#define NEW_PAGE "\x0C"
#define NEW_LINE "\x0D"

#define CALL_STRING_X "\x10"
#define CALL_DOTS_PICTURE_X "\x14"

#define NO_HOLD_SPEED "\x09"
#define SPEED_SLOWEST "\x15"
#define SPEED_LOW "\x16"
#define SPEED_MEDIUM "\x17"
#define SPEED_HIGH "\x18"
#define SPEED_FASTEST "\x19"

#define COLOR_RED "\x1C\x31"
#define COLOR_GREEN "\x1C\x32"
#define COLOR_AMBER "\x1C\x33"
#define COLOR_DIMRED "\x1C\x34"
#define COLOR_DIMGREEN "\x1C\x35"
#define COLOR_BROWN "\x1C\x36"
#define COLOR_ORANGE "\x1C\x37"
#define COLOR_YELLOW "\x1C\x38"
#define COLOR_RAINBOW1 "\x1C\x39"
#define COLOR_RAINBOW2 "\x1C\x41"
#define COLOR_MIX "\x1C\x42"
#define COLOR_AUTO "\x1C\x43"

#define WIDTH_PROPORTIONAL "\x1E\x30"
#define WIDTH_FIXEX "\x1E\x31"

#define TEMPERATURE_CELSIUS "\x08\x1C"
#define TEMPERATURE_FARENHEIT "\x08\x1D"

#define CALL_COUNTER_1 "\x08\x7A"
#define CALL_COUNTER_2 "\x08\x7B"
#define CALL_COUNTER_3 "\x08\x7C"
#define CALL_COUNTER_4 "\x08\x7D"
#define CALL_COUNTER_5 "\x08\x7E"

/* POSITIONS */

#define MIDDLE "\x20"
#define TOP "\x22"
#define BOTTOM "\x26"
#define FILL "\x30"

/* MODES */

#define ROTATE "\x61"
#define HOLD "\x62"
#define FLASH "\x63"
#define ROLL_UP "\x65"
#define ROLL_DOWN "\x66"
#define ROLL_LEFT "\x67"
#define ROLL_RIGHT "\x68"
#define WIPE_UP "\x69"
#define WIPE_DOWN "\x6a"
#define WIPE_LEFT "\x6b"
#define WIPE_RIGHT "\x6c"
#define SCROLL "\x6d"
#define AUTOMODE "\x6f"
#define ROLL_IN "\x70"
#define ROLL_OUT "\x71"
#define WIPE_IN "\x72"
#define WIPE_OUT "\x73"
#define COMPRESSED_ROTATE "\x74"
#define TWINKLE "\x6e\x30"
#define SPARKLE "\x6e\x31"
#define SNOW "\x6e\x32"
#define INTERLOCK "\x6e\x33"
#define SWITCH "\x6e\0x34"
#define SLIDE "\x63\0x35"
#define SPRAY "\x63\0x36"
#define STARBURST "\x63\0x37"
#define WELCOME "\x63\0x38"
#define SLOT_MACHINE "\x63\0x39"
