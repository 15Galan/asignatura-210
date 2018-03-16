#!/bin/bash

if [ -e $1 -a -d $1 ]	# Si existe y es directorio...
then
	for i in $1/*	# Recorre el contenido.
	do
		if [ -d $i ]
		then
			FILES1=`ls -l "$i" | wc -l`
			FILES2=`expr $FILES1 - 1`
			
			if [ $FILES2 -eq -1 ]
			then
				FILES2=0
			fi
			
			echo "$i: $FILES2"
		fi
	done
else
	echo "No es algo."
fi
