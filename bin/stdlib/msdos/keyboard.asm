global _recv_key

; uint8 recv_key();
_recv_key:
	push bp
	mov bp, sp
	mov ah, 0
	int 0x16
	xor ah, ah
	pop bp
	ret
