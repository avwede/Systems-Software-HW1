// Systems Software Fall 2021
// Group 5 - Nicholas Gray, Abraham Hernandez, Prathik Ramesh, Ashley Voglewede

/*
	This is the lex.c file for the UCF Fall 2021 Systems Software Project.
	For HW2, you must implement the function lexeme *lexanalyzer(char *input).
	You may add as many constants, global variables, and support functions
	as you desire.

	If you choose to alter the printing functions or delete list or lex_index,
	you MUST make a note of that in you readme file, otherwise you will lose
	5 points.
*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#define MAX_NUMBER_TOKENS 500
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5

// struct lexeme {
//     int type;
//     char value[MAX_NUMBER_LEN];
//     char name[MAX_IDENT_LEN]; 
// };

// typedef enum {
// constsym = 1, varsym, procsym, beginsym, endsym, whilesym, dosym, ifsym, thensym, 
// elsesym, callsym, writesym, readsym, identsym, numbersym, assignsym, addsym, 
// subsym, multsym, divsym, modsym, eqlsym, neqsym, lsssym, leqsym, gtrsym, geqsym, 
// oddsym, lparensym, rparensym, commasym, periodsym, semicolonsym
// } token_type;

lexeme *list;
int lex_index;

void printlexerror(int type);
void printtokens();

lexeme *lexanalyzer(char *input)
{
	int i, code_len, in_comment, num_len, iden_len;
	char buffer[12];

	// Initialize lexeme array
	list = (lexeme *)calloc(MAX_NUMBER_TOKENS, sizeof(lexeme));
	lex_index = 0;
	code_len = strlen(input);

	// TODO: Proper parsing.
	// We need to parse from special symbols, and parse for indentifiers and numbers.
	// Maybe make special functions for checking if a word (in buffer) is reserved or
	// the symbol is special.
	for (i = 0; i < code_len; i++)
	{
		// Ignore if in a comment, until we hit a newline.
		if (in_comment)
		{
			if (input[i] == '\n')
			{
				in_comment = 0;
			}
			
			continue;
		}
		
		// Ignore whitespace.
		if (isspace(input[i]) || iscntrl(input[i]))
		{
			continue;
		}

		
		if (isdigit(input[i]))
		{
			;
		}

		if (isalpha(input[i]))
		{
			;
		}
	}

 
	printtokens();
	return list;
}

int isReserved(char* token)
{
	return 0;
}

void parseSpecialSymbols(char ch)
{
	// switch(input[i])
	// {
	// 	case '==':
	// 	case '!=':
	// 	case '<':
	// 	case '<=':
	// 	case '>':
	// 	case '>=':
	// 	case '%':
	// 	case '*':
	// 	case '/':
	// 	case '+':
	// 	case '-':
	// 	case '(':
	// 	case ')':
	// 	case ',':
	// 	case '.':
	// 	case ';':
	// }
}

void parseReservedWords(){

}

void parseNumbers(){

}


void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
		case oddsym:
			printf("%11s\t%d", "odd", oddsym);
			break;
		case eqlsym:
			printf("%11s\t%d", "==", eqlsym);
			break;
		case neqsym:
			printf("%11s\t%d", "!=", neqsym);
			break;
		case lsssym:
			printf("%11s\t%d", "<", lsssym);
			break;
		case leqsym:
			printf("%11s\t%d", "<=", leqsym);
			break;
		case gtrsym:
			printf("%11s\t%d", ">", gtrsym);
			break;
		case geqsym:
			printf("%11s\t%d", ">=", geqsym);
			break;
		case modsym:
			printf("%11s\t%d", "%", modsym);
			break;
		case multsym:
			printf("%11s\t%d", "*", multsym);
			break;
		case divsym:
			printf("%11s\t%d", "/", divsym);
			break;
		case addsym:
			printf("%11s\t%d", "+", addsym);
			break;
		case subsym:
			printf("%11s\t%d", "-", subsym);
			break;
		case lparensym:
			printf("%11s\t%d", "(", lparensym);
			break;
		case rparensym:
			printf("%11s\t%d", ")", rparensym);
			break;
		case commasym:
			printf("%11s\t%d", ",", commasym);
			break;
		case periodsym:
			printf("%11s\t%d", ".", periodsym);
			break;
		case semicolonsym:
			printf("%11s\t%d", ";", semicolonsym);
			break;
		case assignsym:
			printf("%11s\t%d", ":=", assignsym);
			break;
		case beginsym:
			printf("%11s\t%d", "begin", beginsym);
			break;
		case endsym:
			printf("%11s\t%d", "end", endsym);
			break;
		case ifsym:
			printf("%11s\t%d", "if", ifsym);
			break;
		case thensym:
			printf("%11s\t%d", "then", thensym);
			break;
		case elsesym:
			printf("%11s\t%d", "else", elsesym);
			break;
		case whilesym:
			printf("%11s\t%d", "while", whilesym);
			break;
		case dosym:
			printf("%11s\t%d", "do", dosym);
			break;
		case callsym:
			printf("%11s\t%d", "call", callsym);
			break;
		case writesym:
			printf("%11s\t%d", "write", writesym);
			break;
		case readsym:
			printf("%11s\t%d", "read", readsym);
			break;
		case constsym:
			printf("%11s\t%d", "const", constsym);
			break;
		case varsym:
			printf("%11s\t%d", "var", varsym);
			break;
		case procsym:
			printf("%11s\t%d", "procedure", procsym);
			break;
		case identsym:
			printf("%11s\t%d", list[i].name, identsym);
			break;
		case numbersym:
			printf("%11d\t%d", list[i].value, numbersym);
			break;
		}
		printf("\n");
	}
	printf("\n");
	printf("Token List:\n");
	for (i = 0; i < lex_index; i++)
	{
		if (list[i].type == numbersym)
			printf("%d %d ", numbersym, list[i].value);
		else if (list[i].type == identsym)
			printf("%d %s ", identsym, list[i].name);
		else
			printf("%d ", list[i].type);
	}
	printf("\n");
	list[lex_index++].type = -1;
}

void printlexerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Excessive Number Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Excessive Identifier Length\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");

	free(list);
	return;
}