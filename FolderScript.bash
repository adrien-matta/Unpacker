#! /bin/bash

for i in $1*.mid
do
  DATANAME=${i/$1/$2}
  DATANAME=${DATANAME/run/data}
  DATANAME=${DATANAME/mid/root}
  SIZE=$(du $i | awk '{ print $1 }')
if [ -f $DATANAME ]
  then echo $i already treated, ignored
else
  ./unpacker $i --user-output-name rawdata --no-bank-tree --user-output-path $2 --intial-load 1000000 --maximum-load 1000000
fi
done
