#!/bin/bash

# El ejercicio lo he planteado tal y como indica el enunciado, solo
# que además, he añadido que dentro del directorio compuesto por la
# fecha, las copias (o backups) se almacenen en subdirectorios según
# la hora en la que se ejecute este script.
# 
# Así los ficheros no se sobreescriben y quedan ordenados y diferenciados
# en distintos subdirectorios dentro de "backup/fecha(YYMMDD)/".

RUTA=backup	# Ruta hasta el directorio (aumentará con la ejecución).

if [ ! -d $RUTA ]	# Comprueba si no existe el directorio "backup"
then				# donde se guardarán las copias de seguridad.
	mkdir $RUTA
fi


RUTA=$RUTA/`date +%g``date +%m``date +%d`	# Actualiza la ruta (añade la fecha).

if [ ! -d $RUTA ]	# Comprueba si no existe el directorio para
then				# esa copia de seguridad (con fecha de hoy).
	mkdir $RUTA
fi

RUTA=$RUTA/`date +%T`	# Actualiza la ruta (añade la hora).

if [ ! -d $RUTA ]	# Comprueba si no existe el directorio para
then				# esa copia de seguridad (con la hora actual).
	mkdir $RUTA
fi

for i in $*
do
	cp $i $RUTA/$i 	# Copia el fichero original ($i) en "RUTA/".
					# La copia se llamará igual que el original ($i).
done

