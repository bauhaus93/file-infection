;Group 6 / 0x0F00
sldt eax
str eax
lldt [eax]
ltr [eax]
verr [eax]
verw [eax]

;Group 7 / 0x0F01
sgdt [eax]
vmcall
vmlaunch
vmresume
vmxoff

sidt [eax]
monitor
mwait
clac
stac
encls

lgdt [eax]
xgetbv
xsetbv
vmfunc
xend
xtest
enclu

lidt [eax]
smsw eax
lmsw [eax]
invlpg [eax]
rdtscp

;Group 8 / 0x0FBA
bt eax, 0
bts eax, 0
btr eax, 0
btc eax, 0

;Group 9 / 0x0FC7
cmpxchg8b [eax]
vmptrld [eax]
vmclear  [eax]
vmxon [eax]
rdrand eax
vmptrst [eax]
rdseed eax
rdpid eax

;Group 10 / 0x0FB9
ud1

;Group 12 / 0x0F71
psrlw mm0, 0
psraw mm0, 0
psllw mm0, 0
psrlw xmm0, 0
psraw xmm0, 0
psllw xmm0, 0

;Group 13 / 0x0F72
psrld mm0, 0
psrad mm0, 0
pslld mm0, 0
psrld xmm0, 0
psrad xmm0, 0
pslld xmm0, 0

;Group 14 / 0x0F73
psrlq mm0, 0
psllq mm0, 0
psrlq xmm0, 0
psrldq xmm0, 0
psllq xmm0, 0
pslldq xmm0, 0

;Group 15 / 0x0FAE
fxsave [eax]
fxrstor [eax]
ldmxcsr [eax]
xsave [eax]
xrstor [eax]
xsaveopt [eax]
clflush [eax]

lfence
mfence
sfence

;Group 16 / 0x0F18
prefetchnta [eax]
prefetcht0 [eax]
prefetcht1 [eax]
prefetcht2 [eax]
