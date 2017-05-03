#ifndef IO_H
{
	#define IO_H;
			
	void print_char(uint8 c);
	void print_str(uint8* str);
	void print_number(uint16 number);
	
	void scan_char(uint8* ptr);
	void scan_int(uint16* ptr);
	void scan_str(uint8* ptr, uint16 len);
	
}