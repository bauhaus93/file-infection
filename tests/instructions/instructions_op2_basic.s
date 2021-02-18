;0x00-0x07 noprefix
lar eax, [eax]
lsl eax, [eax]
clts

;0x08-0x0F
invd
wbinvd
ud2
prefetchw [eax]

;0x10-0x17

;prefix=None
movups xmm1, [eax]
movups [eax], xmm1
movlps xmm1, [eax]
movhlps xmm1, xmm2
movlps [eax], xmm1
unpcklps xmm1, [eax]
unpckhps xmm1, [eax]
movhps xmm1, [eax]
movlhps xmm1, xmm2
movhps xmm1, [eax]

;prefix=0x66
movupd xmm1, [eax]
movupd [eax], xmm1
movlpd xmm1, [eax]
movlpd [eax], xmm1
unpcklpd xmm1, [eax]
unpckhpd xmm1, [eax]
movhpd xmm1, [eax]
movhpd [eax], xmm1

;prefix=0xF3
movss xmm1, [eax]
movss [eax], xmm1
movsldup xmm1, xmm2
movshdup xmm1, xmm2

;prefix=0xF2
movsd xmm1, [eax]
movsd [eax], xmm1
movddup xmm1, xmm2

;0x18-0x1F

;prefix=None
bndldx bnd0, [eax]
bndstx [eax], bnd0
nop eax

;prefix=0x66
bndmov [eax], bnd0

;prefix=0xF2
bndcl bnd0, eax
bndmk bnd0, [eax]

;prefix=0xF3
bndcu bnd0, eax
bndcn bnd0, eax

;0x20-0x27
mov eax, cr0
mov eax, dr0
mov cr0, eax
mov dr0, eax

;0x28-0x2F

;prefix=None
movaps xmm0, [eax]
movaps [eax], xmm1
cvtpi2ps xmm0, mm0
movntps [eax], xmm0
cvttps2pi mm0, xmm0
cvtps2pi mm0, xmm0
ucomiss xmm0, xmm1
comiss xmm0, xmm1

;prefix=0x66
movapd xmm0, [eax]
movapd [eax], xmm1
cvtpi2pd xmm0, mm0
movntpd [eax], xmm0
cvttpd2pi mm0, xmm0
cvtpd2pi mm0, xmm0
ucomisd xmm0, xmm1
comisd xmm0, xmm1

;prefix=0xF3
cvtsi2ss xmm1, [eax]
cvttss2si eax, xmm1
cvtss2si eax, xmm1

;prefix=0xF2
cvtsi2sd xmm1, [eax]
cvttsd2si eax, xmm1
cvtsd2si eax, xmm1

;0x30-0x37
wrmsr
rdtsc
rdmsr
rdpmc
sysenter
sysexit
getsec

;0x40-0x47
cmovo eax, [eax]
cmovno eax, [eax]
cmovb eax, [eax]
cmovnb eax, [eax]
cmove eax, [eax]
cmovne eax, [eax]
cmovbe eax, [eax]
cmovnbe eax, [eax]

;0x48-0x4F
cmovs eax, [eax]
cmovns eax, [eax]
cmovpe eax, [eax]
cmovpo eax, [eax]
cmovnge eax, [eax]
cmovge eax, [eax]
cmovng eax, [eax]
cmovg eax, [eax]

;0x50-0x57

;prefix=None
movmskps eax, xmm0
sqrtps xmm0, xmm1
rsqrtps xmm0, xmm1
rcpps xmm0, xmm1
andps xmm0, xmm1
andnps xmm0, xmm1
orps xmm0, xmm1
xorps xmm0, xmm1

;prefix=0x66
movmskpd eax, xmm0
sqrtpd xmm0, xmm1
andpd xmm0, xmm1
andnpd xmm0, xmm1
orpd xmm0, xmm1
xorpd xmm0, xmm1

;prefix=0xF3
sqrtss xmm0, xmm1
rsqrtss xmm0, xmm1
rcpss xmm0, xmm1

;prefix=0xF2
sqrtsd xmm0, xmm1

;0x58-0x5F

;prefix=None
addps xmm0, xmm1
mulps xmm0, xmm1
cvtps2pd xmm0, xmm1
cvtdq2ps xmm0, xmm1
subps xmm0, xmm1
minps xmm0, xmm1
divps xmm0, xmm1
maxps xmm0, xmm1

;prefix=0x66
addpd xmm0, xmm1
mulpd xmm0, xmm1
cvtpd2ps xmm0, xmm1
cvtps2dq xmm0, xmm1
subpd xmm0, xmm1
minpd xmm0, xmm1
divpd xmm0, xmm1
maxpd xmm0, xmm1

;prefix=0xF3
addss xmm0, xmm1
mulss xmm0, xmm1
cvtss2sd xmm0, xmm1
cvttps2dq xmm0, xmm1
subss xmm0, xmm1
minss xmm0, xmm1
divss xmm0, xmm1
maxss xmm0, xmm1

;prefix=F2
addsd xmm0, xmm1
mulsd xmm0, xmm1
cvtsd2ss xmm0, xmm1
subsd xmm0, xmm1
minsd xmm0, xmm1
divsd xmm0, xmm1
maxsd xmm0, xmm1

;0x60-0x67

;prefix=0x66
punpcklbw xmm0, [eax]
punpcklwd xmm0, [eax]
punpckldq xmm0, [eax]
packsswb xmm0, xmm1
pcmpgtb xmm0, xmm1
pcmpgtw xmm0, xmm1
pcmpgtd xmm0, xmm1
packuswb xmm0, xmm1

;0x68-0x6F

;prefix=0x66
punpckhbw xmm0, xmm1
punpckhwd xmm0, xmm1
punpckhdq xmm0, xmm1
packssdw xmm0, xmm1
punpcklqdq xmm0, xmm1
punpckhqdq xmm0, xmm1
movd xmm0, [eax]
movdqa xmm0, [eax]

;prefix=0xF3
movdqu xmm0, [eax]

;0x70-0x77

;prefix=None
pshufw mm0, [eax], 0FFh
pcmpeqw mm0, [eax]
pcmpeqd mm0, [eax]
emms

;prefix=0x66
pshufd xmm0, [eax], 0FFh
pcmpeqb mm0, [eax]
pcmpeqw mm0, [eax]
pcmpeqd mm0, [eax]

;prefix=0xF3
pshufhw xmm0, [eax], 0FFh

;prefix=0xF2
pshuflw xmm0, [eax], 0FFh

;0x78-0x7F

;prefix=None
vmread [eax], eax
vmwrite eax, [eax]

;prefix=0x66
haddpd xmm0, [eax]
hsubpd xmm0, [eax]
movd [eax], xmm0
movdqa [eax], xmm0

;prefix=0xF2
haddps xmm0, [eax]
hsubps xmm0, [eax]

;0x80-0x87
jo 0FFFFFFFFh
jno 0FFFFFFFFh
jb 0FFFFFFFFh
jnb 0FFFFFFFFh
je 0FFFFFFFFh
jne 0FFFFFFFFh
jbe 0FFFFFFFFh
jnbe 0FFFFFFFFh

;0x88-0x8F
js 0FFFFFFFFh
jns 0FFFFFFFFh
jp 0FFFFFFFFh
jnp 0FFFFFFFFh
jl 0FFFFFFFFh
jnl 0FFFFFFFFh
jle 0FFFFFFFFh
jnle 0FFFFFFFFh

;0x90-097
seto [eax]
setno [eax]
setb [eax]
setnb [eax]
sete [eax]
setne [eax]
setbe [eax]
setnbe [eax]

;0x98-0x9F
sets [eax]
setns [eax]
setp [eax]
setnp [eax]
setl [eax]
setnl [eax]
setle [eax]
setnle [eax]

;0xA0-0xA7
push fs
pop fs
cpuid
bt [eax], eax
shld [eax], eax, 0FFh
shld [eax], eax, cl

;0xA8-0xAF
push gs
pop gs
rsm
bts [eax], eax
shrd [eax], eax, 0FFh
shrd [eax], eax, cl
imul eax, [eax]

;0xB0-0xB7
cmpxchg [eax], al
cmpxchg [eax], eax
lss eax, [eax]
btr [eax], eax
lfs eax, [eax]
lgs eax, [eax]
movzx eax, byte [eax]
movzx eax, word [eax]

;0xB8-0xBF

;prefix=None
ud1
btc [eax], eax
bsf eax, eax
bsr eax, eax
movsx eax, byte [eax]
movsx eax, word [eax]

;prefix=0xF3
popcnt eax, [eax]
tzcnt eax, [eax]
lzcnt eax, [eax]

;0xC0-0xC7

;prefix=None
xadd [eax], al
xadd [eax], eax
cmpps xmm0, xmm1, 0FFh
movnti [eax], eax
shufps xmm0, xmm1, 0FFh

;prefix=0x66
cmppd xmm0, xmm1, 0FFh
pinsrw xmm0, [eax], 0FFh
pextrw eax, xmm0, 0FFh
shufpd xmm0, xmm1, 0FFh

;prefix=0xF3
cmpss xmm0, xmm1, 0FFh

;prefix=0xF2
cmpsd xmm0, xmm1, 0FFh

;0xC8-0xCF
bswap eax
bswap ecx
bswap edx
bswap ebx
bswap esp
bswap ebp
bswap esi
bswap edi

;0xD0-0xD7

;prefix=0x66
psrlw xmm0, xmm1
psrld xmm0, xmm1
psrlq xmm0, xmm1
paddq xmm0, xmm1
pmullw xmm0, xmm1
movq [eax], xmm1
pmovmskb eax, xmm0

;prefix=0xF3
movq2dq xmm0, mm1

;prefix=0xF2
addsubps xmm0, [eax]
movdq2q mm0, xmm0

;0xD8-0xDF

;prefix=0x66
psubusb xmm0, [eax]
psubusw xmm0, [eax]
pminub xmm0, [eax]
pand xmm0, [eax]
paddusb xmm0, [eax]
paddusw xmm0, [eax]
pmaxub xmm0, [eax]
pandn xmm0, [eax]

;0xE0-0xE7

;prefix=0x66
pavgb xmm0, [eax]
psraw xmm0, [eax]
psrad xmm0, [eax]
pavgw xmm0, [eax]
pmulhuw xmm0, [eax]
pmulhw xmm0, [eax]
cvttpd2dq xmm0, [eax]
movntdq [eax], xmm0

;prefix=0xF3
cvtdq2pd xmm0, [eax]

;prefix=0xF2
cvtpd2dq xmm0, [eax]

;0xE8-0xEF

;prefix=0x66
psubsb xmm0, [eax]
psubsw xmm0, [eax]
pminsw xmm0, [eax]
por xmm0, [eax]
paddsb xmm0, [eax]
paddsw xmm0, [eax]
pmaxsw xmm0, [eax]
pxor xmm0, [eax]

;0xF0-0xF7

;prefix=0x66
psllw xmm0, [eax]
pslld xmm0, [eax]
psllq xmm0, [eax]
pmuludq xmm0, [eax]
pmaddwd xmm0, [eax]
psadbw xmm0, [eax]
maskmovdqu xmm0, xmm1

;prefix=0xF2
lddqu xmm0, [eax]

;0xF8-0xFF

;prefix=None
ud0 eax, [eax]

;prefix=0x66
psubb xmm0, [eax]
psubw xmm0, [eax]
psubd xmm0, [eax]
psubq xmm0, [eax]
paddb xmm0, [eax]
paddw xmm0, [eax]
paddd xmm0, [eax]
