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
  int BP = 0, SP = 0, PC = 0, DP = 0, GP = 0, FREE = 0, IC = 0, HALT = 1;
  int OP, L, M;
  int index = 0, line = 0;
  int IR[INSTRUCTION_FIELDS];
  int *PAS = (int *)calloc(MAX_PAS_LENGTH, sizeof(int));
  char buffer[50];
  char opCode[4];

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

  printf("\t\t\t\tPC\tBP\tSP\tDP\tdata\n");
  printf("Initial values:\t\t\t%d\t%d\t%d\t%d\n", PC, BP, SP, DP);

  // Run the main program
  while (PC < IC && HALT)
  {
    // Fetch the instruction from text
    // Load the instruction into the register
    IR[0] = PAS[PC];
    IR[1] = PAS[PC + 1];
    IR[2] = PAS[PC + 2];

    line = PC / 3;
    PC += 3;

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

      print_execution(line, "LIT", IR, PC, BP, SP, DP, PAS, GP);

      break;
    case 2:
      // OPR
      switch (IR[2])
      {
      case 0:
        // RTN
        strcpy(opCode, "RTN");
        SP = BP + 1;
        BP = PAS[SP - 2];
        PC = PAS[SP - 3];
        break;
      case 1:
        // NEG
        strcpy(opCode, "NEG");
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
        strcpy(opCode, "ADD");
        if (BP == GP)
        {
          DP--;
          PAS[DP] = PAS[DP] + PAS[DP + 1];
        }
        else
        {
          SP++;
          PAS[SP] = PAS[SP] + PAS[SP - 1];
        }
        break;
      case 3:
        // SUB
        strcpy(opCode, "SUB");
        if (BP == GP)
        {
          DP--;
          PAS[DP] = PAS[DP] - PAS[DP + 1];
        }
        else
        {
          SP++;
          PAS[SP] = PAS[SP] - PAS[SP - 1];
        }
        break;
      case 4:
        // MUL
        strcpy(opCode, "MUL");
        if (BP == GP)
        {
          DP--;
          PAS[DP] = PAS[DP] * PAS[DP + 1];
        }
        else
        {
          SP++;
          PAS[SP] = PAS[SP] * PAS[SP - 1];
        }
        break;
      case 5:
        // DIV
        strcpy(opCode, "DIV");
        if (BP == GP)
        {
          DP--;
          PAS[DP] = PAS[DP] / PAS[DP + 1];
        }
        else
        {
          SP++;
          PAS[SP] = PAS[SP] / PAS[SP - 1];
        }
        break;
      case 6:
        // ODD
        strcpy(opCode, "ODD");
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
        strcpy(opCode, "MOD");
        if (BP == GP)
        {
          DP--;
          PAS[DP] = PAS[DP] % PAS[DP + 1];
        }
        else
        {
          SP++;
          PAS[SP] = PAS[SP] % PAS[SP - 1];
        }
        break;
      case 8:
        // EQL
        strcpy(opCode, "EQL");
        if (BP == GP)
        {
          DP--;
          PAS[DP] = ((PAS[DP] == PAS[DP + 1]) ? 1 : 0);
        }
        else
        {
          SP++;
          PAS[SP] = ((PAS[SP] == PAS[SP - 1]) ? 1 : 0);
        }
        break;
      case 9:
        // NEQ
        strcpy(opCode, "NEQ");
        if (BP == GP)
        {
          DP--;
          PAS[DP] = ((PAS[DP] != PAS[DP + 1]) ? 1 : 0);
        }
        else
        {
          SP++;
          PAS[SP] = ((PAS[SP] != PAS[SP - 1]) ? 1 : 0);
        }
        break;
      case 10:
        // LSS
        strcpy(opCode, "LSS");
        if (BP == GP)
        {
          DP--;
          PAS[DP] = ((PAS[DP] < PAS[DP + 1]) ? 1 : 0);
        }
        else
        {
          SP++;
          PAS[SP] = ((PAS[SP] < PAS[SP - 1]) ? 1 : 0);
        }
        break;
      case 11:
        // LEQ
        strcpy(opCode, "LEQ");
        if (BP == GP)
        {
          DP--;
          PAS[DP] = ((PAS[DP] <= PAS[DP + 1]) ? 1 : 0);
        }
        else
        {
          SP++;
          PAS[SP] = ((PAS[SP] <= PAS[SP - 1]) ? 1 : 0);
        }
        break;
      case 12:
        // GTR
        strcpy(opCode, "GTR");
        if (BP == GP)
        {
          DP--;
          PAS[DP] = ((PAS[DP] > PAS[DP + 1]) ? 1 : 0);
        }
        else
        {
          SP++;
          PAS[SP] = ((PAS[SP] > PAS[SP - 1]) ? 1 : 0);
        }
        break;
      case 13:
        // GEQ
        strcpy(opCode, "GEQ");
        if (BP == GP)
        {
          DP--;
          PAS[DP] = ((PAS[DP] >= PAS[DP + 1]) ? 1 : 0);
        }
        else
        {
          SP++;
          PAS[SP] = ((PAS[SP] >= PAS[SP - 1]) ? 1 : 0);
        }

        break;
      }
      print_execution(line, opCode, IR, PC, BP, SP, DP, PAS, GP);
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

      print_execution(line, "LOD", IR, PC, BP, SP, DP, PAS, GP);
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

      print_execution(line, "STO", IR, PC, BP, SP, DP, PAS, GP);
      break;
    case 5:
      // CAL
      PAS[SP - 1] = base(L, BP, PAS);
      PAS[SP - 2] = BP;
      PAS[SP - 3] = PC;
      BP = SP - 1;
      PC = IR[2];
      print_execution(line, "CAL", IR, PC, BP, SP, DP, PAS, GP);
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
      print_execution(line, "INC", IR, PC, BP, SP, DP, PAS, GP);
      break;
    case 7:
      // JMP
      PC = IR[2];
      print_execution(line, "JMP", IR, PC, BP, SP, DP, PAS, GP);
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
      print_execution(line, "JPC", IR, PC, BP, SP, DP, PAS, GP);
      break;
    case 9:
      // SYS
      switch (IR[2])
      {
      case 1:
        printf("Top of Stack Value: ");
        if (BP == GP)
        {
          printf("%d\n", PAS[DP--]);
        }

        else
        {
          printf("%d\n", PAS[SP++]);
        }
        break;

      case 2:
        printf("Please Enter an Integer: ");
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
      print_execution(line, "SYS", IR, PC, BP, SP, DP, PAS, GP);
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
