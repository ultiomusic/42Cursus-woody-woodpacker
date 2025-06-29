section .text
global woody_entry

woody_entry:
    push    rbx
    push    rbp
    push    r12
    push    r13
    push    r14
    push    r15
    mov     r12, rsp
    sub     rsp, 8
    mov     r15, rdx                ; save rdx (_rtld_fini)          

    mov     rax, 1
    mov     rdi, 1
    lea     rsi, [rel msg]
    mov     rdx, 16
    syscall

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

    mov     rsp, r12
    mov     rdx, r15                ; restore rdx
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     rbp
    pop     rbx
    align 8
    mov     rax, 0x3333333333333333      ; orig_entry (patched at runtime)
    jmp     rax

fake_argv:
    dq fake_progname
    dq 0
fake_progname:
    db "woody",0

msg:        db "....WOODY....", 10
