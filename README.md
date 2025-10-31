# Tarea 2 – Planificador de Procesos en XV6
**Integrantes:** Max Lastra y Daniel Álvarez  

---

## 1. Funcionamiento y lógica de la implementación

El objetivo de esta tarea fue reemplazar el planificador **Round Robin** original de xv6 por un algoritmo de planificación **Lottery Scheduling**.  
En este sistema, cada proceso posee una cantidad de *tickets* que determina la probabilidad de ser elegido para usar el CPU.  
De esta forma, los procesos con más tickets tienen una probabilidad más alta de ser seleccionados, mientras que todos los procesos RUNNABLE conservan al menos una pequeña posibilidad de ejecución.

### Lógica general del Scheduler
1. **Cálculo de tickets totales:**  
   En cada iteración del scheduler, se recorren todos los procesos RUNNABLE para sumar el total de tickets activos en el sistema.
2. **Generación de número aleatorio:**  
   Se genera un número pseudoaleatorio `r` entre 1 y el total de tickets usando un generador lineal congruencial (LCG) dentro del kernel.
3. **Selección del proceso ganador:**  
   Se vuelve a recorrer la lista de procesos acumulando tickets hasta que el valor acumulado supera el número aleatorio `r`.  
   El proceso que cumple esta condición se elige como ganador, pasa a estado `RUNNING` y se ejecuta mediante `swtch()`.
4. **Contabilidad de CPU:**  
   Cada vez que un proceso gana, se incrementa su contador `cpu_slices`, que permite verificar cuántas veces fue seleccionado por el scheduler.

### Syscall `settickets(int n)`
Se implementó una nueva llamada al sistema llamada `settickets(int n)` que permite modificar la cantidad de tickets del proceso actual.  
Si se entrega un valor menor a 1, el sistema lo corrige automáticamente a 1 para evitar exclusiones del sorteo.  
Esto permite ajustar dinámicamente la prioridad de los procesos.

### Programa de prueba `demo.c`
Para validar el funcionamiento del algoritmo, se desarrolló un programa de usuario (`demo.c`) que:
- Crea 10 procesos hijos.
- Asigna a cada proceso una cantidad distinta de tickets (50, 100, 150, ...).
- Ejecuta una carga de trabajo intensiva en CPU.
- Finaliza y permite visualizar con **Ctrl + P** los `tickets` y `slices` de cada proceso.


---

## 2. Explicación de las modificaciones realizadas

Durante la implementación del Lottery Scheduler se modificaron y agregaron los siguientes archivos y líneas de código:

| Archivo | Descripción del cambio |
|----------|------------------------|
| **kernel/proc.h** | Se agregaron los campos `int tickets;` y `int cpu_slices;` dentro de la estructura `proc` para almacenar los tickets y la cantidad de veces que un proceso fue seleccionado. |
| **kernel/proc.c** | Se inicializaron los nuevos campos en `allocproc()` y `userinit()`. Se reescribió la función `scheduler()` implementando la lógica de Lottery Scheduling en reemplazo del Round Robin. |
| **kernel/sysproc.c** | Se creó la función `sys_settickets()` que permite modificar la cantidad de tickets de un proceso mediante una syscall. |
| **kernel/syscall.c** | Se agregó la referencia a `sys_settickets()` en la tabla de llamadas del sistema (`syscalls[]`). |
| **kernel/syscall.h** | Se definió el identificador de la nueva syscall `#define SYS_settickets 24`. |
| **user/user.h** | Se agregó la declaración del prototipo `int settickets(int n);`. |
| **user/usys.pl** | Se añadió la línea `entry("settickets");` para generar el stub de la syscall en espacio de usuario. |
| **user/demo.c** | Se creó un nuevo programa que lanza múltiples procesos con diferentes cantidades de tickets y mide la distribución del uso de CPU. |
| **Makefile** | Se agregó `$U/_demo` en la variable `UPROGS` para incluir el ejecutable `demo` en la imagen del sistema de archivos (`fs.img`). |

Estas modificaciones integran el nuevo scheduler con la estructura del kernel y los programas de usuario, permitiendo una verificación completa del comportamiento del sistema.

---

## 3. Dificultades encontradas y soluciones implementadas

Durante el desarrollo se presentaron distintos inconvenientes técnicos, los cuales se resolvieron de la siguiente manera:

| Dificultad | Solución implementada |
|-------------|-----------------------|
| **Error de compilación en `scheduler()` (variable sin uso)** | Se eliminó la declaración redundante `struct proc *p;` antes del bucle principal, ya que el puntero `p` se redeclaraba dentro de los bucles `for`. |
| **Error “recipe commences before first target” en Makefile** | Se corrigió la indentación del bloque `UPROGS` asegurando que todas las líneas (excepto la última) terminen con `\` y no contengan espacios extra. |
| **Problemas con `Ctrl + P` en VS Code** | VS Code intercepta el atajo `Ctrl + P`, por lo que se ejecutó `make qemu` desde una terminal de Ubuntu externa para permitir que el comando del kernel funcione. |
| **Procesos con 0 tickets no ejecutaban** | Se añadió una validación en `sys_settickets()` para forzar `tickets = 1` cuando se ingresa un valor menor a 1. |
| **Verificación del funcionamiento del demo** | Se comprobó que los procesos con más tickets acumulan más `cpu_slices` al ejecutar `demo` y presionar `Ctrl + P`. |

---

## 4. Posibles problemas del Lottery Scheduling

El algoritmo de planificación por lotería ofrece una aproximación equitativa y flexible, pero presenta limitaciones teóricas y prácticas que deben considerarse:

1. **No determinismo y variabilidad en resultados:**  
   Debido al componente aleatorio, los resultados pueden variar entre ejecuciones.  
   Esto puede ser problemático en tareas que requieran tiempos de respuesta predecibles.

2. **Starvation estadístico:**  
   Aunque improbable, un proceso con pocos tickets podría pasar largos periodos sin ser elegido debido al azar, provocando tiempos de espera excesivos.

3. **Falta de prioridades rígidas:**  
   El scheduler no asegura prioridades absolutas. Un proceso con más tickets tiene más probabilidad, pero no garantía de ejecución inmediata.

4. **Mayor sobrecarga que Round Robin:**  
   Cada iteración requiere calcular el total de tickets y recorrer toda la tabla de procesos, agregando una mínima sobrecarga al planificador.

5. **Dificultad para depurar y reproducir errores:**  
   Como las ejecuciones no son deterministas, es más difícil reproducir escenarios idénticos durante la depuración del kernel.

6. **Inadecuado para sistemas de tiempo real:**  
   Al basarse en probabilidades, no puede garantizar tiempos de ejecución exactos ni cumplir con plazos estrictos.

---

## 5. Conclusión

Se implementó con éxito el **planificador por lotería (Lottery Scheduling)** en xv6, junto con la syscall `settickets()` y el programa de prueba `demo.c`.  
El sistema reparte el tiempo de CPU proporcionalmente a los tickets asignados a cada proceso, cumpliendo con el comportamiento esperado de este algoritmo probabilístico.  
Los resultados observados en las pruebas confirman que los procesos con más tickets fueron seleccionados con mayor frecuencia, validando la correcta implementación del scheduler.

Esta versión de xv6 demuestra cómo un modelo de planificación basado en probabilidad puede coexistir con los mecanismos clásicos del kernel, manteniendo la equidad y la oportunidad de ejecución para todos los procesos.
