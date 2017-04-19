global _terminal_backspace

; void terminal_backspace();
_terminal_backspace:
	push bp
	mov bp, sp
	mov ah, 0x0a
    mov al, 0   
    mov bh, 0
    mov cx, 1
	int 0x10
	xor ah, ah
	pop bp
	ret
