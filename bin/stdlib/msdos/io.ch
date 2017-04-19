#ifndef IO_H
{
	#define IO_H;
	#define KB_SIZE 512
	// Try to make compatible with FILE*
			
	void print_char(uint8 c);
	void print_str(uint8* str);
	void print_number(uint16 number);
	// Currently unlimited arguments are not supported.
	void print(uint8* str, uint16 arg1, uint16 arg2, uint16 arg3);
	
	void scan_char(uint8* ptr);
	void scan_uint8(uint8* ptr);
	
}