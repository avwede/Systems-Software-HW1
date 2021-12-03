Members of the group:
Nicholas Gray, 
Abraham Hernandez, 
Prathik Ramesh, 
Ashley Voglewede

No alterations have been made to the original files.

To compile the program, use the makefile by running the command `make` in the directory that
has both the makefile and compiler.h, driver.c, parser.c, lex.o, and vm.o.

This makefile runs the command `gcc driver.c lex.o parser.c vm.o -lm`
To run the executable, run the command `./a.out <InputFileName>.txt <-l | -s | -a | -v>`