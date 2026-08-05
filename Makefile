.PHONY: all
all:
	make -C lib $@
	make -C examples $@

.PHONY: clean
clean:
	make -C lib $@
	make -C examples $@
