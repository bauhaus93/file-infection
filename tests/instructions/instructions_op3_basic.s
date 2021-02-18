;#####################
;######## 0F38 #######
;#####################

;0x00-0x07 / prefix = 0x66
pshufb xmm0, [eax]
phaddw xmm0, [eax]
phaddd xmm0, [eax]
phaddsw xmm0, [eax]
pmaddubsw xmm0, [eax]
phsubw xmm0, [eax]
phsubd xmm0, [eax]
phsubsw xmm0, [eax]

;0x08-0x0F  / prefix = 0x66
psignb xmm0, [eax]
psignw xmm0, [eax]
psignd xmm0, [eax]
pmulhrsw xmm0, [eax]

;0x10-0x17 / prefix = 0x66
pblendvb xmm0, [eax]
blendvps xmm0, [eax]
blendvpd xmm0, [eax]
ptest xmm0, [eax]

;0x18-0x1F / prefix = 0x66
pabsb xmm0, [eax]
pabsw xmm0, [eax]
pabsd xmm0, [eax]

;0x20-0x27 / prefix = 0x66
pmovsxbw xmm0, [eax]
pmovsxbd xmm0, [eax]
pmovsxwd xmm0, [eax]
pmovsxwq xmm0, [eax]
pmovsxdq xmm0, [eax]

;0x28-0x2F / prefix = 0x66
pmuldq xmm0, [eax]
pcmpeqq xmm0, [eax]
movntdqa xmm0, [eax]
packusdw xmm0, [eax]

;0x30-0x37 / prefix = 0x66
pmovzxbw xmm0, [eax]
pmovzxbd xmm0, [eax]
pmovzxbq xmm0, [eax]
pmovzxwd xmm0, [eax]
pmovzxwq xmm0, [eax]
pmovzxdq xmm0, [eax]
pcmpgtq xmm0, [eax]

;0x38-0x3F / prefix = 0x66
pminsb xmm0, [eax]
pminsd xmm0, [eax]
pminuw xmm0, [eax]
pminud xmm0, [eax]
pmaxsb xmm0, [eax]
pmaxsd xmm0, [eax]
pmaxuw xmm0, [eax]
pmaxud xmm0, [eax]

;0x40-0x47 / prefix = 0x66
pmulld xmm0, [eax]
phminposuw xmm0, [eax]

;0x80-0x87 / prefix = 0x66
invept eax, [eax]
invvpid eax, [eax]
invpcid eax, [eax]

;0xC8-0xCF
sha1nexte xmm0, [eax]
sha1msg1 xmm0, [eax]
sha1msg2 xmm0, [eax]
sha256rnds2 xmm0, [eax]
sha256msg1 xmm0, [eax]
sha256msg2 xmm0, [eax]

;0xD8-0xDF / prefix = 0x66
aesimc xmm0, [eax]
aesenc xmm0, [eax]
aesenclast xmm0, [eax]
aesdec xmm0, [eax]
aesdeclast xmm0, [eax]

;0xF0-0xF7

;prefix = None
movbe eax, [eax]
movbe [eax], eax

;prefix = 0x66
movbe ax, [eax]
movbe [eax], ax
adcx eax, [eax]

;prefix = 0xF3
adox eax, [eax]

;prefix = 0xF2
crc32 eax, byte [eax]
crc32 eax, dword [eax]

;prefix = 0x66 & 0xF2
crc32 eax, word [eax] 

;#####################
;######## 0F3A #######
;#####################

;0x07-0x0F / prefix = 0x66
roundps xmm0, [eax], 0FFh
roundpd xmm0, [eax], 0FFh
roundss xmm0, [eax], 0FFh
roundsd xmm0, [eax], 0FFh
roundps xmm0, [eax], 0FFh
roundpd xmm0, [eax], 0FFh
pblendw xmm0, [eax], 0FFh
palignr xmm0, [eax], 0FFh

;0x10-0x17 / prefix = 0x66
pextrb eax, xmm0, 0FFh
pextrw eax, xmm0, 0FFh
pextrd eax, xmm0, 0FFh
extractps [eax], xmm0, 0FFh

;0x20-0x27 / prefix = 0x66
pinsrb xmm0, [eax], 0FFh
insertps xmm0, [eax], 0FFh
pinsrd xmm0, [eax], 0FFh

;0x40-0x47 / prefix = 0x66
dpps xmm0, [eax], 0FFh
dppd xmm0, [eax], 0FFh
mpsadbw xmm0, [eax], 0FFh
pclmulqdq xmm0, [eax], 0FFh

;0x60-0x67 / prefix = 0x66
pcmpestrm xmm0, [eax], 0FFh
pcmpestri xmm0, [eax], 0FFh
pcmpistrm xmm0, [eax], 0FFh
pcmpistri xmm0, [eax], 0FFh

;0xC8-0CF
sha1rnds4 xmm0, [eax], 0FFh
