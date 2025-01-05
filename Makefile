HDRS := $(wildcard src/*.h)
SRCS := $(wildcard src/*.c)
PCDEPS := sdl3
BUILDDIR ?= build
OBJS := $(patsubst src/%.c,$(BUILDDIR)/%.o,$(SRCS))
CC ?= clang
CFLAGS := $(shell grep '^[^#]' compile_flags.txt) -UAPP_VERSION -DAPP_VERSION='"$(file < version)"' $(shell pkg-config $(PCDEPS) -cflags)
LDFLAGS := $(shell pkg-config $(PCDEPS) --libs)
BINNAME ?= projektor
ARG ?=

.PHONY: run
run: $(BUILDDIR)/$(BINNAME)
	./$< $(ARG)

$(BUILDDIR)/$(BINNAME): $(OBJS) Makefile compile_flags.txt | $(BUILDDIR)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

$(BUILDDIR)/%.o: src/%.c $(HDRS) Makefile compile_flags.txt | $(BUILDDIR)
	$(CC) $(CFLAGS) $< -c -o $@

$(BUILDDIR):
	mkdir $@

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

.PHONY: help
help:
	@echo -e 'About ./$(MAKEFILE_LIST)'
	@echo -e 'Targets (some target names depend on env vars):'
	@echo -e '  run\t\tbuild and run $(BUILDDIR)/$(BINNAME)'
	@echo -e '  $(BINNAME)\tcompile the executable'
	@echo -e '  $(BUILDDIR)\t\tcreate build directory'
	@echo -e '  clean\t\tremove $$BUILDDIR (rm -rf)'
	@echo -e '  help\t\tprint this help message'
	@echo -e 'Environment variables:'
	@echo -e '  CC\t\tcompiler (default: clang)'
	@echo -e '  ARG\t\targuments for run to pass to $(BUILDDIR)/$(BINNAME) (default: )'
	@echo -e '  BINNAME\tcompiled executable name (default: projektor)'
	@echo -e '  BUILDDIR\toutput directory for generated files (default: build)'
	@echo -e 'Other config files:'
	@echo -e '  compile_flags.txt\tused to read CFLAGS'
	@echo -e '  version\t\tused to read project version'
