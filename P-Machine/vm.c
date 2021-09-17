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
  int BP, SP, PC, DP, GP, FREE, IC = 0, HALT = 1;
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
  while (PC < IC && HALT)
  {
    // Fetch the instruction from text
    OP = PAS[PC];
    L = PAS[PC + 1];
    M = PAS[PC + 2];

    PC += 3;

    // Load the instruction into the register
    IR[0] = OP;
    IR[1] = L;
    IR[2] = M;

    // Execute the instruction in the register
    switch (IR[0])
    {
    case 1:
      // LIT
      if (BP == GP)
      {
        PAS[++DP] = IR[2];
      }

      else
      {
        PAS[--SP] = IR[2];
      }

      break;
    case 2:
      // OPR
      switch (IR[2])
      {
        case 0:
          // RTN
          SP = BP + 1;
          BP = PAS[SP - 2];
          PC = PAS[SP - 3];
          break;
        case 1:
          // NEG
          if (BP == GP)
          {
            PAS[DP] *= -1;
          }
          else
          {
            PAS[SP] *= -1;
          }
          break;
        case 2:
          // ADD
          if (BP == GP) 
          {
            PAS[--DP] = PAS[DP] + PAS[DP + 1];
          }
          else 
          {
            PAS[++SP] = PAS[SP] + PAS[SP - 1];
          }
          break;
        case 3:
          // SUB
          if (BP == GP) 
          {
            PAS[--DP] = PAS[DP] - PAS[DP + 1];
          }
          else
          {
            PAS[++SP] = PAS[SP] - PAS[SP - 1];
          }
          break;
        case 4:
          // MUL
          if (BP == GP)
          {
            PAS[--DP] = PAS[DP] * PAS[DP + 1];
          }
          else
          {
            PAS[++SP] = PAS[SP] * PAS[SP - 1];
          }
          break;
        case 5:
          // DIV
          if (BP == GP)
          {
            PAS[--DP] = PAS[DP] / PAS[DP + 1];
          }
          else 
          {
            PAS[++SP] = PAS[SP] / PAS[SP - 1];
          }
          break;
        case 6:
          // ODD
          if (BP == GP) 
          {
            PAS[DP] = PAS[DP] % 2;
          }
          else 
          {
            PAS[SP] = PAS[SP] % 2;
          }
          break;
        case 7:
          // MOD
          if (BP == GP)
          {
            PAS[--DP] = PAS[DP] % PAS[DP + 1];
          }
          else 
          {
            PAS[++SP] = PAS[SP] % PAS[SP - 1];
          }
          break;
        case 8:
          // EQL
          if (BP == GP) 
          {
            PAS[--DP] = ((PAS[DP] == PAS[DP + 1]) ? 1 : 0);
          }
          else 
          {
            PAS[++SP] = ((PAS[SP] == PAS[SP - 1]) ? 1 : 0);
          }
          break;
        case 9:
          // NEQ
          if (BP == GP)
          {
            PAS[--DP] = ((PAS[DP] != PAS[DP + 1]) ? 1 : 0);
          }
          else 
          {
            PAS[++SP] = ((PAS[SP] != PAS[SP - 1]) ? 1 : 0);
          }
          break;
        case 10:
          // LSS
          if (BP == GP)
          {
            PAS[--DP] = ((PAS[DP] < PAS[DP + 1]) ? 1 : 0);
          }
          else 
          {
            PAS[++SP] = ((PAS[SP] < PAS[SP - 1]) ? 1 : 0);
          }
          break;
        case 11: 
          // LEQ
          if (BP == GP)
          {
            PAS[--DP] = ((PAS[DP] <= PAS[DP + 1]) ? 1 : 0);
          }
          else 
          {
            PAS[++SP] = ((PAS[SP] <= PAS[SP - 1]) ? 1 : 0);
          }
          break;
        case 12:
          // GTR
          if (BP == GP)
          {
            PAS[--DP] = ((PAS[DP] > PAS[DP + 1]) ? 1 : 0);
          }
          else 
          {
            PAS[++SP] = ((PAS[SP] > PAS[SP - 1]) ? 1 : 0);
          }
          break;
        case 13:
          // GEQ
          if (BP == GP)
          {
            PAS[--DP] = ((PAS[DP] >= PAS[DP + 1]) ? 1 : 0);
          }
          else 
          {
            PAS[++SP] = ((PAS[SP] >= PAS[SP - 1]) ? 1 : 0);
          }
          break;
      }
      
      break;
    case 3:
      // LOD
      if (BP == GP)
      {
        PAS[++DP] = PAS[GP + IR[2]];
      }

      else if (base(IR[1], BP, PAS) == GP)
      {
        PAS[--SP] = PAS[GP + IR[2]];
      }

      else
      {
        PAS[--SP] = PAS[base(IR[1], BP, PAS) - IR[2]];
      }

      break;
    case 4:
      // STO
      if (BP == GP)
      {
        PAS[++DP] = PAS[GP + IR[2]];
      }

      else if (base(IR[1], BP, PAS) == GP)
      {
        PAS[GP + IR[2]] = PAS[SP++];
      }

      else
      {
        PAS[base(IR[1], BP, PAS) - IR[2]] = PAS[SP++];
      }

      break;
    case 5:
      // CAL
      PAS[SP - 1] = base(L, BP, PAS);
      PAS[SP - 2] = BP;
      PAS[SP - 3] = PC;
      BP = SP - 1;
      PC = IR[2];
      break;
    case 6:
      // INC
      if (BP == GP)
      {
        DP += IR[2];
      }

      else
      {
        SP -= IR[2];
      }
      break;
    case 7:
      // JMP
      PC = IR[2];
      break;
    case 8:
      // JPC
      if (BP == GP)
      {
        if (PAS[DP] == 0)
        {
          PC = IR[2];
        }

        --DP;
      }

      else
      {
        if (PAS[SP] == 0)
        {
          PC = IR[2];
        }

        ++SP;
      }
      break;
    case 9:
      // SYS
      switch (IR[2])
      {
        case 1:
          if (BP == GP)
          {
            printf("%d", PAS[DP--]);
          }

          else
          {
            printf("%d", PAS[SP++]);
          }
          break;

        case 2:
          if (BP == GP)
          {
            scanf("%d", &PAS[++DP]);
          }
          else
          {
            scanf("%d", &PAS[--SP]);
          }
          break;

        default:
          HALT = 0;
      }
      break;
    default:
      // INVALID INSTRUCTION
      HALT = 0;
      break;
    }
  }

  free(PAS);
  return 0;
}
