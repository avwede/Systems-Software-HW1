// Systems Software Fall 2021
// Group 5 - Nicholas Gray, Abraham Hernandez, Prathik Ramesh, Ashley Voglewede

#include <stdio.h>

int fetch_instruction();
int instruction_decode();
int execute_instruction();

int base(int L)
{
  int arb = BP;
  while (L > 0)
  {
    arb = pas[arb];
    L--;
  }

  return arb;
}

void print_execution(int line, char *opname, int *IR, int PC, int BP, int SP, int DP, int *pas, int GP)
{
  int i;

  // print out instruction and registers
  printf("%2d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t", line, opname, IR[1], IR[2], PC, BP, SP, DP);

  // print data section
  for (i = GP; i <= DP; i++)
    printf("%d ", pas[i]);

  printf("\n");

  // print stack
  printf("\tstack : ");
  for (i = MAX_PAS_LENGTH - 1; i >= SP; i--)
    printf("%d ", pas[i]);
  
  printf("\n");
}

int main(void)
{
  printf("Hello, World!\n");
  
  return 0;
}
