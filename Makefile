release:
	$(MAKE) -C ./GoblinArgumentParser CONF=Release
	$(MAKE) -C ./GoblinLibraryLoader CONF=Release
	$(MAKE) -C ./Compiler CONF=Release
	$(MAKE) -C ./codegens/8086CodeGen CONF=Release
	$(MAKE) -C ./libs/MagicOMF CONF=Release
	$(MAKE) -C ./linkers/BinLinker CONF=Release
	$(MAKE) -C ./obj_formats/OMFObjFormat CONF=Release
	$(MAKE) -C ./CraftCompiler CONF=Release 
debug:
	$(MAKE) -C ./GoblinArgumentParser CONF=Debug
	$(MAKE) -C ./GoblinLibraryLoader CONF=Debug
	$(MAKE) -C ./Compiler CONF=Debug
	$(MAKE) -C ./codegens/8086CodeGen CONF=Debug
	$(MAKE) -C ./libs/MagicOMF CONF=Debug
	$(MAKE) -C ./linkers/BinLinker CONF=Debug
	$(MAKE) -C ./obj_formats/OMFObjFormat CONF=Debug
	$(MAKE) -C ./CraftCompiler CONF=Debug 
clean:
	$(MAKE) -C ./CraftCompiler clean
	$(MAKE) -C ./Compiler clean
	$(MAKE) -C ./GoblinArgumentParser clean
	$(MAKE) -C ./GoblinLibraryLoader clean
	$(MAKE) -C ./codegens/8086CodeGen clean
	$(MAKE) -C ./libs/MagicOMF clean
	$(MAKE) -C ./linkers/BinLinker clean
	$(MAKE) -C ./obj_formats/OMFObjFormat clean