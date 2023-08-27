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

// P_NONE, P_VOID, P_CHAR, P_INT, P_LONG
static int psize[] = { 0, 0, 1, 4, 8 };

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

void cgpreamble(void)
{
	free_allregs();
	fprintf(OutFile, "\t.text\n");
}

void cgpostamble(void)
{
}

void cgfuncpreamble(int id)
{
	char *name = Gsym[id].name;
	fprintf(OutFile,
			"\t.text\n"
			"\t.globl\t%s\n"
			"\t.type\t%s, @function\n"
			"%s:\n"
			"\tpushq\t%%rbp\n"
			"\tmovq	%%rsp, %%rbp\n",
			name, name, name);
}

void cgfuncpostamble(int id)
{
	cglabel(Gsym[id].end_label);
	fprintf(OutFile, "\tpopq %%rbp\n"
					 "\tret\n");
}

int cgaddress(int id)
{
	int r = alloc_reg();
	fprintf(OutFile, "\tleaq\t%s(%%rip), %s\n", Gsym[id].name, reglist[r]);
	return r;
}

int cgderef(int r, int type)
{
	switch (type) {
	case P_CHARPTR:
		fprintf(OutFile, "\tmovzbq\t(%s), %s\n", reglist[r], reglist[r]);
		break;
	case P_INTPTR:
	case P_LONGPTR:
		fprintf(OutFile, "\tmovq\t(%s), %s\n", reglist[r], reglist[r]);
		break;
	}

	return r;
}

int cgloadint(int val, int type)
{
	(void)type;
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

int cgcall(int r, int id)
{
	int outr = alloc_reg();

	fprintf(OutFile, "\tmovq\t%s, %%rdi\n", reglist[r]);
	fprintf(OutFile, "\tcall\t%s\n", Gsym[id].name);
	fprintf(OutFile, "\tmovq\t%%rax, %s\n", reglist[outr]);
	free_reg(r);

	return outr;
}

void cgreturn(int reg, int id)
{
	switch (Gsym[id].type) {
	case P_CHAR:
		fprintf(OutFile, "\tmovzbl\t%sb, %%eax\n", reglist[reg]);
		break;
	case P_INT:
		fprintf(OutFile, "\tmovl\t%sd, %%eax\n", reglist[reg]);
		break;
	case P_LONG:
		fprintf(OutFile, "\tmovq\t%s, %%rip\n", reglist[reg]);
		break;
	default:
		fatald("Bad function type in cgreturn:", Gsym[id].type);
	}

	cgjump(Gsym[id].end_label);
}

int cgloadglob(int id)
{
	int r = alloc_reg();

	switch (Gsym[id].type) {
	case P_CHAR:
		fprintf(OutFile, "\tmovzbq\t%s(\%%rip), %s\n", Gsym[id].name,
				reglist[r]);
		break;
	case P_INT:
		fprintf(OutFile, "\tmovzb\t%s(\%%rip), %s\n", Gsym[id].name,
				reglist[r]);
		break;
	case P_LONG:
	case P_CHARPTR:
	case P_INTPTR:
	case P_LONGPTR:
		fprintf(OutFile, "\tmovq\t%s(\%%rip), %s\n", Gsym[id].name, reglist[r]);
		break;
	default:
		fatald("Bad type in cgloadglob:", Gsym[id].type);
	}
	return r;
}

int cgstoreglob(int r, int id)
{
	switch (Gsym[id].type) {
	case P_CHAR:
		fprintf(OutFile, "\tmovb\t%sb, %s(\%%rip)\n", reglist[r],
				Gsym[id].name);
		break;
	case P_INT:
		fprintf(OutFile, "\tmovl\t%sd, %s(\%%rip)\n", reglist[r],
				Gsym[id].name);
		break;
	case P_LONG:
	case P_CHARPTR:
	case P_INTPTR:
	case P_LONGPTR:
		fprintf(OutFile, "\tmovq\t%s, %s(\%%rip)\n", reglist[r], Gsym[id].name);
		break;
	default:
		fatald("Bad type in cgstoreglob:", Gsym[id].type);
	}

	return r;
}

void cgglobsym(int id)
{
	int type_size = cgprimsize(Gsym[id].type);
	fprintf(OutFile, "\t.comm\t%s,%d,%d\n", Gsym[id].name, type_size,
			type_size);
}

void cgprintint(int r)
{
	fprintf(OutFile, "\tmovq\t%s, %%rdi\n", reglist[r]);
	fprintf(OutFile, "\tcall\tprintint\n");
	free_reg(r);
}

int cgprimsize(int type)
{
	if (type < P_NONE || type > P_LONGPTR) {
		fatal("Bad type in cgprimsize()");
	}

	return psize[type];
}

int cgwiden(int r, int old_type, int new_type)
{
	(void)old_type;
	(void)new_type;
	return r;
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

void gen_globsym(int id)
{
	cgglobsym(id);
}

int gen_primesize(int type)
{
	return cgprimsize(type);
}