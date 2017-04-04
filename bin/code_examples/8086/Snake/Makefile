all: graphics.omf keyboard.omf misc.omf main.omf	
	../../../craft -input "main.omf,graphics.omf,keyboard.omf,misc.omf" -output "snake.com" -L -format "bin" -org_data "0x100"
	
keyboard.omf: keyboard.asm
	nasm keyboard.asm -f obj -o keyboard.omf
graphics.omf: graphics.asm
	nasm graphics.asm -f obj -o graphics.omf
misc.omf: misc.asm
	nasm misc.asm -f obj -o misc.omf
main.omf : main.craft
	../../../craft -input "main.craft" -output "main.omf" -codegen 8086CodeGen -O -format "omf"

clean:
	rm ./graphics.omf
	rm ./keyboard.omf
	rm ./misc.omf
	rm ./main.omf
	