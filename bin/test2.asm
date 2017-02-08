[BITS 16]
segment code
mov dx, _test
mov ah, 9
int 0x21

_o:

mov  ah, 0x4c
int  0x21   

segment data
_test:
	db 'Test'