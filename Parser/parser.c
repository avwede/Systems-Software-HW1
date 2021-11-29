#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 1000
#define MAX_SYMBOL_COUNT 100

instruction *code;
int cIndex = 0;
symbol *table;
int tIndex = 0;
int lIndex = 0;
int currLevel = 0;
int earlyHalt = 0;

void program(lexeme *list);
void block(lexeme *list);
void constDeclaration(lexeme *list);
void procedureDeclaration(lexeme *list);
void term(lexeme *list);
void factor(lexeme *list);
void mark();
int varDeclaration(lexeme *list);
int multipleDeclarationCheck(lexeme token);
void statement(lexeme *list);
void expression(lexeme *list);
void condition(lexeme *list);
int findSymbol(lexeme symbol, int x);
void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();

instruction *parse(lexeme *list, int printTable, int printCode)
{
	code = malloc(MAX_CODE_LENGTH * sizeof(instruction));
	table = malloc(MAX_SYMBOL_COUNT * sizeof(symbol));

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
	int i;

	// emit JMP, the jump address is fixed later.
	emit(7, 0, 0);
	addToSymbolTable(3, "main", 0, 0, 0, 0);

	currLevel = -1;

	block(list);

	if (earlyHalt)
	{
		return;
	}

	// Check if the code ends with a period.
	if (list[lIndex].type != periodsym)
	{
		printparseerror(1);
		earlyHalt = 1;
		return;
	}

	// emit HALT
	emit(9, 0, 3);

	// Go through each line of code.
	for (i = 0; i < cIndex; i++)
	{
		// Line has OPR 5 (CALL)
		if (code[i].opcode == 5)
		{
			code[i].m = table[code[i].m].addr;
		}
	}

	// Fix JMP address.
	code[0].m = table[0].addr;
}

void block(lexeme *list)
{
	currLevel++;
	int procedure_idx = tIndex - 1;
	constDeclaration(list);


	int x = varDeclaration(list);


	procedureDeclaration(list);

	table[procedure_idx].addr = cIndex * 3;

	if (currLevel == 0)
	{
		emit(6, 0, x);
	}
	else
	{
		emit(6, 0, x + 3);
	}

	statement(list);


	mark();
	currLevel--;
}

// Constant declarations should follow the pattern 'ident := number {, ident := number}
void constDeclaration(lexeme *list)
{
	int symidx = 0;

	if (list[lIndex].type == constsym)
	{
		do
		{
			lIndex++;
			if (list[lIndex].type != identsym)
			{
				// Const must have an identifier after it.
				printparseerror(2);
				earlyHalt = 1;
				return;
			}

			symidx = multipleDeclarationCheck(list[lIndex]);
			if (symidx != -1)
			{
				// Declaration already found in symbol tree.
				printparseerror(18);
				earlyHalt = 1;
				return;
			}

			char savedName[MAX_SYMBOL_COUNT];
			strcpy(savedName, list[lIndex].name);

			lIndex++;
			if (list[lIndex].type != assignsym)
			{
				// Identifier must have := after it.
				printparseerror(2);
				earlyHalt = 1;
				return;
			}

			lIndex++;

			if (list[lIndex].type != numbersym)
			{
				// := must have a number after it.
				printparseerror(2);
				earlyHalt = 1;
				return;
			}

			addToSymbolTable(1, savedName, list[lIndex].value, currLevel, 0, 0);

			lIndex++;

		} while (list[lIndex].type == commasym);

		if (list[lIndex].type != semicolonsym)
		{
			if (list[lIndex].type == identsym)
			{
				printparseerror(13);
				earlyHalt = 1;
				return;
			}
			else
			{
				printparseerror(14);
				earlyHalt = 1;
				return;
			}
		}

		lIndex++;
	}
}

int varDeclaration(lexeme *list)
{
	int numVars = 0;
	if (list[lIndex].type == varsym)
	{
		do
		{
			numVars++;
			lIndex++;

			if (list[lIndex].type != identsym)
			{
				printparseerror(3);
				earlyHalt = 1;
				return -1;
			}

			int symidx = multipleDeclarationCheck(list[lIndex]);

			if (symidx != -1)
			{
				printparseerror(18);
				earlyHalt = 1;
				return -1;
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
				return -1;
			}
			else
			{
				printparseerror(14);
				earlyHalt = 1;
				return -1;
			}
		}

		lIndex++;
	}

	return numVars;
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
			return;
		}

		symidx = multipleDeclarationCheck(list[lIndex]);

		if (symidx != -1)
		{
			// Declaration already found in symbol tree.
			printparseerror(18);
		}

		addToSymbolTable(3, list[lIndex].name, 0, currLevel, 0, 0);

		lIndex += 1;

		if (list[lIndex].type != semicolonsym)
		{
			// Not following grammar: ident ';'
			printparseerror(4);
			earlyHalt = 1;
			return;
		}

		lIndex += 1;

		block(list);

		if (list[lIndex].type != semicolonsym)
		{
			// Symbol declarations should close with a semicolon.
			printparseerror(14);
			earlyHalt = 1;
			return;
		}

		lIndex += 1;

		// emit RTN
		emit(2, 0, 0);
	}
}

void statement(lexeme *list)
{
	if (list[lIndex].type == identsym)
	{
		int symIdx = findSymbol(list[lIndex], 2);

		if (symIdx == -1)
		{
			if (findSymbol(list[lIndex], 1) != findSymbol(list[lIndex], 3))
			{
				// Identifier is not a variable?
				printparseerror(6);
				earlyHalt = 1;
				return;
			}
			else
			{
				// Undeclared identifier?
				printparseerror(19);
				earlyHalt = 1;
				return;
			}
		}

		lIndex++;

		if (list[lIndex].type != assignsym)
		{
			// Ident must be followed by :=
			printparseerror(7);
			earlyHalt = 1;
			return;
		}

		lIndex++;

		expression(list);

		if (earlyHalt)
		{
			return;
		}

		emit(4, currLevel - table[symIdx].level, table[symIdx].addr);

		return;
	}

	if (list[lIndex].type == beginsym)
	{
		do
		{
			lIndex++;

			statement(list);

			if (earlyHalt)
			{
				return;
			}

		} while (list[lIndex].type == semicolonsym);

		if (list[lIndex].type != endsym)
		{
			if (list[lIndex].type == identsym || list[lIndex].type == beginsym || list[lIndex].type == ifsym || list[lIndex].type == whilesym || list[lIndex].type == readsym || list[lIndex].type == writesym || list[lIndex].type == callsym)
			{
				// End symbol expected, following above found instead.
				printparseerror(15);
				earlyHalt = 1;
				return;
			}

			else
			{
				// End symbol expected, following above not found.
				printparseerror(16);
				earlyHalt = 1;
				return;
			}
		}

		lIndex++;
		return;
	}

	if (list[lIndex].type == ifsym)
	{
		lIndex++;

		condition(list);

		if (earlyHalt)
		{
			return;
		}

		int jpcIdx = cIndex;
		emit(8, 0, 0);

		if (list[lIndex].type != thensym)
		{
			// If must be followed by then
			printparseerror(8);
			earlyHalt = 1;
			return;
		}

		lIndex++;

		statement(list);

		if (earlyHalt)
		{
			return;
		}

		if (list[lIndex].type == elsesym)
		{
			int jmpIdx = cIndex;
			emit(7, 0, 0);
			code[jpcIdx].m = cIndex * 3;

			lIndex++;

			statement(list);

			if (earlyHalt)
			{
				return;
			}

			code[jmpIdx].m = cIndex * 3;
		}
		else
		{
			code[jpcIdx].m = cIndex * 3;
		}

		return;
	}

	if (list[lIndex].type == whilesym)
	{
		lIndex++;

		int loopIdx = cIndex;
		condition(list);

		if (list[lIndex].type != dosym)
		{
			// While must be followed by do
			printparseerror(9);
			earlyHalt = 1;
			return;
		}

		lIndex++;

		int jpcIdx = cIndex;
		emit(8, 0, 0);
		statement(list);

		if (earlyHalt)
		{
			return;
		}

		emit(7, 0, loopIdx * 3);
		code[jpcIdx].m = cIndex * 3;

		return;
	}
	if (list[lIndex].type == readsym)
	{
		lIndex++;

		if (list[lIndex].type != identsym)
		{
			// Identifier is missing
			printparseerror(6);
			earlyHalt = 1;
			return;
		}

		int symIdx = findSymbol(list[lIndex], 2);

		if (symIdx == -1)
		{
			if (findSymbol(list[lIndex], 1) != findSymbol(list[lIndex], 3))
			{
				// Identifier present is not a variable?
				printparseerror(6);
				earlyHalt = 1;
				return;
			}
			else
			{
				// Undeclared identifier?
				printparseerror(19);
				earlyHalt = 1;
				return;
			}
		}

		lIndex++;

		emit(9, 0, 2);
		emit(4, currLevel - table[symIdx].level, table[symIdx].addr);

		return;
	}
	if (list[lIndex].type == writesym)
	{
		lIndex++;

		expression(list);

		if (earlyHalt)
		{
			return;
		}

		emit(9, 0, 1);

		return;
	}
	if (list[lIndex].type == callsym)
	{
		lIndex++;

		int symIdx = findSymbol(list[lIndex], 3);

		if (symIdx == -1)
		{
			if (findSymbol(list[lIndex], 1) != findSymbol(list[lIndex], 2))
			{
				// Not a procedure?
				printparseerror(7);
				earlyHalt = 1;
				return;
			}
			else
			{
				// Undeclared identifier?
				printparseerror(19);
				earlyHalt = 1;
				return;
			}
		}

		lIndex++;

		emit(5, currLevel - table[symIdx].level, symIdx);
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

void expression(lexeme *list)
{
	if (list[lIndex].type == subsym)
	{
		lIndex++;

		term(list);

		if (earlyHalt)
		{
			return;
		}

		emit(2, currLevel, 1); // Emit NEG
		while (list[lIndex].type == addsym || list[lIndex].type == subsym)
		{
			if (list[lIndex].type == addsym)
			{
				lIndex++;
				term(list);

				if (earlyHalt)
				{
					return;
				}

				emit(2, 0, 2); // Emit ADD
			}
			else
			{
				lIndex++;

				term(list);

				if (earlyHalt)
				{
					return;
				}

				emit(2, 0, 3); // Emit SUB
			}
		}
	}

	else
	{
		if (list[lIndex].type == addsym)
		{
			lIndex++;
		}

		term(list);

		if (earlyHalt)
		{
			return;
		}

		while (list[lIndex].type == addsym || list[lIndex].type == subsym)
		{
			if (list[lIndex].type == addsym)
			{
				lIndex++;

				term(list);

				if (earlyHalt)
				{
					return;
				}

				emit(2, 0, 2); // Emit ADD
			}
			else
			{
				lIndex++;

				term(list);

				if (earlyHalt)
				{
					return;
				}

				emit(2, currLevel, 3); // Emit SUB
			}
		}
	}

	if (list[lIndex].type == lparensym || list[lIndex].type == identsym ||
		list[lIndex].type == numbersym || list[lIndex].type == oddsym)
	{
		printparseerror(17);
		earlyHalt = 1;
		return;
	}
}

void condition(lexeme *list)
{
	if (list[lIndex].type == oddsym)
	{
		lIndex++;

		expression(list);

		if (earlyHalt)
		{
			return;
		}

		emit(2, 0, 6); // Emit ODD
	}
	else
	{
		expression(list);

		if (earlyHalt)
		{
			return;
		}

		if (list[lIndex].type == eqlsym)
		{
			lIndex++;

			expression(list);

			if (earlyHalt)
			{
				return;
			}

			emit(2, 0, 8); // Emit EQL
		}
		else if (list[lIndex].type == neqsym)
		{
			lIndex++;

			expression(list);

			if (earlyHalt)
			{
				return;
			}

			emit(2, 0, 9); // Emit NEQ
		}
		else if (list[lIndex].type == lsssym)
		{
			lIndex++;

			expression(list);

			if (earlyHalt)
			{
				return;
			}

			emit(2, 0, 10); // Emit LSS
		}
		else if (list[lIndex].type == leqsym)
		{
			lIndex++;

			expression(list);

			if (earlyHalt)
			{
				return;
			}

			emit(2, 0, 11); // Emit LEQ
		}
		else if (list[lIndex].type == gtrsym)
		{
			lIndex++;

			expression(list);

			if (earlyHalt)
			{
				return;
			}

			emit(2, 0, 12); // Emit GTR
		}
		else if (list[lIndex].type == geqsym)
		{
			lIndex++;

			expression(list);

			if (earlyHalt)
			{
				return;
			}

			emit(2, 0, 13); // Emit GEQ
		}
		else
		{
			// Relational operator missing from condition
			printparseerror(10);
			earlyHalt = 1;
			return;
		}
	}
}

void term(lexeme *list)
{
	factor(list);

	if (earlyHalt)
	{
		return;
	}

	while (list[lIndex].type == multsym || list[lIndex].type == divsym || list[lIndex].type == modsym)
	{
		if (earlyHalt)
		{
			return;
		}

		if (list[lIndex].type == multsym)
		{
			lIndex++;

			factor(list);

			if (earlyHalt)
			{
				return;
			}

			emit(2, 0, 4);
		}

		else if (list[lIndex].type == divsym)
		{
			lIndex++;

			factor(list);

		if (earlyHalt)
		{
			return;
		}

			emit(2, 0, 5);
		}
		else
		{
			lIndex++;

			factor(list);

			if (earlyHalt)
			{
				return;
			}

			emit(2, 0, 7);
		}
	}
}

void factor(lexeme *list)
{
	int symIdx_var, symIdx_const;
	int L, M;

	if (list[lIndex].type == identsym)
	{
		symIdx_var = findSymbol(list[lIndex], 2);
		symIdx_const = findSymbol(list[lIndex], 1);

		if (symIdx_var == -1 && symIdx_const == -1)
		{
			if (findSymbol(list[lIndex], 3) == -1)
			{
				// Identifier not found unmarked, undeclared identifier.
				printparseerror(19);
				earlyHalt = 1;
				return;
			}

			else
			{
				// Identifier is a procedure
				printparseerror(11);
				earlyHalt = 1;
				return;
			}
		}

		if (symIdx_var == -1) // const
		{
			// emit LIT
			M = table[symIdx_const].val;
			emit(1, 0, M);
		}

		else if (symIdx_const == -1 || table[symIdx_var].level > table[symIdx_const].level)
		{
			// emit LOD
			L = currLevel - table[symIdx_var].level;
			M = table[symIdx_var].addr;
			emit(3, L, M);
		}

		else
		{
			// emit LIT
			M = table[symIdx_const].val;
			emit(1, 0, M);
		}

		lIndex += 1;
	}

	else if (list[lIndex].type == numbersym)
	{
		// emit LIT (What is M? Is it the number?)
		emit(1, 0, list[lIndex].value);
		lIndex += 1;
	}

	else if (list[lIndex].type == lparensym)
	{
		lIndex += 1;

		expression(list);

		if (earlyHalt)
		{
			return;
		}

		if (list[lIndex].type != rparensym)
		{
			// '(' must be followed by ')'
			printparseerror(12);
			earlyHalt = 1;
			return;
		}

		lIndex += 1;

	}

	else
	{
		// Symbol is neither a number, identifier, nor a ( smybol
		printparseerror(11);
		earlyHalt = 1;
		return;
	}
}

// Finds the correct symbol name with a linear search.
int findSymbol(lexeme symbol, int kind)
{
	int i, retVal = -1, maxLevel = -1;

	for (i = 0; i < tIndex; i++)
	{
		// Check if the entry has the correct name.
		if (strcmp(symbol.name, table[i].name) == 0)
		{
			// Check if the entry has the correct kind value and is unmarked and maximize the level.
			if (kind == table[i].kind && table[i].mark == 0 && table[i].level > maxLevel)
			{
				retVal = i;
				maxLevel = table[i].level;
			}
		}
	}
	return retVal;
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