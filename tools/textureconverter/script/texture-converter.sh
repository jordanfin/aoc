#!/bin/bash

#
# Pequeño script para convertir graficos BMP a PNG usando convert de 
# imagemagick. Todo lo que no sea BMP lo copia al directorio destino.
#
# Licencia: 
# Atribución-Compartir Obras Derivadas Igual 2.5 Argentina
# http://creativecommons.org/licenses/by-sa/2.5/ar/
#


IN=$1
OUT=$2

BASENAME="/usr/bin/basename"
SED="/bin/sed"
CONVERT="/usr/bin/convert"

if [ $# \< 2 ]; then
    echo "Debe invocar al script con dos rutas: origen destino"
    exit 1
fi

if [ ! -x $BASENAME ]; then
    echo "No se encuentra basename"
    exit 1
fi

if [ ! -x $SED ]; then
    echo "No se encuentra sed"
    exit 1
fi

if [ ! -x $CONVERT ]; then
    echo "No se encuentra convert"
    exit 1
fi

if [ ! -d $IN ] || [ ! -r $IN ]; then
    echo "Debera indicar como primer argumento un directorio valido"
    exit 1
fi

mkdir -p $OUT

if [ ! -w $OUT ]; then
    echo "No se puede escribir en: $OUT"
    exit 1
fi

find $IN -iname '*.bmp' -type f | while read line; do 
  FA="$line"
  FB="$OUT$(echo $(basename $line) | sed 's/.bmp$/.png/i')"
  echo $FA -\> $FB
  convert $FA $FB
done 

find $IN \! -iname '*.bmp' -type f | while read line; do
  FA="$line"
  FB="$OUT$(basename $line)"
  cp $FA $FB
done

