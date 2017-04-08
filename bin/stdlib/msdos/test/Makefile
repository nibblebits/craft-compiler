all: test.omf	
	../../../craft -input "test.omf,../io.omf" -output "test.com" -L -format "bin" -org_data "0x100"
test.omf : test.craft
	../../../craft -input "test.craft" -output "test.omf" -codegen 8086CodeGen -O -format "omf"

clean:
	rm ./test.omf
	