#!/bin/bash

if [ -e $1 -a -f $1 -a -x $1 ]
then
  echo "El fichero "$1" es ejecutable."
else
  echo "El fichero "$1" no es algo."
fi
