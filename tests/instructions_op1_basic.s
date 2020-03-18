;0x00-0x07
add bl, cl
add ebx, ecx
add bl, [eax]
add ebx, [eax]
add al, 0xAB
add eax, 0xABCDABCD
push es
pop es
;0x08-0x0F
or bl, cl
or ebx, ecx
or bl, [eax]
or ebx, [eax]
or al, 0xAB
or eax, 0xABCDABCD
push cs
;0x10-0x17
adc bl, cl
adc ebx, ecx
adc bl, [eax]
adc ebx, [eax]
adc al, 0xAB
adc eax, 0xABCDABCD
push ss
pop ss
;0x18-0x1F
sbb bl, cl
sbb ebx, ecx
sbb bl, [eax]
sbb ebx, [eax]
sbb al, 0xAB
sbb eax, 0xABCDABCD
push ds
pop ds
;0x20-0x27
and bl, cl
and ebx, ecx
and bl, [eax]
and ebx, [eax]
and al, 0xAB
and eax, 0xABCDABCD
daa
;0x28-0x2F
sub bl, cl
sub ebx, ecx
sub bl, [eax]
sub ebx, [eax]
sub al, 0xAB
sub eax, 0xABCDABCD
das
;0x30-0x37
xor bl, cl
xor ebx, ecx
xor bl, [eax]
xor ebx, [eax]
xor al, 0xAB
xor eax, 0xABCDABCD
aaa
;0x38-0x3F
cmp bl, cl
cmp ebx, ecx
cmp bl, [eax]
cmp ebx, [eax]
cmp al, 0xAB
cmp eax, 0xABCDABCD
aas
;0x40-0x47
inc eax
inc ecx
inc edx
inc edx
inc esp
inc ebp
inc esi
inc edi
;0x48-0x4F
dec eax
dec ecx
dec edx
dec edx
dec esp
dec ebp
dec esi
dec edi
;0x50-0x57
push eax
push ecx
push edx
push edx
push esp
push ebp
push esi
inc edi
;0x58-0x5F
pop eax
pop ecx
pop edx
pop edx
pop esp
pop ebp
pop esi
pop edi
;0x60-0x67
pusha
popa
bound eax, [ebx]
arpl [ebx], cx
;0x68-0x6F
push dword 0xABCDABCD
imul ebx, edx, 0xABCDABCD
push byte 0xFF
imul ebx, edx, byte 0xFF
insb
insd
outsb
outsd
;0x70-0x76
jo short 0x12
jno short 0x12
jb short 0x12
jnb short 0x12
je short 0x12
jne short 0x12
jbe short 0x12
jnbe short 0x12
;0x78-0x7F
js short 0x12
jns short 0x12
jp short 0x12
jnp short 0x12
jl short 0x12
jge short 0x12
jle short 0x12
jg short 0x12
;0x80-0x87
test [ebx], dl
test [ebx], edx
xchg [ebx], dl
xchg [ebx], edx
;0x88-0x8F
mov [ebx], dl
mov [ebx], edx
mov dl, [ebx]
mov edx, [ebx]
mov [edx], ds
lea edx, [ebx]
mov ds, [edx]
;0x90-0x97
nop
xchg eax, ecx
xchg eax, edx
xchg eax, ebx
xchg eax, esp
xchg eax, ebp
xchg eax, esi
xchg eax, edi
;0x98-0x9F
cwde
cdq
call 0xABCD:0xABCDABCD
fwait
pushf
popf
sahf
lahf
;0xA0-0xA7
mov al, [eax]
mov eax, [eax]
mov [eax], al
mov [eax], eax
movsb
movsd
cmpsb
cmpsd
;0xA8-0xAF
test al, 0xFF
test eax, 0xABCDABCD
stosb
stosd
lodsb
lodsd
scasb
scasd
;0xB8-0xBF
mov eax, 0xABCDABCD
mov eax, 0xABCDABCD
mov ecx, 0xABCDABCD
mov edx, 0xABCDABCD
mov ebx, 0xABCDABCD
mov esp, 0xABCDABCD
mov ebp, 0xABCDABCD
mov esi, 0xABCDABCD
mov edi, 0xABCDABCD
;0xC0-0xC7
retn word 0xABCD
retn
les eax, [edx]
lds eax, [edx]
;0xC8-0xCF
enter 0xFF, 0xABCDABCD
leave
retf word 0xABCD
retf
int3
int 0x0A
into
iretd
;0xD0-0xD7
aam byte 0xFF
aad byte 0xFF
xlatb
;0xE0-0xE7
loopne 0xAB
loope 0xAB
loop 0xAB
jecxz 0xAB
in al, 0xAB
in eax, 0xAB
out 0xAB, al
out 0xAB, eax
;0xE8-0xEF
call 0xABCDABCD
jmp near 0xABCDABCD
jmp 0xABCD:0xABCDABCD
jmp short 0xAB
in al, dx
in eax, dx
out dx, al
out dx, eax
;0xF0-0xF7
int1
hlt
cmc
clc
stc
cli
sti
cld
std
