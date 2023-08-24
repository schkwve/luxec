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
	return p ? p - s : -1;
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
	default:
		if (isdigit(c)) {
			t->int_val = scan_int(c);
			t->token = T_INTLIT;
			break;
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