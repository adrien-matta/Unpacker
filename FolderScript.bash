#! /bin/bash

for i in $1*.mid
do
  echo Working on $i
  DATANAME=${i/$1/$2}
  DATANAME=${DATANAME/run/data}
  DATANAME=${DATANAME/mid/root}
  SIZE=$(du $i | awk '{ print $1 }')
  echo Working on $i, size: $SIZE

if [ -f $DATANAME ]
  then echo $i already treated, ignored
else
  #echo treating $i
  #./unpacker $i --user-output-tree-name rawdata  --user-output-path $2 --intial-load 1000000 --maximum-load 1000000
  ./unpacker  $i --user-output-tree-name rawdata  --user-output-path  $2   --bank-output-tree-name bankdata   --bank-output-path  $2 --intial-load 1000000 --maximum-load 1000000
fi
done

# ./FolderScript.sh /Users/moukaddam/Work/TRIUMF/S1719/MidasData/   /Users/moukaddam/Work/TRIUMF/S1719/RawData/
# ./FolderScript.sh /Volumes/ne23dp/MidasData/   /Volumes/ne23dp/RawData/
