CFLAGS := $(shell grep '^[^#]' ../compile_flags.txt) -UAPP_VERSION -DAPP_VERSION='"$(file < version)"'
ARG ?=

run: t4
	./$< '$(ARG)'

t4: main.c t4.c types.h ../compile_flags.txt Makefile
	clang $(CFLAGS) $< -o $@

.PHONY:
clean:
	rm t4
