# lib.fsm

Deterministic Finite State Machines for the C Programming Language

## Dynamic Linking

Linking to `lib.fsm`:

```bash
# 1) include and use lib.fsm in your project
# 2) generate object file for your project
gcc -I ../path/to/libfsm -c main.c -o main.o
# 3) generate shared object file
make
# 4) link your project to lib.fsm
gcc -o main main.o -L../path/to/libfsm -llibfsm
# you may need to add the lib location to your PATH
```

Linking to `lib.fsm` on Windows:

```bash
# 1) include and use lib.fsm in your project
# 2) generate object file for your project
gcc -I ../path/to/libfsm -c main.c -o main.o
# 3) generate shared object file
make win
# 3) link your project to lib.fsm
gcc -o main.exe main.o -L /path/to/lib.fsm -llib_fsm.dll
# you may need to add the lib location to your PATH
```
