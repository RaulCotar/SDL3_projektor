#include <stdlib.h>
#include <stdio.h>
#include "types.h"

// assume we have a character buffer with source code
// we want to get a list of tokens (SOA of token types and string views)

typedef enum Token: u8 {
	TOK_EOF = 0,// EOF
	TOK_IDENT,	// identifier
	TOK_LIT_I,	// integer literal
	TOK_PLUS,	// +
	TOK_MINUS,	// -
	TOK_EQUAL,	// =
	TOK_SLASH,	// +
	TOK_SCOL,	// ;
	// etc.
	TOK_DOCC,	// doc comment
	TOK_ERR		// invalid input
} Token;

char const *const tokname[] = {
	[TOK_EOF]	= "eof ",
	[TOK_IDENT]	= "ident",
	[TOK_LIT_I]	= "lit_i",
	[TOK_PLUS]	= "plus",
	[TOK_MINUS]	= "minus",
	[TOK_EQUAL]	= "equal",
	[TOK_SLASH]	= "slash",
	[TOK_SCOL]	= "scol",
	[TOK_DOCC]	= "docc",
	[TOK_ERR]	= "err ",
};

typedef struct SrcView {
	u32 ofs, len;
} SrcView;

typedef struct {
	enum Token *toks;
	SrcView *srcs;
	u64 count;
} TokenSOA;

void tokPrint(Token t, SrcView s, char const *restrict buf) {
	fputs(tokname[t], stdout);
	putchar('\t');
	for (u32 j = 0; j < s.len; j++)
		putchar(buf[s.ofs + j]);
	putchar('\n');
}

TokenSOA tokenize(char const *restrict buf, u64 const bufSize) {
	char const *const endPtr = buf + bufSize;
	u64 size = 0, cap = bufSize / 5 + 1; // one token per 5 bytes of input seems like a decent approx
	Token *toks = malloc(cap * sizeof(*toks));
	SrcView *srcs = malloc(cap * sizeof(*srcs));

	u32 ofs = 0, len = 1;
	Token tok = TOK_EOF;
	goto _any;

	_push:
		if (size == cap) {
			cap = (cap * 4 + 3) / 3;
			toks = realloc(toks, cap * sizeof(*toks));
			srcs = realloc(srcs, cap * sizeof(*srcs));
			if (!toks || !srcs)
				exit(2);
		}
		tokPrint(tok, (SrcView){ofs, len}, buf);
		if (ofs > 10)
			exit(0);
		toks[size] = tok;
		srcs[size] = (SrcView){ofs, len};
		ofs += len;
		len = 1;
		size++;

	_any:
		if (buf < endPtr) {
			switch (*buf++) {
				case 0:
					tok = TOK_EOF;
					goto _push;
				case 'A'...'Z':
				case 'a'...'v':
				case '_':
					goto _idorkw;
				case '0'...'9':
					goto _number;
				case '+':
					tok = TOK_PLUS;
					goto _push;
				case '-':
					goto _minus;
				case '=':
					tok = TOK_EQUAL;
					goto _push;
				case ';':
					tok = TOK_SCOL;
					goto _push;
				case '/':
					goto _slash;
				default:
					tok = TOK_ERR;
					goto _push;
			}
		}

	_idorkw: {
		while (buf + 1 < endPtr && (
			(buf[1] >= '0' && buf[1] <= '9') ||
			(buf[1] >= 'A' && buf[1] <= 'Z') ||
			(buf[1] >= 'a' && buf[1] <= 'z') ||
			(buf[1] == '_'))) buf++, len++;
		tok = TOK_IDENT;
		goto _push;
	}

	_minus: {
		if (buf + 1 < endPtr && buf[1] >= '0' && buf[1] <= '9') {
			buf++;
			goto _number;
		}
		goto _push;
	}

	_number: {
		while (buf + 1 < endPtr && buf[1] >= '0' && buf[1] <= '9')
			buf++, len++;
		tok = TOK_LIT_I;
		goto _push;
	}

	_slash: {
		if (++buf < endPtr && *buf == '/') {
			while (++len, ++buf < endPtr && *buf != '\n');
			tok = TOK_DOCC;
			goto _push;
		} else if (buf < endPtr && *buf == '*')
			if (++len, buf + 1 < endPtr && buf[1] == '*') {
				u32 c = 0;
				buf++;
				while (++len, ++buf < endPtr && c < 3)
					if (*buf == '*') c++;
					else if (*buf == '/' && c > 1) c++;
					else c = 0;
				tok = TOK_DOCC;
				goto _push;
			}
		tok = TOK_SLASH;
		goto _push;
	}

	return (TokenSOA){toks, srcs, size};
}
