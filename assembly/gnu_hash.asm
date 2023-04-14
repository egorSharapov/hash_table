global Gnu_hash_asm

Gnu_hash_asm:
        push    rbp
        mov     rbp, rsp
        and     rsp, -32
        vmovdqa yword [rsp], ymm0
        lea     rdx, [rsp]
        mov     rsi, rdx
        add     rsi, 32
        mov     eax, 5381
.L1:
        mov     ecx, eax
        sal     ecx, 5
        add     eax, ecx
        add     eax, dword [rdx]
        add     rdx, 8
        cmp     rdx, rsi
        jne     .L1
        leave
        ret