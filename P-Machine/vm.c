// Systems Software Fall 2021
// Group 5 - Nicholas Gray, Abraham Hernandez, Prathik Ramesh, Ashley Voglewede

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int MAX_PAS_LENGTH = 500;
const int INSTRUCTION_FIELDS = 3;

int base(int L, int BP, int *PAS)
{
  int arb = BP;
  while (L > 0)
  {
    arb = PAS[arb];
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

void load_text(int *PAS, FILE *text, int *IC)
{
  char buffer[50];
  int index = 0;

  // Fetch every line in file
  while (fgets(buffer, sizeof(buffer), text) != NULL)
  {
    // Split each line into respective instruction fields
    int OP = atoi(strtok(buffer, " "));
    int L = atoi(strtok(NULL, " "));
    int M = atoi(strtok(NULL, " "));

    // Store instruction into the text portion of the PAS
    PAS[index++] = OP;
    PAS[index++] = L;
    PAS[index++] = M;

    // Increment Instruction Counter by 3
    *IC += 3;
  }
}

void initialize_CPU_registers(int *BP, int *SP, int *PC, int *DP, int *GP, int *FREE, int *IC, int *IR)
{
  int i;

  // Initialize each register appropriately
  *GP = *IC;
  *DP = *IC - 1;
  *FREE = *IC + 40;
  *BP = *IC;
  *SP = MAX_PAS_LENGTH;

  // Fill the Instruction Register with zeros
  for (i = 0; i < INSTRUCTION_FIELDS; i++)
  {
    IR[i] = 0;
  }
}

void load_instruction_register(int *IR, int OP, int L, int M)
{
  IR[0] = OP;
  IR[1] = L;
  IR[2] = M;
}

void fetch_instruction(int *PAS, int PC, int *OP, int *L, int *M)
{
  *OP = PAS[PC];
  *L = PAS[PC + 1];
  *M = PAS[PC + 2];
}

void execute_instruction(int *BP, int *SP, int *PC, int *DP, int *GP, int *FREE, int *IC, int *IR)
{
  // TODO
}

void run_program(int *PAS, int *BP, int *SP, int *PC, int *DP, int *GP, int *FREE, int *IC, int *IR)
{
  while (*PC < *IC)
  {
    int OP, L, M;

    fetch_instruction(PAS, *PC, &OP, &L, &M);
    *PC += 3;

    load_instruction_register(IR, OP, L, M);
    execute_instruction(BP, SP, PC, DP, GP, FREE, IC, IR);
  }
}

int main(int argc, char *argv[])
{
  int BP, SP, PC, DP, GP, FREE, IC = 0;
  int IR[INSTRUCTION_FIELDS];
  int *PAS = (int *)calloc(MAX_PAS_LENGTH, sizeof(int));

  if (argc != 2)
  {
    printf("Error: Incorrect amount of commands (2)!");
    return 1;
  }

  char *filename = argv[1];
  FILE *text = fopen(filename, "r");

  load_text(PAS, text, &IC);
  initialize_CPU_registers(&BP, &SP, &PC, &DP, &GP, &FREE, &IC, IR);

  run_program(PAS, &BP, &SP, &PC, &DP, &GP, &FREE, &IC, IR);

  return 0;
}
