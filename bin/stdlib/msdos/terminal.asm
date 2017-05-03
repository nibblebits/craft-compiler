segment code
global _terminal_backspace

; void terminal_backspace();
_terminal_backspace:
	push bp
	mov ah, 0x0e
	mov al, 0x08
	int 0x10
	call _backspace
	xor ax, ax
	pop bp
	ret

_backspace:
	mov bp, sp
	mov ah, 0x0a
    mov al, 0   
    mov bh, 0
    mov cx, 1
	int 0x10
	ret