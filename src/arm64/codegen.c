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
static char *reglist[4] = { "%r4", "%r5", "%r6", "%r7" };
static char *cmplist[] = {
	"moveq", "movne", "movlt", "movgt", "movle", "movge"
};
static char *invcmplist[] = { "movne", "moveq", "movge",
							  "movle", "movgt", "movlt" };

// P_NONE, P_VOID, P_CHAR, P_INT, P_LONG
static int psize[] = { 0, 0, 1, 4, 4 };

static int IntSlot = 0;
int IntList[MAX_INTS];

static void set_int_offset(int val)
{
	int offset = -1;
	for (int i = 0; i < IntSlot; i++) {
		if (IntList[i] == val) {
			offset = 4 * i;
			break;
		}
	}

	if (offset == -1) {
		offset = 4 * IntSlot;
		if (IntSlot == MAX_INTS) {
			fatal("Out of int slots in set_int_offset()");
			IntList[IntSlot++] = val;
		}
	}

	fprintf(OutFile, "\tldr\tr3, .L3+%d\n", offset);
}

static void set_var_offset(int id)
{
	int offset = 0;

	for (int i = 0; i < id; i++) {
		if (Gsym[i].stype == S_VAR) {
			offset += 4;
		}
	}

	fprintf(OutFile, "\tldr\tr3, .L2+%d\n", offset);
}

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
	fprintf(OutFile, ".L2:\n");
	for (int i = 0; i < Globs; i++) {
		if (Gsym[i].stype == S_VAR) {
			fprintf(OutFile, "\t.word %s\n", Gsym[i].name);
		}
	}

	fprintf(OutFile, ".L3:\n");
	for (int i = 0; i < IntSlot; i++) {
		fprintf(OutFile, "\t.word %d\n", IntList[i]);
	}
}

void cgfuncpreamble(int id)
{
	char *name = Gsym[id].name;
	fprintf(OutFile,
			"\t.text\n"
			"\t.globl\t%s\n"
			"\t.type\t%s, %%function\n"
			"%s:\n"
			"\tpush\t{fp, lr}\n"
			"\tadd\tfp, sp, #4\n"
			"\tsub\tsp, sp, #8\n"
			"\tstr\tr0, [fp, #-8]\n",
			name, name, name);
}

void cgfuncpostamble(int id)
{
	cglabel(Gsym[id].end_label);
	fprintf(OutFile, "\tsub\tsp, fp, #4\n"
					 "\tpop\t{fp, pc}\n"
					 "\t.align\t2\n");
}

int cgloadint(int val, int type)
{
	(void)type;
	int r = alloc_reg();

	if (val <= 1000) {
		fprintf(OutFile, "\tmov\t%s, #%d\n", reglist[r], val);
	} else {
		set_int_offset(val);
		fprintf(OutFile, "\tldr\t%s, [r3]\n", reglist[r]);
	}

	return r;
}

void cglabel(int l)
{
	fprintf(OutFile, "L%d:\n", l);
}

void cgjump(int l)
{
	fprintf(OutFile, "\tb\tL%d\n", l);
}

int cgadd(int a, int b)
{
	fprintf(OutFile, "\tadd\t%s, %s, %s\n", reglist[b], reglist[a], reglist[b]);
	free_reg(a);
	return b;
}

int cgsub(int a, int b)
{
	fprintf(OutFile, "\tsub\t%s, %s, %s\n", reglist[a], reglist[a], reglist[b]);
	free_reg(b);
	return a;
}

int cgmul(int a, int b)
{
	fprintf(OutFile, "\tmul\t%s, %s, %s\n", reglist[b], reglist[a], reglist[b]);
	free_reg(a);
	return b;
}

int cgdiv(int a, int b)
{
	fprintf(OutFile, "\tmov\tr0, %s\n", reglist[a]);
	fprintf(OutFile, "\tmov\tr1, %s\n", reglist[b]);
	fprintf(OutFile, "\tbl\t__aeabi_idiv\n");
	free_reg(b);
	return a;
}

int cgcompare_and_set(int ast_op, int a, int b)
{
	if (ast_op < A_EQ || ast_op > A_GE) {
		fatal("Bad ast_op in cgcompare_and_set()");
	}

	fprintf(OutFile, "\tcmp\t%s, %s\n", reglist[a], reglist[b]);
	fprintf(OutFile, "\t%s\t%s, #1\n", cmplist[ast_op - A_EQ], reglist[b]);
	fprintf(OutFile, "\t%s\t%s, #0\n", invcmplist[ast_op - A_EQ], reglist[b]);
	fprintf(OutFile, "\tuxtb\t%s, %s\n", reglist[b], reglist[b]);
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
	fprintf(OutFile, "\tmov\tr0, %s\n", reglist[r]);
	fprintf(OutFile, "\tbl\t%s\n", Gsym[id].name);
	fprintf(OutFile, "\tmov\t%s, r0\n", reglist[r]);

	return r;
}

void cgreturn(int reg, int id)
{
	fprintf(OutFile, "\tmov\tr0, %s\n", reglist[reg]);
	cgjump(Gsym[id].end_label);
}

int cgloadglob(int id)
{
	int r = alloc_reg();

	set_var_offset(id);
	fprintf(OutFile, "\ttldr\t%s, [r3]\n", reglist[r]);

	return r;
}

int cgstoreglob(int r, int id)
{
	set_var_offset(id);

	switch (Gsym[id].type) {
	case P_CHAR:
		fprintf(OutFile, "\tstrb\t%s, [r3\n", reglist[r]);
		break;
	case P_INT:
	case P_LONG:
		fprintf(OutFile, "\tstr\t%s, [r3]\n", reglist[r]);
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
	fprintf(OutFile,
			"\tmov\tr0, %s\n"
			"\tbl\tprintint\n"
			"\tnop\n",
			reglist[r]);
	free_reg(r);
}

int cgprimsize(int type)
{
	if (type < P_NONE || type > P_LONG) {
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