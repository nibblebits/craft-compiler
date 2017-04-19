release:
	$(MAKE) -C ./GoblinArgumentParser CONF=Release
	$(MAKE) -C ./GoblinLibraryLoader CONF=Release
	$(MAKE) -C ./Compiler CONF=Release
	$(MAKE) -C ./codegens/8086CodeGen CONF=Release
	$(MAKE) -C ./libs/MagicOMF CONF=Release
	$(MAKE) -C ./linkers/BinLinker CONF=Release
	$(MAKE) -C ./obj_formats/OMFObjFormat CONF=Release
	$(MAKE) -C ./obj_formats/CraftStaticLibrary CONF=Release
	$(MAKE) -C ./linkers/CraftStaticLibraryLinker CONF=Release
	$(MAKE) -C ./CraftCompiler CONF=Release 
debug:
	$(MAKE) -C ./GoblinArgumentParser CONF=Debug
	$(MAKE) -C ./GoblinLibraryLoader CONF=Debug
	$(MAKE) -C ./Compiler CONF=Debug
	$(MAKE) -C ./codegens/8086CodeGen CONF=Debug
	$(MAKE) -C ./libs/MagicOMF CONF=Debug
	$(MAKE) -C ./linkers/BinLinker CONF=Debug
	$(MAKE) -C ./obj_formats/OMFObjFormat CONF=Debug
	$(MAKE) -C ./obj_formats/CraftStaticLibrary CONF=Debug
	$(MAKE) -C ./linkers/CraftStaticLibraryLinker CONF=Debug
	$(MAKE) -C ./CraftCompiler CONF=Debug 
clean:
	$(MAKE) -C ./GoblinArgumentParser CONF=Debug clean
	$(MAKE) -C ./GoblinLibraryLoader CONF=Debug clean
	$(MAKE) -C ./Compiler CONF=Debug clean
	$(MAKE) -C ./codegens/8086CodeGen CONF=Debug clean
	$(MAKE) -C ./libs/MagicOMF CONF=Debug clean
	$(MAKE) -C ./linkers/BinLinker CONF=Debug clean
	$(MAKE) -C ./obj_formats/OMFObjFormat CONF=Debug clean
	$(MAKE) -C ./obj_formats/CraftStaticLibrary CONF=Debug clean
	$(MAKE) -C ./linkers/CraftStaticLibraryLinker CONF=Debug clean
	$(MAKE) -C ./CraftCompiler CONF=Debug clean
	
	$(MAKE) -C ./GoblinArgumentParser CONF=Release clean
	$(MAKE) -C ./GoblinLibraryLoader CONF=Release clean
	$(MAKE) -C ./Compiler CONF=Release clean
	$(MAKE) -C ./codegens/8086CodeGen CONF=Release clean
	$(MAKE) -C ./libs/MagicOMF CONF=Release clean
	$(MAKE) -C ./linkers/BinLinker CONF=Release clean
	$(MAKE) -C ./obj_formats/OMFObjFormat CONF=Release clean
	$(MAKE) -C ./obj_formats/CraftStaticLibrary CONF=Release clean
	$(MAKE) -C ./linkers/CraftStaticLibraryLinker CONF=Release clean
	$(MAKE) -C ./CraftCompiler CONF=Release clean