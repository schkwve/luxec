/**
 * LuxeC (c) 2023 by Jozef Nagy
 *
 * LuxeC is licensed under a
 * Creative Commons Attribution-NoDerivatives 4.0 International License.
 *
 * You should have received a copy of the license along with this
 * work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
 */

#ifndef __DATA_H_
#define __DATA_H_

#ifndef extern_
#define extern_ extern
#endif

#define TEXTLEN 512
#define SYM_COUNT 1024

extern_ struct symtable Gsym[SYM_COUNT]; // global symbol table
extern_ char Text[TEXTLEN + 1];
extern_ int Line;
extern_ int Putback;
extern_ int Globs;
extern_ FILE *InFile;
extern_ FILE *OutFile;
extern_ struct token Token;
extern_ int FuncId;

#endif /* __DATA_H_ */