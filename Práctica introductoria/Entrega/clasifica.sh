#!/bin/bash

if [ -d $1 ]	# Comprueba si existe el directorio.
then
	for i in `ls $1*.mp3`	# Lista de ficheros .mp3 (canciones) que haya en $1.
	do
		if [ ! -d $2 ]	# Comprueba si no existe la carpeta de destino.
		then
			mkdir $2
		fi
		
		grep "autor" $i >> $2/autor.txt
		AUTOR=`cut -f 2 -d":" -s $2/autor.txt`	# Conseguir autor de $i.
		
		grep "titulo" $i >> $2/titulo.txt
		TITULO=`cut -f 2 -d":" -s $2/titulo.txt`	# Conseguir título de $i.
		
		if [ ! -d $2/$AUTOR ]	# Comprueba si no existe la carpeta del autor.
		then
			mkdir $2/$AUTOR
		fi
		
		cp $i $2/$AUTOR/$TITULO.mp3	# Copia la canción en un fichero con su título.
		
		rm $2/autor.txt		# Se eliminan los
		rm $2/titulo.txt	# ficheros auxiliares.
	done
else
	echo "No existe el directorio de origen."
fi
