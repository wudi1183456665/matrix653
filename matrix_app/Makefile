COMPONENTS := l4app1
COMPONENTS += l4app2
COMPONENTS += l4app3
COMPONENTS += l4DfewOS
COMPONENTS += l4sylixos

all: $(COMPONENTS)
	@for target in $(COMPONENTS); do make -C $$target all; done
#	$(MAKE) -C "../matrix_sylixos_bsp_imx6/" clean
#	$(MAKE) -C "../matrix_sylixos_bsp_imx6/" all

clean: $(COMPONENTS)
	@for target in $(COMPONENTS); do make -C $$target clean; done
