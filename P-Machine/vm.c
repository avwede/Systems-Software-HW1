// Systems Software Fall 2021
// Group 5 - Nicholas Gray, Abraham Hernandez, Prathik Ramesh, Ashley Voglewede

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int fetch_instruction();
int instruction_decode();
int execute_instruction();

// int base(int L)
// {
//   int arb = BP;
//   while (L > 0)
//   {
//     arb = pas[arb];
//     L--;
//   }

//   return arb;
// }

// void print_execution(int line, char *opname, int *IR, int PC, int BP, int SP, int DP, int *pas, int GP)
// {
//   int i;

//   // print out instruction and registers
//   printf("%2d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t", line, opname, IR[1], IR[2], PC, BP, SP, DP);

//   // print data section
//   for (i = GP; i <= DP; i++)
//     printf("%d ", pas[i]);

//   printf("\n");

//   // print stack
//   printf("\tstack : ");
//   for (i = MAX_PAS_LENGTH - 1; i >= SP; i--)
//     printf("%d ", pas[i]);

//   printf("\n");
// }

const int MAX_PAS_LENGTH = 500;

int load_text(int *PAS, FILE *text)
{
  char buffer[50];
  int index = 0;
  while (fgets(buffer, sizeof(buffer), text) != NULL)
  {
    int OP = atoi(strtok(buffer, " "));
    int L = atoi(strtok(NULL, " "));
    int M = atoi(strtok(NULL, " "));

    PAS[index++] = OP;
    PAS[index++] = L;
    PAS[index++] = M;
  }

  return index;
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    printf("Error: Incorrect amount of commands (2)!");
    return 1;
  }

  int *PAS = (int *)calloc(MAX_PAS_LENGTH, sizeof(int));

  char *filename = argv[1];
  FILE *text = fopen(filename, "r");

  int text_end = load_text(PAS, text);

  return 0;
}
