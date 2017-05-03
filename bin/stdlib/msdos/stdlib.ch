#ifndef STDLIB_H
{
	#define STDLIB_H;
	uint8* itoa_bin(uint16 value, uint8* str);
	uint8* itoa_dec(uint16 value, uint8* str);
	uint8* itoa_hex(uint16 value, uint8* str);
	uint8* itoa(uint16 value, uint8* str, uint8 base);
	uint16 atoi(uint8* str);
	
	uint8 str_valid_number(uint8* ptr);
	
}