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

int cgload(int val);
int cgloadint(int val);
int cgadd(int a, int b);
int cgsub(int a, int b);
int cgmul(int a, int b);
int cgdiv(int a, int b);
int cgloadglob(char *ident);
int cgstoreglob(int r, char *ident);

void gen_preamble();
void gen_postamble();
void gen_freeregs();
void gen_printint(int a);
void gen_globsym(char *s);

#endif /* __CODEGEN_H_ */