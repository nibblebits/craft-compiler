all: keyboard.omf terminal_asm.omf terminal.omf string.omf stdlib.omf io.omf
	../../craft -input "keyboard.omf,terminal_asm.omf,terminal.omf,string.omf,stdlib.omf,io.omf" -output "./stdlib.csl" -L -format "csl"
keyboard.omf: keyboard.asm
	nasm "keyboard.asm" -f obj -o "keyboard.omf"

terminal_asm.omf: terminal.asm
	nasm "terminal.asm" -f obj -o "terminal_asm.omf"

terminal.omf: terminal.craft
	../../craft -input "terminal.craft" -output "terminal.omf" -codegen 8086CodeGen -O -format "omf"

string.omf : string.craft
	../../craft -input "string.craft" -output "string.omf" -codegen 8086CodeGen -O -format "omf"
stdlib.omf : stdlib.craft
	../../craft -input "stdlib.craft" -output "stdlib.omf" -codegen 8086CodeGen -O -format "omf"
io.omf : io.craft
	../../craft -input "io.craft" -output "io.omf" -codegen 8086CodeGen -O -format "omf"

clean:
	rm ./stdlib.omf
	rm ./io.omf
	rm ./keyboard.omf
	rm ./terminal_asm.omf
	rm ./terminal.omf
	