Alumnos: Daniel Alvarez
Max Lastra


Seguimos las instrucciones de la tarea.
1.-Clonamos el repositorio a wsl.
2.-Creamos la rama tarea0
3.-Despues instalamos las depencencias correctas para poder usarlo con wsl como build-essential, meson, ninja-build, pkg-config, libglib2.0-dev, libpixman-1-dev, python3 y otras mas.
4.- Continuamos yendo a la carpeta raiz de xv6 para compilar xv6, con make clean && make, make qemu
5.- Ejecutamos los comandos pedidos 
 
 Problemas: 
Los problemas que encontramos es que tuvimos que actualizar qemu ya que el que estaba por defecto era mas antigua de 7.2 por lo cual no se estaba inicializando correctamente (ERROR: Need qemu version >= 7.2)
Ademas de esto tuvimos problemas por la falta de un modulo y tuvimos que instalarlo (ERROR: python venv creation failed (ensurepip not found))
