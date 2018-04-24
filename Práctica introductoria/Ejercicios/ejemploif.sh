#!/bin/bash

if [ -d $1 ]
then
	echo $1	es un directorio.
else
	echo $1 no es un directorio.
fi
