#!/bin/bash

if [ -f $1 -a -x $1 ]	# -f file -> Ya indica si existe.
then
	echo $1 es un fichero ejecutable.
else
	echo $1 no es un fichero ejecutable.
fi
