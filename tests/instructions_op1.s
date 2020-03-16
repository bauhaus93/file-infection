[bits 32]
push es
pop es
push ss
pop ss
push cs
push ds
pop ds
daa
das
aaa
aas
inc eax
inc edi
dec eax
dec edi
push eax
push edi
pop eax
pop edi
pushad
popad
nop
xchg ecx, eax
xchg edi, eax
cwde
cdq
fwait
wait
pushf
popf
sahf
lahf
movsb
movsd
cmpsb
cmpsd
stosb
stosd
lodsb
lodsd
scasb
scasd
ret
leave
int3
into
iret
xlat
insb
insd
outsb
outsd
int1
hlt
cmc
clc
stc
cli
sti
cld
std
