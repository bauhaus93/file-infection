add bl, [0xABCDABCD]
add ebx, [0xABCDABCD]
or bl, [0xABCDABCD]
or ebx, [0xABCDABCD]
adc bl, [0xABCDABCD]
adc ebx, [0xABCDABCD]
sbb bl, [0xABCDABCD]
sbb ebx, [0xABCDABCD]
and bl, [0xABCDABCD]
and ebx, [0xABCDABCD]
sub bl, [0xABCDABCD]
sub ebx, [0xABCDABCD]
xor bl, [0xABCDABCD]
xor ebx, [0xABCDABCD]
cmp bl, [0xABCDABCD]
cmp ebx, [0xABCDABCD]
bound eax, [0xABCDABCD]
arpl [0xABCDABCD], cx
test [0xABCDABCD], dl
test [0xABCDABCD], edx
xchg [0xABCDABCD], dl
xchg [0xABCDABCD], edx
mov [0xABCDABCD], dl
mov [0xABCDABCD], edx
mov dl, [0xABCDABCD]
mov edx, [0xABCDABCD]
lea edx, [0xABCDABCD]
mov al, [0xABCDABCD]
mov eax, [0xABCDABCD]
mov [0xABCDABCD], al
mov [0xABCDABCD], eax
les eax, [0xABCDABCD]
lds eax, [0xABCDABCD]

;groups 
add byte [0xABCDABCD], 0xAB
or byte [0xABCDABCD], 0xAB
adc byte [0xABCDABCD], 0xAB
sbb byte [0xABCDABCD], 0xAB
and byte [0xABCDABCD], 0xAB
sub byte [0xABCDABCD], 0xAB
xor byte [0xABCDABCD], 0xAB
cmp byte [0xABCDABCD], 0xAB

add dword [0xABCDABCD], 0xAB
or dword [0xABCDABCD], 0xAB
adc dword [0xABCDABCD], 0xAB
sbb dword [0xABCDABCD], 0xAB
and dword [0xABCDABCD], 0xAB
sub dword [0xABCDABCD], 0xAB
xor dword [0xABCDABCD], 0xAB
cmp dword [0xABCDABCD], 0xAB

inc byte [0xABCDABCD]
dec byte [0xABCDABCD]


;group 5 / 0xFF
inc dword [0xABCDABCD]
dec dword [0xABCDABCD]
call near [0xABCDABCD]
call far [0xABCDABCD]
jmp near [0xABCDABCD]
jmp far [0xABCDABCD]
push dword [0xABCDABCD]

