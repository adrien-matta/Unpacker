#! /bin/bash
# ./FolderScript.bash /Users/moukaddam/Work/TRIUMF/S1719/MidasData/   /Users/moukaddam/Work/TRIUMF/S1719/RawData/
for i in $1*.mid
do
  DATANAME=${i/$1/$2}
  DATANAME=${DATANAME/run/data}
  DATANAME=${DATANAME/mid/root}
  SIZE=$(du $i | awk '{ print $1 }')
if [ -f $DATANAME ]
  then echo $i already treated, ignored
else
  ./unpacker $i --user-output-tree-name rawdata --no-bank-tree --user-output-path $2 --intial-load 1000000 --maximum-load 1000000
  #./unpacker  $i --user-output-tree-name rawdata  --user-output-path  $2   --bank-output-tree-name bankdata   --bank-output-path  $2 --intial-load 1000000 --maximum-load 1000000
fi
done
