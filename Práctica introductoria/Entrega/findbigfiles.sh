#!/bin/bash

for i in `find $1 -size +$2c`	# Lista con los archivos cuyo tamaño es >= a $2 (bytes).
do
	if [ -f $i ]	# Solo para los ficheros.
	then
		echo $i": "`stat -c%s $i`" bytes"	# Muestra el fichero y su tamaño (bytes).
	fi
done
