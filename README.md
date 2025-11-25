# Tarea 3: Protección de Lectura en XV6

**Integrantes:** Max Lastra y Daniel Alvarez

## Resumen Ejecutivo

Se implementó con éxito un mecanismo de **Protección de Lectura** (memoria "Solo Escritura" o Write-Only) en el *kernel* **xv6-riscv**. El objetivo fue proteger datos sensibles (como claves criptográficas) al revocar el permiso de lectura de una región de memoria de usuario. Esto se logró mediante dos nuevas llamadas al sistema que manipulan el bit de lectura (`PTE_R`) en las Entradas de la Tabla de Páginas (PTE) de la memoria de usuario.

Al intentar leer la memoria protegida, el *hardware* genera una **Falla de Acceso a Página** (`scause 0xf`), lo que obliga al *kernel* a terminar el proceso, cumpliendo el objetivo de proteger datos sensibles.

***

## Diseño e Implementación Técnica

La solución requirió la integración de nuevas funciones en la capa de *user-space* y *kernel-space* para manipular los permisos de la Tabla de Páginas.

### 1. Funciones de Llamada al Sistema (System Calls)

Se implementaron las siguientes llamadas al sistema, que invocan la lógica central a través de la interfaz del *kernel* (`sysproc.c`):

* **`mrdprotect(void *addr, int len)`**: Marca la región como sin lectura, limpiando el bit `PTE_R`.
    * **Mecanismo en PTE:** `*pte &= ~PTE_R;`
* **`munrdprotect(void *addr, int len)`**: Restaura el permiso de lectura, activando el bit `PTE_R`.
    * **Mecanismo:** `*pte |= PTE_R;`

### 2. Lógica del Kernel (`kernel/vm.c`)

La lógica principal (auxiliar de las *system calls*) recorre el rango de direcciones virtuales afectado en pasos de `PGSIZE` y realiza las siguientes operaciones en cada PTE:

* **Obtención del PTE:** Se utiliza la función `walk` para obtener el puntero a la entrada de la tabla de páginas.
* **Modificación:** Se aplica la operación bit a bit sobre el PTE.
* **Sincronización:** Es crucial ejecutar `sfence.vma()` tras la modificación del PTE para invalidar las entradas obsoletas en el TLB, asegurando que el *hardware* aplique el nuevo permiso de inmediato.

### 3. Manejo de Errores (Validación Rigurosa)

Se implementaron verificaciones al inicio del bucle de recorrido para validar la integridad de los argumentos y la seguridad del sistema:

* **Alineación:** `addr` no debe estar alineada a una página.
* **Longitud:** `len` debe ser positivo (`len > 0`).
* **Mapeo:** La página debe ser válida (`PTE_V` activo).
* **Dominio de Acceso:** La página debe ser de usuario (`PTE_U` activo) para evitar modificar la memoria del *kernel*.

***

## Análisis de Resultados Obtenidos

La funcionalidad fue probada con el programa **`rdprotect_test.c`**, confirmando la terminación del proceso por falla de acceso.

| Etapa | Instrucción Clave | Permisos en PTE (R/W) | Resultado Esperado | Conclusión y Análisis Técnico |
| :--- | :--- | :--- | :--- | :--- |
| **1. Protección** | `mrdprotect(addr, 1)` | **R=0, W=1** | Éxito silencioso. | El bit `PTE_R` se limpia, estableciendo el estado de "Solo Escritura". |
| **2. Escritura** | `addr[0] = 'A';` | **R=0, W=1** | Éxito silencioso. | La escritura es exitosa, confirmando que la restricción es solo sobre la lectura. |
| **3. Intento de Lectura** | `char c = addr[0];` | **R=0** | **Falla (usertrap)**. | El *hardware* detecta la instrucción de lectura prohibida, generando una **Falla de Acceso a Página** (`scause 0xf`). El *kernel* mata el proceso de prueba, impidiendo la lectura. |

**Conclusión:** La terminación controlada (`usertrap`) en el intento de lectura confirma que el *hardware* está aplicando la restricción de acceso definida por el *kernel*, probando el modelo de memoria "solo escritura" de la tarea.
