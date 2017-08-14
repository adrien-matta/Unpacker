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
  #./unpacker --verbose 0  $i --user-output-tree-name rawdata    --user-output-path $2  
  ./unpacker  --verbose 0  $i --user-output-tree-name rawdata    --user-output-path $2   --bank-output-tree-name bankdata   --bank-output-path  $2 
  #./unpacker  -v 0    $i   -ut rawdata    -up $2    -bt bankdata    -bp $2    -il 1000000 -ml 1000000
done

# Examples run on terminal:
# ./FolderScript.bash /Users/moukaddam/Work/TRIUMF/S1719/MidasData/   /Users/moukaddam/Work/TRIUMF/S1719/RawData/
# ./FolderScript.bash /Volumes/ne23dp/MidasData/   /Volumes/ne23dp/RawData/
# nohup sh FolderScript.bash /Volumes/ne23dp/MidasData/   /Volumes/ne23dp/RawData/   >/dev/null 2>&1 &
# nohup sh FolderScript.bash /Volumes/ne23dp/MidasData/   /Volumes/ne23dp/RawData/   &