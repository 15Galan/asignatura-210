#!/bin/bash

if [ $# -eq 1 ]
then
	if [ -d $1 -a -e $1 ]
	then
	  echo "$1 es un directorio."
	  
	else
	  echo "$1 no es un directorio."
	fi
	
else
	echo "ERROR: El programa necesita un parámetro."
fi
