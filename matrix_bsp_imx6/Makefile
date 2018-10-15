#COMPONENTS := l4loader
#COMPONENTS += l4sylixos
COMPONENTS += l4matrix

all: $(COMPONENTS)
	@rm -f "l4matrix/Debug/l4matrix.elf"
	@rm -f "l4matrix/Debug/l4matrix.bin"
	@rm -f "l4matrix/Debug/l4matrix.lzo"
	@rm -f "l4matrix/Debug/l4matrix.siz"
	@rm -rf "l4matrix/Debug/obj/"
	@rm -f "l4matrix/Release/l4matrix.elf"
	@rm -f "l4matrix/Release/l4matrix.bin"
	@rm -f "l4matrix/Release/l4matrix.lzo"
	@rm -f "l4matrix/Release/l4matrix.siz"
	@rm -rf "l4matrix/Release/obj/"
	@for target in $(COMPONENTS); do make -C $$target all; done

clean: $(COMPONENTS)
	@for target in $(COMPONENTS); do make -C $$target clean; done
