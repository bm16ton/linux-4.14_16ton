#!/bin/bash

#    ST2205U: interactively patch firmware
#    Copyright (C) 2008 Jeroen Domburg <jeroen@spritesmods.com>
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.


echo "Interactive script to hack the firmware of your keychain photo"
echo "player."
if [ ! -e "$1" ]; then
    echo "Usage: $0 /dev/sdX [oldfirmware.bin]"
    echo "where /dev/sdX is the path to the device your photo-unit is connected to."
    echo "Optional: give a backup of the original firmware to skip reading out the"
    echo "current firmware image."
    echo "Make sure the device is in upload-mode!"
    exit 0;
fi

if [ ! -e "phack" -o ! -e "splice" -o ! -e "bgrep" ]; then
    echo "Please run 'make' first to compile the tools."
    exit 0;
fi

if ! ./phack -m "baks r ok" $1; then
    echo "Sorry, there doesn't seem to be a device using the ST2205U chipset"
    echo "at $1."
    exit 1;
fi

if [ -z "$2" ]; then
    echo
    echo "Ok, first off all, we're going to backup the firmware and memory of your"
    echo "device to fwimage.bak and memimage.bak. Please save fwimage.bak, you"
    echo "need it to flash a newer version into your unit."
    if [ -e fwimage.bak ]; then
	echo "Found existing fwimage.bak, moving to fwimage.bak.old";
	mv fwimage.bak fwimage.bak.old
    fi
    ./phack -df fwimage.bak $1  > /dev/null || exit 1 
    ./phack -d memimage.bak $1  > /dev/null || exit 1

    echo "Making a working copy..."
    cp fwimage.bak fwimage.bin
else 
    echo "Making a working copy..."
    cp $2 fwimage.bin || exit 1
fi


match=false;
echo "Looking for a known device profile..."
for x in hack/m_*; do
    echo "$x ..."
    em=`cat $x/spec | grep '^EMPTY_AT' | cut -d '$' -f 2 | tr 'A-Z' 'a-z'`
    pa=`cat $x/spec | grep '^PATCH_AT' | cut -d '$' -f 2 | tr 'A-Z' 'a-z'`
    if ./bgrep fwimage.bin $x/lookforme.bin -h | grep -q $pa; then
	if ./bgrep fwimage.bin hack/empty.bin -h | grep -q $em; then
	    echo "We have a match!"
	    match=true
	    break;
	fi
    fi
    echo "...nope."
done
if [ $match = false ]; then
    echo "Sorry, I couldn't find a matching device profile. If you want to give "
    echo "creating it yourself a shot, please read ./hack/newhack.txt for more"
    echo "info."
    echo "(Btw: this can also mean your device already has a hacked firmware. If"
    echo "you want to upgrade your device using this script, please flash back"
    echo "the fwimage.bak the previous version saved first.)"
    exit 1
fi

patchdir=$x;
echo "Requirements OK, we can try to hack the device. Proceed? (yes/no)"
./phack -m "Yay! \\o/" $1 > /dev/null
read yn
if ! [ "$yn" = "yes" ]; then
    echo "No 'yes' received. OK, bailing."
    ./phack -m "Kbyetnx." $1 > /dev/null
    exit 0;
fi
echo "Patching fw..."
echo $off
./splice fwimage.bin $x/hack_jmp.bin 0x$pa >/dev/null || exit 1
./splice fwimage.bin $x/hack.bin 0x$em >/dev/null || exit 1
echo "Uploading fw"
./phack -m "Eeeek!" $1 > /dev/null
./phack -uf fwimage.bin $1
echo
echo
echo "All done. To test, disconnect the device and when it has rebooted, connect"
echo "it again, go into 'update mode' and press enter. To quit, use ctrl-c."
read
echo "Ok, just a sec..."
sleep 5
LD_LIBRARY_PATH=./libst2205 ./setpic/setpic $1 test.png
