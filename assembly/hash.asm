global CRC32_hash_asm

CRC32_hash_asm:
        push    rbp
        mov     rbp, rsp
        and     rsp, -32
        vmovdqa yword [rsp-32], ymm0
        mov     rdi, rsp
        sub     rdi, 32
        mov     rax, 0
        crc32   rax, qword [rdi]
        crc32   rax, qword [rdi+8]
        crc32   rax, qword [rdi+16]
        crc32   rax, qword [rdi+24]
        leave 
        ret
