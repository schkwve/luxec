/**
 * LuxeC (c) 2023 by Jozef Nagy
 *
 * LuxeC is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <def.h>
#include <data.h>

#include <misc.h>
#include <scanner.h>

void match(int t, char *what)
{
	if (Token.token == t) {
		scan(&Token);
	} else {
		printf("%s expected on line %d\n", what, Line);
		exit(1);
	}
}

void semi(void)
{
	match(T_SEMI, ";");
}

void ident(void)
{
	match(T_IDENT, "Identifier");
}

void lbrace(void)
{
	match(T_LBRACE, "{");
}

void rbrace(void)
{
	match(T_RBRACE, "}");
}

void lparen(void)
{
	match(T_LPAREN, "(");
}

void rparen(void)
{
	match(T_RPAREN, ")");
}

void fatal(char *s)
{
	fprintf(stderr, "%s on line %d\n", s, Line);
	exit(1);
}

void fatals(char *s1, char *s2)
{
	fprintf(stderr, "%s:%s on line %d\n", s1, s2, Line);
	exit(1);
}

void fatald(char *s, int d)
{
	fprintf(stderr, "%s:%d on line %d\n", s, d, Line);
	exit(1);
}

void fatalc(char *s, int c)
{
	fprintf(stderr, "%s:%c on line %d\n", s, c, Line);
	exit(1);
}

/*
 * strdup is not part of c99,
 * so i hacked together a poor implementation of it
 */
char *__strdup(const char *str)
{
	size_t len = strlen(str) + 1;
	char *dst = malloc(len);
	if (dst != NULL) {
		memcpy(dst, str, len);
	}

	return dst;
}