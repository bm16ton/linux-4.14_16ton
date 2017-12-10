#!/bin/bash
#<<So you've got an unsupported ST2205U-device and want to create support for it?
#<<I hope you understand 65C02 assembly, or at least are willing to learn a little
#<<of it. Disclaimer: if you end up with a bricked device, I'm not responsible!
#<<This is what's needed:
#<<

MODEL="$1"
DEVICE="$2"

usage() {
	echo "Usage:" 1>&2
	echo "$0 model_name /dev/sdX" 1>&2
}

if [[ -z "$2" ]]; then
	usage
	exit -1
fi

if [[ ! -b $DEVICE ]]; then
	echo "Device $DEVICE is not a block device" 1>&2
	exit -1
fi

if [[ -d m_$MODEL ]]; then
	echo "Module $MODEL already exists" 1>&2
	exit -1
fi
mkdir m_$MODEL
cd m_$MODEL

#<<- Dump the firmware.
#<<./phack -df fwimage.bin /dev/sda
../../phack -df fwimage.bin "$DEVICE"

#<<- Break up the firmware in segments. Each segment is 16K:
#<<for x in `seq 0 3`; do dd if=fwimage.bin of=fwimage-seg$x.bin bs=16384 skip=$x count=1; done
for x in $(seq 0 3); do
	dd if=fwimage.bin bs=16384 skip=$x count=1 | hexdump -ve '1/1 "%02x  "' > fwimage-seg$x.hex
done

#<<- Disassemble segment 1 using a 68c02 disassembler. You can use the symbols.def
#<<  file in this directory. Segment should be loaded at address 0x4000.
## No: Using opcodes

#<<- Open up the disassembly file and look for a shitload of consecutive writes to
#<<  the addresses 0x8000 and 0xc000. If these are there, the firmware uses an
#<<  external LCD-controller. If you can't find these writes anywhere (not even
#<<  in the other 3 segments), it uses the internal one and you'll have to figure 
#<<  out the code to modify my assembly yourself.
for x in `seq 0 3`; do
	## (STA $[C0|80]00 LDA #$XX){4}
	if grep -qE "(8d  00  (c0|80)  a9  ..  ){4}" fwimage-seg$x.hex; then
		EXT_LCD=yes
		break
	fi
done
if [[ -z "$EXT_LCD" ]]; then
	echo "Internal LCD-controller no supported" 1>&2
	exit -1
fi

#<<- Copy hack/m_coby_dp151 to hack/m_[the name of your device]. For the patch to
#<<  work, you'll need to create a correct spec-file. Use the existing one as
#<<  a template.
cp ../m_coby_dp151/spec .

#<<- Disassemble segment 0 using a 68c02 disassembler. You can use the symbols.def
#<<  file in this directory. Segment should be loaded at address 0x4000.
## No: Using opcodes

#<<- Open the disassembly file and look for something like this:
#<<LDA     D037c
#<<CMP     #$31
#<<BNE     L73c1
#<<LDA     D037d
#<<CMP     #$00
#<<BNE     L73c1
#<<BRA     L73d3
#<<  The important thing here is that two subsequent memory locations (here 
#<<  037C:037D) get compared to 0x3100,  and later (not shown) to 0x3300.
#<<  Modify the CMP_VAR1 and CMP_VAR2 in the specs-file to reflect these
#<<  memory locations.
## LDA $XXXX CMP #$31 BNE $XXXX
## LDA $XXXX CMP #$00 BNE $XXXX
## BRA $XXXX
## LDA $XXXX CMP #$33 BNE $XXXX
## LDA $XXXX CMP #$00 BNE $XXXX
CMP_VAR=$(grep -oE "\
ad  ..  ..  c9  31  d0  ..  \
ad  ..  ..  c9  00  d0  ..  \
80  ..  \
ad  ..  ..  c9  33  d0  ..  \
ad  ..  ..  c9  00  d0  ..  " fwimage-seg0.hex | head -n 1)
if [[ -z "$CMP_VAR" ]]; then
	echo "No CMP_VAR found" 1>&2
	exit -1
fi
CMP_VAR1=$(echo "$CMP_VAR" | awk '{print $3$2}')
CMP_VAR1_OPCODE=$(echo "$CMP_VAR" | awk '{print $2 "  " $3 "  "}')
CMP_VAR2=$(echo "$CMP_VAR" | awk '{print $10$9}')
CMP_VAR2_OPCODE=$(echo "$CMP_VAR" | awk '{print $9 "  " $10 "  "}')
CMP_VAR_DIFF=$(( 0x$CMP_VAR2 - 0x$CMP_VAR1 ))
if [[ "$CMP_VAR_DIFF" -ne 1 ]]; then
	echo "No CMP_VARs differs 1" 1>&2
	exit -1
fi
echo CMP_VAR = $CMP_VAR1:$CMP_VAR2 $CMP_VAR_DIFF

#<<- A few lines later, you should see something like this:
#<<LDA     #$ff
#<<LDX     #$ff
#<<STA     D037c
#<<STX     D037d
#<<RTS
#<<  The location of the LDA-instruction, minus 0x4000, should go into the 
#<<  specs-file as PATCH_AT.

find_first_4_hex() {
	#Pattern: $1
	#File: $2
	OFFSET4=$(grep -boE "$1" "$2" | head -n 1 | cut -d: -f1)
	if [[ -z "$OFFSET4" ]]; then
		return
	fi
	OFFSET=$(( OFFSET4 / 4 ))
	OFFSET_HEX=$(printf "%04x" $OFFSET)
	echo $OFFSET_HEX
}

## LDA #$ff LDX #$ff
## STA $038a STX $038b
## RTS
PATCH_AT=$(find_first_4_hex "\
a9  ff  a2  ff  \
8d  ${CMP_VAR1_OPCODE}8e  ${CMP_VAR2_OPCODE}\
60  \
" fwimage-seg0.hex)
if [[ -z "$PATCH_AT" ]]; then
	echo "No PATCH_AT found" 1>&2
	exit -1
fi
echo PATCH_AT = $PATCH_AT

#<<- Locate some free space. We require a location in the first segment that has
#<<  about 240 consecutive FFs in it. Load up fwimage-seg0.bin in your favourite 
#<<  hexeditor and  go look for a location like that. Hint: there's usually one 
#<<  at the end of the file. This should go into the specs-file as EMPTY_AT.
#<<  If the space is very small (<500 bytes), please make a note of it in the
#<<  spec-file.
EMPTY_AT_BIG=$(find_first_4_hex "(ff  ){500}" fwimage-seg0.hex)
EMPTY_AT_SMALL=$(find_first_4_hex "(ff  ){245}" fwimage-seg0.hex)
if [[ -z "$EMPTY_AT_SMALL" ]]; then
	echo "No free space found" 1>&2
	exit -1
fi
if [[ -z "$EMPTY_AT_BIG" ]]; then
	EMPTY_AT=$EMPTY_AT_SMALL
	EMPTY_AT_IS_SMALL=yes
else
	EMPTY_AT=$EMPTY_AT_BIG
fi
echo EMPTY_AT = $EMPTY_AT small:${EMPTY_AT_IS_SMALL-no}

#<<- Locate a routine starting with
#<<PHA
#<<LDA     #$55    ; 85 U                                  
#<<STA     D0240                                           
#<<LDA     #$53    ; 83 S                                  
#<<STA     D0241                                           
#<<LDA     #$42    ; 66 B                                  
#<<STA     D0242                                           
#<<LDA     #$53    ; 83 S         
#<<  Make a note of the location of the start of the routine and enter this in the
#<<  specs-file, minus 0x4000, as SEND_CSW.
## PHA
## LDA #$55 STA $0240
## LDA #$53 STA $0241
## LDA #$42 STA $0242
## LDA #$53
SEND_CSW=$(find_first_4_hex "\
48  \
a9  55  8d  40  02  \
a9  53  8d  41  02  \
a9  42  8d  42  02  \
a9  53  \
" fwimage-seg0.hex)
if [[ -z "$SEND_CSW" ]]; then
	echo "No SEND_CSW found" 1>&2
	exit -1
fi
echo SEND_CSW = $SEND_CSW

#<<- Locate a piece of code looking like this. (The refs to adresses 2xx will be
#<<  the same, the ones to 37x may differ):
#<<STZ     D0370
#<<STZ     D0371
#<<LDA     D0216
#<<STZ     D0370
#<<LDA     D0217
#<<STA     D036f
#<<ASL     D036f
#<<ROL     D0370
#<<ROL     D0371
#<<- Fill LEN1 to LEN3 with the ASL/ROLled adresses, fill LEN0 with LEN1-1. In this
#<<  example, LEN0=$36E, LEN1=$36F, LEN2=$370, LEN3=$371
## STZ $03XX
## STZ $03XX
## LDA $0216
## STZ $03XX
## LDA $0217
## STA $03XX
## ASL $03XX
## ROL $03XX
## ROL $03XX
LEN=$(grep -oE "\
9c  ..  03  \
9c  ..  03  \
ad  16  02  \
9c  ..  03  \
ad  17  02  \
8d  ..  03  \
0e  ..  03  \
2e  ..  03  \
2e  ..  03  \
" fwimage-seg0.hex | head -n 1)
if [[ -z "$LEN" ]]; then
	echo "No LEN found" 1>&2
	exit -1
fi
LEN1=$(echo "$LEN" | awk '{print $21$20}')
LEN2=$(echo "$LEN" | awk '{print $24$23}')
LEN3=$(echo "$LEN" | awk '{print $27$26}')
LEN0=$( printf "%04x" $(( 0x$LEN1 - 1 )) )
echo LENs = $LEN0, $LEN1, $LEN2, $LEN3

#<<- Check the kind of controller you have: if you find this in your disassembly
#<<  code (mostly in segment 1), your controller most likely is an PCF8833 and you
#<<  should leave your CTRTYPE at 0:
#<<LDA	#$2a	; 42 *
#<<STA	X8000
#<<(Snip! 2 or 4 stores to C000)
#<<LDA	#$2b	; 43 +
#<<STA	X8000
#<<(Snip! 2 or 4 stores to C000)
#<<  On the other hand, if your code looks more like this:
#<<LDA	#$f4	; 244 .
#<<STA	X8000
#<<(Snip! one aditional store to 8000)
#<<LDA	#$f6	; 244 .
#<<STA	X8000
#<<(Snip! one aditional store to 8000)
#<<LDA	#$f5	; 244 .
#<<STA	X8000
#<<(Snip! one aditional store to 8000)
#<<LDA	#$f7	; 244 .
#<<STA	X8000
#<<(Snip! one aditional store to 8000)
#<<  you've got a UC1697V on your hands and CTRTYPE should be 1.
## LDA #$2a STA $8000 ( LDA #$XX STA $C000 ) {2,4}
## LDA #$2b STA $8000 ( LDA #$XX STA $C000 ) {2,4}
OPCODES_CTRTYPE_0="\
a9  2a  8d  00  80  (a9  ..  8d  00  c0  ){2,4}\
a9  2b  8d  00  80  (a9  ..  8d  00  c0  ){2,4}\
"
## LDA #$f4 STA $8000 LDA #$XX STA $8000
## LDA #$f6 STA $8000 LDA #$XX STA $8000
## LDA #$f5 STA $8000 LDA #$XX STA $8000
## LDA #$f7 STA $8000 LDA #$XX STA $8000
OPCODES_CTRTYPE_1="\
a9  f4  8d  00  80  a9  ..  8d  00  80  \
a9  f6  8d  00  80  a9  ..  8d  00  80  \
a9  f5  8d  00  80  a9  ..  8d  00  80  \
a9  f7  8d  00  80  a9  ..  8d  00  80  \
"
if grep -qE "$OPCODES_CTRTYPE_0" fwimage-seg1.hex; then
	CTRTYPE="0 ;PCF8833"
else
	if grep -qE "$OPCODES_CTRTYPE_1" fwimage-seg1.hex; then
		CTRTYPE="1 ;UC1697V"
	else
		echo "No CTRTYPE deteceted" 1>&2
		exit -1
	fi
fi
echo CTRTYPE = $CTRTYPE

## Process file
sed -e "s/^;Spec-file for .*/;Spec-file for $MODEL/" -i spec
sed -e "s/^CMP_VAR1=.*/CMP_VAR1=\$$CMP_VAR1/" -i spec
sed -e "s/^CMP_VAR2=.*/CMP_VAR2=\$$CMP_VAR2/" -i spec
sed -e "s/^PATCH_AT=.*/PATCH_AT=\$$PATCH_AT/" -i spec
sed -e "s/^EMPTY_AT=.*/EMPTY_AT=\$$EMPTY_AT${EMPTY_AT_IS_SMALL+ ;<500 bytes}/" -i spec
sed -e "s/^SEND_CSW=.*/SEND_CSW=\$$SEND_CSW/" -i spec
sed -e "s/^LEN0=.*/LEN0=\$$LEN0/" -i spec
sed -e "s/^LEN1=.*/LEN1=\$$LEN1/" -i spec
sed -e "s/^LEN2=.*/LEN2=\$$LEN2/" -i spec
sed -e "s/^LEN3=.*/LEN3=\$$LEN3/" -i spec
sed -e "s/^CTRTYPE=.*/CTRTYPE=$CTRTYPE/" -i spec

echo "Follow this instructions refered to m_$MODEL/spec:"
cat <<EOF | sed -e "s/^#<<//"
#<<- Fill out CONF_XRES, CONF_YRES with the horizontal and vertical resolution of
#<<  your device.
#<<  Leave CONF_PROTO at 0, which is the PCF8833-protocol.
#<<- Regenerate the binaries using the 'assembleme'-script. You'll need to have
#<<  crasm installed for this.
#<<- Try the hackfw script. If the script finds your device profile and asks if
#<<  you really want to flash the hacked firmware, !disconnect your device! and
#<<  enter 'yes'.
#<<- The hacked firmware now is in the 'fwimage.bin'-file. Please revise the
#<<  patched areas around PATCH_AT and EMPTY_AT: a mistake here could in theory
#<<  brick your device.
#<<- If you're sure it'll work, you can try the hackfw.sh script for real.
#<<- Rejoice if you see the 'It Works!' image :) 
#<<- If you see a garbled mess or if the image is offset, your OFFX, OFFY or 
#<<  CONF_BPP-settings are wrong. Go to the setpic-dir and run
#<<./setpic /dev/sdX -test test.png
#<<  and fiddle with the keys. As soon as your image is correct, you can copy
#<<  the displayed settings to the spec-file.
#<<- Zip up the m_yourmachine directory and mail it to 
#<<  me <picframe@spritesmods.com>, if you like.
EOF
