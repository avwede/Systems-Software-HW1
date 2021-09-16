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

int main(int argc, char *argv[])
{
  int BP, SP, PC, DP, GP, FREE, IC = 0;
  int OP, L, M;
  int index = 0;
  int IR[INSTRUCTION_FIELDS];
  int *PAS = (int *)calloc(MAX_PAS_LENGTH, sizeof(int));
  char buffer[50];

  char *filename;
  FILE *text;

  // Command line argument check
  if (argc != 2)
  {
    printf("Syntax: %s <input file>\n", argv[0]);
    free(PAS);
    return 1;
  }

  filename = argv[1];
  text = fopen(filename, "r");

  // Load the program from the file

  // Fetch every line in file
  while (fgets(buffer, sizeof(buffer), text) != NULL)
  {
    // Split each line into respective instruction fields
    OP = atoi(strtok(buffer, " "));
    L = atoi(strtok(NULL, " "));
    M = atoi(strtok(NULL, " "));

    // Store instruction into the text portion of the PAS
    PAS[index++] = OP;
    PAS[index++] = L;
    PAS[index++] = M;

    // Increment Instruction Counter by 3
    IC += 3;
  }
  fclose(text);

  // Initialize CPU registers

  // Initialize each register appropriately
  GP = IC;
  DP = IC - 1;
  FREE = IC + 40;
  BP = IC;
  SP = MAX_PAS_LENGTH;

  // Fill the Instruction Register with zeros
  for (index = 0; index < INSTRUCTION_FIELDS; index++)
  {
    IR[index] = 0;
  }

  // Run the main program

  while (PC < IC)
  {
    // int OP, L, M;

    // fetch_instruction(PAS, *PC, &OP, &L, &M);
    OP = PAS[PC];
    L = PAS[PC + 1];
    M = PAS[PC + 2];

    PC += 3;

    // load_instruction_register(IR, OP, L, M);

    IR[0] = OP;
    IR[1] = L;
    IR[2] = M;
    // execute_instruction(BP, SP, PC, DP, GP, FREE, IC, IR);

    printf("in\n");
    switch (IR[0])
    {
    case 1:
      // LIT
      break;
    case 2:
      // OPR
      break;
    case 3:
      // LOD
      break;
    case 4:
      // STO
      break;
    case 5:
      // CAL
      break;
    case 6:
      // INC
      break;
    case 7:
      // JMP
      break;
    case 8:
      // JPC
      break;
    case 9:
      // SYS
      break;
    default:
      // INVALID INSTRUCTION
      break;
    }
  }

  free(PAS);
  return 0;
}
