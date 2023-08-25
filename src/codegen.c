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

#include <def.h>
#include <data.h>

#include <codegen.h>
#include <misc.h>

static int freereg[4];
static char *reglist[4] = { "%r8", "%r9", "%r10", "%r11" };
static char *cmplist[] = { "sete", "setne", "setl", "setg", "setle", "setge" };
static char *invcmplist[] = { "jne", "je", "jge", "jle", "jg", "jl" };

// Set all registers as available
void free_allregs(void)
{
	freereg[0] = freereg[1] = freereg[2] = freereg[3] = 1;
}

static int alloc_reg(void)
{
	for (int i = 0; i < 4; i++) {
		if (freereg[i]) {
			freereg[i] = 0;
			return i;
		}
	}
	fprintf(stderr, "Out of registers!\n");
	exit(1);
}

static void free_reg(int reg)
{
	if (freereg[reg] != 0) {
		fprintf(stderr, "Error trying to free register %d\n", reg);
		exit(1);
	}
	freereg[reg] = 1;
}

static int cgcompare(int a, int b, char *how)
{
	fprintf(OutFile, "\tcmpq\t%s, %s\n", reglist[b], reglist[a]);
	fprintf(OutFile, "\t%s\t%sb\n", how, reglist[b]);
	fprintf(OutFile, "\tmovzbq\t%sb,%s\n", reglist[b], reglist[b]);
	free_reg(a);
	return b;
}

void cgpreamble(void)
{
	free_allregs();
	fprintf(OutFile, "\t.text\n"
					 ".LC0:\n"
					 "\t.string\t\"%%d\\n\"\n"
					 "printint:\n"
					 "\tpushq\t%%rbp\n"
					 "\tmovq\t%%rsp, %%rbp\n"
					 "\tsubq\t$16, %%rsp\n"
					 "\tmovl\t%%edi, -4(%%rbp)\n"
					 "\tmovl\t-4(%%rbp), %%eax\n"
					 "\tmovl\t%%eax, %%esi\n"
					 "\tleaq	.LC0(%%rip), %%rdi\n"
					 "\tmovl	$0, %%eax\n"
					 "\tcall	printf@PLT\n"
					 "\tnop\n"
					 "\tleave\n"
					 "\tret\n"
					 "\n");
}

void cgfuncpreamble(char *name)
{
	fprintf(OutFile,
			"\t.text\n"
			"\t.globl\t%s\n"
			"\t.type\t%s, @function\n"
			"%s:\n"
			"\tpushq\t%%rbp\n"
			"\tmovq	%%rsp, %%rbp\n",
			name, name, name);
}

void cgfuncpostamble()
{
	fprintf(OutFile, "\tmovl	$0, %%eax\n"
					 "\tpopq	%%rbp\n"
					 "\tret\n");
}

int cgload(int value)
{
	int r = alloc_reg();

	fprintf(OutFile, "\tmovq\t$%d, %s\n", value, reglist[r]);
	return r;
}

int cgloadint(int val)
{
	int r = alloc_reg();
	fprintf(OutFile, "\tmovq\t$%d, %s\n", val, reglist[r]);
	return r;
}

void cglabel(int l)
{
	fprintf(OutFile, "L%d:\n", l);
}

void cgjump(int l)
{
	fprintf(OutFile, "\tjmp\tL%d\n", l);
}

int cgadd(int a, int b)
{
	fprintf(OutFile, "\taddq\t%s, %s\n", reglist[a], reglist[b]);
	free_reg(a);
	return b;
}

int cgsub(int a, int b)
{
	fprintf(OutFile, "\tsubq\t%s, %s\n", reglist[b], reglist[a]);
	free_reg(b);
	return a;
}

int cgmul(int a, int b)
{
	fprintf(OutFile, "\timulq\t%s, %s\n", reglist[a], reglist[b]);
	free_reg(a);
	return b;
}

int cgdiv(int a, int b)
{
	fprintf(OutFile, "\tmovq\t%s,%%rax\n", reglist[a]);
	fprintf(OutFile, "\tcqo\n");
	fprintf(OutFile, "\tidivq\t%s\n", reglist[b]);
	fprintf(OutFile, "\tmovq\t%%rax,%s\n", reglist[a]);
	free_reg(b);
	return a;
}

int cgequal(int a, int b)
{
	return cgcompare(a, b, "sete");
}

int cgnotequal(int a, int b)
{
	return cgcompare(a, b, "setne");
}

int cglessthan(int a, int b)
{
	return cgcompare(a, b, "setl");
}

int cggreaterthan(int a, int b)
{
	return cgcompare(a, b, "setg");
}

int cglessequal(int a, int b)
{
	return cgcompare(a, b, "setle");
}

int cggreaterequal(int a, int b)
{
	return cgcompare(a, b, "setge");
}

int cgcompare_and_set(int ast_op, int a, int b)
{
	if (ast_op < A_EQ || ast_op > A_GE) {
		fatal("Bad ast_op in cgcompare_and_set()");
	}

	fprintf(OutFile, "\tcmpq\t%s, %s\n", reglist[b], reglist[a]);
	fprintf(OutFile, "\t%s\t%sb\n", cmplist[ast_op - A_EQ], reglist[b]);
	fprintf(OutFile, "\tmovzbq\t%sb, %s\n", reglist[b], reglist[b]);
	free_reg(a);

	return b;
}

int cgcompare_and_jump(int ast_op, int a, int b, int label)
{
	if (ast_op < A_EQ || ast_op > A_GE) {
		fatal("Bad ast_op in cgcompare_and_jump()");
	}

	fprintf(OutFile, "\tcmpq\t%s, %s\n", reglist[b], reglist[a]);
	fprintf(OutFile, "\t%s\tL%d\n", invcmplist[ast_op - A_EQ], label);
	free_allregs();

	return NOREG;
}

int cgloadglob(char *ident)
{
	int r = alloc_reg();

	fprintf(OutFile, "\tmovq\t%s(\%%rip), %s\n", ident, reglist[r]);
	return r;
}

int cgstoreglob(int r, char *ident)
{
	fprintf(OutFile, "\tmovq\t%s, %s(\%%rip)\n", reglist[r], ident);
	return r;
}

void cgglobsym(char *sym)
{
	fprintf(OutFile, "\t.comm\t%s,8,8\n", sym);
}

void cgprintint(int r)
{
	fprintf(OutFile, "\tmovq\t%s, %%rdi\n", reglist[r]);
	fprintf(OutFile, "\tcall\tprintint\n");
	free_reg(r);
}

void gen_preamble()
{
	cgpreamble();
}

void gen_freeregs()
{
	free_allregs();
}

void gen_printint(int reg)
{
	cgprintint(reg);
}

void gen_globsym(char *s)
{
	cgglobsym(s);
}