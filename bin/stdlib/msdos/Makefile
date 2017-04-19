
keyboard.omf: keyboard.asm
	nasm "keyboard.asm" -f obj -o "keyboard.omf"
	
io.omf : io.craft
	../../craft -input "io.craft" -output "io.omf" -codegen 8086CodeGen -O -format "omf"

clean:
	rm ./io.omf
	