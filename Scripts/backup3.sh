#!/bin/bash

if [ ! -d backup ]	# Comprueba si no existe el directorio "backup"
then				# donde se guardarán las copias de seguridad.
	mkdir backup
fi

FECHA="`date +%g``date +%m``date +%d`"	# Fecha de la creación de la copia.

if [ ! -d backup/$FECHA ]	# Comprueba si no existe el directorio para
then				# esa copia de seguridad (con fecha de hoy).
	mkdir backup/$FECHA
fi

# ------------------------------------------------------------------
for i in $*
do
	cp $i backup/$FECHA/$i 	# Copia el fichero original en otro con el mismo nombre.
done
# ------------------------------------------------------------------
