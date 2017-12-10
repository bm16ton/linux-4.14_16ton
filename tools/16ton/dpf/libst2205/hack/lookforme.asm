    CPU 65c02
    OUTPUT HEX
    INCLUDE spec
    * = PATCH_AT+$4000

;Look for this bit to patch.
    LDA #$FF
    LDX #$FF
    STA CMP_VAR1
    STX CMP_VAR2
    RTS
