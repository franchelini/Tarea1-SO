<h1 align="center"> Proyecto 1 de Sistemas Operativos 2024-2. </h1>

## Descripción del proyecto:
Desarrollo de un intérprete de comandos simple en Linux (shell). La shell a implementar será similar
a las disponibles actualmente en Linux.

## Requisitos:
- `Requisito 1`: Sistema operativo Linux
- `Requisito 2`: Compilador de C++

## Uso de la shell:
- `Paso 1`: Abrir la terminal de Linux (Ctrl+Alt+t)
- `Paso 2`: Dirigirse localizar la carpeta donde se encuentra shell.cpp
- `Paso 3`: Compilar el archivo: g++ shell.cpp
- `Paso 4`: Ejecutar el código mediante: ./a.out

## Pruebas con comandos:
Al ejecutar la shell se encontrará espera de la ejecución de los comandos que se ingresen desde el teclado.
Este aceptará comandos comunes o que se comuniquen mediante pipes para realizar procesos simultaneos:

**Por ejemplo:**
- ls
- ls | wc
- ps -aux | sort -nr -k 4 | head -10

En caso de error se entregará un mensaje que dice "Error en el comando ingresado" u otro dependiendo del tipo de error.

**Ejemplo de errores:**
- xd
- |
- ls||wc

Luego, para terminar la shell se debe ejecutar el comando **exit**.

## Comandos Personalizados:
