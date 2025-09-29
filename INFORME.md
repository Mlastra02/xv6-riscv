# INFORME — Tarea 1 
Alumnos: Daniel Alvarez
Max Lastra

## Funcionamiento 
'getppid() → int'
-Retorna el PID del padre del proceso llamante.
-Si no existe el padre retorna -1

'getancestor(int n) → int'
-Retorna el PID del ancestro n-ésimo del proceso llamante,
'n=0'yo', n=1 'padre', etc, n<0 o no existe retorna -1'  


## Modificaciones realizadas
En kernel/syscall.h se modificaron los numeros de sycall, SYS_getppid como 22 y SYS_getancestor23, en syscall.c se implementaron las funciones de getppid y getancestor, 
en user.h estan los "prototipos", usys.pl las entradas, entry("getppid") y entry("getancestors") y finalmente en el makefile se agrega _yosoytupadre.

## Dificultades y soluciones
El primer problema que se creo en sys_getancestor ya que hubo un error en el argint(), quedo en void, por lo cual se debio cambiar de la manera argint(0,&n) validando n posteriormente.
