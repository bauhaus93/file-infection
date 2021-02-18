;0x00-0x0F
add [ebx + 4 * edx], cl
add [ebx + 4 * edx], ecx
add cl, [ebx + 4 * edx]
add ecx, [ebx + 4 * edx]

or [ebx + 4 * edx], cl
or [ebx + 4 * edx], ecx
or cl, [ebx + 4 * edx]
or ecx, [ebx + 4 * edx]

;0x10-0x1F
adc [ebx + 4 * edx], cl
adc [ebx + 4 * edx], ecx
adc cl, [ebx + 4 * edx]
adc ecx, [ebx + 4 * edx]

sbb [ebx + 4 * edx], cl
sbb [ebx + 4 * edx], ecx
sbb cl, [ebx + 4 * edx]
sbb ecx, [ebx + 4 * edx]

;0x20-0x2F
and [ebx + 4 * edx], cl
and [ebx + 4 * edx], ecx
and cl, [ebx + 4 * edx]
and ecx, [ebx + 4 * edx]

sub [ebx + 4 * edx], cl
sub [ebx + 4 * edx], ecx
sub cl, [ebx + 4 * edx]
sub ecx, [ebx + 4 * edx]

;0x30-0x3F
xor [ebx + 4 * edx], cl
xor [ebx + 4 * edx], ecx
xor cl, [ebx + 4 * edx]
xor ecx, [ebx + 4 * edx]

cmp [ebx + 4 * edx], cl
cmp [ebx + 4 * edx], ecx
cmp cl, [ebx + 4 * edx]
cmp ecx, [ebx + 4 * edx]

;0x60-0x6F
bound eax, [eax + 4 * ebx]
arpl [eax + 4 * ebx], cx
imul ebx, [eax + 4 * esi], byte 0xFF

;0x80-0x8F
;group 1
add byte [ebx + 2 * edx], 0xAB
or byte [ebx + 2 * edx], 0xAB
adc byte [ebx + 2 * edx], 0xAB
sbb byte [ebx + 2 * edx], 0xAB
and byte [ebx + 2 * edx], 0xAB
sub byte [ebx + 2 * edx], 0xAB
xor byte [ebx + 2 * edx], 0xAB
cmp byte [ebx + 2 * edx], 0xAB

add dword [ebx + 4 * edx], 0xAB
or dword [ebx + 4 * edx], 0xAB
adc dword [ebx + 4 * edx], 0xAB
sbb dword [ebx + 4 * edx], 0xAB
and dword [ebx + 4 * edx], 0xAB
sub dword [ebx + 4 * edx], 0xAB
xor dword [ebx + 4 * edx], 0xAB
cmp dword [ebx + 4 * edx], 0xAB
;end group 1

test [ebx + 4 * edx], dl
test [ebx + 4 * edx], edx
xchg [ebx + 4 * edx], dl
xchg [ebx + 4 * edx], edx

mov [ebx + 4 * eax], dl
mov [ebx + 4 * eax], edx
mov dl, [ebx + 4 * eax]
mov edx, [ebx + 4 * eax]
mov [edx + 4 * eax], ds
lea edx, [ebx + 4 * eax]
mov ds, [edx + 4 * eax]

;group 1a
pop dword [eax + 4 * ebx]

;0xC0-0xCF
;group 2
rol byte [ebx + 4 * eax], 0x0A
ror byte [ebx + 4 * eax], 0x0A
rcl byte [ebx + 4 * eax], 0x0A
rcr byte [ebx + 4 * eax], 0x0A
shl byte [ebx + 4 * eax], 0x0A
shr byte [ebx + 4 * eax], 0x0A
sar byte [ebx + 4 * eax], 0x0A

rol byte [ebx + 4 * eax], 0x01
ror byte [ebx + 4 * eax], 0x01
rcl byte [ebx + 4 * eax], 0x01
rcr byte [ebx + 4 * eax], 0x01
shl byte [ebx + 4 * eax], 0x01
shr byte [ebx + 4 * eax], 0x01
sar byte [ebx + 4 * eax], 0x01
;end group 2

;0xF0-0xFF
;group 3
test byte [eax + 4 * edx], 0xFF
not byte [eax + 4 * edx]
neg byte [eax + 4 * edx]
mul byte [eax + 4 * edx]
imul byte [eax + 4 * edx]
div byte [eax + 4 * edx]
idiv byte [eax + 4 * edx]

test dword [eax + 4 * edx], 0xFFFFFFFF
not dword [eax + 4 * edx]
neg dword [eax + 4 * edx]
mul dword [eax + 4 * edx]
imul dword [eax + 4 * edx]
div dword [eax + 4 * edx]
idiv dword [eax + 4 * edx]

;group 4
inc byte [eax + 4 * ecx]
dec byte [eax + 4 * ecx]


;group 5
inc dword [eax + 4 * ecx]
dec dword [eax + 4 * ecx]
call near [eax + 4 * ecx]
call far [eax + 4 * ecx]
jmp near [eax + 4 * ecx]
jmp far [eax + 4 * ecx]
push dword [eax + 4 * ecx]
