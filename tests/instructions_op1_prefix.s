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


;REPE
