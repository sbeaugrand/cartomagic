.PHONY: all
all:
	@make --no-print-directory -C src -f Makefile-lin -j`nproc`
