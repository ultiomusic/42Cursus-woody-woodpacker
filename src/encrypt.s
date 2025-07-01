segment .text
	global encrypt

encrypt:
	push rbp
	mov rbp, rsp
	xor r8, r8
	xor rax, rax
routine:
	mov al, byte [rdi + r8]
	add al, ah
	xor [rdx], al
	inc rdx
	inc r8
	cmp r8, rsi
	jne loopinstr
	add ah, byte 42
	xor r8, r8
loopinstr:
	loop routine
	mov rsp, rbp
	pop rbp
	ret