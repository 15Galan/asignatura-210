#!/bin/bash
hello="Hello world 2"
a=1
b=2
c=`expr $a + $b`	#c = a + b; c = 1 + 2 = 3.

echo $0 $1 $# $*	#Mostrar por pantalla.

# $# -> Número de argumentos (sin $0).
# $* -> Devuelve todos los argumentos (sin $0).
