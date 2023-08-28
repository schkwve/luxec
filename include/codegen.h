/**
 * LuxeC (c) 2023 by Jozef Nagy
 *
 * LuxeC is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __CODEGEN_H_
#define __CODEGEN_H_

#define MAX_INTS 1024

void cgpreamble(void);
void cgpostamble(void);

void cgfuncpreamble(int id);
void cgfuncpostamble(int id);

int cgaddress(int id);
int cgderef(int r, int type);

int cgload(int val);
int cgloadint(int val, int type);

void cglabel(int l);
void cgjump(int l);

int cgadd(int a, int b);
int cgsub(int a, int b);
int cgmul(int a, int b);
int cgdiv(int a, int b);

int cgequal(int a, int b);
int cgnotequal(int a, int b);
int cglessthan(int a, int b);
int cggreaterthan(int a, int b);
int cglessequal(int a, int b);
int cggreaterequal(int a, int b);

int cgcompare_and_set(int ast_op, int a, int b);
int cgcompare_and_jump(int ast_op, int a, int b, int label);

int cgcall(int r, int id);
void cgreturn(int reg, int id);

int cgloadglob(int id);
int cgstoreglob(int r, int id);

int cgprimsize(int type);
int cgwiden(int r, int old_type, int new_type);

void gen_preamble();
void gen_postamble();
void gen_freeregs();
void gen_printint(int a);
void gen_globsym(int id);
int gen_label(void);
int gen_primesize(int type);

#endif /* __CODEGEN_H_ */