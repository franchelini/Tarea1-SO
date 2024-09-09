<h1 align="center"> Proyecto 1 de Sistemas Operativos 2024-2. </h1>

## Descripción del proyecto:
Desarrollo de un intérprete de comandos simple en Linux (shell). La shell a implementar será similar
a las disponibles actualmente en Linux.

## Requisitos:
- `Requisito 1`: Sistema operativo Linux
- `Requisito 2`: Compilador de C++ 11 o posterior

## Uso de la shell:
- `Paso 1`: Abrir la terminal de Linux (Ctrl+Alt+t)
- `Paso 2`: Dirigirse localizar la carpeta donde se encuentra **shell.cpp**
- `Paso 3`: Compilar el archivo: **g++ shell.cpp**
- `Paso 4`: Ejecutar el código mediante: **./a.out**

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
- ls | | wc

Luego, para terminar la shell se debe ejecutar el comando **exit**.

## Comandos Personalizados:
Descripción y modo de uso de los nuevos comandos disponibles que podemos utilizar:

- **favs crear ruta/archivo.txt**
  - **Modo de uso**: Al estar dentro de la shell podemos ejecutar el comando **crear**, que creará un archivo .txt en la dirección de la ruta especificada.
  
  - **Ejemplo de uso**: favs crear /home/usuario/Documentos/prueba.txt
    
- **favs mostrar**
  - **Modo de uso**: Al estar dentro de la shell podemos ejecutar el comando **mostrar**, este escribirá en pantalla los comandos que se hayan ejecutado anteriormente y se encuentren almacenados en la memoria.
  - **Ejemplo de uso**: favs mostrar
  - 
- **favs eliminar num1,num2**
  - **Modo de uso**: Al estar dentro de la shell podemos ejecutar el comando **eliminar**, que dado un rango entre los números **num1** y **num 2** se eliminarán de la lista de favoritos los comandos correspondientes a ese rango.
  - **Ejemplo de uso**: favs eliminar 2,5
 
- **favs buscar cmd**
  - **Modo de uso**: Al estar dentro de la shell podemos ejecutar el comando **buscar**, que dado un comando valido en la terminal, buscará en la lista de favoritos aquel comando y lo mostrará en pantalla junto a su número asociado.
  - **Ejemplo de uso**: favs buscar ls

- **favs borrar**
  - **Modo de uso**: Al estar dentro de la shell podemos ejecutar el comando **borrar**, este comando eliminará todos los comandos almacenados en la lista de favoritos actual.
  - **Ejemplo de uso**: favs borrar

- **favs num ejecutar**
  - **Modo de uso**: Al estar dentro de la shell podemos ejecutar el comando **ejecutar**, esto ejecutará el comando asociado al número entregado en **num** que se encuentre en la lista de favoritos actual.
  - **Ejemplo de uso**: favs 2 ejecutar

- **favs cargar ruta/archivo.txt**
  - **Modo de uso**: Al estar dentro de la shell podemos ejecutar el comando **cargar**, que dada una ruta y un archivo .txt, este guardará los datos almacenados en el .txt en la lista de favoritos actual de la shell.
  - **Ejemplo de uso**: favs cargar /home/usuario/Documentos/prueba.txt
 
- **favs guardar ruta/archivo.txt**
  - **Modo de uso**: Al estar dentro de la shell podemos ejecutar el comando **guardar**, este guardará dentro de un archivo .txt los comandos almacenados en la lista de favoritos actual.
  - **Ejemplo de uso**: favs guardar /home/usuario/Documentos/prueba.txt
