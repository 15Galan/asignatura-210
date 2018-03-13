#!/bin/bash

if [ ! -f "$1" ]	# Comprueba si el parámetro de entrada representa un fichero existente.
then
	echo "$1 no existe."
else
	A=$(ls $1* | wc -w)	# Lista los ficheros que comienzan por el nombre dado
						# como parámetro, indica cuántos hay y lo almacena en A.
	if [ $A -ge 9 ]	# Si hay 9 o más:
	then
		echo "Se ha superado el número máximo de versiones."
	else
		Num=`expr $A + 1`	# Se suma 1 al número de ficheros que comienzan
							# con ese nombre (A) para que sea la terminación
							# de la nueva copia.
		Version=$1.$Num	# Se compone el nombre del nuevo nombre de
						# fichero (Version) como el nombre original ($1)
						# seguido por "." y el número actual de copia (Num).
		cp $1 $Version	# Copia el fichero original ($1) con el nombre nuevo.
	fi
fi
