#!/bin/bash

for i in $1*.mid
do
  DATANAME=${i/$1/$2}
  DATANAME=${DATANAME/run/data}
  DATANAME=${DATANAME/mid/root}
  SIZE=$(du $i | awk '{ print $1 }')
if [ -f $DATANAME ]
  then echo $i already treated, ignored
else
  ./unpacker $i --user-output-name DataS1554 --no-bank-tree --user-output-path $2 --intial-load 100000 --maximum-load 500000
fi
done
