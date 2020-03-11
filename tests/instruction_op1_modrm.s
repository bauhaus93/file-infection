[bits 32]
add eax, ebx
add al, bl
adc eax, ebx
adc al, bl
and eax, ebx
and al, bl
xor eax, ebx
xor al, bl
or eax, ebx
or al, bl
sbb eax, ebx
sbb al, bl
sub eax, ebx
sub al, bl
cmp eax, ebx
cmp al, bl
bound eax, [ebx]
arpl ax, bx
test eax, ebx
test al, bl
xchg ebx, edx
xchg bl, dl
mov ebx, edx
mov bl, dl
mov ebx, ds
lea eax, [ebx]
mov ds, bx
pop dword [ebx]
les eax, [ebx]
lds eax, [ebx]
rol bl, 1
rol ebx, 1
ror bl, 1
ror ebx, 1
rcl bl, 1
rcl ebx, 1
rcr ebx, 1
rcr bl, 1
shl ebx, 1
shl bl, 1
shr ebx, 1
shr bl, 1
sar bl, 1
rol bl, cl
rol ebx, cl
ror bl, cl
ror ebx, cl
rcl bl, cl
rcl ebx, cl
rcr ebx, cl
rcr bl, cl
shl ebx, cl
shl bl, cl
shr ebx, cl
shr bl, cl
sar bl, cl
not eax
not al
neg eax
neg al
mul ebx
mul bl
mul dword [ebx]
mul byte [ebx]

