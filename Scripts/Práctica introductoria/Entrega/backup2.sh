#!/bin/bash

for i in $*
do
	if [ ! -f $i ]	# Comprueba si el parámetro de entrada representa un fichero existente.
	then
		echo "$i no existe."
	else
		A=$(ls $i* | wc -w)	# Lista los ficheros que comienzan por el nombre dado
							# como parámetro, indica cuántos hay y lo almacena en A.
		if [ $A -ge 9 ]	# Si hay 9 o más:
		then
			echo "Se ha superado el número máximo de versiones."
		else
			YY=`date +%g`	# Últimos 2 dígitos del año del sistema.
			MM=`date +%m`	# 2 dígitos del mes.
			DD=`date +%d`	# 2 dígitos del día.
			
			Version=$YY$MM$DD"_"$i	# Se compone el nombre del nuevo nombre de
									# fichero (Version) como el nombre original ($i)
									# precedido por "YYMMDD_" indicado anteriormente.
			cp $i $Version	# Copia el fichero original ($i) con el nombre nuevo.
		fi
	fi
done
