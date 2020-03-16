;group 1 / 0x80-0x83
add byte [ebx], 0xAB
or byte [ebx], 0xAB
adc byte [ebx], 0xAB
sbb byte [ebx], 0xAB
and byte [ebx], 0xAB
sub byte [ebx], 0xAB
xor byte [ebx], 0xAB
cmp byte [ebx], 0xAB

add dword [ebx], 0xAB
or dword [ebx], 0xAB
adc dword [ebx], 0xAB
sbb dword [ebx], 0xAB
and dword [ebx], 0xAB
sub dword [ebx], 0xAB
xor dword [ebx], 0xAB
cmp dword [ebx], 0xAB

add ebx, byte 0xAB
or ebx, byte 0xAB
adc ebx, byte 0xAB
sbb ebx, byte 0xAB
and ebx, byte 0xAB
sub ebx, byte 0xAB
xor ebx, byte 0xAB
cmp ebx, byte 0xAB

;group 1a / 0x8F
pop dword [ebx]

;group 2 / 0xC0-0xC1 / 0xD0-0xD3
rol bl, 0x0A
ror bl, 0x0A
rcl bl, 0x0A
rcr bl, 0x0A
shl bl, 0x0A
shr bl, 0x0A
sar bl, 0x0A

rol ebx, 0x0A
ror ebx, 0x0A
rcl ebx, 0x0A
rcr ebx, 0x0A
shl ebx, 0x0A
shr ebx, 0x0A
sar ebx, 0x0A

rol bl, 0x1
ror bl, 0x1
rcl bl, 0x1
rcr bl, 0x1
shl bl, 0x1
shr bl, 0x1
sar bl, 0x1

rol ebx, 0x1
ror ebx, 0x1
rcl ebx, 0x1
rcr ebx, 0x1
shl ebx, 0x1
shr ebx, 0x1
sar ebx, 0x1

rol bl, cl
ror bl, cl
rcl bl, cl
rcr bl, cl
shl bl, cl
shr bl, cl
sar bl, cl

rol ebx, cl
ror ebx, cl
rcl ebx, cl
rcr ebx, cl
shl ebx, cl
shr ebx, cl
sar ebx, cl

;group 3 / 0xF6-0xF7
test bl, 0xFF
not bl
neg bl
mul bl
imul bl
div bl
idiv bl

test ebx, 0xFF
not ebx
neg ebx
mul ebx
imul ebx
div ebx
idiv ebx

;group 4 / 0xFE
inc byte [ebx]
dec byte [ebx]


;group 5 / 0xFF
inc dword [ebx]
dec dword [ebx]
call near [ebx]
call far [ebx]
jmp near [ebx]
jmp far [ebx]
push dword [ebx]
