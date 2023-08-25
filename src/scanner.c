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

#include <def.h>
#include <data.h>

#include <scanner.h>
#include <misc.h>

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
	case 'e':
		if (strcmp(s, "else") == 0) {
			return T_ELSE;
		}
		break;
	case 'f':
		if (strcmp(s, "for") == 0) {
			return T_FOR;
		}
		break;
	case 'i':
		if (strcmp(s, "if") == 0) {
			return T_IF;
		}
		if (strcmp(s, "int") == 0) {
			return T_INT;
		}
		break;
	case 'p':
		if (strcmp(s, "print") == 0) {
			return T_PRINT;
		}
		break;
	case 'v':
		if (strcmp(s, "void") == 0) {
			return T_VOID;
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
	int c;
	int token_type;

	c = skip();

	switch (c) {
	case EOF:
		t->token = T_EOF;
		return 0;
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
	case '{':
		t->token = T_LBRACE;
		break;
	case '}':
		t->token = T_RBRACE;
		break;
	case '(':
		t->token = T_LPAREN;
		break;
	case ')':
		t->token = T_RPAREN;
		break;
	case '=':
		if ((c = next()) == '=') {
			t->token = T_EQ;
		} else {
			putback(c);
			t->token = T_ASSIGN;
		}
		break;
	case '!':
		if ((c = next()) == '=') {
			t->token = T_NE;
		} else {
			fatalc("Unrecognized character", c);
		}
		break;
	case '<':
		if ((c = next()) == '=') {
			t->token = T_LE;
		} else {
			putback(c);
			t->token = T_LT;
		}
		break;
	case '>':
		if ((c = next()) == '=') {
			t->token = T_GE;
		} else {
			putback(c);
			t->token = T_GT;
		}
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

			t->token = T_IDENT;
			break;
		}

		fatalc("Unrecognized character", c);
	}

	return 1;
}