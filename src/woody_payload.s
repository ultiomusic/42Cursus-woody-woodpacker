section .text
global woody_entry

woody_entry:
    push    rbx
    push    rbp
    push    r12
    push    r13
    push    r14
    push    r15
    mov     r12, rsp        ; Save stack pointer after pushes
    sub     rsp, 8          ; Align stack to 16 bytes (if needed)

    ; write(1, msg, 16)
    mov     rax, 1
    mov     rdi, 1
    lea     rsi, [rel msg]
    mov     rdx, 16
    syscall

    ; mprotect (page align)
    mov     rdi, 0x1111111111111111      ; TEXT_START_PATCH
    mov     r8, 4096
    mov     rax, rdi
    and     rax, -4096
    mov     rdi, rax
    mov     rsi, 0x2222222222222222      ; TEXT_SIZE_PATCH
    mov     rdx, 7                       ; PROT_READ|PROT_WRITE|PROT_EXEC
    mov     rax, 10                      ; syscall: mprotect
    syscall

    ; XOR decrypt loop
    mov     rdi, 0x1111111111111111      ; text_start
    mov     rcx, 0x2222222222222222      ; text_size
    mov     al, 42                       ; key
    xor     rbx, rbx
xor_loop:
    cmp     rbx, rcx
    jge     xor_done
    xor     byte [rdi+rbx], al
    inc     rbx
    jmp     xor_loop
xor_done:

    ; write(1, jumpmsg, 22)
    mov     rax, 1
    mov     rdi, 1
    lea     rsi, [rel jumpmsg]
    mov     rdx, 22
    syscall

    mov     rax, 0x3333333333333333      ; ORIG_ENTRY_PATCH
    mov     rsp, r12        ; Restore stack pointer (after pushes)
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     rbp
    pop     rbx
    call    rax

msg:        db "hello from woody", 10
jumpmsg:    db "jumping to orig_entry", 10
