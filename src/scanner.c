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
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <data.h>
#include <def.h>

#include <scanner.h>

char *tokstr[] = { "+", "-", "*", "/", "intlit" };

static void putback(int c)
{
	Putback = c;
}

static int next(void)
{
	int c;

	if (Putback) {
		c = Putback;
		Putback = 0;
		return c;
	}

	c = fgetc(InFile);
	if ('\n' == c) {
		Line++;
	}

	return c;
}

static int skip(void)
{
	int c;

	c = next();
	while (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f') {
		c = next();
	}

	return c;
}

static int chrpos(char *s, int c)
{
	char *p;

	p = strchr(s, c);
	return (p ? p - s : -1);
}

static int keyword(char *s)
{
	// SubC optimization:
	// Match the keyword against the first
	//   letter to match keywords faster
	switch (*s) {
	case 'p':
		if (strcmp(s, "print") == 0) {
			return (T_PRINT);
		}
		break;
	}

	return 0;
}

static int scan_ident(int c, char *buf, int lim)
{
	int i = 0;

	while (isalpha(c) || isdigit(c) || c == '_') {
		if (i == (lim - 1)) {
			printf("Identifier too long on line %d\n", Line);
			exit(1);
		} else if (i < (lim - 1)) {
			buf[i++] = c;
		}
		c = next();
	}

	putback(c);
	buf[i] = '\0';

	return i;
}

int scan_int(int c)
{
	int k, val = 0;

	while ((k = chrpos("0123456789", c)) >= 0) {
		val = val * 10 + k;
		c = next();
	}

	// non-integer character
	putback(c);
	return val;
}

int scan(struct token *t)
{
	int c = skip();
	int token_type;

	switch (c) {
	case EOF:
		t->token = T_EOF;
		break;
	case '+':
		t->token = T_PLUS;
		break;
	case '-':
		t->token = T_MINUS;
		break;
	case '*':
		t->token = T_STAR;
		break;
	case '/':
		t->token = T_SLASH;
		break;
	case ';':
		t->token = T_SEMI;
		break;
	default:
		if (isdigit(c)) {
			t->int_val = scan_int(c);
			t->token = T_INTLIT;
			break;
		} else if (isalpha(c) || '_' == c) {
			scan_ident(c, Text, TEXTLEN);

			// is it a recognized keyword?
			if ((token_type = keyword(Text))) {
				t->token = token_type;
				break;
			}

			printf("Unrecognized symbol '%s' on line %d\n", Text, Line);
			exit(1);
		}

		printf("Unrecognized character '%c' on line %d\n", c, Line);
		exit(1);
	}

	return 1;
}

void scan_file()
{
	struct token T;

	while (scan(&T)) {
		printf("Found token '%s'", tokstr[T.token]);
		if (T.token == T_INTLIT) {
			printf(", value %d", T.int_val);
		}
		printf("\n");
	}
}