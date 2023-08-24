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

#include <data.h>

#include <codegen.h>

static int reg_is_free[4];
static char *reglist[4] = {
	"r8",
	"r9",
	"r10",
	"r11",
};

static int reg_alloc(void)
{
	for (int i = 0; i < 4; i++) {
		if (reg_is_free[i]) {
			reg_is_free[i] = 0;
			return i;
		}
	}

	fprintf(stderr, "Out of registers\n");
	exit(1);
}

static void reg_free(int reg)
{
	if (reg_is_free[reg] != 0) {
		fprintf(stderr, "Error trying to free register %d\n", reg);
		exit(1);
	}

	reg_is_free[reg] = 1;
}

void free_allregs(void)
{
	reg_is_free[0] = 1;
	reg_is_free[1] = 1;
	reg_is_free[2] = 1;
	reg_is_free[3] = 1;
}

void cg_preamble()
{
	free_allregs();
	fputs("\tglobal\tmain\n"
		  "\textern\tprintf\n"
		  "\tsection\t.text\n"
		  "LC0:\n"
		  "\t.string\t\"%d\\n\"\n"
		  "printint:\n"
		  "\tpush\trbp\n"
		  "\tmov\trbp, rsp\n"
		  "\tsub\trsp, 16\n"
		  "\tmov\t[rbp-4], edi\n"
		  "\tmov\teax, [rbp-4]\n"
		  "\tmov\tesi, eax\n"
		  "\tlea\trdi, [rel LC0]\n"
		  "\tmov\teax, 0\n"
		  "\tcall printf\n"
		  "\tnop\n"
		  "\tleave\n"
		  "\tret\n"
		  "\n"
		  "main:\n"
		  "\tpush\trbp\n"
		  "\tmov\trbp, rsp\n",
		  OutFile);
}

void cg_postamble()
{
	fprintf(OutFile, "\tmov\teax, 0\n"
					 "\tpop rbp\n"
					 "\tret\n");
}

int cgload(int val)
{
	int r = reg_alloc();

	fprintf(OutFile, "\tmovq\t%d, %s\n", val, reglist[r]);
	return r;
}

int cgadd(int a, int b)
{
	fprintf(OutFile, "\taddq\t%s, %s\n", reglist[a], reglist[b]);
	reg_free(a);

	return b;
}

int cgsub(int a, int b)
{
	fprintf(OutFile, "\tsubq\t%s, %s\n", reglist[b], reglist[a]);
	reg_free(b);

	return a;
}

int cgmul(int a, int b)
{
	fprintf(OutFile, "\timulq\t%s, %s\n", reglist[a], reglist[b]);
	reg_free(a);

	return b;
}

int cgdiv(int a, int b)
{
	fprintf(OutFile, "\tmovq\t%s,%%rax\n", reglist[a]);
	fprintf(OutFile, "\tcqo\n");
	fprintf(OutFile, "\tidivq\t%s\n", reglist[b]);
	fprintf(OutFile, "\tmovq\t%%rax,%s\n", reglist[a]);
	reg_free(b);

	return a;
}

void cg_printint(int a)
{
	fprintf(OutFile, "\tmovq\t%s, %%rdi\n", reglist[a]);
	fprintf(OutFile, "\tcall\tprintint\n");
	reg_free(a);
}