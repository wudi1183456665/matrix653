#COMPONENTS +=  l4loader
COMPONENTS +=  l4matrix
COMPONENTS +=  l4api
COMPONENTS +=  l4libc
#COMPONENTS +=  l4libtest
#COMPONENTS +=  libsylixos

all: $(COMPONENTS)
	@for target in $(COMPONENTS); do make -C $$target all; done
#	$(MAKE) -C "../matrix_sylixos_bsp_imx6/" clean
#	$(MAKE) -C "../matrix_sylixos_bsp_imx6/" all
#	$(MAKE) -C "../matrix_app/" clean
#	$(MAKE) -C "../matrix_app/" all

clean: $(COMPONENTS)
	@for target in $(COMPONENTS); do make -C $$target clean; done
