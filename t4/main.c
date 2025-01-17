#include <stdio.h>
#include <string.h>
#include "t4.c"

int main(int argc, char *argv[argc]) {
	if (argc < 2) {
		puts("\e[33mYou must provide one cmdline argument.\e[39m");
		exit(1);
	}

	TokenSOA soa = tokenize(argv[1], strlen(argv[1]));
	for(u64 i = 0; i < soa.count; i++)
		tokPrint(soa.toks[i], soa.srcs[i], argv[1]);

	free(soa.toks);
	free(soa.srcs);

	return 0;
}
