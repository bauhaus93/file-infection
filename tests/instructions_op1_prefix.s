;Group 1

;LOCK
lock add [eax], eax
lock add [0xABCDABCD], eax
lock add [eax + 4 * ebx], eax
lock add [0xABCDABCD + 4 * ebx], eax
lock adc [eax], eax
lock and [eax], eax
lock or [eax], eax
lock xor [eax], eax
lock xchg [eax], eax
lock sbb [eax], eax
lock dec dword [eax]
lock neg dword [eax]
lock not dword [eax]

;REPNE
repne insb
repne insd
repne movsb
repne movsd
repne outsb
repne outsd
repne movsb
repne movsd
repne stosb
repne stosd
repne lodsb
repne lodsd
repne scasb
repne scasd

;REPE
repe insb
repe insd
repe movsb
repe movsd
repe outsb
repe outsd
repe movsb
repe movsd
repe stosb
repe stosd
repe lodsb
repe lodsd
repe scasb
repe scasd


;BND
bnd call 0xABCDABCD
bnd call near [ebx]
bnd jmp near [ebx]
bnd jmp near 0xABCDABCD
bnd jo short 0x12
bnd jno short 0x12
bnd jb short 0x12
bnd jnb short 0x12
bnd je short 0x12
bnd jne short 0x12
bnd jbe short 0x12
bnd jnbe short 0x12
bnd js short 0x12
bnd jns short 0x12
bnd jp short 0x12
bnd jnp short 0x12
bnd jl short 0x12
bnd jge short 0x12
bnd jle short 0x12
bnd jg short 0x12
bnd retn word 0xABCD
bnd retn

;Group 2

;Segment overrides
add byte [cs:ebx], 0xAB
or byte [ss:ebx], 0xAB
adc byte [ds:ebx], 0xAB
sbb byte [es:ebx], 0xAB
and byte [fs:ebx], 0xAB
sub byte [gs:ebx], 0xAB
xor byte [cs:ebx + 4 * edx], 0xAB
cmp byte [ds:0xABCDABCD + 8 * esi], 0xAB
add dword [es:0xABCDABCD], 0xAB

;Group 3

;Operand size override
add word [ebx], 0xABCD
or word [ebx], 0xABCD
adc word [ebx], 0xABCD
sbb word [ebx], 0xABCD
and word [ebx], 0xABCD
sub word [ebx], 0xABCD
xor word [ebx], 0xABCD
cmp word [ebx], 0xABCD

add bx, 0xABCD
or bx, 0xABCD
adc bx, 0xABCD
sbb bx, 0xABCD
and bx, 0xABCD
sub bx, 0xABCD
xor bx, 0xABCD
cmp bx, 0xABCD

rol bx, 0x0A
ror bx, 0x0A
rcl bx, 0x0A
rcr bx, 0x0A
shl bx, 0x0A
shr bx, 0x0A
sar bx, 0x0A

out 0xABCD, ax
pop ax
xor si, di
cmp ax, 0xABCD
imul bx, dx, 0xFFFF
xchg ax, dx
movsw
cmpsw
stosw
lodsw
scasw

;Group 4
;Address size override
