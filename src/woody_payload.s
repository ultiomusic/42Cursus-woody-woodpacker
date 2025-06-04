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

    ; write(1, afterxor, 16)
    mov     rax, 1
    mov     rdi, 1
    lea     rsi, [rel afterxor]
    mov     rdx, 16
    syscall

    mov     rsp, r12
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     rbp
    pop     rbx
    jmp     rax

fake_argv:
    dq fake_progname
    dq 0
fake_progname:
    db "woody",0

msg:        db "hello from woody", 10
beforexor:  db "before xor loop", 10
afterxor:   db "after xor loop", 10
jumpmsg:    db "jumping to orig_entry", 10
insidexor:  db "inside xor loop", 10
mprotectfail: db "mprotect failed!\n", 0
