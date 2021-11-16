#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 1000
#define MAX_SYMBOL_COUNT 100

instruction *code;
int cIndex = 0;
symbol *table;
int tIndex;
int lIndex;
int currLevel = 0;
int earlyHalt = 0;

void program(lexeme *list);
void block(lexeme *list);
void procedureDeclaration(lexeme *list);
void term(lexeme *list);
void factor(lexeme *list);
void varDeclaration(lexeme *list);
void statement(lexeme *list);
void expression(lexeme *list);
void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();

instruction *parse(lexeme *list, int printTable, int printCode)
{
	code = NULL;

	program(list);

	if (earlyHalt)
	{
		return NULL;
	}

	code[cIndex].opcode = -1;
	if (printTable)
	{
		printsymboltable();
	}

	if (printCode)
	{
		printassemblycode();
	}

	return code;
}

void program(lexeme *list)
{
	int codeLen;
	cIndex = 0;
	lIndex = 0;

	// emit JMP, the jump address is fixed later.
	emit(7, 0, 0);
	addToSymbolTable(3, "main", 0, 0, 0, 0);

	currLevel = -1;

	// BLOCK FUNCTION
	block(list);

	if (earlyHalt)
	{
		return NULL;
	}

	// Check if the code ends with a period.
	if (list[lIndex].type != periodsym)
	{
		printparseerror(1);
		earlyHalt = 1;
		return NULL;
	}

	// emit HALT
	emit(9, 0, 3);

	// Go through each line of code.
	codeLen = len(code);
	for (cIndex = 0; cIndex < codeLen; cIndex++)
	{
		// Line has OPR 5 (CALL)
		if (code[cIndex].opcode == 5)
		{
			code[cIndex].m = table[code[cIndex].m].addr;
		}
	}

	// Fix JMP address.
	code[0].m = table[0].addr;
}

void block(lexeme *list)
{
	currLevel++;
	int procedure_idx = tIndex - 1;
	const_declaration();
	int x = var_declaration();
	procedure_declaration();
	table[procedure_idx].addr = cIndex * 3;

	if (currLevel == 0)
	{
		emit(6, currLevel, x);
	}
	else
	{
		emit(6, currLevel, x + 3);
	}
	statement(list);
	mark();
	currLevel--;
}

void statement(lexeme *list)
{
	// STATEMENT
}

void procedureDeclaration(lexeme *list)
{
	int symidx;

	while (list[lIndex].type == procsym)
	{
		lIndex += 1;

		if (list[lIndex].type != identsym)
		{
			// Not following grammar: ident ';'
			printparseerror(4);
			earlyHalt = 1;
			return NULL;
		}

		symidx = multipleDeclarationCheck(list[lIndex]);

		if (symidx != -1)
		{
			// Declaration already found in symbol tree.
			printparseerror(18);
		}

		lIndex += 1;

		if (list[lIndex].type != semicolonsym)
		{
			// Not following grammar: ident ';'
			printparseerror(4);
			earlyHalt = 1;
			return NULL;
		}

		lIndex += 1;

		// BLOCK FUNCTION

		if (list[lIndex].type != semicolonsym)
		{
			// Symbol declarations should close with a semicolon.S
			printparseerror(14);
			earlyHalt = 1;
			return NULL;
		}

		lIndex += 1;

		// emit RTN
		emit(2, 0, 0);
	}
}
void mark()
{
	int i;
	for (i = tIndex; i >= 0; i--)
	{
		if (table[i].mark == 0)
		{
			if (table[i].level == currLevel)
				table[i].mark = 1;

			else if (table[i].level < currLevel)
				return;
		}
	}
}

int multipleDeclarationCheck(lexeme l)
{
	int i;

	for (i = 0; i < tIndex; i++)
	{
		if (strcmp(table[i].name, l.name) == 0 && table[i].mark == 0 && table[i].level == currLevel)
			return i;
	}

	return -1;
}

void varDeclaration(lexeme *list)
{
	int numVars = 0;
	if (list[lIndex].type == varsym)
		do
		{
			numVars++;
			lIndex++;

			if (list[lIndex].type != identsym)
			{
				printparseerror(3);
				earlyHalt = 1;
				return NULL;
			}

			int symidx = multipleDeclarationCheck(token);

			if (symidx != -1)
			{
				printparseerror(18);
				earlyHalt = 1;
				return NULL;
			}

			if (currLevel == 0)
			{
				addToSymbolTable(2, list[lIndex].name, 0, currLevel, numVars - 1, 0);
			}
			else
			{
				addToSymbolTable(2, list[lIndex].name, 0, currLevel, numVars + 2, 0);
			}
			lIndex++;
		} while (list[lIndex].type == commasym);

	if (list[lIndex].type != semicolonsym)
	{
		if (list[lIndex].type == identsym)
		{
			printparseerror(13);
			earlyHalt = 1;
			return NULL;
		}
		else
		{
			printparseerror(14);
			earlyHalt = 1;
			return NULL;
		}
	}

	lIndex++;

	return numVars;
}

void expression(lexeme *list)
{
	if (list[lIndex].type == subsym)
	{
		lIndex++;
		term(list);
		emit(2, currLevel, 1); // Emit NEG
		while (list[lIndex].type == addsym || list[lIndex].type == subsym)
		{
			if (list[lIndex].type == addsym)
			{
				lIndex++;
				term(list);
				emit(2, currLevel, 2); // Emit ADD
			}
			else
			{
				lIndex++;
				term(list);
				emit(2, currLevel, 3); // Emit SUB
			}
		}
	}

	else
	{
		if (list[lIndex].type == addsym)
			lIndex++;
		term(list);
		while (list[lIndex].type == addsym || list[lIndex].type == subsym)
		{
			if (list[lIndex].type == addsym)
			{
				lIndex++;
				term(list);
				emit(2, currLevel, 2); // Emit ADD
			}
			else
			{
				lIndex++;
				term(list);
				emit(2, currLevel, 3); // Emit SUB
			}
		}
	}

	if (list[lIndex].type == lparensym || list[lIndex].type == identsym ||
			list[lIndex].type == numbersym || list[lIndex].type == oddsym)
	{
		printparseerror(17);
		earlyHalt = 1;
		return NULL;
	}
}

void term(lexeme *list)
{
	lexeme token = list[lIndex];

	factor(list);

	while (token.type == multsym || token.type == divsym || token.type == modsym)
	{
		lIndex++;
		token = list[lIndex];
		factor(list);

		if (token.type == multsym)
		{
			emit(2, currLevel, 4);
		}
		else if (token.type == divsym)
		{
			emit(2, currLevel, 5);
		}
		else
		{
			emit(2, currLevel, 7);
		}
	}
}

void emit(int opname, int level, int mvalue)
{
	code[cIndex].opcode = opname;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}

void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].val = v;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
}

void printparseerror(int err_code)
{
	switch (err_code)
	{
	case 1:
		printf("Parser Error: Program must be closed by a period\n");
		break;
	case 2:
		printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
		break;
	case 3:
		printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
		break;
	case 4:
		printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
		break;
	case 5:
		printf("Parser Error: Variables must be assigned using :=\n");
		break;
	case 6:
		printf("Parser Error: Only variables may be assigned to or read\n");
		break;
	case 7:
		printf("Parser Error: call must be followed by a procedure identifier\n");
		break;
	case 8:
		printf("Parser Error: if must be followed by then\n");
		break;
	case 9:
		printf("Parser Error: while must be followed by do\n");
		break;
	case 10:
		printf("Parser Error: Relational operator missing from condition\n");
		break;
	case 11:
		printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
		break;
	case 12:
		printf("Parser Error: ( must be followed by )\n");
		break;
	case 13:
		printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
		break;
	case 14:
		printf("Parser Error: Symbol declarations should close with a semicolon\n");
		break;
	case 15:
		printf("Parser Error: Statements within begin-end must be separated by a semicolon\n");
		break;
	case 16:
		printf("Parser Error: begin must be followed by end\n");
		break;
	case 17:
		printf("Parser Error: Bad arithmetic\n");
		break;
	case 18:
		printf("Parser Error: Confliciting symbol declarations\n");
		break;
	case 19:
		printf("Parser Error: Undeclared identifier\n");
		break;
	default:
		printf("Implementation Error: unrecognized error code\n");
		break;
	}

	free(code);
	free(table);
}

void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark);

	free(table);
	table = NULL;
}

void printassemblycode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		switch (code[i].opcode)
		{
		case 1:
			printf("LIT\t");
			break;
		case 2:
			switch (code[i].m)
			{
			case 0:
				printf("RTN\t");
				break;
			case 1:
				printf("NEG\t");
				break;
			case 2:
				printf("ADD\t");
				break;
			case 3:
				printf("SUB\t");
				break;
			case 4:
				printf("MUL\t");
				break;
			case 5:
				printf("DIV\t");
				break;
			case 6:
				printf("ODD\t");
				break;
			case 7:
				printf("MOD\t");
				break;
			case 8:
				printf("EQL\t");
				break;
			case 9:
				printf("NEQ\t");
				break;
			case 10:
				printf("LSS\t");
				break;
			case 11:
				printf("LEQ\t");
				break;
			case 12:
				printf("GTR\t");
				break;
			case 13:
				printf("GEQ\t");
				break;
			default:
				printf("err\t");
				break;
			}
			break;
		case 3:
			printf("LOD\t");
			break;
		case 4:
			printf("STO\t");
			break;
		case 5:
			printf("CAL\t");
			break;
		case 6:
			printf("INC\t");
			break;
		case 7:
			printf("JMP\t");
			break;
		case 8:
			printf("JPC\t");
			break;
		case 9:
			switch (code[i].m)
			{
			case 1:
				printf("WRT\t");
				break;
			case 2:
				printf("RED\t");
				break;
			case 3:
				printf("HAL\t");
				break;
			default:
				printf("err\t");
				break;
			}
			break;
		default:
			printf("err\t");
			break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	if (table != NULL)
		free(table);
}